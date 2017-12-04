/**
 * @file    bno055_support.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief   Functions to support the bno055 on a Freescale KW41z processor
 */

#include "bno055_support.h"
#include "board.h"
#include "fsl_i2c_freertos.h"
#include "fsl_os_abstraction.h"
#include "fsl_gpio.h"
#include "GPIO_Adapter.h"

#define	I2C_BUFFER_LEN 8
#define	BNO055_I2C_BUS_WRITE_ARRAY_INDEX	((u8)1)

static gpioInputPinConfig_t mBno055IntCfg =
{ .gpioPort = BOARD_BNO055_INT_GPIO_PORT, .gpioPin = BOARD_BNO055_INT_PIN, .pullSelect = pinPull_Down_c,
		.interruptSelect = pinInt_RisingEdge_c, };

static i2c_rtos_handle_t* master_rtos_handle;
static i2c_master_transfer_t masterXfer;

/* Pointer to a callback function provided by the application */
static void (*bno055_app_function_notification_callback)(void);

/*	\Brief: The API is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be read
 *	\param reg_data : This data read from the sensor,
 *   which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
static s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

/*	\Brief: The API is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *	will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
static s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

/*	Brief : The delay routine
 *	\param : delay in ms
 */
static void BNO055_delay_msek(u32 msek);


/*!
 * @brief Interrupt service each second when the gps is synchronized
 */
static void bno055_irq(void)
{

	if (bno055_app_function_notification_callback != NULL)
	{
		bno055_app_function_notification_callback();
	}
	GpioClearPinIntFlag(&mBno055IntCfg);
}

static void init_bno055_irq()
{

	GpioInstallIsr(bno055_irq, gGpioIsrPrioLow_c, gGpioDefaultNvicPrio_c, &mBno055IntCfg);
	GpioInputPinInit(&mBno055IntCfg, 1);
}

void bno055_kw41z_I2C_routines_init(struct bno055_t* bno055, i2c_rtos_handle_t* i2c_master_rtos_handle, void (*bno055_app_notification_callback)(void))
{

	bno055_app_function_notification_callback = bno055_app_notification_callback;

	init_bno055_irq();

	bno055->bus_write = BNO055_I2C_bus_write;
	bno055->bus_read = BNO055_I2C_bus_read;
	bno055->delay_msec = BNO055_delay_msek;
	bno055->dev_addr = BNO055_I2C_ADDR1;


	master_rtos_handle = i2c_master_rtos_handle;
}

/*-------------------------------------------------------------------*
 *
 *	This is a sample code for read and write the data by using I2C
 *	Use either I2C  based on your need
 *	The device address defined in the bno055.h file
 *
 *--------------------------------------------------------------------*/

/*	\Brief: The API is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
static s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	masterXfer.slaveAddress = dev_addr;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = reg_addr;
	masterXfer.subaddressSize = sizeof(reg_addr);
	masterXfer.data = reg_data;
	masterXfer.dataSize = cnt;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	status_t status = I2C_RTOS_Transfer(master_rtos_handle, &masterXfer);
	if (status == kStatus_Success)
	{
		return (s8) BNO055_SUCCESS;
	}
	else
	{
		return (s8) BNO055_ERROR;
	}
}

/*	\Brief: The API is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *  will data is going to be read
 *	\param reg_data : This data read from the sensor,
 *   which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
static s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	masterXfer.slaveAddress = dev_addr;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = reg_addr;
	masterXfer.subaddressSize = sizeof(reg_addr);
	masterXfer.data = reg_data;
	masterXfer.dataSize = cnt;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	status_t status = I2C_RTOS_Transfer(master_rtos_handle, &masterXfer);
	if (status == kStatus_Success)
	{
		return (s8) BNO055_SUCCESS;
	}
	else
	{
		return (s8) BNO055_ERROR;
	}

}
/*	Brief : The delay routine
 *	\param : delay in ms
 */
static void BNO055_delay_msek(u32 msek)
{
	OSA_TimeDelay(msek);
}
