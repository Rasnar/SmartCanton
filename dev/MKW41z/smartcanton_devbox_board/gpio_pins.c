/**
 * @file    gpio_pins.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief	Defining the GPIO available on the SmartCantonDevBox (LEDs and switches)
 */

#include "GPIO_Adapter.h"
#include "gpio_pins.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief Define the switches pinout connected to the KW41z
 * 
 */
gpioInputPinConfig_t switchPins[] = {
    {
        .gpioPort = gpioPort_B_c,
        .gpioPin = 2,
        .pullSelect = pinPull_Up_c,
        .interruptSelect = pinInt_FallingEdge_c
    },
    {
        .gpioPort = gpioPort_B_c,
        .gpioPin = 3,
        .pullSelect = pinPull_Up_c,
        .interruptSelect = pinInt_FallingEdge_c
    }
};

/**
 * @brief Define the LEDs pinout connected to the KW41z
 * 
 */
gpioOutputPinConfig_t ledPins[] = {
    {
        .gpioPort = gpioPort_A_c,
        .gpioPin = 19,
        .outputLogic = 0,
        .slewRate = pinSlewRate_Slow_c,
        .driveStrength = pinDriveStrength_Low_c
    },
    {
        .gpioPort = gpioPort_A_c,
        .gpioPin = 18,
        .outputLogic = 0,
        .slewRate = pinSlewRate_Slow_c,
        .driveStrength = pinDriveStrength_Low_c
    },
    {
        .gpioPort = gpioPort_A_c,
        .gpioPin = 17,
        .outputLogic = 0,
        .slewRate = pinSlewRate_Slow_c,
        .driveStrength = pinDriveStrength_Low_c
    },
    {
        .gpioPort = gpioPort_A_c,
        .gpioPin = 16,
        .outputLogic = 0,
        .slewRate = pinSlewRate_Slow_c,
        .driveStrength = pinDriveStrength_Low_c
    }
};


