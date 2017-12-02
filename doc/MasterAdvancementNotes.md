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




## 18.09.2017

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

