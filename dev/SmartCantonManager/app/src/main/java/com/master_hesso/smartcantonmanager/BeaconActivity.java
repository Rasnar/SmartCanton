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

package com.master_hesso.smartcantonmanager;

import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;

import com.idevicesinc.sweetblue.BleManager;
import com.idevicesinc.sweetblue.BleManagerConfig;
import com.idevicesinc.sweetblue.utils.BluetoothEnabler;
import com.master_hesso.smartcantonmanager.utils.Constants;
import com.master_hesso.smartcantonmanager.utils.FormatConversions;
import com.shawnlin.numberpicker.NumberPicker;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Locale;

/**
 * Activity offrering the possibility to generate a number of beacons selectable by the
 * user
 */
public class BeaconActivity extends AppCompatActivity {

    public static final String TAG = BeaconActivity.class.getSimpleName();

    /**
     * SweetBlue BLE configuration
     */
    private final BleManagerConfig mBleManagerConfig = new BleManagerConfig() {{
        this.forceBondDialog = true;
        this.stopScanOnPause = true;
    }};
    private BleManager mBleManager;
    private BluetoothLeAdvertiser mBluetoothLeAdvertiser;
    private ArrayList<AdvertiseCallback> advertiseCallbacks = new ArrayList<AdvertiseCallback>();

    /* Count the number of successfully started advertisers */
    private Integer mAdvertisersSuccessfullyStarted = 0;

    /* Variable used to be sure to only display once the message for the maximum number of
    advertisers */
    boolean isMessageAlreadyShown = false;

    private FloatingActionButton fab;
    Toolbar toolbar;

    private TextView tvBeaconType;
    private TextView tvBeaconManufacturerId;
    private TextView tvBeaconCode;
    private TextView tvBeaconUuid;
    private TextView tvBeaconMajor;
    private TextView tvBeaconMinor;
    private TextView tvBeaconRssi;
    private TextView tvBeaconManufacturerFeature;

    NumberPicker npParallelAdvertisers;

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_beacon);

        initViews();

        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle("Beacon generator");
        setSupportActionBar(toolbar);

        try {
            this.getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        } catch (Exception ignored) {
        }

        // This class helps you navigate the treacherous waters of Android M Location requirements for scanning.
        // First it enables bluetooth itself, then location permissions, then location services. The latter two
        // are only needed in Android M. This must be called from an Activity instance.
        BluetoothEnabler.start(getApplicationContext());

        mBleManager = BleManager.get(getApplicationContext());
        mBleManager.setConfig(mBleManagerConfig);

        mBluetoothLeAdvertiser = mBleManager.getNativeAdapter().getBluetoothLeAdvertiser();


        fab.setOnClickListener(view -> {

            if (mAdvertisersSuccessfullyStarted == 0) {
                startAdvertisers(npParallelAdvertisers.getValue());
                fab.setImageDrawable(getResources().getDrawable(R.drawable.ic_stop_box_white));
            } else {
                stopAdvertisers();
                fab.setImageDrawable(getResources().getDrawable(R.drawable.ic_play_arrow_white));
            }
        });

    }

    /**
     * This function help us to populate the Advertisement data with the content that we wish.
     * In this case we use the AltBeacon payload (https://github.com/AltBeacon/spec)
     *
     * @return The advertisement data to be used by the BluetoothLeAdvertiser.
     */
    private AdvertiseData buildAdvertisementData() {
        /* We populate the advertisement data by hand. It's quite simple because we use a static
         * representation.
         * More information about how the AltBeacon packet is build can be found here :
         * https://github.com/AltBeacon/spec
         */

        byte[] manufacturerData;
        ByteBuffer manufacturerDataBuffer = ByteBuffer.allocate(
                Constants.ALTBEACON_CODE.length +
                        Constants.ALTBEACON_UUID_BYTES.length +
                        Constants.ALTBEACON_MAJOR.length +
                        Constants.ALTBEACON_MINOR.length +
                        Constants.ALTBEACON_RSSI_1M.length +
                        Constants.ALTBEACON_MANUFACTURER_FEATURE.length
        );

        manufacturerDataBuffer.put(Constants.ALTBEACON_CODE);
        manufacturerDataBuffer.put(Constants.ALTBEACON_UUID_BYTES);
        manufacturerDataBuffer.put(Constants.ALTBEACON_MAJOR);
        manufacturerDataBuffer.put(Constants.ALTBEACON_MINOR);
        manufacturerDataBuffer.put(Constants.ALTBEACON_RSSI_1M);
        manufacturerDataBuffer.put(Constants.ALTBEACON_MANUFACTURER_FEATURE);

        /* Retrieve the final byte buffer */
        manufacturerData = manufacturerDataBuffer.array();

        AdvertiseData advertiseData = new AdvertiseData.Builder()
                .addManufacturerData(Constants.BEACON_MANUFACTURER_ID, manufacturerData)
                .setIncludeDeviceName(false)
                .setIncludeTxPowerLevel(true)
                .build();

        return advertiseData;
    }

    /**
     * Extract all views from the current load layout
     */
    private void initViews() {
        fab = findViewById(R.id.fab);
        toolbar = findViewById(R.id.toolbar);

        tvBeaconType = findViewById(R.id.tv_beacon_type);
        tvBeaconManufacturerId = findViewById(R.id.tv_beacon_manufacturer_id);
        tvBeaconCode = findViewById(R.id.tv_beacon_code);
        tvBeaconUuid = findViewById(R.id.tv_beacon_uuid);
        tvBeaconMajor = findViewById(R.id.tv_beacon_major);
        tvBeaconMinor = findViewById(R.id.tv_beacon_minor);
        tvBeaconRssi = findViewById(R.id.tv_beacon_rssi);
        tvBeaconManufacturerFeature = findViewById(R.id.tv_beacon_manufacturer_feature);

        tvBeaconType.setText(R.string.tv_beacon_type);
        tvBeaconManufacturerId.setText(String.format("Manufacture ID : 0x%s",
                FormatConversions.bytesArrayToHexString(Constants.BEACON_MANUFACTURER_ID_BYTES)));
        tvBeaconCode.setText(String.format("AltBeacon code : 0x%s",
                FormatConversions.bytesArrayToHexString(Constants.ALTBEACON_CODE)));
        tvBeaconUuid.setText(String.format("AltBeacon UUID : %s",
                Constants.ALTBEACON_UUID_STRING));
        tvBeaconMajor.setText(String.format("AltBeacon major : 0x%s",
                FormatConversions.bytesArrayToHexString(Constants.ALTBEACON_MAJOR)));
        tvBeaconMinor.setText(String.format("AltBeacon minor : 0x%s",
                FormatConversions.bytesArrayToHexString(Constants.ALTBEACON_MINOR)));
        tvBeaconRssi.setText(String.format(Locale.getDefault(),
                "RSSI at 1m : %d dBm", Constants.ALTBEACON_RSSI_1M[0]));
        tvBeaconManufacturerFeature.setText(String.format("AltBeacon manufacturer feature : 0x%s",
                FormatConversions.bytesArrayToHexString(Constants.ALTBEACON_MANUFACTURER_FEATURE)));


        npParallelAdvertisers = findViewById(R.id.np_parallel_advertisers);

    }

    /**
     * Build the advertiser settings.
     *
     * @return The advertisement settings to be used by the BluetoothLeAdvertiser.
     */
    private AdvertiseSettings buildAdvertiseSettings() {

        AdvertiseSettings advertiseSettings = new AdvertiseSettings.Builder()
                .setAdvertiseMode(AdvertiseSettings.ADVERTISE_MODE_LOW_POWER)
                .setConnectable(false)
                .build();

        return advertiseSettings;
    }

    /**
     * Start the number of wanted advertisers. We can't know in advance how many can be started on a
     * device. We need to start them to know.
     *
     * @param numberOfAdvertisersWanted Maximum number of advertiser wanted.
     */
    private void startAdvertisers(int numberOfAdvertisersWanted) {
        // Disable the use of the button while starting new advertisers
        fab.setEnabled(false);

        // Execute code in background thread to prevent the UI from slowing down
        AsyncTask.execute(() -> {
            for (int i = 0; i < numberOfAdvertisersWanted; i++) {

                AdvertiseCallback advertiseCallback = new AdvertiseCallback() {
                    @Override
                    public void onStartSuccess(AdvertiseSettings settingsInEffect) {
                        super.onStartSuccess(settingsInEffect);
                        Log.i(TAG, settingsInEffect.toString());
                        synchronized (mAdvertisersSuccessfullyStarted) {
                            mAdvertisersSuccessfullyStarted++;

                            if(mAdvertisersSuccessfullyStarted == numberOfAdvertisersWanted){
                                fab.setEnabled(true);
                                showSnackBarMessage(String.format(Locale.getDefault(),
                                        "All %d advertiser(s) started successfully!",
                                        mAdvertisersSuccessfullyStarted));
                            }
                        }
                    }

                    @Override
                    public void onStartFailure(int errorCode) {
                        super.onStartFailure(errorCode);
                        Log.i(TAG, Integer.valueOf(errorCode).toString());

                        // The user can now stop the running advertisers
                        fab.setEnabled(true);

                        if (errorCode == AdvertiseCallback.ADVERTISE_FAILED_TOO_MANY_ADVERTISERS) {
                            if (!isMessageAlreadyShown) {
                            /* Update the numberpicker with the maximum value possible */
                                npParallelAdvertisers.setValue(mAdvertisersSuccessfullyStarted);
                                isMessageAlreadyShown = true;
                                showSnackBarMessage("Number maximum of simultaneous advertisers reached " +
                                        "by your phone : " + mAdvertisersSuccessfullyStarted);
                            }
                            return;
                        }

                        String description;
                        if (errorCode == AdvertiseCallback.ADVERTISE_FAILED_FEATURE_UNSUPPORTED)
                            description = "ADVERTISE_FAILED_FEATURE_UNSUPPORTED";
                        else if (errorCode == AdvertiseCallback.ADVERTISE_FAILED_ALREADY_STARTED)
                            description = "ADVERTISE_FAILED_ALREADY_STARTED";
                        else if (errorCode == AdvertiseCallback.ADVERTISE_FAILED_DATA_TOO_LARGE)
                            description = "ADVERTISE_FAILED_DATA_TOO_LARGE";
                        else if (errorCode == AdvertiseCallback.ADVERTISE_FAILED_INTERNAL_ERROR)
                            description = "ADVERTISE_FAILED_INTERNAL_ERROR";
                        else description = "unknown";

                        showSnackBarMessage("Advertisement start failure : " + description);
                    }
                };

                /* Register the callback to be able to stop the corresponding advertisement later */
                advertiseCallbacks.add(advertiseCallback);

                /* Start the advertisement */
                mBluetoothLeAdvertiser.startAdvertising(
                        buildAdvertiseSettings(),
                        buildAdvertisementData(),
                        null,
                        advertiseCallback);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopAdvertisers();
    }

    /**
     * Stop all the advertisers started
     */
    private void stopAdvertisers() {
        for (int i = 0; i < advertiseCallbacks.size(); i++) {

            mBluetoothLeAdvertiser.stopAdvertising(advertiseCallbacks.get(i));
        }

        mAdvertisersSuccessfullyStarted = 0;
        isMessageAlreadyShown = false;
    }


    /**
     * Display a snackbar with a custom message
     *
     * @param message Message to display
     */
    private void showSnackBarMessage(String message) {

        Snackbar.make(findViewById(R.id.activity_beacon), message, Snackbar.LENGTH_LONG).show();
    }
}
