/**
 * @file    bme680_support.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief   Functions to support the bme680 BSEC libraries on a Freescale KW41z processor
 */

#ifndef _BME680_SUPPORT_H_
#define _BME680_SUPPORT_H_
#include "BME680_driver/bme680.h"
#include "fsl_i2c_freertos.h"
#include "BME680_BSEC/bsec_integration.h"

typedef enum bme680BsecIntegrationStatus_tag
{
	bme680BsecIntegration_Success = 0U, /*!< Success */
	bme680BsecIntegration_Error = 1U, /*!< Failed */
} bme680BsecIntegrationStatus_t;

/**
 * @brief Wrapper for the bsec_iot_init to remove the need to provide 
 * the device and the handle with each call. And to remove the need to
 * implement inside the task every function required by the bme680 bsec
 * library such has the delay, read/write i2c, etc.
 * 
 * @param bme680 Structure defining the device that we want to access
 * @param i2c_master_rtos_handle I2C handle that is thread safe
 * @return return_values_init Return 0 if success.
 */
return_values_init bme680_bsec_kw41z_I2C_routines_init(struct bme680_dev *bme680,
		i2c_rtos_handle_t* i2c_master_rtos_handle);

/**
 * @brief This function is an loop that will acquisition data from 
 * the bme680 everytime he is availble. 
 * TOFIX: This function can return when an overflow of the time is
 * detected. When this is the case, a new bme680_bsec_kw41z_I2C_routines_init
 * needs to be done. See inside the code the comment that describe 
 * the current bug.
 * 
 * @param output_ready Function that will be called when new data are 
 * available from the BME680.
 */
void bme680_bsec_kw41z_iot_loop(output_ready_fct output_ready);

bme680BsecIntegrationStatus_t bme680_bsec_kw41z_controller_flash_init(void);

#endif /* _BME680_SUPPORT_H_ */
