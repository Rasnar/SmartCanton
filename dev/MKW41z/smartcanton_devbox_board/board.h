/**
 * @file    board.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Functions and definitions hard on the SmartCanton DevBox board.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "fsl_gpio.h"
#include "fsl_i2c_freertos.h"
#include "fsl_dspi_freertos.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The board name */
#define MANUFACTURER_NAME               "NXP"
#define BOARD_NAME                      "SmartCantonDevBox"

#define BOARD_XTAL0_CLK_HZ    32000000U
#define BOARD_XTAL32K_CLK_HZ  32768U

#define CORE_CLOCK_FREQ 20970000U

/* Connectivity */
#ifndef APP_SERIAL_INTERFACE_TYPE
#define APP_SERIAL_INTERFACE_TYPE (gSerialMgrLpuart_c)
#endif

#ifndef APP_SERIAL_INTERFACE_INSTANCE
#define APP_SERIAL_INTERFACE_INSTANCE (0)
#endif

#ifndef APP_SERIAL_INTERFACE_SPEED
#define APP_SERIAL_INTERFACE_SPEED (115200)
#endif

/* EEPROM type used */
#ifndef gEepromType_d
#define gEepromType_d gEepromDevice_AT45DB041E_c
#endif

/* DCDC driver default mode */
#ifndef APP_DCDC_MODE
#define APP_DCDC_MODE    (gDCDC_Mode_Buck_c)
#endif

/* Vbat monitor Interval in Ms */
#ifndef APP_DCDC_VBAT_MONITOR_INTERVAL
#define APP_DCDC_VBAT_MONITOR_INTERVAL    (50000)
#endif

/* 1<<x to enable PINx as wakup source in LLWU */
#define BOARD_LLWU_PIN_ENABLE_BITMAP ( 1<<12 | 1<<13 )

/*! @brief TSI electrodes count */
#ifndef gKBD_TsiElectdCount_c
#define gKBD_TsiElectdCount_c 0U
#endif
/*! @brief TSI electrodes mapping: 1<<x to mark KBD pin as TSI */
#define gKBD_SwTypeMapping_d ( 1<<0 | 1<<3 )
/*! @brief TSI electrodes pins */
#define gKBD_TsiElecList_d {4U, 7U}

/* The PWM channels used for RGB LED */
#ifndef BOARD_USE_PWM_FOR_RGB_LED
#define BOARD_USE_PWM_FOR_RGB_LED       1
#endif

#ifndef gStackTimerInstance_c
#define gStackTimerInstance_c           1
#endif
#ifndef gRedLedPwmTimerInstance_c
#define gRedLedPwmTimerInstance_c       0
#endif
#ifndef gGreenLedPwmTimerInstance_c
#define gGreenLedPwmTimerInstance_c     2
#endif
#ifndef gBlueLedPwmTimerInstance_c
#define gBlueLedPwmTimerInstance_c      2
#endif

#ifndef gRedLedPwmTimerChannel_c
#define gRedLedPwmTimerChannel_c        2
#endif
#ifndef gGreenLedPwmTimerChannel_c
#define gGreenLedPwmTimerChannel_c      1
#endif
#ifndef gBlueLedPwmTimerChannel_c
#define gBlueLedPwmTimerChannel_c       0
#endif
/* Index into the ledPins[] array */
#ifndef gRedLedIdx_c
#define gRedLedIdx_c                    1
#endif
#ifndef gGreenLedIdx_c
#define gGreenLedIdx_c                  2
#endif
#ifndef gBlueLedIdx_c
#define gBlueLedIdx_c                   3
#endif

#define BOARD_BNO055_INT_PIN			19U
#define BOARD_BNO055_INT_GPIO			GPIOC
#define BOARD_BNO055_INT_GPIO_PORT		gpioPort_C_c
#define BOARD_BNO055_INT_PORT			PORTC

#define BOARD_GPS_TIMEPULSE_PIN			1U
#define BOARD_GPS_TIMEPULSE_GPIO		GPIOC
#define BOARD_GPS_TIMEPULSE_GPIO_PORT	gpioPort_C_c
#define BOARD_GPS_TIMEPULSE_PORT		PORTC

#define BOARD_GPS_nCS_PIN				5U
#define BOARD_GPS_nCS_GPIO				GPIOC
#define BOARD_GPS_nCS_PORT				PORTC

#define BOARD_I2C_EMBEDDED_SENSORS_BASEADDR I2C0
#define BOARD_I2C_EMBEDDED_SENSORS_CLK_SRC I2C0_CLK_SRC
#define BOARD_I2C_EMBEDDED_SENSORS_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define BOARD_I2C_EMBEDDED_SENSORS_BAUDRATE 400000U

#define gADC16_ReferenceVoltageSource_d kADC16_ReferenceVoltageSourceValt

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/************************************************************************************
 *************************************************************************************
 * Public prototypes
 *************************************************************************************
 ************************************************************************************/

void hardware_init(void);

/* Function to initialize ADC on board configuration. */
void BOARD_InitAdc(void);

/**
 * @brief Initialize the I2C used by all sensors on the board.
 * The configuration is the following:
 * masterConfig.baudRate_Bps = 100000U;
 * masterConfig.enableStopHold = false;
 * masterConfig.glitchFilterWidth = 0U;
 * masterConfig.enableMaster = true;
 *
 */
status_t BOARD_InitI2CEmbeddedSensors(void);

/**
 * @brief Get the current i2c handler for all the sensors on the board.
 * The handler is thread safe.
 * 
 * @return i2c_rtos_handle_t* I2C handler to be used to read/write
 */
i2c_rtos_handle_t* BOARD_GetI2CEmbeddedSensorsHandle(void);

/**
 * @brief Initialize the DCDC if used.  
 */
void BOARD_DCDCInit(void);

/* Function to read battery level on board configuration. */

/**
 * @brief Get the battery level if we are in DCDC mode
 * connected on the channel ADC16_BATLVL_CHN
 * 
 * @param void 
 * @return uint8_t Battery pourcentage
 */
uint8_t BOARD_GetBatteryLevelDCDC(void);

/**
 * @brief Get the battery level connected on the channel ADC16_BATLVL_CHN
 * 
 * @return uint8_t Battery pourcentage
 */
uint8_t BOARD_GetBatteryLevel(void);

/**
 * @brief Get the current temperature on the board from the 
 * internal sensor.
 * 
 * @return int32_t Temperature in °C multiplied by 1000
 */
int32_t BOARD_GetTemperature(void);

/* Function to generate a random number if no potentiometer is present on the board. */
uint16_t BOARD_GetPotentiometerLevel(void);

/* Function to install callbacks for actions before and after entering low power. */
extern void BOARD_InstallLowPowerCallbacks(void);

/* Function called by low power module prior to entering low power. */
extern void BOARD_EnterLowPowerCb(void);

/* Function called by low power module after exiting low power. */
extern void BOARD_ExitLowPowerCb(void);

/* Function called by the BLE connection manager to generate PER MCU keys */
extern void BOARD_GetMCUUid(uint8_t* aOutUid16B, uint8_t* pOutLen);

/* Functions used to determine the frequency of a module's input clock. */
uint32_t BOARD_GetLpuartClock(uint32_t instance);
uint32_t BOARD_GetTpmClock(uint32_t instance);
uint32_t BOARD_GetSpiClock(uint32_t instance);
uint32_t BOARD_GetI2cClock(uint32_t instance);
void BOARD_BLPEtoBLPI(void);
void BOARD_BLPItoBLPE(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __BOARD_H__ */
