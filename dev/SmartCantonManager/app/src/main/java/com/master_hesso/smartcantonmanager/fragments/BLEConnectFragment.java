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

import android.app.Fragment;
import android.app.ProgressDialog;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v7.widget.CardView;
import android.text.Html;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.auth0.android.jwt.DecodeException;
import com.auth0.android.jwt.JWT;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.idevicesinc.sweetblue.BleDevice;
import com.idevicesinc.sweetblue.BleManager;
import com.idevicesinc.sweetblue.BleManagerConfig;
import com.idevicesinc.sweetblue.BleScanApi;
import com.idevicesinc.sweetblue.utils.BluetoothEnabler;
import com.idevicesinc.sweetblue.utils.BluetoothEnabler.DefaultBluetoothEnablerFilter;
import com.master_hesso.smartcantonmanager.R;
import com.master_hesso.smartcantonmanager.model.Response;
import com.master_hesso.smartcantonmanager.model.SmartCantonDevBoxDevice;
import com.master_hesso.smartcantonmanager.network.NetworkUtil;
import com.master_hesso.smartcantonmanager.utils.Constants;

import java.io.IOException;
import java.util.Date;

import retrofit2.adapter.rxjava.HttpException;
import rx.android.schedulers.AndroidSchedulers;
import rx.schedulers.Schedulers;
import rx.subscriptions.CompositeSubscription;


/**
 * Scans for Bluetooth Low Energy Advertisements matching a filter and displays them to the user.
 */
public class BLEConnectFragment extends Fragment {

    public static final String TAG = BLEConnectFragment.class.getSimpleName();

    private BleDevice bleDevice = null;

    private CompositeSubscription mSubscriptions;

    private ProgressDialog mProgressDialog;

    private SharedPreferences mSharedPreferences;
    private String mToken;
    private String mUsername;
    private String mUserId;
    private Date mTokenExpiresAt;
    private JWT mJwt;

    private TextView tvCardViewTitle;
    private TextView tvBleTitleServ;
    private TextView tvBleMacAddrServ;
    private TextView tvBlePasskeyServ;
    private TextView tvLoRaTitleServ;
    private TextView tvDevEuiServ;
    private TextView tvAppEuiServ;
    private TextView tvAppKeyServ;
    private TextView tvDeviceOwnerTitleServ;
    private TextView tvPublicIdServ;
    private CardView cvDeviceBLE;
    private CardView cvDeviceServer;
    private Button btnBleConnectDevice;

    /**
     * SweetBlue BLE configuration
     */
    private final BleManagerConfig mBleManagerConfig = new BleManagerConfig() {{
        this.scanApi = BleScanApi.POST_LOLLIPOP;
        this.stopScanOnPause = true;
    }};

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);
    }


    private void initSharedPreferences() {

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(getActivity());
        mToken = mSharedPreferences.getString(Constants.TOKEN,"");
        mUsername = mSharedPreferences.getString(Constants.USERNAME,"");
    }

    private void extractTokenInformation() {
        try {
            mJwt = new JWT(mToken);
        } catch (DecodeException exception){

        }

        mUserId = mJwt.getClaim("public_id").asString();
        mTokenExpiresAt = mJwt.getExpiresAt();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_ble_connect,container,false);
        mSubscriptions = new CompositeSubscription();
        setHasOptionsMenu(true);
        initViews(view);
        initSharedPreferences();
        extractTokenInformation();
        getDataFromArguments();

        loadDevice();

        return view;
    }

    private void initViews(View view) {

        cvDeviceServer = view.findViewById(R.id.card_serv);
        cvDeviceBLE = view.findViewById(R.id.card_ble);
        cvDeviceBLE.setVisibility(View.GONE);

        btnBleConnectDevice = view.findViewById(R.id.btn_ble_connect_device);

        tvCardViewTitle = view.findViewById(R.id.tv_cv_title_serv);
        tvBleTitleServ = view.findViewById(R.id.tv_ble_title_serv);
        tvBleMacAddrServ  = view.findViewById(R.id.tv_ble_mac_address_serv);
        tvBlePasskeyServ  = view.findViewById(R.id.tv_ble_passkey_serv);
        tvLoRaTitleServ  = view.findViewById(R.id.tv_lora_title_serv);
        tvDevEuiServ = view.findViewById(R.id.tv_dev_eui_serv);
        tvAppEuiServ = view.findViewById(R.id.tv_app_eui_serv);
        tvAppKeyServ = view.findViewById(R.id.tv_app_key_serv);
        tvDeviceOwnerTitleServ = view.findViewById(R.id.tv_device_owner_title_serv);
        tvPublicIdServ = view.findViewById(R.id.tv_owner_public_id_serv);



//        mBtChangePassword.setOnClickListener(view -> showDialog());
//        mBtLogout.setOnClickListener(view -> logout());
    }

    private void getDataFromArguments() {

        Bundle bundle = getArguments();
        String ble_mac_addr = bundle.getString(Constants.BLE_DEVICE_MAC);
        if(ble_mac_addr != null) {
            bleDevice = BleManager.get(getActivity()).getDevice(ble_mac_addr);
        } else {

        }

    }

    private void loadDevice() {
        String macAddrURI = bleDevice.getMacAddress().replaceAll(":", "");
        mSubscriptions.add(NetworkUtil.getRetrofit(mToken).getDevice(macAddrURI)
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.io())
                .subscribe(this::handleResponse,this::handleError));

        mProgressDialog = new ProgressDialog(getActivity());
        mProgressDialog.setTitle("Loading");
        mProgressDialog.setMessage("Contacting server for device information...");
        mProgressDialog.setCancelable(false); // disable dismiss by tapping outside of the dialog
        mProgressDialog.show();
    }

    private void handleResponse(SmartCantonDevBoxDevice device) {

        mProgressDialog.dismiss();


        tvBleMacAddrServ.setText(String.format("Mac Address : %s", device.getBleMacAddr()));

        tvBleMacAddrServ.setText( Html.fromHtml(
                getColoredSpanned("Mac Address : %s",
                        getResources().getColor(R.color.colorTextPrimaryLightDark)) +
                        getColoredSpanned(device.getBleMacAddr(),
                        getResources().getColor(R.color.colorTextPrimaryDark))));

        tvBlePasskeyServ.setText(String.format("Passkey : %s", device.getBlePassKey()));
        tvDevEuiServ.setText(String.format("DevEUI : %s", device.getDevEui()));
        tvAppEuiServ.setText(String.format("AppEUI : %s", device.getAppEui()));
        tvAppKeyServ.setText(String.format("AppKey : %s", device.getAppKey()));
        tvPublicIdServ.setText(String.format("Owner ID : %s", device.getOwnerPublicId()));

        // This class helps you navigate the treacherous waters of Android M Location requirements for scanning.
        // First it enables bluetooth itself, then location permissions, then location services. The latter two
        // are only needed in Android M. This must be called from an Activity instance.
        BluetoothEnabler.start(getActivity(), new DefaultBluetoothEnablerFilter()
        {
            @Override public Please onEvent(BluetoothEnablerEvent e)
            {
                if( e.isDone() )
                {
                    bleDevice.connect();
                }
                return super.onEvent(e);
            }
        });
    }

    private void handleError(Throwable error) {

        mProgressDialog.dismiss();

        tvCardViewTitle.setText(R.string.cv_device_not_found_title);
        tvBleTitleServ.setText(R.string.cv_device_not_found_msg);
        tvLoRaTitleServ.setVisibility(View.GONE);
        tvDeviceOwnerTitleServ.setVisibility(View.GONE);
        tvBleMacAddrServ.setVisibility(View.GONE);
        tvBlePasskeyServ.setVisibility(View.GONE);
        tvDevEuiServ.setVisibility(View.GONE);
        tvAppEuiServ.setVisibility(View.GONE);
        tvAppKeyServ.setVisibility(View.GONE);
        tvPublicIdServ.setVisibility(View.GONE);

        btnBleConnectDevice.setEnabled(false);

        if (error instanceof HttpException) {

            Gson gson = new GsonBuilder().create();

            try {

                String errorBody = ((HttpException) error).response().errorBody().string();
                Response response = gson.fromJson(errorBody,Response.class);
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
        if(view !=null) {
            Snackbar.make(getView(),message,Snackbar.LENGTH_LONG).show();
        }
    }

    private String getColoredSpanned(String text, int color) {
        String colorHex = "#" + Integer.toHexString(color & 0x00ffffff);
        String input = "<font color=" + color + ">" + text + "</font>";
        return input;
    }
}
