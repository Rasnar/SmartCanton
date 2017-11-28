#include "neo-m8.h"
#include "SerialManager.h"
#include "board.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_dspi.h"
#include "fsl_common.h"
#include "GPIO_Adapter.h"
#include "pin_mux.h"

#include "dev_box_app_task.h"

/**
 * Serial Manager parameters
 */
#define SERIAL_MANAGER_INTERFACE_TYPE gSerialMgrSPIMaster_c
#define SERIAL_MANAGER_INSTANCE_SERIAL 0 // If the SPI0 is used, this value should be 0

#define DSPI_MASTER_BAUDRATE 5000000
#define DSPI_MASTER_BASEADDR SPI0
#define DSPI_MASTER_CLK_SRC DSPI0_CLK_SRC
#define DSPI_MASTER_CLK_FREQ CLOCK_GetFreq(DSPI0_CLK_SRC)

/* Chips selects can be directly triggered by the DSPI component,
 * unfortunately the GPS_nCS isn't connected to a compatible pin */
//#define DSPI_MASTER_PCS_FOR_INIT kDSPI_Pcs0
//#define DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0
#define DISABLE_CHIP_SELECT_GPS()	GPIO_WritePinOutput(BOARD_GPS_nCS_GPIO, BOARD_GPS_nCS_PIN, 1)
#define ENABLE_CHIP_SELECT_GPS()	GPIO_WritePinOutput(BOARD_GPS_nCS_GPIO, BOARD_GPS_nCS_PIN, 0)

static gpioInputPinConfig_t mTimePulseCfg =
{ 		.gpioPort = BOARD_GPS_TIMEPULSE_GPIO_PORT,
		.gpioPin = BOARD_GPS_TIMEPULSE_PIN,
		.pullSelect = pinPull_Down_c,
		.interruptSelect = pinInt_FallingEdge_c,
};

static dspi_master_config_t masterConfig;
static dspi_master_handle_t g_m_handle;
static uint32_t srcClock_Hz;

#define TRANSFER_SIZE 1024U        /*! Transfer dataSize */
static dspi_transfer_t masterXfer;
static uint8_t masterRxData[TRANSFER_SIZE] =
{ 0U };
static uint8_t masterTxData[TRANSFER_SIZE] =
{ 0U };

/* Pointer to a callback function provided by the application */
static void (*app_function_notification_callback)(void);

static void timepulse_irq_callback(void);

static void init_spi_master()
{

	/* Define the init structure for the CS pin */
	gpio_pin_config_t cs_config =
	{ kGPIO_DigitalOutput, 1, };

	GPIO_PinInit(BOARD_GPS_nCS_GPIO, BOARD_GPS_nCS_PIN, &cs_config);
	DISABLE_CHIP_SELECT_GPS();

	/* Master config */
	masterConfig.whichCtar = kDSPI_Ctar0;
	masterConfig.ctarConfig.baudRate = DSPI_MASTER_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame = 8U;
	masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
	masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
	masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / DSPI_MASTER_BAUDRATE;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000000000U / DSPI_MASTER_BAUDRATE;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / DSPI_MASTER_BAUDRATE;

	//masterConfig.whichPcs = EXAMPLE_DSPI_MASTER_PCS_FOR_INIT;
	//masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK = false;
	masterConfig.enableRxFifoOverWrite = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint = kDSPI_SckToSin0Clock;

	srcClock_Hz = DSPI_MASTER_CLK_FREQ;
	DSPI_MasterInit(DSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);
}

static void init_timepulse_irq()
{
	GpioInstallIsr(timepulse_irq_callback, gGpioIsrPrioNormal_c, 0x80, &mTimePulseCfg);
	GpioInputPinInit(&mTimePulseCfg, 1);
}

void read_spi_blocking(uint8_t *txData, uint8_t *rxData, size_t dataSize)
{
	/* Start master transfer */
	masterXfer.txData = txData;
	masterXfer.rxData = rxData;
	masterXfer.dataSize = dataSize;

	// If using a PCs pin configured :
	//masterXfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;

	masterXfer.configFlags = kDSPI_MasterCtar0;

	DSPI_MasterTransferBlocking(DSPI_MASTER_BASEADDR, &masterXfer);
}

void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
{

	DISABLE_CHIP_SELECT_GPS();
	if (status == kStatus_Success)
	{
		if (app_function_notification_callback != NULL)
		{
			app_function_notification_callback();
		}
	}
}

void read_spi_non_blocking(uint8_t *txData, uint8_t *rxData, size_t dataSize)
{
	/* Start master transfer */
	masterXfer.txData = txData;
	masterXfer.rxData = rxData;
	masterXfer.dataSize = dataSize;

	// If using a PCs pin configured :
	//masterXfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;

	masterXfer.configFlags = kDSPI_MasterCtar0;

	/* Set up master transfer */
	DSPI_MasterTransferCreateHandle(DSPI_MASTER_BASEADDR, &g_m_handle, DSPI_MasterUserCallback, NULL);

	DSPI_MasterTransferNonBlocking(DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
}

void write_spi_blocking()
{
	/* Start master transfer */
	masterXfer.txData = masterTxData;
	masterXfer.rxData = masterRxData;
	masterXfer.dataSize = TRANSFER_SIZE;

	// If using a PCs pin configured :
	//masterXfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;

	masterXfer.configFlags = kDSPI_MasterCtar0;

	DSPI_MasterTransferBlocking(DSPI_MASTER_BASEADDR, &masterXfer);
}

/*!
 * @brief Interrupt service each second when the gps is synchronized
 */
void timepulse_irq_callback(void)
{
	ENABLE_CHIP_SELECT_GPS();
	read_spi_non_blocking(masterTxData, masterRxData, TRANSFER_SIZE);

	GpioClearPinIntFlag(&mTimePulseCfg);
}

gpsNeoStatus_t gps_neo_m8_read_rmc(struct minmea_sentence_rmc *frame)
{

	if (minmea_parse_rmc(frame, (char*) masterRxData) == true)
	{
		return gpsNeo_Success;
	}
	else
	{
		return gpsNeo_ParseError;
	}

	return gpsNeo_ReadError;
}

osaStatus_t gps_neo_m8_init(void (*app_notification_callback)(void))
{
	init_spi_master();

	DISABLE_CHIP_SELECT_GPS();

	init_timepulse_irq();

	app_function_notification_callback = app_notification_callback;

	return osaStatus_Success;
}

