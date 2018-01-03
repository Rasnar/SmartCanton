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
