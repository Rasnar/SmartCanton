/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.master_hesso.smartcantonmanager.fragments;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.CardView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;

import com.auth0.android.jwt.DecodeException;
import com.auth0.android.jwt.JWT;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.idevicesinc.sweetblue.BleDevice;
import com.idevicesinc.sweetblue.BleDevice.StateListener;
import com.idevicesinc.sweetblue.BleDeviceState;
import com.idevicesinc.sweetblue.BleManager;
import com.idevicesinc.sweetblue.BleManagerConfig;
import com.idevicesinc.sweetblue.DeviceStateListener;
import com.idevicesinc.sweetblue.utils.BluetoothEnabler;
import com.master_hesso.smartcantonmanager.R;
import com.master_hesso.smartcantonmanager.model.Response;
import com.master_hesso.smartcantonmanager.model.SmartCantonDevBoxDevice;
import com.master_hesso.smartcantonmanager.network.NetworkUtil;
import com.master_hesso.smartcantonmanager.utils.Constants;
import com.master_hesso.smartcantonmanager.utils.FormatConversions;
import com.master_hesso.smartcantonmanager.utils.SmartCantonDevBoxBLEServices;

import java.io.IOException;
import java.util.Date;

import retrofit2.adapter.rxjava.HttpException;
import rx.android.schedulers.AndroidSchedulers;
import rx.schedulers.Schedulers;
import rx.subscriptions.CompositeSubscription;

import static com.idevicesinc.sweetblue.BleDevice.ReadWriteListener.Status;
import static com.idevicesinc.sweetblue.BleDevice.StateListener.*;


/**
 * Scans for Bluetooth Low Energy Advertisements matching a filter and displays them to the user.
 */
public class BLEConnectFragment extends Fragment {

    public static final String TAG = BLEConnectFragment.class.getSimpleName();

    private static final int LORA_BLE_SERVICE_NUMBER_OF_CHARACTERISTICS = 4;
    private static final int GPS_BLE_SERVICE_NUMBER_OF_CHARACTERISTICS = 5;
    /**
     * SweetBlue BLE configuration
     */
    private final BleManagerConfig mBleManagerConfig = new BleManagerConfig() {{
//        this.scanApi = BleScanApi.POST_LOLLIPOP;
//        this.useLeTransportForBonding
//        this.alwaysBondOnConnect = true;

//        this.forceBondDialog = true;
        this.connectFailRetryConnectingOverall = true;
        this.stopScanOnPause = true;
    }};

    ProgressDialog bleReadWriteProgressDialog;
    ProgressDialog bleConnectionProgressDialog;

    private BleDevice mBleDevice = null;
    private BleManager mBleManager;
    private CompositeSubscription mSubscriptions;
    private ProgressDialog mProgressDialog;
    private SharedPreferences mSharedPreferences;
    private String mToken;
    private String mUsername;
    private String mUserId;
    private Date mTokenExpiresAt;
    private JWT mJwt;
    private CardView cvDeviceServer;
    private TextView tvCvTitleServ;
    private ImageButton btnDownloadServ;
    private ImageButton btnUploadServ;
    private TextView tvMessageServ;
    private TextView tvBleTitleServ;
    private TextView tvBleMacAddrServ;
    private TextView tvBlePasskeyServ;
    private TextView tvLoRaTitleServ;
    private TextView tvDevEuiServ;
    private TextView tvAppEuiServ;
    private TextView tvAppKeyServ;
    private TextView tvDeviceOwnerTitleServ;
    private TextView tvPublicIdServ;
    private CardView cvDeviceBLELora;
    private TextView tvCvTitleBleLora;
    private ImageButton btnBleLoraDownloadDevice;
    private ImageButton btnBleLoraUploadDevice;
    private TextView tvLoraMessageDevice;
    private TextView tvLoraDevEuiDevice;
    private TextView tvLoraAppEuiDevice;
    private TextView tvLoraNetworkJoinStatusDevice;
    private TextView tvLoraDeviceAddressDevice;
    private CardView cvDeviceBleGps;
    private TextView tvCvTitleBleGps;
    private ImageButton btnBleGpsDownloadDevice;
    private RelativeLayout rlBleGpsNotifsSwitch;
    private Switch swGpsEnableNotifications;
    private TextView tvGpsMessageDevice;
    private TextView tvGpsServicePositionDevice;
    private TextView tvGpsServiceSpeedDevice;
    private TextView tvGpsServiceCourseDevice;
    private TextView tvGpsServiceDateDevice;
    private TextView tvGpsServiceTimeDevice;
    private Button btnBleConnectDevice;
    private DeviceStateListener deviceStateListener = new DeviceStateListener() {
        @Override
        public void onEvent(StateEvent
                                    e) {
            if (e.didEnter(BleDeviceState.UNDISCOVERED)) {
                Log.i(TAG, "UNDISCOVERED");
            }
            if (e.didEnter(BleDeviceState.RECONNECTING_LONG_TERM)) {
                Log.i(TAG, "RECONNECTING_LONG_TERM");
            }
            if (e.didEnter(BleDeviceState.RECONNECTING_SHORT_TERM)) {
                Log.i(TAG, "RECONNECTING_SHORT_TERM");
            }
            if (e.didEnter(BleDeviceState.RETRYING_BLE_CONNECTION)) {
                Log.i(TAG, "RETRYING_BLE_CONNECTION");
            }
            if (e.didEnter(BleDeviceState.ADVERTISING)) {
                Log.i(TAG, "ADVERTISING");
            }
            if (e.didEnter(BleDeviceState.DISCONNECTED)) {
                Log.i(TAG, "DISCONNECTED");
            }
            if (e.didEnter(BleDeviceState.UNBONDED)) {
                Log.i(TAG, "UNBONDED");
            }
            if (e.didEnter(BleDeviceState.BONDING)) {
                Log.i(TAG, "BONDING");
            }
            if (e.didEnter(BleDeviceState.BONDED)) {
                Log.i(TAG, "BONDED");
            }
            if (e.didEnter(BleDeviceState.CONNECTING_OVERALL)) {
                Log.i(TAG, "CONNECTING_OVERALL");
            }
            if (e.didEnter(BleDeviceState.CONNECTING)) {
                Log.i(TAG, "CONNECTING");
                showBleConnectionProgressDialog();
            }
            if (e.didEnter(BleDeviceState.CONNECTED)) {
                Log.i(TAG, "CONNECTED");
            }
            if (e.didEnter(BleDeviceState.DISCOVERING_SERVICES)) {
                Log.i(TAG, "DISCOVERING_SERVICES");
            }
            if (e.didEnter(BleDeviceState.SERVICES_DISCOVERED)) {
                Log.i(TAG, "SERVICES_DISCOVERED");
            }
            if (e.didEnter(BleDeviceState.AUTHENTICATING)) {
                Log.i(TAG, "AUTHENTICATING");
            }
            if (e.didEnter(BleDeviceState.AUTHENTICATED)) {
                Log.i(TAG, "AUTHENTICATED");
            }
            if (e.didEnter(BleDeviceState.INITIALIZING)) {
                Log.i(TAG, "INITIALIZING");
            }
            if (e.didEnter(BleDeviceState.INITIALIZED)) {
                Log.i(TAG, "INITIALIZED");

                bleConnectionProgressDialog.dismiss();

//                e.device().getNativeServices_List().contains()

                tvGpsMessageDevice.setVisibility(View.GONE);
                tvLoraMessageDevice.setVisibility(View.GONE);
                setGpsCardFieldsVisibility(View.VISIBLE);
                setLoraCardFieldsVisibility(View.VISIBLE);

                /*--------------------------------------
                 *      LoRa Service
                 *-------------------------------------*/
                readBleLoraCharacteristics();

                /*--------------------------------------
                 *      GPS Service
                 *-------------------------------------*/
                readBleGpsCharacteristics();

                showBleReadWriteProgressDialog("GPS and LoRa characteristics",
                        "Please be patient, the read command can take a few seconds to retrieve results...",
                        LORA_BLE_SERVICE_NUMBER_OF_CHARACTERISTICS +
                                GPS_BLE_SERVICE_NUMBER_OF_CHARACTERISTICS);
            }
            if (e.didEnter(BleDeviceState.PERFORMING_OTA)) {
                Log.i(TAG, "PERFORMING_OTA");
            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);

        // This class helps you navigate the treacherous waters of Android M Location requirements for scanning.
        // First it enables bluetooth itself, then location permissions, then location services. The latter two
        // are only needed in Android M. This must be called from an Activity instance.
        BluetoothEnabler.start(getActivity());

        ProgressDialog bleProgressDialog = new ProgressDialog(getActivity());

        mBleManager = BleManager.get(getActivity());
        mBleManager.setConfig(mBleManagerConfig);
    }


    private void initSharedPreferences() {

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(getActivity());
        mToken = mSharedPreferences.getString(Constants.TOKEN, "");
        mUsername = mSharedPreferences.getString(Constants.USERNAME, "");
    }

    private void extractTokenInformation() {
        try {
            mJwt = new JWT(mToken);
        } catch (DecodeException exception) {

        }

        mUserId = mJwt.getClaim("public_id").asString();
        mTokenExpiresAt = mJwt.getExpiresAt();
    }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_ble_connect, container, false);
        mSubscriptions = new CompositeSubscription();
        setHasOptionsMenu(true);
        initViews(view);
        initSharedPreferences();
        extractTokenInformation();
        getDataFromArguments();

        loadDevice();

//        mBleDevice.setListener_Bond(new BleDevice.BondListener() {
//            @Override
//            public void onEvent(BondEvent bondEvent) {
//
//                showSnackBarMessage("BOND EVENT WITH CODE : " + bondEvent.failReason());
//                Log.i(TAG, "BOND EVENT WITH CODE : " + bondEvent.failReason());
//7
////                mBleManager.reset();
//            }
//        });

        mBleDevice.setListener_State(deviceStateListener);
        btnBleConnectDevice.setOnClickListener((View view1) -> mBleDevice.connect(
                connectionFailEvent -> {
                    bleConnectionProgressDialog.dismiss();
                    showSnackBarMessage("Connection fail event with number : "
                            + connectionFailEvent.bondFailReason());
                    return null;
                })
        );


        btnBleLoraDownloadDevice.setOnClickListener(view14 -> {
            readBleLoraCharacteristics();
            showBleReadWriteProgressDialog("LoRa characteristics",
                    "Please be patient, the read command can take a few seconds to retrieve results...",
                    LORA_BLE_SERVICE_NUMBER_OF_CHARACTERISTICS);
        });

        btnBleLoraUploadDevice.setOnClickListener(view16 -> {
        });

        btnDownloadServ.setOnClickListener(view15 -> loadDevice());

        btnUploadServ.setOnClickListener(view15 -> {
            // TODO : Implement a dialog to create or modify a device on the server
        });

        btnBleGpsDownloadDevice.setOnClickListener(view13 -> {
            readBleGpsCharacteristics();
            showBleReadWriteProgressDialog("GPS characteristics",
                    "Please be patient, the read command can take a few seconds to retrieve results...",
                    GPS_BLE_SERVICE_NUMBER_OF_CHARACTERISTICS);
        });


        swGpsEnableNotifications.setOnClickListener(view12 -> {
            if (swGpsEnableNotifications.isChecked()) {
                enableBleGpsNotifications();
            } else {
                disableBleGpsNotifications();
            }
        });

        return view;
    }


    private void showBleReadWriteProgressDialog(String title, String message, int maxValue) {
        bleReadWriteProgressDialog = new ProgressDialog(getContext());
        bleReadWriteProgressDialog.setIndeterminate(false);
        bleReadWriteProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);

        bleReadWriteProgressDialog.setTitle(title);
        bleReadWriteProgressDialog.setMessage(message);

        bleReadWriteProgressDialog.setMax(maxValue);
        bleReadWriteProgressDialog.show();

        new Thread(() -> {
            try {
                while (bleReadWriteProgressDialog.getProgress() <= bleReadWriteProgressDialog
                        .getMax()) {
                    Thread.sleep(100);
                    if (bleReadWriteProgressDialog.getProgress() == bleReadWriteProgressDialog
                            .getMax()) {
                        bleReadWriteProgressDialog.dismiss();
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    private void showBleConnectionProgressDialog() {
        bleConnectionProgressDialog = new ProgressDialog(getContext());
        bleConnectionProgressDialog.setIndeterminate(true);

        bleConnectionProgressDialog.setTitle("BLE connection in progress");
        bleConnectionProgressDialog.setMessage("Please be patient, the connection can take a few seconds to be set...");

        bleConnectionProgressDialog.show();

        new Thread(() -> {
            try {
                while (bleConnectionProgressDialog.getProgress() <= bleConnectionProgressDialog
                        .getMax()) {
                    Thread.sleep(100);
                    if (bleConnectionProgressDialog.getProgress() == bleConnectionProgressDialog
                            .getMax()) {
                        bleConnectionProgressDialog.dismiss();
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    private void initViews(View view) {

        cvDeviceServer = view.findViewById(R.id.card_serv);
        cvDeviceBleGps = view.findViewById(R.id.card_ble_gps);
        cvDeviceBLELora = view.findViewById(R.id.card_ble_lora);

        btnBleConnectDevice = view.findViewById(R.id.btn_ble_connect_device);

        tvCvTitleServ = view.findViewById(R.id.tv_cv_title_serv);
        tvMessageServ = view.findViewById(R.id.tv_message_serv);
        tvBleTitleServ = view.findViewById(R.id.tv_ble_title_serv);
        btnUploadServ = view.findViewById(R.id.btn_upload_serv);
        btnDownloadServ = view.findViewById(R.id.btn_download_serv);
        tvBleMacAddrServ = view.findViewById(R.id.tv_ble_mac_address_serv);
        tvBlePasskeyServ = view.findViewById(R.id.tv_ble_passkey_serv);
        tvLoRaTitleServ = view.findViewById(R.id.tv_lora_title_serv);
        tvDevEuiServ = view.findViewById(R.id.tv_dev_eui_serv);
        tvAppEuiServ = view.findViewById(R.id.tv_app_eui_serv);
        tvAppKeyServ = view.findViewById(R.id.tv_app_key_serv);
        tvDeviceOwnerTitleServ = view.findViewById(R.id.tv_device_owner_title_serv);
        tvPublicIdServ = view.findViewById(R.id.tv_owner_public_id_serv);

        cvDeviceBLELora = view.findViewById(R.id.card_ble_lora);
        tvCvTitleBleLora = view.findViewById(R.id.tv_cv_title_ble_lora);
        btnBleLoraDownloadDevice = view.findViewById(R.id.btn_ble_lora_download_device);
        btnBleLoraUploadDevice = view.findViewById(R.id.btn_ble_lora_upload_device);
        tvLoraMessageDevice = view.findViewById(R.id.tv_lora_message_device);
        tvLoraDevEuiDevice = view.findViewById(R.id.tv_lora_dev_eui_device);
        tvLoraAppEuiDevice = view.findViewById(R.id.tv_lora_app_eui_device);
        tvLoraNetworkJoinStatusDevice = view.findViewById(R.id.tv_lora_network_join_status_device);
        tvLoraDeviceAddressDevice = view.findViewById(R.id.tv_lora_device_address_device);

        cvDeviceBleGps = view.findViewById(R.id.card_ble_gps);
        tvCvTitleBleGps = view.findViewById(R.id.tv_cv_title_ble_gps);
        btnBleGpsDownloadDevice = view.findViewById(R.id.btn_ble_gps_download_device);
        rlBleGpsNotifsSwitch = view.findViewById(R.id.rl_ble_gps_notifs_switch);
        swGpsEnableNotifications = view.findViewById(R.id.sw_gps_enable_notifications);
        tvGpsMessageDevice = view.findViewById(R.id.tv_gps_message_device);
        tvGpsServicePositionDevice = view.findViewById(R.id.tv_gps_service_position_device);
        tvGpsServiceSpeedDevice = view.findViewById(R.id.tv_gps_service_speed_device);
        tvGpsServiceCourseDevice = view.findViewById(R.id.tv_gps_service_course_device);
        tvGpsServiceDateDevice = view.findViewById(R.id.tv_gps_service_date_device);
        tvGpsServiceTimeDevice = view.findViewById(R.id.tv_gps_service_time_device);

        /* Start the app by masking the 2 cards
         * If the device as been found on the server, the cards will be shown
         */
        setGpsCardFieldsVisibility(View.GONE);
        setLoraCardFieldsVisibility(View.GONE);
    }

    private void setGpsCardFieldsVisibility(int visibility) {
        rlBleGpsNotifsSwitch.setVisibility(visibility);
        btnBleGpsDownloadDevice.setVisibility(visibility);
        tvGpsServicePositionDevice.setVisibility(visibility);
        tvGpsServiceSpeedDevice.setVisibility(visibility);
        tvGpsServiceCourseDevice.setVisibility(visibility);
        tvGpsServiceDateDevice.setVisibility(visibility);
        tvGpsServiceTimeDevice.setVisibility(visibility);
    }

    private void setLoraCardFieldsVisibility(int visibility) {
        tvLoraDevEuiDevice.setVisibility(visibility);
        tvLoraAppEuiDevice.setVisibility(visibility);
        btnBleLoraUploadDevice.setVisibility(visibility);
        btnBleLoraDownloadDevice.setVisibility(visibility);
        tvLoraNetworkJoinStatusDevice.setVisibility(visibility);
        tvLoraDeviceAddressDevice.setVisibility(visibility);
    }

    private void getDataFromArguments() {

        Bundle bundle = getArguments();
        String ble_mac_addr = bundle.getString(Constants.BLE_DEVICE_MAC);
        if (ble_mac_addr != null) {
            mBleDevice = mBleManager.getDevice(ble_mac_addr);
        }
    }

    private void loadDevice() {
        String macAddrURI = mBleDevice.getMacAddress().replaceAll(":", "");
        mSubscriptions.add(NetworkUtil.getRetrofit(mToken).getDevice(macAddrURI)
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.io())
                .subscribe(this::handleResponse, this::handleError));

        mProgressDialog = new ProgressDialog(getActivity());
        mProgressDialog.setTitle("Loading");
        mProgressDialog.setMessage("Contacting server for device information...");
        mProgressDialog.setCancelable(false); // disable dismiss by tapping outside of the dialog
        mProgressDialog.show();
    }

    private void handleResponse(SmartCantonDevBoxDevice device) {

        mProgressDialog.dismiss();

        tvMessageServ.setVisibility(View.GONE);

        // If we want double colors on the same text
//        tvBleMacAddrServ.setText( Html.fromHtml(
//                getColoredSpanned("Mac Address : %s",
//                        getResources().getColor(R.color.colorTextPrimaryLightDark)) +
//                        getColoredSpanned(device.getBleMacAddr(),
//                                getResources().getColor(R.color.colorTextPrimaryDark))));

        tvBleMacAddrServ.setText(String.format("Mac Address : %s", device.getBleMacAddr()));
        tvBlePasskeyServ.setText(String.format("Passkey : %s", device.getBlePassKey()));
        tvDevEuiServ.setText(String.format("DevEUI : %s", device.getDevEui()));
        tvAppEuiServ.setText(String.format("AppEUI : %s", device.getAppEui()));
        tvAppKeyServ.setText(String.format("AppKey : %s", device.getAppKey()));
        tvPublicIdServ.setText(String.format("Owner ID : %s", device.getOwnerPublicId()));
    }

    private void handleError(Throwable error) {

        mProgressDialog.dismiss();

        setGpsCardFieldsVisibility(View.GONE);

        tvMessageServ.setVisibility(View.VISIBLE);
        tvMessageServ.setText(R.string.cv_device_not_found_msg);

        setLoraCardFieldsVisibility(View.GONE);

        tvLoRaTitleServ.setVisibility(View.GONE);
        tvDeviceOwnerTitleServ.setVisibility(View.GONE);
        tvBleMacAddrServ.setVisibility(View.GONE);
        tvBlePasskeyServ.setVisibility(View.GONE);
        tvBleTitleServ.setVisibility(View.GONE);
        tvDevEuiServ.setVisibility(View.GONE);
        tvAppEuiServ.setVisibility(View.GONE);
        tvAppKeyServ.setVisibility(View.GONE);
        tvPublicIdServ.setVisibility(View.GONE);

        if (error instanceof HttpException) {

            Gson gson = new GsonBuilder().create();

            try {

                String errorBody = ((HttpException) error).response().errorBody().string();
                Response response = gson.fromJson(errorBody, Response.class);
                showSnackBarMessage(response.getMessage());

            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {

            showSnackBarMessage("Network Error !");
        }
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);


    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super.onCreateOptionsMenu(menu, inflater);
    }


    @Override
    public void onResume() {
        super.onResume();
        ((AppCompatActivity) getActivity()).getSupportActionBar().setTitle("Device");
        BleManager.get(getActivity()).onResume();
        getActivity().invalidateOptionsMenu();
    }

    @Override
    public void onPause() {
        super.onPause();
        BleManager.get(getActivity()).onPause();
    }

    @Override
    public void onPrepareOptionsMenu(Menu menu) {
        super.onPrepareOptionsMenu(menu);
    }

    private void showSnackBarMessage(String message) {

        View view = getView();
        if (view != null) {
            Snackbar.make(getView(), message, Snackbar.LENGTH_LONG).show();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mBleManager.disconnectAll();
    }

    private String getColoredSpanned(String text, int color) {
        String colorHex = "#" + Integer.toHexString(color & 0x00ffffff);
        String input = "<font color=" + color + ">" + text + "</font>";
        return input;
    }

    private void enableBleGpsNotifications() {
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_POSITION,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServicePositionDevice.setText(
                                    String.format("Position : %s", e1.data_utf8()));
                    }
                });
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SPEED,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServiceSpeedDevice.setText(
                                    String.format("Speed : %s", e1.data_utf8()));
                    }
                });
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_COURSE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServiceCourseDevice.setText(
                                    String.format("Course : %s", e1.data_utf8()));
                    }
                });
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServiceDateDevice.setText(
                                    String.format("Date : %s", e1.data_utf8()));
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServiceDateDevice.setText(
                                    String.format("Date : %s", e1.data_utf8()));
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_TIME,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServiceTimeDevice.setText(
                                    String.format("Time : %s", e1.data_utf8()));
                    }
                });
    }

    private void disableBleGpsNotifications() {
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_POSITION);

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SPEED);

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_COURSE);

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE);

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE);

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_TIME);
    }

    private void readBleGpsCharacteristics() {
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_POSITION,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvGpsServicePositionDevice.setText(
                                String.format("Position : %s", e1.data_utf8()));
                    } else if (e1.status() == Status.EMPTY_DATA) {
                        tvGpsServicePositionDevice.setText(
                                String.format("Position : %s", "<no signal yet>"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SPEED,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvGpsServiceSpeedDevice.setText(
                                String.format("Speed : %s", e1.data_utf8()));
                    } else if (e1.status() == Status.EMPTY_DATA) {
                        tvGpsServiceSpeedDevice.setText(
                                String.format("Speed : %s", "<no signal yet>"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_COURSE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvGpsServiceCourseDevice.setText(
                                String.format("Course : %s", e1.data_utf8()));
                    } else if (e1.status() == Status.EMPTY_DATA) {
                        tvGpsServiceCourseDevice.setText(
                                String.format("Course : %s", "<no signal yet>"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvGpsServiceDateDevice.setText(
                                String.format("Date : %s", e1.data_utf8()));
                    } else if (e1.status() == Status.EMPTY_DATA) {
                        tvGpsServiceDateDevice.setText(
                                String.format("Date : %s", "<no signal yet>"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvGpsServiceDateDevice.setText(
                                String.format("Date : %s", e1.data_utf8()));
                    } else if (e1.status() == Status.EMPTY_DATA) {
                        tvGpsServiceDateDevice.setText(
                                String.format("Date : %s", "<no signal yet>"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_TIME,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvGpsServiceTimeDevice.setText(
                                String.format("Time : %s", e1.data_utf8()));
                    } else if (e1.status() == Status.EMPTY_DATA) {
                        tvGpsServiceTimeDevice.setText(
                                String.format("Time : %s", "<no signal yet>"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
    }

    private void readBleLoraCharacteristics() {
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_APP_EUI,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraAppEuiDevice.setText(String.format("App EUI : %s",
                                FormatConversions.bytesToHex(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_DEVICE_EUI,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraDevEuiDevice.setText(String.format("Dev EUI : %s",
                                FormatConversions.bytesToHex(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_NETWORK_DEVICE_ADDRESS,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraDeviceAddressDevice.setText(String.format("Dev Addr : %s",
                                FormatConversions.bytesToHex(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_NETWORK_NWTORK_SESSION_KEY,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraNetworkJoinStatusDevice.setText(String.format("Network Join : %s",
                                FormatConversions.bytesToHex(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
    }

    private void writeBleLoraCharacteristicsFromServer() {
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_APP_EUI,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraAppEuiDevice.setText(
                                String.format("App EUI : %s", e1.data_utf8()));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_DEVICE_EUI,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraDevEuiDevice.setText(
                                String.format("Dev EUI : %s", e1.data_utf8()));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_NETWORK_DEVICE_ADDRESS,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraDeviceAddressDevice.setText(
                                String.format("Dev Addr : %s", e1.data_utf8()));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_NETWORK_NWTORK_SESSION_KEY,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraNetworkJoinStatusDevice.setText(
                                String.format("Network Join : %s", e1.data_byte()));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
    }

}


