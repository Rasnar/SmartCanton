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


public class User {

    private String username;
    private String password;
    private String public_id;
    private Boolean admin;
    private String new_Password;
    //private String token;

    public void setUsername(String username) {
        this.username = username;
    }

    public void setPassword(String password) {
        this.password = password;
    }


    public String getUsername() {
        return username;
    }

    public String getPublicId() {
        return public_id;
    }

    public Boolean getAdmin() {
        return admin;
    }

    public void setNewPassword(String new_Password) {
        this.new_Password = new_Password;
    }

//    public void setToken(String token) {
//        this.token = token;
//    }
}
