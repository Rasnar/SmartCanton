package com.master_hesso.smartcantonmanager.utils;

import android.text.TextUtils;

public class Validation {

    public static boolean validateFields(String name) {

        return !TextUtils.isEmpty(name);
    }

    public static boolean validateUsername(String string) {

        return !string.isEmpty();
    }

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
