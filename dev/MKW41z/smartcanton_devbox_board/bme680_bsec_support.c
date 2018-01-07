/**
 * @file    bme680_support.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 * @brief   Functions to support the bno055 driver libraries on a Freescale KW41z processor
 */

#include <bme680_bsec_support.h>
#include "board.h"
#include "fsl_i2c.h"
#include "fsl_os_abstraction.h"
#include "fsl_gpio.h"
#include "GPIO_Adapter.h"
#include "FunctionLib.h"

/* Handle to the i2c (thread safe) */
static i2c_rtos_handle_t* master_rtos_handle;

/* Structure to define the transfert by specifying the buffers and the 
length to read/write */
static i2c_master_transfer_t masterXfer;

/* These arrays are configuration availables for the BME680
 * Please refer to the PDF named "Integration Guide Bosch Software
 * Environmental Cluster (BSEC)" for more information */
/* generic_33v_3s_4d 3.3V 3s 4 days */
const uint8_t generic_33v_3s_4d[400] =
{ 1, 5, 4, 1, 61, 0, 0, 0, 0, 0, 0, 0, 120, 1, 0, 0, 105, 0, 1, 0, 3, 205, 204, 76, 62, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 113, 61, 138, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 137, 65, 0, 63, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 225, 68, 0, 192, 168, 71, 40, 0, 2, 0, 0, 0, 0,
		0, 0, 0, 250, 67, 0, 0, 97, 67, 0, 0, 200, 65, 0, 0, 200, 65, 173, 250, 228, 64, 41, 28, 93, 67, 0, 0,
		112, 65, 0, 0, 0, 63, 9, 0, 5, 0, 0, 0, 0, 0, 1, 51, 0, 9, 0, 10, 215, 163, 59, 205, 204, 204, 61,
		225, 122, 148, 62, 41, 92, 15, 61, 0, 0, 0, 63, 0, 0, 0, 63, 154, 153, 89, 63, 154, 153, 25, 62, 1, 1,
		0, 0, 128, 63, 6, 236, 81, 184, 61, 51, 51, 131, 64, 40, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 0, 254, 0, 2, 1, 5, 48,
		117, 100, 0, 44, 1, 151, 7, 132, 3, 197, 0, 144, 1, 64, 1, 64, 1, 48, 117, 48, 117, 48, 117, 48, 117,
		100, 0, 100, 0, 100, 0, 48, 117, 48, 117, 48, 117, 100, 0, 100, 0, 48, 117, 100, 0, 100, 0, 100, 0,
		100, 0, 48, 117, 48, 117, 48, 117, 100, 0, 100, 0, 100, 0, 48, 117, 48, 117, 100, 0, 44, 1, 44, 1, 44,
		1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		44, 1, 0, 0, 0, 0, 178, 202, 0, 0 };

/* generic_33v_3s_4d 3.3V 300s 4 days */
const uint8_t generic_33v_300s_4d[400] =
{ 1, 5, 4, 1, 61, 0, 0, 0, 0, 0, 0, 0, 120, 1, 0, 0, 105, 0, 1, 0, 3, 205, 204, 76, 62, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 113, 61, 138, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 137, 65, 0, 63, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 225, 68, 0, 192, 168, 71, 40, 0, 2, 0, 0, 0, 0,
		0, 0, 0, 250, 67, 0, 0, 97, 67, 0, 0, 200, 65, 0, 0, 200, 65, 173, 250, 228, 64, 41, 28, 93, 67, 0, 0,
		112, 65, 0, 0, 0, 63, 9, 0, 5, 0, 0, 0, 0, 0, 1, 51, 0, 9, 0, 10, 215, 163, 59, 205, 204, 204, 61,
		225, 122, 148, 62, 41, 92, 15, 61, 0, 0, 0, 63, 0, 0, 0, 63, 154, 153, 89, 63, 154, 153, 25, 62, 1, 1,
		0, 0, 128, 63, 6, 236, 81, 184, 61, 51, 51, 131, 64, 40, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 0, 254, 0, 2, 1, 5, 48,
		117, 100, 0, 44, 1, 151, 7, 132, 3, 197, 0, 144, 1, 64, 1, 64, 1, 48, 117, 48, 117, 48, 117, 48, 117,
		100, 0, 100, 0, 100, 0, 48, 117, 48, 117, 48, 117, 100, 0, 100, 0, 48, 117, 100, 0, 100, 0, 100, 0,
		100, 0, 48, 117, 48, 117, 48, 117, 100, 0, 100, 0, 100, 0, 48, 117, 48, 117, 100, 0, 44, 1, 44, 1, 44,
		1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 44, 1, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		48, 117, 0, 0, 0, 0, 32, 195, 0, 0 };

/*!
 * @brief           Load previous library state from non-volatile memory
 *
 * @param[in,out]   state_buffer    buffer to hold the loaded state string
 * @param[in]       n_buffer        size of the allocated state buffer
 *
 * @return          number of bytes copied to state_buffer
 */
uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer)
{
    // ...
	// TODO :
    // Load a previous library state from non-volatile memory, if available.
    //
    // Return zero if loading was unsuccessful or no state was available,
    // otherwise return length of loaded state string.
    // ...
    return 0;
}

/*!
 * @brief           Save library state to non-volatile memory
 *
 * @param[in]       state_buffer    buffer holding the state to be stored
 * @param[in]       length          length of the state string to be stored
 *
 * @return          none
 */
void state_save(const uint8_t *state_buffer, uint32_t length)
{
    // ...
	// TODO :
    // Save the string some form of non-volatile memory, if possible.
    // ...
}

/*!
 * @brief           Load library config from non-volatile memory
 *
 * @param[in,out]   config_buffer    buffer to hold the loaded state string
 * @param[in]       n_buffer        size of the allocated state buffer
 *
 * @return          number of bytes copied to config_buffer
 */
uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer)
{
	/* By default we load the generic_33v_3s_4d config. More configuration can be found in the
	 * BSEC folder.
	 */
	if (n_buffer >= sizeof(generic_33v_3s_4d))
	{
		FLib_MemCpy(config_buffer, (uint8_t *)generic_33v_3s_4d, sizeof(generic_33v_3s_4d));
		return sizeof(generic_33v_3s_4d);
	}

	return 0;
}

/*!
 * @brief           Read operation in either I2C
 *
 * param[in]        dev_addr        I2C device address
 * param[in]        reg_addr        register address
 * param[out]       reg_data_ptr    pointer to the memory to be used to store the read data
 * param[in]        data_len        number of bytes to be read
 *
 * @return          result of the bus communication function
 */
static int8_t user_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len)
{

	masterXfer.slaveAddress = dev_addr;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = reg_addr;
	masterXfer.subaddressSize = sizeof(reg_addr);
	masterXfer.data = reg_data_ptr;
	masterXfer.dataSize = data_len;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return (int8_t) I2C_RTOS_Transfer(master_rtos_handle, &masterXfer);
}

/*!
 * @brief           Write operation in either I2C
 *
 * param[in]        dev_addr        I2C or SPI device address
 * param[in]        reg_addr        register address
 * param[in]        reg_data_ptr    pointer to the data to be written
 * param[in]        data_len        number of bytes to be written
 *
 * @return          result of the bus communication function
 */
static int8_t user_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len)
{
	masterXfer.slaveAddress = dev_addr;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = reg_addr;
	masterXfer.subaddressSize = sizeof(reg_addr);
	masterXfer.data = reg_data_ptr;
	masterXfer.dataSize = data_len;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return (int8_t) I2C_RTOS_Transfer(master_rtos_handle, &masterXfer);
}

/*!
 * @brief           System specific implementation of sleep function
 *
 * @param[in]       t_ms    time in milliseconds
 *
 * @return          none
 */
static void sleep(uint32_t t_ms)
{
	OSA_TimeDelay(t_ms);
}

/*!
 * @brief           Capture the system time in microseconds
 *
 * @return          system_current_time    current system timestamp in microseconds
 */
int64_t get_timestamp_us()
{
	return ((int64_t)OSA_TimeGetMsec()) * ((int64_t)1000);
}

return_values_init bme680_bsec_kw41z_I2C_routines_init(struct bme680_dev *bme680,
		i2c_rtos_handle_t* i2c_master_rtos_handle)
{

	master_rtos_handle = i2c_master_rtos_handle;

	/* Call to the function which initializes the BSEC library
	 * Switch on low-power mode and provide no temperature offset */
	return_values_init ret = bsec_iot_init(BSEC_SAMPLE_RATE_LP, 0.0f, user_i2c_write, user_i2c_read, sleep,
			state_load, config_load);
	if (ret.bme680_status)
	{
		/* Could not intialize BME680 */
		return ret;
	}
	else if (ret.bsec_status)
	{
		/* Could not intialize BSEC library */
		return ret;
	}
	return ret;
}

/*!
 * @brief       Runs the main (endless) loop that queries sensor settings, applies them, and processes the measured data
 *
 * @param[in]   sleep               pointer to the system specific sleep function
 * @param[in]   get_timestamp_us    pointer to the system specific timestamp derivation function
 * @param[in]   output_ready        pointer to the function processing obtained BSEC outputs
 * @param[in]   state_save          pointer to the system-specific state save function
 * @param[in]   save_intvl          interval at which BSEC state should be saved (in samples)
 *
 * @return      none
 */
void bme680_bsec_kw41z_iot_loop(output_ready_fct output_ready)
{
	/* Call to endless loop function which reads and processes data based on sensor settings */
	/* State is saved every 10.000 samples, which means every 5.000 * 3 secs = 250 minutes  */
	bsec_iot_loop(sleep, get_timestamp_us, output_ready, state_save, 5000);

}
