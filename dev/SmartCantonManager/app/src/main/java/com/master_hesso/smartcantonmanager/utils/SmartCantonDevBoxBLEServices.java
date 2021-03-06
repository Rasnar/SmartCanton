/*
 *   ______                              _______
 *  / _____)                        _   (_______)                 _
 * ( (____   ____   _____   ____  _| |_  _        _____  ____   _| |_   ___   ____
 *  \____ \ |    \ (____ | / ___)(_   _)| |      (____ ||  _ \ (_   _) / _ \ |  _ \
 *  _____) )| | | |/ ___ || |      | |_ | |_____ / ___ || | | |  | |_ | |_| || | | |
 * (______/ |_|_|_|\_____||_|       \__) \______)\_____||_| |_|   \__) \___/ |_| |_|
 *  ______                 ______
 * (______)               (____  \
 *  _     _  _____  _   _  ____)  )  ___   _   _
 * | |   | || ___ || | | ||  __  (  / _ \ ( \ / )
 * | |__/ / | ____| \ V / | |__)  )| |_| | ) X (
 * |_____/  |_____)  \_/  |______/  \___/ (_/ \_)
 *
 * @author  Da Silva Andrade David
 * @version V1.0
 * @date    02-01-2018
 */

package com.master_hesso.smartcantonmanager.utils;

import java.util.UUID;

/**
 * Constants defining the Servicies presents in the SmartCanton DevBox
 */
public class SmartCantonDevBoxBLEServices {

    /* SMARTCANTON_DEVBOX_LORA_SERVICE */
    public static final UUID SMARTCANTON_DEVBOX_LORA_SERVICE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA00000000");

    public static final UUID SMARTCANTON_DEVBOX_LORA_APP_EUI =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA01000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_APP_KEY =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA02000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_DEVICE_EUI =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA03000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_LORA_CONFIRM_MODE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA04000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_NETWORK_JOIN_STATUS =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA05000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_NETWORK_DEVICE_ADDRESS =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA06000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_NETWORK_NEWTORK_SESSION_KEY =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA07000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_NETWORK_APP_SESSION_KEY =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA08000000");
    public static final UUID SMARTCANTON_DEVBOX_LORA_NETWORK_VALIDATE_NEW_CONFIGURATION =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA09000000");

    /* SMARTCANTON_DEVBOX_GPS_SERVICE */
    public static final UUID SMARTCANTON_DEVBOX_GPS_SERVICE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA00000010");

    public static final UUID SMARTCANTON_DEVBOX_GPS_POSITION =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA01000010");
    public static final UUID SMARTCANTON_DEVBOX_GPS_SPEED =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA02000010");
    public static final UUID SMARTCANTON_DEVBOX_GPS_COURSE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA03000010");
    public static final UUID SMARTCANTON_DEVBOX_GPS_DATE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA04000010");
    public static final UUID SMARTCANTON_DEVBOX_GPS_TIME =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA05000010");

    /* SMARTCANTON_DEVBOX_BME680_SERVICE */
    public static final UUID SMARTCANTON_DEVBOX_BME680_SERVICE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA00000020");

    public static final UUID SMARTCANTON_DEVBOX_BME680_IAQ =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA01000020");
    public static final UUID SMARTCANTON_DEVBOX_BME680_IAQ_ACCURACY =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA02000020");
    public static final UUID SMARTCANTON_DEVBOX_BME680_TEMPERATURE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA03000020");
    public static final UUID SMARTCANTON_DEVBOX_BME680_HUMIDITY =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA04000020");
    public static final UUID SMARTCANTON_DEVBOX_BME680_PRESSURE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA05000020");
    public static final UUID SMARTCANTON_DEVBOX_BME680_RAW_TEMPERATURE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA06000020");
    public static final UUID SMARTCANTON_DEVBOX_BME680_RAW_HUDMITIY =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA07000020");
    public static final UUID SMARTCANTON_DEVBOX_BME680_RAW_GAS =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA08000020");

    /* SMARTCANTON_DEVBOX_BNO055_SERVICE */
    public static final UUID SMARTCANTON_DEVBOX_BNO055_SERVICE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA00000030");

    public static final UUID SMARTCANTON_DEVBOX_BNO055_ACCELEROMETER =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA01000030");
    public static final UUID SMARTCANTON_DEVBOX_BNO055_GYROSCOPE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA02000030");
    public static final UUID SMARTCANTON_DEVBOX_BNO055_MAGNETOMETER =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA03000030");
    public static final UUID SMARTCANTON_DEVBOX_BNO055_MEASURE_INTERVAL =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA04000030");


    /* SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE */
    public static final UUID SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA00000040");

    public static final UUID SMARTCANTON_DEVBOX_BLE_DEVICES_SCANNED =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA01000040");
    public static final UUID SMARTCANTON_DEVBOX_BLE_SCAN_WINDOW =
            UUID.fromString("DDDDDDDD-DDDD-CCCC-BBBB-AAAA02000040");

}
