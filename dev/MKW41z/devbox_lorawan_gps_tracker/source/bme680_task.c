/**
 * @file    bme680_task.c
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief
 */

#include "bme680_task.h"
#include "dev_box_app_task.h"
#include "LED.h"
#include "Panic.h"
#include "bme680_support.h"

OSA_TASK_DEFINE(Bme680_Task, gBme680TaskPriority_c, 1, gBme680TaskStackSize_c, FALSE);
osaTaskId_t gBme680TaskId = 0;

static i2c_rtos_handle_t* master_rtos_handle;

struct bme680_dev bme680;
/**
 * Main BME680 task function
 */
void Bme680_Task(osaTaskParam_t argument)
{
	int8_t rslt = 0;
	(void)rslt;

	bme680_kw41z_I2C_routines_init(&bme680, master_rtos_handle);
	rslt = bme680_init(&bme680);


	while (1)
	{
		uint8_t set_required_settings;

		/* Set the temperature, pressure and humidity settings */
		bme680.tph_sett.os_hum = BME680_OS_16X;
		bme680.tph_sett.os_pres = BME680_OS_16X;
		bme680.tph_sett.os_temp = BME680_OS_16X;
		bme680.tph_sett.filter = BME680_FILTER_SIZE_3;

		/* Set the remaining gas sensor settings and link the heating profile */
		bme680.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
		/* Create a ramp heat waveform in 3 steps */
		bme680.gas_sett.heatr_temp = 320; /* degree Celsius */
		bme680.gas_sett.heatr_dur = 150; /* milliseconds */

		/* Select the power mode */
		/* Must be set before writing the sensor configuration */
		bme680.power_mode = BME680_FORCED_MODE;

		/* Set the required sensor settings needed */
		set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL
			| BME680_GAS_SENSOR_SEL;

		/* Set the desired sensor configuration */
		rslt = bme680_set_sensor_settings(set_required_settings,&bme680);

		/* Set the power mode */
		rslt = bme680_set_sensor_mode(&bme680);

		/* Get the total measurement duration so as to sleep or wait till the
		 * measurement is complete */
		uint16_t meas_period;
		bme680_get_profile_dur(&meas_period, &bme680);
		OSA_TimeDelay(meas_period); /* Delay till the measurement is ready */

		rslt = 0;
		struct bme680_field_data data;
		rslt = bme680_get_sensor_data(&data, &bme680);
		OSA_TimeDelay(1000);
	}
}

osaStatus_t Bme680_TaskInit(i2c_rtos_handle_t* i2c_master_rtos_handle)
{
	if (gBme680TaskId)
	{
		return osaStatus_Error;
	}

	master_rtos_handle = i2c_master_rtos_handle;

	/* Task creation */
	gBme680TaskId = OSA_TaskCreate(OSA_TASK(Bme680_Task), NULL);

	if ( NULL == gBme680TaskId)
	{
		panic(0, 0, 0, 0);
		return osaStatus_Error;
	}

	return osaStatus_Success;
}

