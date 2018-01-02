/**
 * @file    neo-m8.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Functions to initialize a GPS UBlox NEO-M8 connected by SPI.
 * This library does not support the GPS configuration using the 
 * UBX protocol. This means that the GPS can only be used to retrieve
 * RMC/GGA frames from the GPS with his default configuration.
 */

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

/* Configuration for the Timepulse interruption */ 
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

/**
 * @brief Function to initialize the SPI with the Neo M8 GPS 
 * TODO: This function should be initialize with a RTOS SPI handler. 
 * For now this isn't problematic because the GPS is the only device
 * used in SPI, but for the future, the same think done with the I2C
 * (Bme680 + Bno055) should be done here too.
 */
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

/**
 * @brief Install a gpio interruption on the timepulse_irq_callback function with the pin specified
 * by the structure mTimePulseCfg.
 */
static void init_timepulse_irq()
{
	GpioInstallIsr(timepulse_irq_callback, gGpioIsrPrioLow_c, gGpioDefaultNvicPrio_c, &mTimePulseCfg);
	GpioInputPinInit(&mTimePulseCfg, 1);
}

static void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
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

/**
 * @brief Read data from the SPI handle. The read isn't blocking, the function
 * DSPI_MasterUserCallback will be called once the read is finished.
 * 
 * @param txData Buffer with data to transmit while reading
 * @param rxData Buffer to store the read data
 * @param dataSize Size of the read and write buffer
 */
static void read_spi_non_blocking(uint8_t *txData, uint8_t *rxData, size_t dataSize)
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

/*!
 * @brief Interrupt service each X second when the gps is synchronized
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
}

osaStatus_t gps_neo_m8_init(void (*app_notification_callback)(void))
{
	init_spi_master();

	DISABLE_CHIP_SELECT_GPS();

	init_timepulse_irq();

	app_function_notification_callback = app_notification_callback;

	return osaStatus_Success;
}

