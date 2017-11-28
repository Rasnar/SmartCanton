/**
 * @file    bno055_support.h
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    25-10-2017
 * @brief   Functions to support the bno055 driver libraries on a Freescale KW41z processor
 */

#ifndef _BNE055_SUPPORT_H_
#define _BNE055_SUPPORT_H_
#include "BNO055_driver/bno055.h"

void bno055_kw41z_I2C_routines_init(struct bno055_t* bno055, void (*app_notification_callback)(void));

#endif /* _BNE055_SUPPORT_H_ */

