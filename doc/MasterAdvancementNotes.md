# Master Thesis : Note taking

Da Silva Andrade

# 13.07.2017

Les gens rencontrés : 

Hopkins Gavin (DSE) <gavin.hopkins@etat.ge.ch> (Ingénieur réseau)

- Si besoin d'accès spécifiques au réseau

Benammour Abdenbi (DSE) <abdenbi.benammour@etat.ge.ch>

- Questions liées à l'application finale

Feroul Maxime (DSE) <maxime.feroul@etat.ge.ch> (Dévelloppeur Kyos et état)

- Questions liées à LoRa et réseau. Si besoin de créer des objets virtuelles sur la plateforme FiWare



Notes prises durant l'entretient : 

- Penser au niveau de batterie
- Penser aux "Quality of data" du côté application finale
- L'échange de clés n'est pas encore fixé
- Taille des clés échangées de type AES-128
- Lorsque qu'un device effectue un join, deux clées sont déjà échangées




# 18.09.2017

### Keil license 

http://www2.keil.com/stmicroelectronics-stm32/mdk



### Bug debuggage EndNode st project : 

```
Error: jtag status contains invalid mode value - communication failure
Polling target stm32f1x.cpu failed, GDB will be halted. Polling again in 100ms
```



Solution :

http://www.jsykora.info/2014/10/error-jtag-status-contains-invalid-mode-value-communication-failure-solved/

Enable DBGMCU sleep, stop and standby. 



Pour ce faire : 

````
#define DEBUG 										1	
````

Dans le fichier debug.c



### Deuxième bug :  Hardfault 

C'est à cause d'une fonction qui n'est pas mappée lorsque l'on est en mode debug. Pour cela il faut mettre le flag 

````
#define LOW_POWER_DISABLE 							1
````



### Enable PRINTF functions in STM32 workbench

http://alphaloewe.com/2017/01/24/enable-semi-hosting-with-openstm32-system-workbench/



##### Install serial terminal inside eclipse

Help -> Eclipse Marketplace -> Search "terminal" 

Help -> Install new software -> "http://rxtx.qbang.org/eclipse"

CTRL-ALT-T to open terminal

To start new terminal : 

https://github.com/theolind/mahm3lib/wiki/Integrating-a-serial-output-window-with-Eclipse



# 19.09.2017

### Debug unconfirmed messages End Node

Perfectly working with confirmed messages but not with unconfirmed.

![bug_unconfirmed_ttn](C:\Users\David\Dropbox\SmartCanton\doc\bug_unconfirmed_ttn.PNG)

This image show a delay of almost 3 minutes beetween the frame 1 and 2.





### Install Zephyr dependencies

Windows step by step : https://www.zephyrproject.org/doc/getting_started/installation_win.html 

More precise step by step instructions : https://mcuoneclipse.com/2017/01/22/zephyr-thoughts-and-first-steps-on-the-arm-cortex-m4f-with-gcc-gdb-and-eclipse/#more-20446 



<u>**Note**</u> : with ubuntu bash... everything is easier...







# 20.09.2017



### Build Zephyr on Eclipse

https://github.com/crops/crops/wiki/Building-Zephyr-applications-with-Eclipse

https://github.com/crops/crops/wiki/how-to-use-plugin-on-windows







# 25.09.2017

http://www.contiki-os.org/download.html



http://www.freertos.org/FreeRTOS-Plus/Nabto/Nabto.shtml



# 07.10.2017

LoRa Geolocalisation : https://www.link-labs.com/blog/lora-localization



IEEE 802.15.4 Security :

https://people.eecs.berkeley.edu/~daw/papers/15.4-wise04.pdf

 http://www.libelium.com/security-802-15-4-zigbee/



BLE Security : https://eewiki.net/display/Wireless/A+Basic+Introduction+to+BLE+Security



# 11.10.2017

## Errors PCB

### LoRa

- Pin TCX0 not connected to VCC nor PA12.  

  - TODO : 

    for VDD_TCXO connection
    Option1: Connect VDD_TCXO to VDD
    Option2: Connect VDD_TCXO to PA12
    to make sure MCU can control TCXO on/off 

- VREF not connected to VCC3.3V

- VDD_USB should be connected to VCC3.3V not 5V

- RX and TX inversed....



### MCU 

(**Not critical**) Move reset button to the right prevent accidental press when plugging the JTAG

(**Swap buttons**) Reorder buttons to have the same organisation that the LEDs



### Power Supply

(**Not critical**) Move connection Solar panel and Battery to contain the connector inside the board

### GPS

Put and active circuit possible (cf. hardware integration PDF, page 15 and 16)

(**Not critical**) put GPS_nCS on a pin directly drived by the DSPI peripheral





# 21.10.2017

Found problem with frame missin. It's because of the module that support the ETSI duty cycle settng!

In file "command" of the project "AT_Slave" of the LRWAN card we can find this help command :

````
  {
    .string = AT_DCS,
    .size_string = sizeof(AT_DCS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_DCS ": Get or Set the ETSI Duty Cycle setting - 0=disable, 1=enable - Only for testing\r\n",
#endif
    .get = at_DutyCycle_get,
    .set = at_DutyCycle_set,
    .run = at_return_error,
  },
````



### Maximum Duty Cycle

The duty cycle of radio devices is often regulated by government. If this is the case, the duty cycle is commonly set to 1%, but make sure to check the regulations of your local government to be sure.

In Europe, duty cycles are regulated by section 7.2.3 of the ETSI EN300.220 standard. This standard defines the following sub-bands and their duty cycles:

- **g** (863.0 – 868.0 MHz): 1%
- **g1** (868.0 – 868.6 MHz): 1%
- **g2** (868.7 – 869.2 MHz): 0.1%
- **g3** (869.4 – 869.65 MHz): 10%
- **g4** (869.7 – 870.0 MHz): 1%

Additionally, the LoRaWAN specification dictates duty cycles for the *join frequencies*, the frequencies devices of all LoRaWAN-compliant networks use for over-the-air activations (OTAA) of devices. In most regions this duty cycle is set to **1%**.

Finally, on The Things Network's public community network, we have a **Fair Access Policy** that limits the **uplink airtime** to **30 seconds per day (24 hours) per node** and the **downlink messages** to **10 messages per day (24 hours) per node**. If you use a private network, these limits do not apply, but you still have to be compliant with the governmental and LoRaWAN limits.



https://www.thethingsnetwork.org/wiki/LoRaWAN/Duty-Cycle



# 25.10.2017

### New project with existing board folder

1. Delete existing `board` folder
2. File New -> Other -> Folder
3. Folder name : `board`
4. Advanced -> Link to alteranate location (Linked Folder) -> `C:\Users\David\Dropbox\SmartCanton\dev\MKW41z\smartcanton_devbox_board`
5. Right click on the `board`project -> Ressource Configurations --> Exclude from Build... 
6. Deselect Debug and Release from the menu 
7. Ready to compile




# 03.11.2017

Change C library :

http://www.support.code-red-tech.com/CodeRedWiki/SwitchingCLibrary

https://community.nxp.com/thread/389104



The easier method : Create a new project with the SDK and copy .cproject file to the old project





# 06.11.2017

## AT Commander modifications



The problem with the AT commander is that when you read a command it will stop when finding the first "\r\n" combinaison. When you use a get request, you will be confronted with 2 of theses. The get_request function had to be modified to accept this case.

For exemple, the AT+DEUI=? give us : 

````
11:22:33:44:55:66:77:88\r\n\r\nOK\r\n
````



From 

````c
int get_request(AtCommanderConfig* config, AtCommand* command,
        char* response_buffer, int response_buffer_length) {
    at_commander_write(config, command->request_format,
            strlen(command->request_format));
    at_commander_delay_ms(config, config->platform.response_delay_ms);

    int bytes_read = at_commander_read(config, response_buffer,
            response_buffer_length - 1, AT_COMMANDER_MAX_RETRIES);
    response_buffer[bytes_read] = '\0';

    if(strncmp(response_buffer, command->error_response, strlen(command->error_response))) {
        return bytes_read;
    }
    return -1;
}
````

To 

````c
int get_request(AtCommanderConfig* config, AtCommand* command,
        char* response_buffer, int response_buffer_length) {
    at_commander_write(config, command->request_format,
            strlen(command->request_format));
    at_commander_delay_ms(config, config->platform.response_delay_ms);

    int bytes_read = at_commander_read(config, response_buffer,
            response_buffer_length - 1, AT_COMMANDER_MAX_RETRIES);
    response_buffer[bytes_read] = '\0';

    char status_cmd[16];
    int status_bytes_read = at_commander_read(config, status_cmd,
                sizeof(status_cmd) - 1, AT_COMMANDER_MAX_RETRIES);
    status_cmd[status_bytes_read] = '\0';

    if(strncmp(status_cmd, command->error_response, strlen(command->error_response))) {
        return bytes_read;
    }
    return -1;
}
````



Another modification is inside "at_commander_read" function. Before this modification it refused to accpet only one character response!

Old : 

````c
int at_commander_read(AtCommanderConfig* config, char* buffer, int size,
	...
        if(bytes_read > 1) {
            if(byte == '\r') {
                sawCarraigeReturn = true;
            } else if(sawCarraigeReturn && byte == '\n') {
                break;
            }
        }
    }
    ...
    return bytes_read;
}
````

New:

```c
int at_commander_read(AtCommanderConfig* config, char* buffer, int size,
	...
        if(bytes_read > 0) {
            if(byte == '\r') {
                sawCarraigeReturn = true;
            } else if(sawCarraigeReturn && byte == '\n') {
                break;
            }
        }
    }
    ...
    return bytes_read;
}
```



## Using the Bluetooth Developer Studio and the Kinetis BLE Stack

All procedure : https://community.nxp.com/docs/DOC-333103



Download plugin (end of the page) : https://www.bluetooth.com/develop-with-bluetooth/developer-resources-tools/developer-kits/bluetooth-developer-plugins



The installation intructions can be found inside the plugin folder:

````
Installing
------------------------------------

To install the NXP Semiconductors Kinetis Plug-in, copy the "Plugins\NXP Kinetis v1.0.0" folder and its contents inside the "<BDS installation folder>\Plugins\" folder.

To install the Kinetis SDK 2.0 add-on for BDS, append the contents of the "Kinetis SDK 2.0 add-on" folder inside the Kinetis SDK 2.0 installation folder.

Testing 
------------------------------------

Open any Bluetooth Developer Studio projects and click on "Tools->Generate Code". Select the NXP Kinetis v1.0.0 item in the list and press the "Generate" button. Verify is there are warnings or errors in the output log. If the geration succeded, check the "Open output location when finished" option and click on "Finish".

The plug-in output folder will contain the following files:

 - app.c : contains application code and service intializations.
 - app.h : contains application configuration macros.
 - app_config.c: contains configuration structures for advertising, pairing or security requirements.
 - gatt_db.h : contains the mark-up of the GATT Database.
 - gatt_uuid128.h (optional): contains custom 128-bit UUIDs created in BDS.
 
Copy the contents of the folder inside "<SDK 2.0 installation folder>\middleware\wireless\bluetooth_1.2.2\examples\bds_template_app"

To generate the embedded project and test it, follow the instructions detailed in the Bluetooth Quick Start Guide document from the SDK.
````

### 1) Copy plugin

Copy `nxp_bds_plugin_1.0.0_KSDK_2.0\Plugins\NXP Semiconductors Kinetis v1.0.0` to `C:\Program Files (x86)\Bluetooth SIG\Bluetooth Developer Studio\Plugins` .



### 2) Create new project inside Bluetooth Developer Studio

Follow instructions explained at https://community.nxp.com/docs/DOC-333103

### 3) Results

It seems we can't directly use the files generated immediatly. But we can use the project to help to build the files. (milny the gatt_db.h)



### What to change to use the code generated by Bluetooth Studio

Using the heart rate service : 

#### On the GenericAccessProfile (file gatt_db.h)

You need to comment these 4 lines because it's not supported by the version.

````c
PRIMARY_SERVICE(service_generic_access, gBleSig_GenericAccessProfile_d)
	CHARACTERISTIC(char_device_name, gBleSig_GapDeviceName_d, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_VARLEN(value_device_name, gBleSig_GapDeviceName_d, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 23, 17, "SmartCantonDevBox")
	CHARACTERISTIC(char_appearance, gBleSig_GapAppearance_d, (gGattCharPropRead_c))
		VALUE(value_appearance, gBleSig_GapAppearance_d, (gPermissionFlagReadable_c), 2, UuidArray(gUnknown_c))
//	CHARACTERISTIC(char_peripheral_privacy_flag, gBleSig_GapPeripheralPrivacyFlag_d, (gGattCharPropRead_c))
//		VALUE(value_peripheral_privacy_flag, gBleSig_GapPeripheralPrivacyFlag_d, (gPermissionFlagReadable_c), 1, 0x00)
//	CHARACTERISTIC(char_reconnection_address, gBleSig_GapReconnectionAddress_d, (gGattCharPropWrite_c))
//		VALUE(value_reconnection_address, gBleSig_GapReconnectionAddress_d, (gPermissionFlagWritable_c), 6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
	CHARACTERISTIC(char_peripheral_preferred_connection_parameters, gBleSig_GapPpcp_d, (gGattCharPropRead_c))
		VALUE(value_peripheral_preferred_connection_parameters, gBleSig_GapPpcp_d, (gPermissionFlagReadable_c), 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
````

#### On the service heart rate  (file gatt_db.h)

You need to change the variable name `value_heart_rate_ctrl_point` to `value_hr_ctrl_point`. The parameter to the caracteristics need to be `22, 2, 0x00, 0xB4)`, without it it can't be read correctly.

	PRIMARY_SERVICE(service_heart_rate, gBleSig_HeartRateService_d)
	  CHARACTERISTIC(char_hr_measurement, gBleSig_HrMeasurement_d, (gGattCharPropNotify_c))
	      VALUE_VARLEN(value_hr_measurement, gBleSig_HrMeasurement_d, (gPermissionNone_c), 22, 2, 0x00, 0xB4)
	      CCCD(cccd_hr_measurement)
	  CHARACTERISTIC(char_body_sensor_location, gBleSig_BodySensorLocation_d, (gGattCharPropRead_c))
	      VALUE(value_body_sensor_location, gBleSig_BodySensorLocation_d, (gPermissionFlagReadable_c), 1, 0x00)
	  CHARACTERISTIC(char_hr_ctrl_point, gBleSig_HrControlPoint_d, (gGattCharPropWrite_c))
	      VALUE(value_hr_ctrl_point, gBleSig_HrControlPoint_d, (gPermissionFlagWritable_c), 1, 0x00)


#### On the app configuration (file app_config.h)

The LTK, RAND, IRK and CSRK structure can't be constantes. Remove the "const" parameter.

````
/* LTK */
static uint8_t smpLtk[gcSmpMaxLtkSize_c] =
    {0xD6, 0x93, 0xE8, 0xA4, 0x23, 0x55, 0x48, 0x99,
     0x1D, 0x77, 0x61, 0xE6, 0x63, 0x2B, 0x10, 0x8E};

/* RAND*/
static uint8_t smpRand[gcSmpMaxRandSize_c] =
    {0x26, 0x1E, 0xF6, 0x09, 0x97, 0x2E, 0xAD, 0x7E};

/* IRK */
static uint8_t smpIrk[gcSmpIrkSize_c] =
    {0x0A, 0x2D, 0xF4, 0x65, 0xE3, 0xBD, 0x7B, 0x49,
     0x1E, 0xB4, 0xC0, 0x95, 0x95, 0x13, 0x46, 0x73};

/* CSRK */
static uint8_t smpCsrk[gcSmpCsrkSize_c] =
    {0x90, 0xD5, 0x06, 0x95, 0x92, 0xED, 0x91, 0xD7,
     0xA8, 0x9E, 0x2C, 0xDC, 0x4A, 0x93, 0x5B, 0xF9};
````



Remove `#include "app.h"` that is not used inside the file.



# 08.11.2017

How to add your custom service to 

1. Delete existing `board` folder
2. File New -> Other -> Folder inside the directory `bluetooth/profiles/`
3. Folder name : `smartcanton_devbox`
4. Advanced -> Link to alteranate location (Linked Folder) -> `C:\Users\David\Dropbox\SmartCanton\dev\MKW41z\smartcanton_bluetooth_profiles`
5. Right click on the `board`project -> Proprieties --> C/C++ Build --> Settings --> Includes --> Add... --> `${workspace_loc:/${ProjName}/bluetooth/profiles/smartcanton_devbox}`
6. C/C++ General--> Paths and Symbols --> Includes --> GNU C --> (is a workspace path) --> `/${ProjName}/bluetooth/profiles/smartcanton_devbox`
7. Ready to compile



# 27.11.2017

## Notifications vs Indiciations

When sending indications on a GATT server we need to wait to have responses from the device. The response if receive on the ***BleApp_GattServerCallback*** function. The event type is called ***gEvtHandleValueConfirmation_c***. 

It means that if we want to send multiples indications we need to wait to have a confirmation from the last indications to send the next one!!!

### Sending notitications  (after writte attribute in db)

````
uint16_t  hCccd;
bool_t isNotifActive;

/* Get handle of CCCD */
if (GattDb_FindCccdHandleForCharValueHandle(handle, &hCccd) != gBleSuccess_c)
	return;

if (gBleSuccess_c == Gap_CheckIndicationStatus(mScDbGPS_SubscribedClientId, hCccd, &isNotifActive) &&
TRUE == isNotifActive) {
	GattServer_SendIndication(mScDbGPS_SubscribedClientId, handle);
}
````

### Sending indications (after writte attribute in db)

````
uint16_t cccdHandle;
bool_t isNotifActive;

/* Get handle of the handle CCCD */
if (GattDb_FindCccdHandleForCharValueHandle(*charHandle, &cccdHandle) != gBleSuccess_c)
	return;

if (mScDbGPS_SubscribedClientId == gInvalidDeviceId_c)
return;

if (gBleSuccess_c == Gap_CheckNotificationStatus(mScDbGPS_SubscribedClientId, cccdHandle, &isNotifActive) && TRUE == isNotifActive) {
	GattServer_SendNotification(mScDbGPS_SubscribedClientId, *charHandle);
}
````

### Sending notifications without saving value inside GATT database before

Data can be sent without been saved inside the GATT database. It means that if we want to access the data after it has been send (eg, with a new connection) it's not possible.

To do this, the only difference is using ***GattServer_SendInstantValueNotification*** instead of ***GattServer_SendNotification***. 



## BME680 driver

Bosch provide a driver that can be used to read data from the BME680 :

https://github.com/BoschSensortec/BME680_driver



## BME680 BSEC

Bosch provide a software API to be able to calculate an index for the air quality :

https://www.bosch-sensortec.com/bst/products/all_products/bme680



board/BME680_BSEC



`project -> Properties -> C/C++ Build -> MCU Linker -> Libraries`

Libraries : 

`algobsec`

Library search path :

`"${workspace_loc:/${ProjName}/board/BME680_BSEC}"`

The algo library also needs to have the MATH library linked (`-lm`).

To link it you can add it as follow : 

`project -> Properties -> C/C++ Build -> MCU Linker -> Libraries`

Libraries : 

`m`

The m need to be put **AFTER** the ***algobsec***.

## BNO055 driver

Bosch provide a driver that can be used to read data from the BME680 :

https://github.com/BoschSensortec/BNO055_driver







# 04.12.2017

Error with the following stack trace :

````
Thread #1 57005 (Suspended : Signal : SIGTRAP:Trace/breakpoint trap)	
	HardFault_Handler() at startup_MKW41Z4.S:210 0x548	
	<signal handler called>() at 0xfffffffd	
	0x20017ff8	
	0x0	
````

And the other error is the following : 

````
Thread #1 57005 (Suspended : Signal : SIGTRAP:Trace/breakpoint trap)	
	HardFault_Handler() at startup_MKW41Z4.S:210 0x548	
	<signal handler called>() at 0xfffffff1	
	osObjectIsAllocated() at fsl_os_abstraction_free_rtos.c:1 090 0x2d0cc	
	OSA_EventSet() at fsl_os_abstraction_free_rtos.c:602 0x2ce6e	
	llh_isr_handler() at 0xa2fa	
	Controller_InterruptHandler() at 0x7eb4	
	<signal handler called>() at 0xfffffffd	
	0x20017ff8	
	0x0	
````

The error isn't always present. But it appears some times. To be sure to reproduce this error it's possible to reduce these 2 parameters : 

````
/* Defines number of timers needed by the application */
#define gTmrApplicationTimers_c         2

/* Defines number of timers needed by the protocol stack */
#define gTmrStackTimers_c               2
````

By default, the **gTmrApplicationTimers_c** had 4 timers and **gTmrStackTimers_c** 5. To prevent any errors we set it to 10 and 10.



Another thing that block the code execution is the number of event, mutex and semaphores. By default we can see these values : 

````
#define osNumberOfEvents        7
#define osNumberOfSemaphores 	5
#define osNumberOfMutexes    	5
````

To have a greater margin : 

````
#define osNumberOfEvents        20
#define osNumberOfSemaphores 	20
#define osNumberOfMutexes    	20
````



## Interrupt Priority Cortex M in FreeRTOS

https://www.freertos.org/RTOS-Cortex-M3-M4.html

https://www.freertos.org/a00110.html#kernel_priority



## Time occupied by each task 

It's possible to ask Freertos to log the time passed in each task. In the file FreeRTOSConfig.h : 

````
#define configGENERATE_RUN_TIME_STATS           1
...
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() OSA_TimeGetMsec()
#define portGET_RUN_TIME_COUNTER_VALUE() OSA_TimeGetMsec()
````

More infos : https://www.freertos.org/rtos-run-time-stats.html



## Micro Trace Buffer 

# Master Thesis : Note taking

Da Silva Andrade

# 13.07.2017

Les gens rencontrés : 

Hopkins Gavin (DSE) <gavin.hopkins@etat.ge.ch> (Ingénieur réseau)

- Si besoin d'accès spécifiques au réseau

Benammour Abdenbi (DSE) <abdenbi.benammour@etat.ge.ch>

- Questions liées à l'application finale

Feroul Maxime (DSE) <maxime.feroul@etat.ge.ch> (Dévelloppeur Kyos et état)

- Questions liées à LoRa et réseau. Si besoin de créer des objets virtuelles sur la plateforme FiWare

Notes prises durant l'entretient : 

- Penser au niveau de batterie
- Penser aux "Quality of data" du côté application finale
- L'échange de clés n'est pas encore fixé
- Taille des clés échangées de type AES-128
- Lorsque qu'un device effectue un join, deux clées sont déjà échangées



# 18.09.2017

### Keil license

http://www2.keil.com/stmicroelectronics-stm32/mdk



### Bug debuggage EndNode st project :

```
Error: jtag status contains invalid mode value - communication failure
Polling target stm32f1x.cpu failed, GDB will be halted. Polling again in 100ms
```



Solution :

http://www.jsykora.info/2014/10/error-jtag-status-contains-invalid-mode-value-communication-failure-solved/

Enable DBGMCU sleep, stop and standby. 



Pour ce faire : 

```
#define DEBUG 										1	
```

Dans le fichier debug.c



### Deuxième bug :  Hardfault

C'est à cause d'une fonction qui n'est pas mappée lorsque l'on est en mode debug. Pour cela il faut mettre le flag 

```
#define LOW_POWER_DISABLE 							1
```



### Enable PRINTF functions in STM32 workbench

http://alphaloewe.com/2017/01/24/enable-semi-hosting-with-openstm32-system-workbench/



##### Install serial terminal inside eclipse

Help -> Eclipse Marketplace -> Search "terminal" 

Help -> Install new software -> "http://rxtx.qbang.org/eclipse"

CTRL-ALT-T to open terminal

To start new terminal : 

https://github.com/theolind/mahm3lib/wiki/Integrating-a-serial-output-window-with-Eclipse



# 19.09.2017

### Debug unconfirmed messages End Node

Perfectly working with confirmed messages but not with unconfirmed.

![bug_unconfirmed_ttn](C:\Users\David\Dropbox\SmartCanton\doc\bug_unconfirmed_ttn.PNG)

This image show a delay of almost 3 minutes beetween the frame 1 and 2.





### Install Zephyr dependencies

Windows step by step : https://www.zephyrproject.org/doc/getting_started/installation_win.html 

More precise step by step instructions : https://mcuoneclipse.com/2017/01/22/zephyr-thoughts-and-first-steps-on-the-arm-cortex-m4f-with-gcc-gdb-and-eclipse/#more-20446 



<u>**Note**</u> : with ubuntu bash... everything is easier...







# 20.09.2017



### Build Zephyr on Eclipse

https://github.com/crops/crops/wiki/Building-Zephyr-applications-with-Eclipse

https://github.com/crops/crops/wiki/how-to-use-plugin-on-windows







# 25.09.2017

http://www.contiki-os.org/download.html



http://www.freertos.org/FreeRTOS-Plus/Nabto/Nabto.shtml



# 07.10.2017

LoRa Geolocalisation : https://www.link-labs.com/blog/lora-localization



IEEE 802.15.4 Security :

https://people.eecs.berkeley.edu/~daw/papers/15.4-wise04.pdf

 http://www.libelium.com/security-802-15-4-zigbee/



BLE Security : https://eewiki.net/display/Wireless/A+Basic+Introduction+to+BLE+Security



# 11.10.2017

## Errors PCB

### LoRa

- Pin TCX0 not connected to VCC nor PA12.  

  - TODO : 

    for VDD_TCXO connection
    Option1: Connect VDD_TCXO to VDD
    Option2: Connect VDD_TCXO to PA12
    to make sure MCU can control TCXO on/off 

- VREF not connected to VCC3.3V

- VDD_USB should be connected to VCC3.3V not 5V

- RX and TX inversed....

### MCU

(**Not critical**) Move reset button to the right prevent accidental press when plugging the JTAG

(**Swap buttons**) Reorder buttons to have the same organisation that the LEDs



### Power Supply

(**Not critical**) Move connection Solar panel and Battery to contain the connector inside the board

### GPS

Put and active circuit possible (cf. hardware integration PDF, page 15 and 16)

(**Not critical**) put GPS_nCS on a pin directly drived by the DSPI peripheral





# 21.10.2017

Found problem with frame missin. It's because of the module that support the ETSI duty cycle settng!

In file "command" of the project "AT_Slave" of the LRWAN card we can find this help command :

```
  {
    .string = AT_DCS,
    .size_string = sizeof(AT_DCS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_DCS ": Get or Set the ETSI Duty Cycle setting - 0=disable, 1=enable - Only for testing\r\n",
#endif
    .get = at_DutyCycle_get,
    .set = at_DutyCycle_set,
    .run = at_return_error,
  },
```



### Maximum Duty Cycle

The duty cycle of radio devices is often regulated by government. If this is the case, the duty cycle is commonly set to 1%, but make sure to check the regulations of your local government to be sure.

In Europe, duty cycles are regulated by section 7.2.3 of the ETSI EN300.220 standard. This standard defines the following sub-bands and their duty cycles:

- **g** (863.0 – 868.0 MHz): 1%
- **g1** (868.0 – 868.6 MHz): 1%
- **g2** (868.7 – 869.2 MHz): 0.1%
- **g3** (869.4 – 869.65 MHz): 10%
- **g4** (869.7 – 870.0 MHz): 1%

Additionally, the LoRaWAN specification dictates duty cycles for the *join frequencies*, the frequencies devices of all LoRaWAN-compliant networks use for over-the-air activations (OTAA) of devices. In most regions this duty cycle is set to **1%**.

Finally, on The Things Network's public community network, we have a **Fair Access Policy** that limits the **uplink airtime** to **30 seconds per day (24 hours) per node** and the **downlink messages** to **10 messages per day (24 hours) per node**. If you use a private network, these limits do not apply, but you still have to be compliant with the governmental and LoRaWAN limits.



https://www.thethingsnetwork.org/wiki/LoRaWAN/Duty-Cycle



# 25.10.2017

### New project with existing board folder

1. Delete existing `board` folder
2. File New -> Other -> Folder
3. Folder name : `board`
4. Advanced -> Link to alteranate location (Linked Folder) -> `C:\Users\David\Dropbox\SmartCanton\dev\MKW41z\smartcanton_devbox_board`
5. Right click on the `board`project -> Ressource Configurations --> Exclude from Build... 
6. Deselect Debug and Release from the menu 
7. Ready to compile



# 03.11.2017

Change C library :

http://www.support.code-red-tech.com/CodeRedWiki/SwitchingCLibrary

https://community.nxp.com/thread/389104



The easier method : Create a new project with the SDK and copy .cproject file to the old project





# 06.11.2017

## AT Commander modifications



The problem with the AT commander is that when you read a command it will stop when finding the first "\r\n" combinaison. When you use a get request, you will be confronted with 2 of theses. The get_request function had to be modified to accept this case.

For exemple, the AT+DEUI=? give us : 

```
11:22:33:44:55:66:77:88\r\n\r\nOK\r\n
```



From 

```c
int get_request(AtCommanderConfig* config, AtCommand* command,
        char* response_buffer, int response_buffer_length) {
    at_commander_write(config, command->request_format,
            strlen(command->request_format));
    at_commander_delay_ms(config, config->platform.response_delay_ms);

    int bytes_read = at_commander_read(config, response_buffer,
            response_buffer_length - 1, AT_COMMANDER_MAX_RETRIES);
    response_buffer[bytes_read] = '\0';

    if(strncmp(response_buffer, command->error_response, strlen(command->error_response))) {
        return bytes_read;
    }
    return -1;
}
```

To 

```c
int get_request(AtCommanderConfig* config, AtCommand* command,
        char* response_buffer, int response_buffer_length) {
    at_commander_write(config, command->request_format,
            strlen(command->request_format));
    at_commander_delay_ms(config, config->platform.response_delay_ms);

    int bytes_read = at_commander_read(config, response_buffer,
            response_buffer_length - 1, AT_COMMANDER_MAX_RETRIES);
    response_buffer[bytes_read] = '\0';

    char status_cmd[16];
    int status_bytes_read = at_commander_read(config, status_cmd,
                sizeof(status_cmd) - 1, AT_COMMANDER_MAX_RETRIES);
    status_cmd[status_bytes_read] = '\0';

    if(strncmp(status_cmd, command->error_response, strlen(command->error_response))) {
        return bytes_read;
    }
    return -1;
}
```



Another modification is inside "at_commander_read" function. Before this modification it refused to accpet only one character response!

Old : 

```c
int at_commander_read(AtCommanderConfig* config, char* buffer, int size,
	...
        if(bytes_read > 1) {
            if(byte == '\r') {
                sawCarraigeReturn = true;
            } else if(sawCarraigeReturn && byte == '\n') {
                break;
            }
        }
    }
    ...
    return bytes_read;
}
```

New:

```c
int at_commander_read(AtCommanderConfig* config, char* buffer, int size,
	...
        if(bytes_read > 0) {
            if(byte == '\r') {
                sawCarraigeReturn = true;
            } else if(sawCarraigeReturn && byte == '\n') {
                break;
            }
        }
    }
    ...
    return bytes_read;
}
```



## Using the Bluetooth Developer Studio and the Kinetis BLE Stack

All procedure : https://community.nxp.com/docs/DOC-333103



Download plugin (end of the page) : https://www.bluetooth.com/develop-with-bluetooth/developer-resources-tools/developer-kits/bluetooth-developer-plugins



The installation intructions can be found inside the plugin folder:

```
Installing
------------------------------------

To install the NXP Semiconductors Kinetis Plug-in, copy the "Plugins\NXP Kinetis v1.0.0" folder and its contents inside the "<BDS installation folder>\Plugins\" folder.

To install the Kinetis SDK 2.0 add-on for BDS, append the contents of the "Kinetis SDK 2.0 add-on" folder inside the Kinetis SDK 2.0 installation folder.

Testing 
------------------------------------

Open any Bluetooth Developer Studio projects and click on "Tools->Generate Code". Select the NXP Kinetis v1.0.0 item in the list and press the "Generate" button. Verify is there are warnings or errors in the output log. If the geration succeded, check the "Open output location when finished" option and click on "Finish".

The plug-in output folder will contain the following files:

 - app.c : contains application code and service intializations.
 - app.h : contains application configuration macros.
 - app_config.c: contains configuration structures for advertising, pairing or security requirements.
 - gatt_db.h : contains the mark-up of the GATT Database.
 - gatt_uuid128.h (optional): contains custom 128-bit UUIDs created in BDS.
 
Copy the contents of the folder inside "<SDK 2.0 installation folder>\middleware\wireless\bluetooth_1.2.2\examples\bds_template_app"

To generate the embedded project and test it, follow the instructions detailed in the Bluetooth Quick Start Guide document from the SDK.
```

### 1) Copy plugin

Copy `nxp_bds_plugin_1.0.0_KSDK_2.0\Plugins\NXP Semiconductors Kinetis v1.0.0` to `C:\Program Files (x86)\Bluetooth SIG\Bluetooth Developer Studio\Plugins` .



### 2) Create new project inside Bluetooth Developer Studio

Follow instructions explained at https://community.nxp.com/docs/DOC-333103

### 3) Results

It seems we can't directly use the files generated immediatly. But we can use the project to help to build the files. (milny the gatt_db.h)



### What to change to use the code generated by Bluetooth Studio

Using the heart rate service : 

#### On the GenericAccessProfile (file gatt_db.h)

You need to comment these 4 lines because it's not supported by the version.

```c
PRIMARY_SERVICE(service_generic_access, gBleSig_GenericAccessProfile_d)
	CHARACTERISTIC(char_device_name, gBleSig_GapDeviceName_d, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_VARLEN(value_device_name, gBleSig_GapDeviceName_d, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 23, 17, "SmartCantonDevBox")
	CHARACTERISTIC(char_appearance, gBleSig_GapAppearance_d, (gGattCharPropRead_c))
		VALUE(value_appearance, gBleSig_GapAppearance_d, (gPermissionFlagReadable_c), 2, UuidArray(gUnknown_c))
//	CHARACTERISTIC(char_peripheral_privacy_flag, gBleSig_GapPeripheralPrivacyFlag_d, (gGattCharPropRead_c))
//		VALUE(value_peripheral_privacy_flag, gBleSig_GapPeripheralPrivacyFlag_d, (gPermissionFlagReadable_c), 1, 0x00)
//	CHARACTERISTIC(char_reconnection_address, gBleSig_GapReconnectionAddress_d, (gGattCharPropWrite_c))
//		VALUE(value_reconnection_address, gBleSig_GapReconnectionAddress_d, (gPermissionFlagWritable_c), 6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
	CHARACTERISTIC(char_peripheral_preferred_connection_parameters, gBleSig_GapPpcp_d, (gGattCharPropRead_c))
		VALUE(value_peripheral_preferred_connection_parameters, gBleSig_GapPpcp_d, (gPermissionFlagReadable_c), 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
```

#### On the service heart rate  (file gatt_db.h)

You need to change the variable name `value_heart_rate_ctrl_point` to `value_hr_ctrl_point`. The parameter to the caracteristics need to be `22, 2, 0x00, 0xB4)`, without it it can't be read correctly.

```
PRIMARY_SERVICE(service_heart_rate, gBleSig_HeartRateService_d)
  CHARACTERISTIC(char_hr_measurement, gBleSig_HrMeasurement_d, (gGattCharPropNotify_c))
      VALUE_VARLEN(value_hr_measurement, gBleSig_HrMeasurement_d, (gPermissionNone_c), 22, 2, 0x00, 0xB4)
      CCCD(cccd_hr_measurement)
  CHARACTERISTIC(char_body_sensor_location, gBleSig_BodySensorLocation_d, (gGattCharPropRead_c))
      VALUE(value_body_sensor_location, gBleSig_BodySensorLocation_d, (gPermissionFlagReadable_c), 1, 0x00)
  CHARACTERISTIC(char_hr_ctrl_point, gBleSig_HrControlPoint_d, (gGattCharPropWrite_c))
      VALUE(value_hr_ctrl_point, gBleSig_HrControlPoint_d, (gPermissionFlagWritable_c), 1, 0x00)
```

#### On the app configuration (file app_config.h)

The LTK, RAND, IRK and CSRK structure can't be constantes. Remove the "const" parameter.

```
/* LTK */
static uint8_t smpLtk[gcSmpMaxLtkSize_c] =
    {0xD6, 0x93, 0xE8, 0xA4, 0x23, 0x55, 0x48, 0x99,
     0x1D, 0x77, 0x61, 0xE6, 0x63, 0x2B, 0x10, 0x8E};

/* RAND*/
static uint8_t smpRand[gcSmpMaxRandSize_c] =
    {0x26, 0x1E, 0xF6, 0x09, 0x97, 0x2E, 0xAD, 0x7E};

/* IRK */
static uint8_t smpIrk[gcSmpIrkSize_c] =
    {0x0A, 0x2D, 0xF4, 0x65, 0xE3, 0xBD, 0x7B, 0x49,
     0x1E, 0xB4, 0xC0, 0x95, 0x95, 0x13, 0x46, 0x73};

/* CSRK */
static uint8_t smpCsrk[gcSmpCsrkSize_c] =
    {0x90, 0xD5, 0x06, 0x95, 0x92, 0xED, 0x91, 0xD7,
     0xA8, 0x9E, 0x2C, 0xDC, 0x4A, 0x93, 0x5B, 0xF9};
```



Remove `#include "app.h"` that is not used inside the file.



# 08.11.2017

How to add your custom service to 

1. Delete existing `board` folder
2. File New -> Other -> Folder inside the directory `bluetooth/profiles/`
3. Folder name : `smartcanton_devbox`
4. Advanced -> Link to alteranate location (Linked Folder) -> `C:\Users\David\Dropbox\SmartCanton\dev\MKW41z\smartcanton_bluetooth_profiles`
5. Right click on the `board`project -> Proprieties --> C/C++ Build --> Settings --> Includes --> Add... --> `${workspace_loc:/${ProjName}/bluetooth/profiles/smartcanton_devbox}`
6. C/C++ General--> Paths and Symbols --> Includes --> GNU C --> (is a workspace path) --> `/${ProjName}/bluetooth/profiles/smartcanton_devbox`
7. Ready to compile

# 27.11.2017

## Notifications vs Indiciations

When sending indications on a GATT server we need to wait to have responses from the device. The response if receive on the ***BleApp_GattServerCallback*** function. The event type is called ***gEvtHandleValueConfirmation_c***. 

It means that if we want to send multiples indications we need to wait to have a confirmation from the last indications to send the next one!!!

### Sending notitications  (after writte attribute in db)

```
uint16_t  hCccd;
bool_t isNotifActive;

/* Get handle of CCCD */
if (GattDb_FindCccdHandleForCharValueHandle(handle, &hCccd) != gBleSuccess_c)
	return;

if (gBleSuccess_c == Gap_CheckIndicationStatus(mScDbGPS_SubscribedClientId, hCccd, &isNotifActive) &&
TRUE == isNotifActive) {
	GattServer_SendIndication(mScDbGPS_SubscribedClientId, handle);
}
```

### Sending indications (after writte attribute in db)

```
uint16_t cccdHandle;
bool_t isNotifActive;

/* Get handle of the handle CCCD */
if (GattDb_FindCccdHandleForCharValueHandle(*charHandle, &cccdHandle) != gBleSuccess_c)
	return;

if (mScDbGPS_SubscribedClientId == gInvalidDeviceId_c)
return;

if (gBleSuccess_c == Gap_CheckNotificationStatus(mScDbGPS_SubscribedClientId, cccdHandle, &isNotifActive) && TRUE == isNotifActive) {
	GattServer_SendNotification(mScDbGPS_SubscribedClientId, *charHandle);
}
```

### Sending notifications without saving value inside GATT database before

Data can be sent without been saved inside the GATT database. It means that if we want to access the data after it has been send (eg, with a new connection) it's not possible.

To do this, the only difference is using ***GattServer_SendInstantValueNotification*** instead of ***GattServer_SendNotification***. 



## BME680 driver

Bosch provide a driver that can be used to read data from the BME680 :

https://github.com/BoschSensortec/BME680_driver



## BME680 BSEC

Bosch provide a software API to be able to calculate an index for the air quality :

https://www.bosch-sensortec.com/bst/products/all_products/bme680



board/BME680_BSEC



`project -> Properties -> C/C++ Build -> MCU Linker -> Libraries`

Libraries : 

`algobsec`

Library search path :

`"${workspace_loc:/${ProjName}/board/BME680_BSEC}"`

The algo library also needs to have the MATH library linked (`-lm`).

To link it you can add it as follow : 

`project -> Properties -> C/C++ Build -> MCU Linker -> Libraries`

Libraries : 

`m`

The m need to be put **AFTER** the ***algobsec***.

## BNO055 driver

Bosch provide a driver that can be used to read data from the BME680 :

https://github.com/BoschSensortec/BNO055_driver







# 04.12.2017

Error with the following stack trace :

```
Thread #1 57005 (Suspended : Signal : SIGTRAP:Trace/breakpoint trap)	
	HardFault_Handler() at startup_MKW41Z4.S:210 0x548	
	<signal handler called>() at 0xfffffffd	
	0x20017ff8	
	0x0	
```

And the other error is the following : 

```
Thread #1 57005 (Suspended : Signal : SIGTRAP:Trace/breakpoint trap)	
	HardFault_Handler() at startup_MKW41Z4.S:210 0x548	
	<signal handler called>() at 0xfffffff1	
	osObjectIsAllocated() at fsl_os_abstraction_free_rtos.c:1 090 0x2d0cc	
	OSA_EventSet() at fsl_os_abstraction_free_rtos.c:602 0x2ce6e	
	llh_isr_handler() at 0xa2fa	
	Controller_InterruptHandler() at 0x7eb4	
	<signal handler called>() at 0xfffffffd	
	0x20017ff8	
	0x0	
```

The error isn't always present. But it appears some times. To be sure to reproduce this error it's possible to reduce these 2 parameters : 

```
/* Defines number of timers needed by the application */
#define gTmrApplicationTimers_c         2

/* Defines number of timers needed by the protocol stack */
#define gTmrStackTimers_c               2
```

By default, the **gTmrApplicationTimers_c** had 4 timers and **gTmrStackTimers_c** 5. To prevent any errors we set it to 10 and 10.



Another thing that block the code execution is the number of event, mutex and semaphores. By default we can see these values : 

```
#define osNumberOfEvents        7
#define osNumberOfSemaphores 	5
#define osNumberOfMutexes    	5
```

To have a greater margin : 

```
#define osNumberOfEvents        20
#define osNumberOfSemaphores 	20
#define osNumberOfMutexes    	20
```



## Interrupt Priority Cortex M in FreeRTOS

https://www.freertos.org/RTOS-Cortex-M3-M4.html

https://www.freertos.org/a00110.html#kernel_priority



## Time occupied by each task 

It's possible to ask Freertos to log the time passed in each task. In the file FreeRTOSConfig.h : 

```
#define configGENERATE_RUN_TIME_STATS           1
...
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() OSA_TimeGetMsec()
#define portGET_RUN_TIME_COUNTER_VALUE() OSA_TimeGetMsec()
```

More infos : https://www.freertos.org/rtos-run-time-stats.html



## Micro Trace Buffer

https://www.nxp.com/docs/en/quick-reference-guide/MCUXpresso_IDE_Instruction_Trace.pdf







# 05.12.2017

## Rest API with json web token (jwt)

Tutorial source code (PY JWT) : 

https://prettyprinted.com/blog/966916/creating-restful-api-flask-json-web-token-auth

JWT advantages :

https://medium.com/vandium-software/5-easy-steps-to-understanding-json-web-tokens-jwt-1164c0adfcec

JWT Flask Methods documentation (**not using now !**) : 

https://pythonhosted.org/Flask-JWT/#flask_jwt.jwt_required 



Why use **JWT Flask Extended** and not **JWT Flask** 

https://github.com/mattupstate/flask-jwt/issues/99



**JWT Flask extended documentation** : 

http://flask-jwt-extended.readthedocs.io/en/latest/index.html



All dependences : 

````
pip install Flask
pip install SQLAlchemy
pip install flask-sqlalchemy
pip install PyJWT
ou
pip install flask-jwt-extended

````

To enable https with flask : https://blog.miguelgrinberg.com/post/running-your-flask-application-over-https



## Convert File PATH to file URI

````
import pathlib
pathlib.Path("E:\Dropbox\SmartCanton\dev\SmartCanton_AppKeyServer\todo.db").as_uri() 
````



## Flask server

### How to create first client login  

Modify the route post for `/user`  :

```python
@app.route('/user', methods=['POST'])
# @token_required
def create_user():
    # if not current_user.admin:
    #     return jsonify({'message': 'Cannot perform that function!'})

    data = request.get_json()

    hashed_password = generate_password_hash(data['password'], method='sha256')

    new_user = User(public_id=str(uuid.uuid4()), name=data['name'], password=hashed_password, admin=True)
    db.session.add(new_user)
    db.session.commit()

    return jsonify({'message': 'New user created!'})
```

The DataBase can be viewed inside Pycharm with `View -> Tools Windows -> Database `.



Post request : **JSON/Application**

With body : 

````
{"name" : "Admin", "password":"SmartCanton2017"}

````



### Authentication 

As **Admin** and password **SmartCanton2017**:

````bash
curl -X POST \
  http://127.0.0.1:5000/auth \
  -H 'cache-control: no-cache' \
  -H 'content-type: application/json' \
  -H 'postman-token: 78885420-8e4b-6e54-193b-b6a662d63ca3' \
  -d '{
    "username": "Admin",
    "password": "SmartCanton2017"
}'
````

Response : 

````json
{
    "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE1MTI1OTMwODgsImlhdCI6MTUxMjU5MjE4OCwibmJmIjoxNTEyNTkyMTg4LCJqdGkiOiIzN2M5ZmY0Zi03ZmNjLTQyZGMtODBiZi1mOWMyMGMyMGFmYmMiLCJpZGVudGl0eSI6ImRmNDVlOGUyLTZlMTAtNGViYS04YmNkLTIyMmNhY2UyOTc0ZSIsImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.BXKWcLrSYzFX65WJVkzv7mb8KIFvqyBGzgJw29LgVnY"
}
````

We can find the following data encoded in base64 in the 2 first data. The last Base64 value is the : 

````
{
  "typ": "JWT",
  "alg": "HS256"
}
{
  "exp": 1512593088,
  "iat": 1512592188,
  "nbf": 1512592188,
  "jti": "37c9ff4f-7fcc-42dc-80bf-f9c20c20afbc",
  "identity": "df45e8e2-6e10-4eba-8bcd-222cace2974e",
  "fresh": false,
  "type": "access"
}
````

Registered claimes (eg. iss, exp, etc...) are explained here : https://tools.ietf.org/html/rfc7519#section-4.1



### Get all users 

````bash
curl -X GET \
  http://127.0.0.1:5000/user \
  -H 'authorization: JWT eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE1MTUxNTEyMTIsImlhdCI6MTUxMjU1OTIxMiwibmJmIjoxNTEyNTU5MjEyLCJwdWJsaWNfaWQiOiJkZjQ1ZThlMi02ZTEwLTRlYmEtOGJjZC0yMjJjYWNlMjk3NGUifQ.qCvAHln-q2N-EEj4iZPmyrTw8lfM1cuTkw5hZuiqin0' \
  -H 'cache-control: no-cache' \
  -H 'postman-token: 1bff814e-840a-c5fe-0e7e-a1ced43fadab'
````

Response :

````json
{
    "users": [
        {
            "admin": true,
            "name": "Admin",
            "password": "sha256$UXfgsAhJ$20559d1cb177cf3236c7f63a44d95db4ac6d260d1bceeda5f266a0ebebfb1ac9",
            "public_id": "df45e8e2-6e10-4eba-8bcd-222cace2974e"
        },
        {
            "admin": false,
            "name": "david",
            "password": "sha256$Ft7ypK8U$8e39836d549222ffc51695a77db0069e748ecfd2740e02cc634e6b5116d878bf",
            "public_id": "93cb11d2-bbdd-4d2a-8e66-e970e6d6bd25"
        }
    ]
}
````

### Create new user 

````bash
curl -X POST \
  http://127.0.0.1:5000/user \
  -H 'authorization: JWT eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE1MTI1OTMwODgsImlhdCI6MTUxMjU5MjE4OCwibmJmIjoxNTEyNTkyMTg4LCJqdGkiOiIzN2M5ZmY0Zi03ZmNjLTQyZGMtODBiZi1mOWMyMGMyMGFmYmMiLCJpZGVudGl0eSI6ImRmNDVlOGUyLTZlMTAtNGViYS04YmNkLTIyMmNhY2UyOTc0ZSIsImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.BXKWcLrSYzFX65WJVkzv7mb8KIFvqyBGzgJw29LgVnY' \
  -H 'cache-control: no-cache' \
  -H 'content-type: application/json' \
  -H 'postman-token: 5f79895d-86f2-4e20-fbc5-3fd57dfbcc95' \
  -d '{"username": "david2", "password": "SmartCanton"}'
````

Response :

````json
{
    "message": "New user created!"
}
````

### Promote user

````bash
curl -X PUT \
  http://127.0.0.1:5000/user/02752fc1-00be-4986-b909-843b6a33aa0f \
  -H 'authorization: JWT eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE1MTI1OTMwODgsImlhdCI6MTUxMjU5MjE4OCwibmJmIjoxNTEyNTkyMTg4LCJqdGkiOiIzN2M5ZmY0Zi03ZmNjLTQyZGMtODBiZi1mOWMyMGMyMGFmYmMiLCJpZGVudGl0eSI6ImRmNDVlOGUyLTZlMTAtNGViYS04YmNkLTIyMmNhY2UyOTc0ZSIsImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.BXKWcLrSYzFX65WJVkzv7mb8KIFvqyBGzgJw29LgVnY' \
  -H 'cache-control: no-cache' \
  -H 'content-type: application/json' \
  -H 'postman-token: 32f51bab-66d3-8e13-cc5f-8db5d04dbef3'
````

Response :

```json
{
    "message": "The user has been promoted!"
}
```

### Delete user

```bash
curl -X DELETE \
  http://127.0.0.1:5000/user/02752fc1-00be-4986-b909-843b6a33aa0f \
  -H 'authorization: JWT eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE1MTI1OTMwODgsImlhdCI6MTUxMjU5MjE4OCwibmJmIjoxNTEyNTkyMTg4LCJqdGkiOiIzN2M5ZmY0Zi03ZmNjLTQyZGMtODBiZi1mOWMyMGMyMGFmYmMiLCJpZGVudGl0eSI6ImRmNDVlOGUyLTZlMTAtNGViYS04YmNkLTIyMmNhY2UyOTc0ZSIsImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.BXKWcLrSYzFX65WJVkzv7mb8KIFvqyBGzgJw29LgVnY' \
  -H 'cache-control: no-cache' \
  -H 'postman-token: 5891bb22-7306-babb-8b1e-ecec346db951'
```

Response :

```json
{
    "message": "The user has been deleted!"
}
```



### Access protected route 

````bash
curl -X GET \
  http://127.0.0.1:5000/todo \
  -H 'authorization: JWT eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE1MTI1OTMwODgsImlhdCI6MTUxMjU5MjE4OCwibmJmIjoxNTEyNTkyMTg4LCJqdGkiOiIzN2M5ZmY0Zi03ZmNjLTQyZGMtODBiZi1mOWMyMGMyMGFmYmMiLCJpZGVudGl0eSI6ImRmNDVlOGUyLTZlMTAtNGViYS04YmNkLTIyMmNhY2UyOTc0ZSIsImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.BXKWcLrSYzFX65WJVkzv7mb8KIFvqyBGzgJw29LgVnY' \
  -H 'cache-control: no-cache' \
  -H 'postman-token: a338e0e0-1ed0-46a0-4595-153671d57651'
````

With valid token : 

````bash
{
    "todos": []
}
````

With invalid token : 

````bash
{
    "description": "Request does not contain an access token",
    "error": "Authorization Required",
    "status_code": 401
}
````



# 06.12.2017

## Android JWT Client

https://medium.com/bcgdv-engineering/use-android-studio-to-stub-web-api-simple-jwt-login-as-an-example-615ee8efe1b0



https://github.com/segunfamisa/android-jwt-authentication

https://github.com/jwtk/jjwt



Base code used for the android app

https://www.learn2crack.com/2016/09/android-user-registration-login-node-server.html

JWT android library

https://github.com/auth0/JWTDecode.Android

## OkHttpClient with self signed certificate

http://www.snip2code.com/Snippet/25364/Get-OkHttpClient-which-ignores-all-SSL-e



# 08.12.2017



## KW41z Bluetooth security 

gap security : http://microchipdeveloper.com/wireless:ble-gap-security

How encryption work : http://www.fte.com/webhelp/sodera/Content/Documentation/WhitePapers/BTLE/HowEncryptionWorksInBluetoothLowEnergy.htm

Ble pairing vs bonding : https://piratecomm.wordpress.com/2014/01/19/ble-pairing-vs-bonding/

A basic introduction to BLE Security : https://eewiki.net/display/Wireless/A+Basic+Introduction+to+BLE+Security

Bluetooth series on pairing and security : 

http://blog.bluetooth.com/bluetooth-pairing-part-1-pairing-feature-exchange

http://blog.bluetooth.com/bluetooth-pairing-part-2-key-generation-methods

http://blog.bluetooth.com/bluetooth-pairing-passkey-entry

https://blog.bluetooth.com/bluetooth-pairing-part-4

### Pairing parameters

The Bluetooth configuration is defined in the file `app_config.c`. The security applied while pairing is device by the following parameters. The default values are the following : 

````
/* SMP Data */
gapPairingParameters_t gPairingParameters = {
    .withBonding = gAppUseBonding_d,
    .securityModeAndLevel = gSecurityMode_1_Level_1_c,
    .maxEncryptionKeySize = mcEncryptionKeySize_c,
    .localIoCapabilities = gIoDisplayOnly_c,
    .oobAvailable = FALSE,
    .centralKeys = gIrk_c,
    .peripheralKeys = (gapSmpKeyFlags_t)(gLtk_c | gIrk_c),
    .leSecureConnectionSupported = TRUE,
    .useKeypressNotifications = FALSE,
};
````



To change the security level we can modify 2 parameters mainly. The first is the `securityModeAndLevel` all options are the following : 

````

````

### Gap security

#### Parameters 

````
/*! LE Security Level */
typedef enum gapSecurityLevel_tag {
    gSecurityLevel_NoSecurity_c =           0x00,  /*!< No security (combined only with Mode 1). */
    gSecurityLevel_NoMitmProtection_c =     0x01,  /*!< Unauthenticated (no MITM protection). */
    gSecurityLevel_WithMitmProtection_c =   0x02,  /*!< Authenticated (MITM protection by PIN or OOB). */
    gSecurityLevel_LeSecureConnections_c =  0x03   /*!< Authenticated with LE Secure Connections (BLE 4.2 only). */
} gapSecurityLevel_t;

/*! Security Mode-and-Level definitions */
typedef enum gapSecurityModeAndLevel_tag {
    gSecurityMode_1_Level_1_c = (uint8_t)gSecurityMode_1_c | (uint8_t)gSecurityLevel_NoSecurity_c,          /*!< Mode 1 Level 1 - No Security. */
    gSecurityMode_1_Level_2_c = (uint8_t)gSecurityMode_1_c | (uint8_t)gSecurityLevel_NoMitmProtection_c,    /*!< Mode 1 Level 2 - Encryption without authentication. */
    gSecurityMode_1_Level_3_c = (uint8_t)gSecurityMode_1_c | (uint8_t)gSecurityLevel_WithMitmProtection_c,  /*!< Mode 1 Level 3 - Encryption with authentication. */
    gSecurityMode_1_Level_4_c = (uint8_t)gSecurityMode_1_c | (uint8_t)gSecurityLevel_LeSecureConnections_c, /*!< Mode 1 Level 4 - Encryption with LE Secure Connections pairing (BLE 4.2 only). */
    gSecurityMode_2_Level_1_c = (uint8_t)gSecurityMode_2_c | (uint8_t)gSecurityLevel_NoMitmProtection_c,    /*!< Mode 2 Level 1 - Data Signing without authentication. */
    gSecurityMode_2_Level_2_c = (uint8_t)gSecurityMode_2_c | (uint8_t)gSecurityLevel_WithMitmProtection_c   /*!< Mode 2 Level 2 - Data Signing with authentication. */
} gapSecurityModeAndLevel_t;
````

#### No security 

````
/* Device Security Requirements */
static const gapSecurityRequirements_t masterSecurity = {
        gSecurityMode_1_Level_1_c,
        FALSE,
        gDefaultEncryptionKeySize_d
};

gapDeviceSecurityRequirements_t deviceSecurityRequirements = {
    .pMasterSecurityRequirements  = (void*)&masterSecurity,
    .cNumServices                 = 0,
    .aServiceSecurityRequirements = NULL
};
````

#### Service security (heart rate example)

````
/* Device Security Requirements */
static const gapSecurityRequirements_t        masterSecurity = gGapDefaultSecurityRequirements_d;
static const gapServiceSecurityRequirements_t serviceSecurity[3] = {
  {
    .requirements = {
        .securityModeLevel = gSecurityMode_1_Level_3_c,
        .authorization = FALSE,
        .minimumEncryptionKeySize = gDefaultEncryptionKeySize_d
    },
    .serviceHandle = service_heart_rate
  },
  {
    .requirements = {
        .securityModeLevel = gSecurityMode_1_Level_3_c,
        .authorization = FALSE,
        .minimumEncryptionKeySize = gDefaultEncryptionKeySize_d
    },
    .serviceHandle = service_battery
  },
  {
    .requirements = {
        .securityModeLevel = gSecurityMode_1_Level_3_c,
        .authorization = FALSE,
        .minimumEncryptionKeySize = gDefaultEncryptionKeySize_d
    },
    .serviceHandle = service_device_information
  }
};

gapDeviceSecurityRequirements_t deviceSecurityRequirements = {
    .pMasterSecurityRequirements    = (void*)&masterSecurity,
    .cNumServices                   = 3,
    .aServiceSecurityRequirements   = (void*)serviceSecurity
};
````





NOTE : Maximum 3 services can have a custom security. But it's possible to put a Master Security that will set the default security.











# 14.12.2017

## Android Bluetooth order pairing (BlueSweet library)

When you connect to a device waiting to pair (pop up Android for passkey) :

````
12-14 17:13:34.353 30273-30273/com.smartcantonmanager I/BLEConnectFragment: BONDING
12-14 17:13:34.360 30273-30273/com.smartcantonmanager I/BLEConnectFragment: CONNECTING_OVERALL
12-14 17:13:34.367 30273-30273/com.smartcantonmanager I/BLEConnectFragment: CONNECTING
````

Time out Passkey request : 

````
12-14 17:13:34.353 30273-30273/com.smartcantonmanager I/BLEConnectFragment: BONDING
12-14 17:13:34.360 30273-30273/com.smartcantonmanager I/BLEConnectFragment: CONNECTING_OVERALL
12-14 17:13:34.367 30273-30273/com.smartcantonmanager I/BLEConnectFragment: CONNECTING
12-14 17:13:46.941 30273-30273/com.smartcantonmanager I/BLEConnectFragment: ADVERTISING
12-14 17:13:46.954 30273-30273/com.smartcantonmanager I/BLEConnectFragment: DISCONNECTED
12-14 17:13:46.969 30273-30273/com.smartcantonmanager I/BLEConnectFragment: BOND EVENT WITH CODE : -1
12-14 17:13:47.163 30273-30273/com.smartcantonmanager I/BLEConnectFragment: UNBONDED
````



States after connection with a **correct** passkey

````
12-15 08:35:17.060 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTING_OVERALL
12-15 08:35:17.061 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTING
12-15 08:35:17.657 28405-28405/com.smartcantonmanager I/BLEConnectFragment: BONDING
12-15 08:35:17.789 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTED
12-15 08:35:17.789 28405-28405/com.smartcantonmanager I/BLEConnectFragment: DISCOVERING_SERVICES
12-15 08:35:24.240 28405-28405/com.smartcantonmanager I/BLEConnectFragment: BONDED
12-15 08:35:24.248 28405-28405/com.smartcantonmanager I/BLEConnectFragment: BOND EVENT WITH CODE : -1
12-15 08:35:30.896 28405-28405/com.smartcantonmanager I/BLEConnectFragment: SERVICES_DISCOVERED
12-15 08:35:30.896 28405-28405/com.smartcantonmanager I/BLEConnectFragment: AUTHENTICATED
12-15 08:35:30.896 28405-28405/com.smartcantonmanager I/BLEConnectFragment: INITIALIZED
````

States after connection with a **wrong** passkey

````
12-15 08:36:19.382 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTING_OVERALL
12-15 08:36:19.396 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTING
12-15 08:36:20.127 28405-28405/com.smartcantonmanager I/BLEConnectFragment: BONDING
12-15 08:36:20.244 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTED
12-15 08:36:20.244 28405-28405/com.smartcantonmanager I/BLEConnectFragment: DISCOVERING_SERVICES
12-15 08:36:25.520 28405-28405/com.smartcantonmanager I/BLEConnectFragment: ADVERTISING
12-15 08:36:25.520 28405-28405/com.smartcantonmanager I/BLEConnectFragment: DISCONNECTED
12-15 08:36:25.520 28405-28405/com.smartcantonmanager I/BLEConnectFragment: UNBONDED
12-15 08:36:25.525 28405-28405/com.smartcantonmanager I/BLEConnectFragment: BOND EVENT WITH CODE : 1
````



States after bonding popup canceled : 

````
12-15 08:37:16.474 28405-28405/com.smartcantonmanager I/BLEConnectFragment: BONDING
12-15 08:37:16.474 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTING_OVERALL
12-15 08:37:16.474 28405-28405/com.smartcantonmanager I/BLEConnectFragment: CONNECTING
12-15 08:37:21.370 28405-28405/com.smartcantonmanager I/BLEConnectFragment: ADVERTISING
12-15 08:37:21.370 28405-28405/com.smartcantonmanager I/BLEConnectFragment: DISCONNECTED
12-15 08:37:21.370 28405-28405/com.smartcantonmanager I/BLEConnectFragment: UNBONDED
12-15 08:37:21.378 28405-28405/com.smartcantonmanager I/BLEConnectFragment: BOND EVENT WITH CODE : 1
````

New bonding after bond canceled :

````
12-15 08:40:02.604 356-356/com.smartcantonmanager I/BLEConnectFragment: BONDING
12-15 08:40:02.604 356-356/com.smartcantonmanager I/BLEConnectFragment: CONNECTING_OVERALL
12-15 08:40:02.604 356-356/com.smartcantonmanager I/BLEConnectFragment: CONNECTING
12-15 08:40:02.979 356-356/com.smartcantonmanager I/BLEConnectFragment: ADVERTISING
12-15 08:40:02.979 356-356/com.smartcantonmanager I/BLEConnectFragment: UNBONDED
12-15 08:40:03.674 356-356/com.smartcantonmanager I/BLEConnectFragment: BONDING
12-15 08:40:08.756 356-356/com.smartcantonmanager I/BLEConnectFragment: DISCONNECTED
12-15 08:40:08.757 356-356/com.smartcantonmanager I/BLEConnectFragment: UNBONDED
12-15 08:40:08.773 356-356/com.smartcantonmanager I/BLEConnectFragment: BOND EVENT WITH CODE : 9
12-15 08:40:11.757 356-356/com.smartcantonmanager I/BLEConnectFragment: UNDISCOVERED
12-15 08:40:14.720 356-356/com.smartcantonmanager I/BLEConnectFragment: ADVERTISING

````





# 18.12.2017

## SSL certificat with letsencrypt (certbot)

download and installation : 

https://certbot.eff.org/#debianjessie-other

```
# sudo certbot certonly --standalone --preferred-challenges http -d lsn.eig.ch
Saving debug log to /var/log/letsencrypt/letsencrypt.log
Starting new HTTPS connection (1): acme-v01.api.letsencrypt.org
Obtaining a new certificate
Performing the following challenges:
http-01 challenge for lsn.eig.ch
Waiting for verification...
Cleaning up challenges
Generating key (2048 bits): /etc/letsencrypt/keys/0000_key-certbot.pem
Creating CSR: /etc/letsencrypt/csr/0000_csr-certbot.pem

IMPORTANT NOTES:
 - Congratulations! Your certificate and chain have been saved at
   /etc/letsencrypt/live/lsn.eig.ch/fullchain.pem. Your cert will
   expire on 2018-03-20. To obtain a new or tweaked version of this
   certificate in the future, simply run certbot again. To
   non-interactively renew *all* of your certificates, run "certbot
   renew"
 - If you like Certbot, please consider supporting our work by:

   Donating to ISRG / Let's Encrypt:   https://letsencrypt.org/donate
   Donating to EFF:                    https://eff.org/donate-le

```

Once the certificates generated they can be listed using the following command : 

````
# certbot certificates
Saving debug log to /var/log/letsencrypt/letsencrypt.log

-------------------------------------------------------------------------------
Found the following certs:
  Certificate Name: lsn.eig.ch
    Domains: lsn.eig.ch
    Expiry Date: 2018-03-20 12:11:40+00:00 (VALID: 89 days)
    Certificate Path: /etc/letsencrypt/live/lsn.eig.ch/fullchain.pem
    Private Key Path: /etc/letsencrypt/live/lsn.eig.ch/privkey.pem
-------------------------------------------------------------------------------
root@debian:/etc/letsencrypt/live/lsn.eig.ch# certbot certificates
Saving debug log to /var/log/letsencrypt/letsencrypt.log

-------------------------------------------------------------------------------
Found the following certs:
  Certificate Name: lsn.eig.ch
    Domains: lsn.eig.ch
    Expiry Date: 2018-03-20 12:11:40+00:00 (VALID: 89 days)
    Certificate Path: /etc/letsencrypt/live/lsn.eig.ch/fullchain.pem
    Private Key Path: /etc/letsencrypt/live/lsn.eig.ch/privkey.pem
-------------------------------------------------------------------------------
````



We can directly use them on our application, eg with flask : 

````
# Provide your own keys to sign the SSL connexion
context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
chain_path = 'ssl_certificates/fullchain.pem'
chain_path = os.path.join(os.path.dirname(__file__), chain_path)
privkey_path = 'ssl_certificates/privkey.pem'
privkey_path = os.path.join(os.path.dirname(__file__), privkey_path)
context.load_cert_chain(chain_path, privkey_path)

...

app.run(host='0.0.0.0', debug=True, port=5000, ssl_context=context)
````



## My devices (cayenne)

https://mydevices.com/cayenne/docs_stage/lora/#lora

https://github.com/sabas1080/CayenneLPP





## Lora serialization

https://github.com/thesolarnomad/lora-serialization



# 24.12.2017

## Bug BSEC 

The function that generate a timing in ms isn't counting to 2^32. In fact, it will count the number of ticks to 2^32 but the real time in ms will not be encoded in this value. It generate a big problem for the BESC library that needs a time in nanoseconds. The time from the OSA_TimeGetMsec only is correct for approximately one hour. After that the counter is set to 0. The BSEC library does not like this kind of beahvear and generate a big sleep (4297718ms....). How to debug the problem inside the function bsec_iot_loop : 

````C
time_stamp_interval_ms = (sensor_settings.next_call - get_timestamp_us() * 1000) / 1000000;

// How to prove the bug :
int64_t time = (int64_t)get_timestamp_us();
int64_t tmp1 = time * 1000;
int64_t tmp2 = (int64_t)sensor_settings.next_call;
int64_t tmp3 = tmp2 - tmp1;
tmp3 = tmp3 / (int64_t)1000000;
//time_stamp_interval_ms = tmp3;
````

Result : 

````C
time	int64_t	96000	
tmp1	int64_t	96000000	
tmp2	int64_t	4297814000000	
tmp3	int64_t	4297718	
time_stamp	int64_t	4294814000000	
time_stamp_interval_ms	volatile int64_t	4297718	

````

To fix it: the easiest way is to qui the function and restart the process. It only happens every hour, so it's acceptable. But to make it cleaner we should implement a nanoseconds counter in 64bit! 



# 30.12.2017

## Library feature not supported

When trying to add the support for the BLE scan, with the following code : 

````
ShellGap_SetScanParameters();
bleResult_t result = Gap_StartScanning(&gAppScanParams, ShellGap_ScanningCallback, TRUE);
result = result;
````

Result had the value ` gBleFeatureNotSupported_c`. This is because the stack linked to the project does not support all the modes. See : https://community.nxp.com/thread/458322 



To fix it : go to `Project` -> `Proprieties`

`C/C++ Build` -> `Settings` -> `Libraries`

Change `_ble_4-2_host_peripheral_cm0p` to `_ble_4-2_host_cm0p`.

Copy the file `lib_ble_4-2_host_cm0p.a` from an existing project using this library (eg. the beacon) to the libs directory.



## Numberpicker android

https://github.com/ShawnLin013/NumberPicker







# 31.12.2017

## LoRaWAN maximum payload by datarate 



Inside the STM32 code we can find the following definition inside the file `RegionEU868.h`:

````
/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateEU868[] = { 51, 51, 51, 115, 242, 242, 242, 242 };
````

The maximum payload is defined by the datarate applied. 

We need to disable the adaptative data rate in the `main.c` :

````
/**
 * @brief LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                              0
````



To change the data rate, we changed `DR_0` to `DR_4` : 

````
/**
 * Initialises the Lora Parameters
 */
static LoRaParam_t LoRaParamInit = {TX_ON_EVENT,
                                    0,
                                    CLASS_A,
                                    LORAWAN_ADR_ON,
									DR_4,
                                    LORAWAN_PUBLIC_NETWORK,
                                    JOINREQ_NBTRIALS};
````



The parameter `DR_4`is the following configuration : 

````
/*!
 * Region       | SF
 * ------------ | :-----:
 * AS923        | SF8  - BW125
 * AU915        | SF8  - BW500
 * CN470        | SF8  - BW125
 * CN779        | SF8  - BW125
 * EU433        | SF8  - BW125
 * EU868        | SF8  - BW125
 * IN865        | SF8  - BW125
 * KR920        | SF8  - BW125
 * US915        | SF8  - BW500
 * US915_HYBRID | SF8  - BW500
 */
#define DR_4                                        4
````

The length of the payload is calculated inside the function `ValidatePayloadLength` in the file `LoRaMAC.c `:

`static bool ValidatePayloadLength( uint8_t lenN, int8_t datarate, uint8_t fOptsLen )`







