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

/**
 * Class with multiple function to convert bytes or string to another format
 */
public class FormatConversions {

    private final static char[] hexArray = "0123456789ABCDEF".toCharArray();

    /**
     * Convert a byte array to a hexadecimal string
     * @param bytes Byte array to be converted
     * @return String holding the result
     */
    public static String bytesArrayToHexString(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    /**
     * Convert a string containing an hexadecimal representation to a byte array
     * @param s String with a hexadecimal representation
     * @return Byte array with the result
     */
    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

    /**
     * Convert a unsigned int to a long value (little endian)
     * @param b Byte array to be converted
     * @return Long converted
     */
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

    /**
     * Convert a unsigned short to a int value (little endian)
     * @param b Byte array to be converted
     * @return Integer converted
     */
    public static final int unsignedShortToInt(byte[] b)
    {
        int integer = 0;
        integer |= b[1] & 0xFF;
        integer <<= 8;
        integer |= b[0] & 0xFF;
        return integer;
    }

}
