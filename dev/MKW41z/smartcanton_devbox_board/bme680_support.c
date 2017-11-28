/**
 * @file    bme680_support.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief   Functions to support the bno055 driver libraries on a Freescale KW41z processor
 */

#include "bme680_support.h"
#include "board.h"
#include "fsl_i2c.h"
#include "fsl_os_abstraction.h"
#include "fsl_gpio.h"
#include "GPIO_Adapter.h"

#define I2C_MASTER_BASEADDR I2C0
#define I2C_MASTER_CLK_SRC I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_BAUDRATE 400000U

static i2c_master_config_t masterConfig;
static i2c_master_transfer_t masterXfer;

void user_delay_ms(uint32_t period)
{
	/*
	 * Return control or wait,
	 * for a period amount of milliseconds
	 */
	OSA_TimeDelay(period);
}

static int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

	/*
	 * The parameter dev_id can be used as a variable to store the I2C address of the device
	 */

	/*
	 * Data on the bus should be like
	 * |------------+---------------------|
	 * | I2C action | Data                |
	 * |------------+---------------------|
	 * | Start      | -                   |
	 * | Write      | (reg_addr)          |
	 * | Stop       | -                   |
	 * | Start      | -                   |
	 * | Read       | (reg_data[0])       |
	 * | Read       | (....)              |
	 * | Read       | (reg_data[len - 1]) |
	 * | Stop       | -                   |
	 * |------------+---------------------|
	 */

	masterXfer.slaveAddress = dev_id;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = reg_addr;
	masterXfer.subaddressSize = sizeof(reg_addr);
	masterXfer.data = reg_data;
	masterXfer.dataSize = len;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(I2C_MASTER_BASEADDR, &masterXfer);

	return rslt;
}

static int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

	/*
	 * The parameter dev_id can be used as a variable to store the I2C address of the device
	 */

	/*
	 * Data on the bus should be like
	 * |------------+---------------------|
	 * | I2C action | Data                |
	 * |------------+---------------------|
	 * | Start      | -                   |
	 * | Write      | (reg_addr)          |
	 * | Write      | (reg_data[0])       |
	 * | Write      | (....)              |
	 * | Write      | (reg_data[len - 1]) |
	 * | Stop       | -                   |
	 * |------------+---------------------|
	 */

	masterXfer.slaveAddress = dev_id;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = reg_addr;
	masterXfer.subaddressSize = sizeof(reg_addr);
	masterXfer.data = reg_data;
	masterXfer.dataSize = len;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	status_t res = I2C_MasterTransferBlocking(I2C_MASTER_BASEADDR, &masterXfer);
	res = res;

	return rslt;
}

void bme680_kw41z_I2C_routines_init(struct bme680_dev *bme680)
{

	 /*
	 * masterConfig.baudRate_Bps = 100000U;
	 * masterConfig.enableStopHold = false;
	 * masterConfig.glitchFilterWidth = 0U;
	 * masterConfig.enableMaster = true;
	 */
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = I2C_BAUDRATE;

	I2C_MasterInit(I2C_MASTER_BASEADDR, &masterConfig, I2C_MASTER_CLK_FREQ);

	bme680->dev_id = BME680_I2C_ADDR_PRIMARY;
	bme680->intf = BME680_I2C_INTF;
	bme680->read = user_i2c_read;
	bme680->write = user_i2c_write;
	bme680->delay_ms = user_delay_ms;
}
