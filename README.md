# Master Thesis - SmartCanton

This repository contain the Master Thesis relised by **Da Silva Andrade David** in collaboration with the DIRECTION GÉNÉRALE DES SYSTÈMES D'INFORMATION (**DGSI**).

The goal of this project is to devellop a device that can connect to a LoRaWAN and provide sensor data that can be trusted from a specific device.

A securised interface was also devellopped to delivers the APPKEY to the new device created. The principle of creation and storage is based on the Proof of Concept devellopped by the DGSI for the project SmartCanton.

## Hardware

A PCB has been devellopped for this project. 

- Bluetooth LE 4.2 : **MKW41Z512**
- LoRa : **CMWX1ZZABZ-078** (SX1276 + STM32L082)
- GPS : **NEO-M8Q** (only with external antenna)
- On board antennas for the LoRa and Bluetooth. Possibility to connect external antennas in both cases.
- 9 Axis inertial platform : **BNO055**
- Gas, temperature and humidity sensor : **BME680**
- 8 LEDs and 4 Buttons accessible both by the LoRa and Bluetooth microcontrollers
- 2 PMOD connectors (1 for each microcontroller)
- Possibility to connect a Li-Ion battery
- USB or Solar charging for the Li-Ion battery



The Altium layout of the board is avaiblable under the directory `SmartCanton\hardware\SmartCantonDevBox_V1_0`.

A case for the hardware has also been designed. You can find it under`SmartCanton\hardware\SmartCantonDevBoxCase`. It can be easily printed using a 3D printer.



## Software

### LoRaWAN

The software for the CMWX1ZZABZ-078 can be found under the directory `SmartCanton\dev\STM32CubeExpansion_LRWAN_V1.1.2`. It requires to be open with the STM32 Workbench IDE.

### Bluetooth

The software for the NXP KW41Z can be found under the directory `SmartCanton\dev\MKW41z`. It requires to be open with the MCUXpresso IDE.

### Android

An Application developed with Android Studio is available under the directory `SmartCanton\dev\SmartCantonManager`.

### Web Server

A web server developed using Python 3.6 and Pycharm has been developed. It run coupled with a SQLite to store all the information about the users and the DevBox (LoRaWAN et BLE configurations).

The project can be opened with Pycharm under the directory `SmartCanton\dev\SmartCanton_AppKeyServer`.

## Documentation

The final report of the project can be found in the root tree of the project under the name `[Da_Silva_Andrade_TM_2017-2018.pdf](https://github.com/Rasnar/SmartCanton/blob/master/Da_Silva_Andrade_TM_2017-2018.pdf)`. It's written in French, with appendices in English. The REST API documentation, an user guide for the Android application and all the BLE services developed can be found as appendices in the PDF file.

All the documentation can be found under the `SmartCanton\documentation` directory.







