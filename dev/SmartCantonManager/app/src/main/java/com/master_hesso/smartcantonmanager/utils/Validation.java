package com.master_hesso.smartcantonmanager.utils;

import android.text.TextUtils;
import android.util.Patterns;

public class Validation {

    public static boolean validateFields(String name){

        if (TextUtils.isEmpty(name)) {

            return false;

        } else {

            return true;
        }
    }

    public static boolean validateUsername(String string) {

        return !string.isEmpty();
    }
}
