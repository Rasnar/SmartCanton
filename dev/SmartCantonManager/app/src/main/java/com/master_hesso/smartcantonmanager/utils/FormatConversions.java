package com.master_hesso.smartcantonmanager.utils;

/**
 * Created by David on 16.12.2017.
 */

public class FormatConversions {

    private final static char[] hexArray = "0123456789ABCDEF".toCharArray();
    public static String bytesArrayToHexString(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }
    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }
    public static final long unsignedIntToLong(byte[] b)
    {
        long l = 0;
        l |= b[3] & 0xFF;
        l <<= 8;
        l |= b[2] & 0xFF;
        l <<= 8;
        l |= b[1] & 0xFF;
        l <<= 8;
        l |= b[0] & 0xFF;
        return l;
    }

    public static final int unsignedShortToInt(byte[] b)
    {
        int integer = 0;
        integer |= b[1] & 0xFF;
        integer <<= 8;
        integer |= b[0] & 0xFF;
        return integer;
    }

}
