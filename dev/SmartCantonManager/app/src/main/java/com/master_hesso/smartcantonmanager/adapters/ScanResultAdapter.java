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

package com.master_hesso.smartcantonmanager.adapters;

import com.master_hesso.smartcantonmanager.R;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.os.SystemClock;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.idevicesinc.sweetblue.BleDevice;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.concurrent.TimeUnit;

/**
 * Adapter to hold the devices scanned and display them in a list view
 */
public class ScanResultAdapter extends BaseAdapter {

    private ArrayList<BleDevice> mArrayList;

    private Context mContext;

    private LayoutInflater mInflater;

    public ScanResultAdapter(Context context, LayoutInflater inflater) {
        super();
        mContext = context;
        mInflater = inflater;
        mArrayList = new ArrayList<>();
    }

    @Override
    public int getCount() {
        return mArrayList.size();
    }

    @Override
    public Object getItem(int position) {
        return mArrayList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return mArrayList.get(position).getMacAddress().hashCode();
    }

    @Override
    public View getView(int position, View view, ViewGroup parent) {

        // Reuse an old view if we can, otherwise create a new one.
        if (view == null) {
            view = mInflater.inflate(R.layout.listitem_scanresult, null);
        }

        TextView deviceNameView = view.findViewById(R.id.device_name);
        TextView deviceAddressView = view.findViewById(R.id.device_address);
        TextView lastSeenView = view.findViewById(R.id.last_seen);

        BleDevice device = mArrayList.get(position);

        String name = device.getName_normalized();
        if (name == null) {
            name = mContext.getResources().getString(R.string.no_name);
        }
        deviceNameView.setText(name);
        deviceAddressView.setText(device.getMacAddress());
        lastSeenView.setText(getTimeSinceString(mContext,
                device.getLastDiscoveryTime().toMilliseconds()));

//        Date date = new Date(device.getLastDiscoveryTime().toMilliseconds());
//        DateFormat formatter = new SimpleDateFormat("HH:mm:ss");
//        String dateFormatted = formatter.format(date);
//        lastSeenView.setText("Last seen : " + dateFormatted);

        return view;
    }

    /**
     * Search the mArrayAdapter for an existing device address and return it, otherwise return -1.
     */
    private int getPosition(String address) {
        int position = -1;
        for (int i = 0; i < mArrayList.size(); i++) {
            if (mArrayList.get(i).getMacAddress().equals(address)) {
                position = i;
                break;
            }
        }
        return position;
    }


    /**
     * Add a ScanResult item to the mArrayAdapter if a result from that device isn't already present.
     * Otherwise updates the existing position with the new ScanResult.
     */
    public void add(BleDevice device) {

        int existingPosition = getPosition(device.getMacAddress());

        if (existingPosition >= 0) {
            // DiscoveryTreeDevice is already in list, update its record.
            mArrayList.set(existingPosition, device);
        } else {
            // Add new DiscoveryTreeDevice's device to list.
            mArrayList.add(device);
        }
    }

    /**
     * Clear out the mArrayAdapter.
     */
    public void clear() {
        mArrayList.clear();
    }

    /**
     * Takes in a number of milliseconds and returns a human-readable string giving a vague
     * description of how long ago that was.
     */
    public static String getTimeSinceString(Context context, long timeMilliseconds) {
        String lastSeenText = context.getResources().getString(R.string.last_seen) + " ";

        long timeSince = System.currentTimeMillis() - timeMilliseconds;
        long secondsSince = TimeUnit.SECONDS.convert(timeSince, TimeUnit.MILLISECONDS);

        if (secondsSince < 5) {
            lastSeenText += context.getResources().getString(R.string.just_now);
        } else if (secondsSince < 60) {
            lastSeenText += secondsSince + " " + context.getResources()
                    .getString(R.string.seconds_ago);
        } else {
            long minutesSince = TimeUnit.MINUTES.convert(secondsSince, TimeUnit.SECONDS);
            if (minutesSince < 60) {
                if (minutesSince == 1) {
                    lastSeenText += minutesSince + " " + context.getResources()
                            .getString(R.string.minute_ago);
                } else {
                    lastSeenText += minutesSince + " " + context.getResources()
                            .getString(R.string.minutes_ago);
                }
            } else {
                long hoursSince = TimeUnit.HOURS.convert(minutesSince, TimeUnit.MINUTES);
                if (hoursSince == 1) {
                    lastSeenText += hoursSince + " " + context.getResources()
                            .getString(R.string.hour_ago);
                } else {
                    lastSeenText += hoursSince + " " + context.getResources()
                            .getString(R.string.hours_ago);
                }
            }
        }

        return lastSeenText;
    }
}
