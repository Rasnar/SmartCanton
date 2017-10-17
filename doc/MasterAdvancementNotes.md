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



### MCU 

(**Not critical**) Move reset button to the right prevent accidental press when plugging the JTAG



### Power Supply

(**Not critical**) Move connection Solar panel and Battery to contain the connector inside the board

### GPS

Put and active circuit possible (cf. hardware integration PDF, page 15 and 16)

(**Not critical**) put GPS_nCS on a pin directly drived by the DSPI peripheral