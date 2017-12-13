package com.master_hesso.smartcantonmanager.model;


public class SmartCantonDevBoxDevice {

    private String app_eui;
    private String app_key;
    private String dev_eui;

    private String owner_public_id;

    private String ble_mac_addr;
    private String ble_passkey;


    public String getAppEui() {
        return app_eui;
    }

    public void setAppEui(String appEui) {
        this.app_eui = appEui;
    }

    public String getAppKey() {
        return app_key;
    }

    public void setAppKey(String appKey) {
        this.app_key = appKey;
    }

    public String getDevEui() {
        return dev_eui;
    }

    public void setDevEui(String devEui) {
        this.dev_eui = devEui;
    }

    public String getOwnerPublicId() {
        return owner_public_id;
    }

    public void setOwnerPublicId(String ownerPublicId) {
        this.owner_public_id = ownerPublicId;
    }

    public String getBleMacAddr() {
        return ble_mac_addr;
    }

    public void setBleMacAddr(String bleMacAddr) {
        this.ble_mac_addr = bleMacAddr;
    }

    public String getBlePassKey() {
        return ble_passkey;
    }

    public void setBlePassKey(String blePassKey) {
        this.ble_passkey = blePassKey;
    }
}
