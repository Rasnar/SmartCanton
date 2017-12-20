// Adapted from https://developer.mbed.org/teams/myDevicesIoT/code/Cayenne-LPP/

// Copyright © 2017 The Things Network
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#ifndef _CAYENNE_LPP_H_
#define _CAYENNE_LPP_H_

#include <stdint.h>
#include <string.h>


//LPP_BATTERY = // TODO Unsupported in IPSO Smart Object
//LPP_PROXIMITY = // TODO Unsupported in IPSO Smart Object

#define LPP_DIGITAL_INPUT 0         // 1 byte
#define LPP_DIGITAL_OUTPUT 1        // 1 byte
#define LPP_ANALOG_INPUT 2          // 2 bytes, 0.01 signed
#define LPP_ANALOG_OUTPUT 3         // 2 bytes, 0.01 signed
#define LPP_LUMINOSITY 101          // 2 bytes, 1 lux unsigned
#define LPP_PRESENCE 102            // 1 byte, 1
#define LPP_TEMPERATURE 103         // 2 bytes, 0.1°C signed
#define LPP_RELATIVE_HUMIDITY 104   // 1 byte, 0.5% unsigned
#define LPP_ACCELEROMETER 113       // 2 bytes per axis, 0.001G
#define LPP_BAROMETRIC_PRESSURE 115 // 2 bytes 0.1 hPa Unsigned
#define LPP_GYROMETER 134           // 2 bytes per axis, 0.01 °/s
#define LPP_GPS 136                 // 3 byte lon/lat 0.0001 °, 3 bytes alt 0.01 meter

// Data ID + Data Type + Data Size
#define LPP_DIGITAL_INPUT_SIZE 3       // 1 byte
#define LPP_DIGITAL_OUTPUT_SIZE 3      // 1 byte
#define LPP_ANALOG_INPUT_SIZE 4        // 2 bytes, 0.01 signed
#define LPP_ANALOG_OUTPUT_SIZE 4       // 2 bytes, 0.01 signed
#define LPP_LUMINOSITY_SIZE 4          // 2 bytes, 1 lux unsigned
#define LPP_PRESENCE_SIZE 3            // 1 byte, 1
#define LPP_TEMPERATURE_SIZE 4         // 2 bytes, 0.1°C signed
#define LPP_RELATIVE_HUMIDITY_SIZE 3   // 1 byte, 0.5% unsigned
#define LPP_ACCELEROMETER_SIZE 8       // 2 bytes per axis, 0.001G
#define LPP_BAROMETRIC_PRESSURE_SIZE 4 // 2 bytes 0.1 hPa Unsigned
#define LPP_GYROMETER_SIZE 8           // 2 bytes per axis, 0.01 °/s
#define LPP_GPS_SIZE 11                // 3 byte lon/lat 0.0001 °, 3 bytes alt 0.01 meter


#define LPP_MAX_BUFFER_SIZE 	128

void cayenneLPPreset(void);
uint8_t cayenneLPPgetSize(void);
uint8_t *cayenneLPPgetBuffer(void);
uint8_t cayenneLPPcopy(uint8_t *buffer);

uint8_t cayenneLPPaddDigitalInput(uint8_t channel, uint8_t value);
uint8_t cayenneLPPaddDigitalOutput(uint8_t channel, uint8_t value);

uint8_t cayenneLPPaddAnalogInput(uint8_t channel, float value);
uint8_t cayenneLPPaddAnalogOutput(uint8_t channel, float value);

uint8_t cayenneLPPaddLuminosity(uint8_t channel, uint16_t lux);
uint8_t cayenneLPPaddPresence(uint8_t channel, uint8_t value);
uint8_t cayenneLPPaddTemperature(uint8_t channel, float celsius);
uint8_t cayenneLPPaddRelativeHumidity(uint8_t channel, float rh);
uint8_t cayenneLPPaddAccelerometer(uint8_t channel, float x, float y, float z);
uint8_t cayenneLPPaddBarometricPressure(uint8_t channel, float hpa);
uint8_t cayenneLPPaddGyrometer(uint8_t channel, float x, float y, float z);
uint8_t cayenneLPPaddGPS(uint8_t channel, float latitude, float longitude, float meters);

#endif
