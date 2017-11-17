
#include "neo-m8.h"
#include "SerialManager.h"
#include "board.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_dspi.h"
#include "fsl_common.h"
#include "GPIO_Adapter.h"
#include "pin_mux.h"

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

gpioInputPinConfig_t mTimePulseCfg = {
	.gpioPort = gpioPort_C_c,
	.gpioPin = BOARD_GPS_TIMEPULSE_PIN,
	.pullSelect = pinPull_Down_c,
	.interruptSelect = pinInt_LogicOne_c,
};

static dspi_master_config_t masterConfig;
uint32_t srcClock_Hz;

#define TRANSFER_SIZE 512U        /*! Transfer dataSize */
static dspi_transfer_t masterXfer;
uint8_t masterRxData[TRANSFER_SIZE] = {0U};
uint8_t masterTxData[TRANSFER_SIZE] = {0U};

void TIMEPULSE_IRQ_CALLBACK(void);

static void init_spi_master(){

	/* Define the init structure for the CS pin */
	gpio_pin_config_t cs_config = {
		kGPIO_DigitalOutput, 1,
	};

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

static void init_timepulse_irq(){

	GpioInstallIsr(TIMEPULSE_IRQ_CALLBACK, gGpioIsrPrioNormal_c, 0x80, &mTimePulseCfg);
	GpioInputPinInit(&mTimePulseCfg, 1);


}

void read_spi(uint8_t *txData, uint8_t *rxData, size_t dataSize){
    /* Start master transfer */
    masterXfer.txData = txData;
    masterXfer.rxData = rxData;
    masterXfer.dataSize = dataSize;

    // If using a PCs pin configured :
    //masterXfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;

    masterXfer.configFlags = kDSPI_MasterCtar0 ;

    DSPI_MasterTransferBlocking(DSPI_MASTER_BASEADDR, &masterXfer);
}

void write_spi(){
	/* Start master transfer */
	masterXfer.txData = masterTxData;
	masterXfer.rxData = masterRxData;
	masterXfer.dataSize = TRANSFER_SIZE;

	// If using a PCs pin configured :
	//masterXfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;

	masterXfer.configFlags = kDSPI_MasterCtar0 ;

	DSPI_MasterTransferBlocking(DSPI_MASTER_BASEADDR, &masterXfer);
}

/*!
 * @brief Interrupt service each second when the gps is synchronized
 */
//void BOARD_GPS_TIMEPULSE(void)
void TIMEPULSE_IRQ_CALLBACK(void) {

	//struct minmea_sentence_rmc rmcData;
	//gps_neo_m8_read_rmc(&rmcData);

	ENABLE_CHIP_SELECT_GPS();
	//read_spi(masterTxData, masterRxData, TRANSFER_SIZE);
	DISABLE_CHIP_SELECT_GPS();

    GpioClearPinIntFlag(&mTimePulseCfg);
}



gpsNeoStatus_t gps_neo_m8_read_rmc(struct minmea_sentence_rmc *frame) {







//	if((Serial_Read(interfaceId, data, sizeof(data), &bytesRead) == gSerial_Success_c)
//			&& (bytesRead > 0)){
//
//		if (minmea_check((char*) data, false) == true) {
//			if (minmea_parse_rmc(frame, (char*) data) == true) {
//				DISABLE_CHIP_SELECT_GPS();
//				return gpsNeo_Success;
//			} else {
//				DISABLE_CHIP_SELECT_GPS();
//				return gpsNeo_ParseError;
//			}
//		} else {
//			DISABLE_CHIP_SELECT_GPS();
//			return gpsNeo_ReadError;
//		}
//	}

	return gpsNeo_ReadError;
}

osaStatus_t gps_neo_m8_init(void)
{
	init_spi_master();

	DISABLE_CHIP_SELECT_GPS();

	init_timepulse_irq();

	return osaStatus_Success;
}

