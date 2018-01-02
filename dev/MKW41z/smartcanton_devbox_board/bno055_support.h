/**
 * @file    bno055_support.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief   Functions to support the bno055 driver libraries on a Freescale KW41z processor
 */

#ifndef _BNE055_SUPPORT_H_
#define _BNE055_SUPPORT_H_
#include "BNO055_driver/bno055.h"
#include "fsl_i2c_freertos.h"

/**
 * @brief Function to provide and handler to the i2c controller and to initialize 
 * map the functions that need to be called by the bno055 driver. Also initialize
 * the callback function when an interruption is generated.
 * 
 * @param bno055 Bno055 defining the callbacks for the driver.
 * @param i2c_master_rtos_handle Handle to the i2c controller 
 * @param bno055_app_notification_callback Function that will be called when an 
 * interrupt from the Bno055 is received. Can be set to NULL if the application
 * does not want to be notified.
 */
void bno055_kw41z_I2C_routines_init(struct bno055_t* bno055, i2c_rtos_handle_t* i2c_master_rtos_handle,
		void (*bno055_app_notification_callback)(void));

#endif /* _BNE055_SUPPORT_H_ */

