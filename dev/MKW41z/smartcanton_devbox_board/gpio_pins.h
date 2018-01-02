/**
 * @file    gpio_pins.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Defining the GPIO available on the SmartCantonDevBox (LEDs and switches)
 */

#ifndef __FSL_GPIO_PINS_H__
#define __FSL_GPIO_PINS_H__

#include "GPIO_Adapter.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* switch pins to be initialized by the low power component if the keyboard isn't enabled*/
/* 1<<n for sitchPins[n] */
#define gLowPower_switchPinsToInitBitmap_d  ( 1<<1 | 1<<0 )

/**
 * @brief Define the switches pinout connected to the KW41z
 * 
 */
extern gpioInputPinConfig_t switchPins[];

/**
 * @brief Define the LEDs pinout connected to the KW41z
 * 
 */
extern gpioOutputPinConfig_t ledPins[];

extern gpioInputPinConfig_t gCoexistence_RfDeny;
extern gpioOutputPinConfig_t gCoexistence_RfActive;
extern gpioOutputPinConfig_t gCoexistence_RfStatus;
#endif /* __FSL_GPIO_PINS_H__ */
