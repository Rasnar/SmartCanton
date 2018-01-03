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

import android.text.TextUtils;


/**
 * Methos to validate if feels are correctly populated
 */
public class Validation {

    /**
     * Test if field is not empty
     * @param name Text in the field
     * @return True if valid, otherwise false
     */
    public static boolean validateFields(String name) {

        return !TextUtils.isEmpty(name);
    }

    /**
     * Test if the username is valid
     * @param name Username text
     * @return True if valid, otherwise false
     */
    public static boolean validateUsername(String string) {

        return !string.isEmpty();
    }

    /**
     * Test if the string contain a valid hexadecimal representation
     * @param name String with the hexadecimal representation
     * @return True if valid, otherwise false
     */
    private static boolean validateHexString(String cadena) {
        try {
            Long.parseLong(cadena, 16);
            return true;
        } catch (NumberFormatException ex) {
            // Error handling code...
            return false;
        }
    }
}
