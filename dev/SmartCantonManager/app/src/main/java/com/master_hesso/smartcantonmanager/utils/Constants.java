package com.master_hesso.smartcantonmanager.utils;

import java.util.UUID;

public class Constants {
    public static final String BASE_URL = "https://lsn.eig.ch:5000/";
    public static final String TOKEN = "token";
    public static final String USERNAME = "username";
    public static final String BLE_DEVICE_MAC = "ble_device_mac";


    /* Payload for an AltBeacon
     * Below the information are :
     * +------------+------------+------------+------------+
     * |   1 byte   |   2 byte   |   3 byte   |   4 byte   |
     * +============+============+============+============+
     * |    0xBE    |    0xAC    |    0x00    |    0x00    |
     * |    0x00    |    0x00    |    0xAA    |    0xAA    |
     * |    0xBB    |    0xBB    |    0xCC    |    0xCC    |
     * |    0xDD    |    0xDD    |    0xDD    |    0xDD    |
     * |    0xDD    |    0xDD    |    0x11    |    0x11    |
     * |    0x22    |    0x22    +------------+------------+
     * +------------+------------+
     * BEACON CODE : 0xBEAC
     * BEACON UUID : 00000000-AAAA-BBBB-CCCC-DDDDDDDDDDDD
     * BEACON MAJOR : 0x1111
     * BEACON MINOR : 0x2222
     * RSSI 1m : 0xBB (-69dBm)
     * MANUFACTURER FEATURE : 0x00
     */
    public static final int BEACON_MANUFACTURER_ID = 0xFFFF;
    public static final byte BEACON_MANUFACTURER_ID_BYTES[] = {(byte)0xFF, (byte)0xFF};
    public static final byte ALTBEACON_CODE[] = {(byte) 0xBE, (byte) 0xAC};
    public static final UUID ALTBEACON_UUID_STRING = UUID.fromString("00000000-AAAA-BBBB-CCCC-DDDDDDDDDDDD");
    public static final byte ALTBEACON_UUID_BYTES [] = {(byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0xAA, (byte) 0xAA, (byte) 0xBB, (byte) 0xBB, (byte) 0xCC,
            (byte) 0xCC, (byte) 0xDD, (byte) 0xDD, (byte) 0xDD, (byte) 0xDD, (byte) 0xDD,
            (byte) 0xDD};
    public static final byte ALTBEACON_MAJOR[] = {(byte) 0x11, (byte) 0x11};
    public static final byte ALTBEACON_MINOR[] = {(byte) 0x22, (byte) 0x22};
    public static final byte ALTBEACON_RSSI_1M[] = {(byte) 0xBB};
    public static final byte ALTBEACON_MANUFACTURER_FEATURE[]= {(byte) 0x00};
}
