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

package com.master_hesso.smartcantonmanager.fragments;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.graphics.Typeface;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v4.app.Fragment;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.CardView;
import android.util.Log;
import android.util.TypedValue;
import android.view.ContextThemeWrapper;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;

import com.auth0.android.jwt.DecodeException;
import com.auth0.android.jwt.JWT;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.idevicesinc.sweetblue.BleDevice;
import com.idevicesinc.sweetblue.BleDevice.ConnectionFailListener;
import com.idevicesinc.sweetblue.BleDeviceState;
import com.idevicesinc.sweetblue.BleManager;
import com.idevicesinc.sweetblue.BleManagerConfig;
import com.idevicesinc.sweetblue.BleNode;
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
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.Date;
import java.util.Locale;

import retrofit2.adapter.rxjava.HttpException;
import rx.android.schedulers.AndroidSchedulers;
import rx.schedulers.Schedulers;
import rx.subscriptions.CompositeSubscription;

import static com.idevicesinc.sweetblue.BleDevice.ReadWriteListener.Status;
import static com.idevicesinc.sweetblue.BleDevice.StateListener.StateEvent;


/**
 * Connect to a SmartCanton Dev Box and display the services available. At the same time, contact
 * the server to know all the parameters of the connected device.
 */
public class BLEConnectFragment extends Fragment {

    public static final String TAG = BLEConnectFragment.class.getSimpleName();

    private static final int GPS_BLE_SERVICE_NUMBER_OF_READ_CHARACTERISTICS = 5;
    private static final int LORA_BLE_SERVICE_NUMBER_OF_READ_CHARACTERISTICS = 4;
    private static final int LORA_BLE_SERVICE_NUMBER_OF_WRITE_CHARACTERISTICS = 3;
    /**
     * SweetBlue BLE configuration
     */
    private final BleManagerConfig mBleManagerConfig = new BleManagerConfig() {{
        this.forceBondDialog = true;

//        this.connectFailRetryConnectingOverall = true;
//        this.alwaysUseAutoConnect = true;
//        this.bondingFailFailsConnection = true;
//        this.connectFailRetryConnectingOverall = true;
        this.stopScanOnPause = true;
    }};

    private ProgressDialog bleReadWriteProgressDialog;
    private ProgressDialog bleConnectionProgressDialog;

    private SmartCantonDevBoxDevice smartCantonDevBoxDevice;

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

    private CardView cardBleBme680;
    private TextView tvCvTitleBme680;
    private RelativeLayout rlBleBme680NotifsSwitch;
    private Switch swBme680EnableNotifications;
    private TextView tvBme680MessageDevice;
    private TextView tvBme680ServiceIaqDevice;
    private TextView tvBme680ServiceIaqAccuracyDevice;
    private TextView tvBme680ServiceTemperatureDevice;
    private TextView tvBme680ServiceHumidityDevice;
    private TextView tvBme680ServicePressureDevice;
    private TextView tvBme680ServiceRawTemperatureDevice;
    private TextView tvBme680ServiceRawHumidityDevice;
    private TextView tvBme680ServiceRawGasDevice;
    private TextView tvBno055ServiceMeasureDelayDevice;

    private CardView cardBleBno055;
    private TextView tvCvTitleBno055;
    private ImageButton btnBno055BleWrite;
    private RelativeLayout rlBleBno055NotifsSwitch;
    private Switch swBno055EnableNotifications;
    private TextView tvBno055MessageDevice;
    private TextView tvBno055ServiceAccelerometerDevice;
    private TextView tvBno055ServiceGyroscopeDevice;
    private TextView tvBno055ServiceMagnetometerDevice;

    private CardView cardBleScan;
    private TextView tvCvTitleBleScan;
    private ImageButton btnBleScanBleWrite;
    private RelativeLayout rlBleScanNotifsSwitch;
    private Switch swBleScanEnableNotifications;
    private TextView tvBleScanMessageDevice;
    private TextView tvBleScanServiceDevicesScannedDevice;
    private TextView tvBleScanServiceScanWindowDevice;

    private Button btnBleConnectDevice;

    private boolean bleScannedFirstBuggedDataRead = false;

    /**
     * Listener that is used when a connected is started. It will force the connection to the device
     * and display if the connection has failed.
     */
    ConnectionFailListener connectionFailListener = connectionFailEvent -> {
        /* TOFIX :
         * The Gatt Status 133 error is a strange error that has been happening in android
         * since the last versions. A thread on this error for the CC2650 can be found here :
         * https://e2e.ti.com/support/wireless_connectivity/bluetooth_low_energy/f/538/t/534737
         * There is no real fix, the only way to fix it is if it fail, try again...
         * Some people say that it works better with AutoConnect set to try. So we activate it
         * for the next retry
         */
        if (connectionFailEvent.gattStatus() == 133) {
            return BleNode.ConnectionFailListener.Please.retryWithAutoConnectTrue();
        } else {
//                bleConnectionProgressDialog.dismiss();
//                BLEConnectFragment.this.showSnackBarMessage("Connection fail event with number : "
//                        + connectionFailEvent.bondFailReason());
//                mBleDevice.disconnect();
            return BleNode.ConnectionFailListener.Please.retryWithAutoConnectTrue();
        }
    };

    /**
     * This listener is used when the app is the disconnected state and that the button
     * is not set to be able to connect to the device.
     */
    View.OnClickListener listenerConnectMode = view1 -> {
        // If there is no PassKey registered, do not inform the user
        if (smartCantonDevBoxDevice.getBlePassKey().isEmpty()) {
            mBleDevice.connect(connectionFailListener);
        } else {
            // Display a dialog with the PassKey to be used
            BLEConnectFragment.this.showBleConnectionConfirmationDialog((dialogInterface, i) -> {
                dialogInterface.dismiss();
                /* Connect to the device and handle failing connections */
                mBleDevice.connect(connectionFailListener);
            });
        }
    };

    /**
     * This listener is used when the app is the connected state and that the button
     * is not set to be able to disconnect from the device.
     */
    View.OnClickListener listenerDisconnectMode = view1 -> mBleDevice.disconnect();

    /**
     * This listener contain the main state machine of the fragment. Every stage on the connection
     * will be logged for debug if needed. It here that the fragment will change his state (connected
     * or disconnected).
     */
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
                tvGpsMessageDevice.setVisibility(View.VISIBLE);
                tvLoraMessageDevice.setVisibility(View.VISIBLE);
                tvBme680MessageDevice.setVisibility(View.VISIBLE);
                tvBno055MessageDevice.setVisibility(View.VISIBLE);
                tvBleScanMessageDevice.setVisibility(View.VISIBLE);
                setGpsCardFieldsVisibility(View.GONE);
                setLoraCardFieldsVisibility(View.GONE);
                setBme680CardFieldsVisibility(View.GONE);
                setBno055CardFieldsVisibility(View.GONE);
                setBleScannerCardFieldsVisibility(View.GONE);
                setBtnBleConnectDeviceToConnectMode();
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

                setBtnBleConnectDeviceToDisconnectMode();

                bleConnectionProgressDialog.dismiss();

                // TODO : Only show supported services cards
//                e.device().getNativeServices_List().contains()

                tvGpsMessageDevice.setVisibility(View.GONE);
                tvLoraMessageDevice.setVisibility(View.GONE);
                tvBme680MessageDevice.setVisibility(View.GONE);
                tvBno055MessageDevice.setVisibility(View.GONE);
                tvBleScanMessageDevice.setVisibility(View.GONE);
                setGpsCardFieldsVisibility(View.VISIBLE);
                setLoraCardFieldsVisibility(View.VISIBLE);
                setBme680CardFieldsVisibility(View.VISIBLE);
                setBno055CardFieldsVisibility(View.VISIBLE);
                setBleScannerCardFieldsVisibility(View.VISIBLE);

                /*--------------------------------------
                 *      LoRa Service
                 *-------------------------------------*/
                readBleLoraCharacteristics();

                /*--------------------------------------
                 *      GPS Service
                 *-------------------------------------*/
                readBleGpsCharacteristics();

                showBleReadWriteProgressDialog("Reading GPS and LoRa characteristics",
                        "Please be patient, the read command can take a few seconds to retrieve results...",
                        LORA_BLE_SERVICE_NUMBER_OF_READ_CHARACTERISTICS +
                                GPS_BLE_SERVICE_NUMBER_OF_READ_CHARACTERISTICS);
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

        mBleManager = BleManager.get(getActivity());
        mBleManager.setConfig(mBleManagerConfig);
    }


    /**
     * Retrieve the token and username information from the shared preferences
     */
    private void initSharedPreferences() {

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(getActivity());
        mToken = mSharedPreferences.getString(Constants.TOKEN, "");
        mUsername = mSharedPreferences.getString(Constants.USERNAME, "");
    }

    /**
     * The token is parser to be able to retrieve the public_id information and the token expiration
     * time.
     */
    private void extractTokenInformation() {
        try {
            mJwt = new JWT(mToken);
        } catch (DecodeException ignored) {

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

        mBleDevice.setListener_State(deviceStateListener);

        setBtnBleConnectDeviceToConnectMode();

        btnBleLoraDownloadDevice.setOnClickListener(view14 -> {
            readBleLoraCharacteristics();
            showBleReadWriteProgressDialog("Reading LoRa characteristics",
                    "Please be patient, the read command can take a few seconds to retrieve results...",
                    LORA_BLE_SERVICE_NUMBER_OF_READ_CHARACTERISTICS);

            setBtnBleConnectDeviceToDisconnectMode();
        });

        btnBleLoraUploadDevice.setOnClickListener(view16 -> {
            writeBleLoraCharacteristicsFromServer();
            showBleReadWriteProgressDialog("Writing LoRa configuration",
                    "Please be patient, the write command can take a few seconds to finish his process...",
                    LORA_BLE_SERVICE_NUMBER_OF_WRITE_CHARACTERISTICS);
        });

        btnDownloadServ.setOnClickListener(view15 -> loadDevice());

        btnUploadServ.setOnClickListener(view15 -> {
            // TODO : Implement a dialog to create or modify a device on the server
        });

        btnBleGpsDownloadDevice.setOnClickListener(view13 -> {
            readBleGpsCharacteristics();
            showBleReadWriteProgressDialog("Reading GPS characteristics",
                    "Please be patient, the read command can take a few seconds to retrieve results...",
                    GPS_BLE_SERVICE_NUMBER_OF_READ_CHARACTERISTICS);
        });

        swGpsEnableNotifications.setOnClickListener(view12 -> {
            if (swGpsEnableNotifications.isChecked()) {
                enableBleGpsNotifications();
            } else {
                disableBleGpsNotifications();
            }
        });

        swBme680EnableNotifications.setOnClickListener(view12 -> {
            if (swBme680EnableNotifications.isChecked()) {
                enableBleBme680Notifications();
            } else {
                disableBleBme680Notifications();
            }
        });

        btnBno055BleWrite.setOnClickListener(view1 -> showBno055WriteDialog());

        btnBleScanBleWrite.setOnClickListener(view1 -> showBleScannerWriteDialog());

        swBno055EnableNotifications.setOnClickListener(view12 -> {
            if (swBno055EnableNotifications.isChecked()) {
                readBleBno055MeasureInterval();
                enableBleBno055Notifications();
            } else {
                disableBleBno055Notifications();
            }
        });

        swBleScanEnableNotifications.setOnClickListener(view12 -> {
            if (swBleScanEnableNotifications.isChecked()) {
                readBleScannerWindow();
                enableBleScannerNotifications();
            } else {
                disableBleScannerNotifications();
            }
        });

        return view;
    }


    /**
     * Show a loading dialog to be displayed when data are been write/read to/from the device
     *
     * @param title    Dialog title
     * @param message  Message to display in the dialog
     * @param maxValue The number of characteristics been acceded
     */
    private void showBleReadWriteProgressDialog(String title, String message, int maxValue) {
        if (bleReadWriteProgressDialog != null) {
            bleReadWriteProgressDialog.dismiss();
        }

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

    /**
     * Display a dialog showing the Passkey to enter if a passkey is required by the system
     *
     * @param onClickListener A listener to be called once the user press OK on the dialog
     */
    private void showBleConnectionConfirmationDialog(DialogInterface.OnClickListener onClickListener) {

        AlertDialog.Builder alertDialog = new AlertDialog.Builder(new ContextThemeWrapper(
                getActivity(), R.style.AppTheme_Dialog_Alert));

        // Setting Dialog Title
        alertDialog.setTitle("Pairing Passkey Entry");

        LinearLayout linearLayout = new LinearLayout(getActivity());
        linearLayout.setOrientation(LinearLayout.VERTICAL);

        TextView message = new TextView(getActivity());
        TextView passKey = new TextView(getActivity());
        message.setText(R.string.passkey_dialog_msg);


        passKey.setText(smartCantonDevBoxDevice.getBlePassKey());
        passKey.setGravity(Gravity.CENTER_HORIZONTAL);
        passKey.setTextAppearance(getActivity(), R.style.TextAppearance_AppCompat_Large);
        passKey.setTextSize(TypedValue.COMPLEX_UNIT_DIP, 48);
        passKey.setTextColor(getResources().getColor(R.color.colorAccent));
        passKey.setTypeface(passKey.getTypeface(), Typeface.BOLD);

        message.setPadding((int) getResources().getDimension(R.dimen.card_padding),
                (int) getResources().getDimension(R.dimen.card_padding),
                (int) getResources().getDimension(R.dimen.card_padding),
                (int) getResources().getDimension(R.dimen.card_padding));
        passKey.setPadding((int) getResources().getDimension(R.dimen.card_padding),
                (int) getResources().getDimension(R.dimen.card_padding),
                (int) getResources().getDimension(R.dimen.card_padding),
                (int) getResources().getDimension(R.dimen.card_padding));

        linearLayout.addView(message);
        linearLayout.addView(passKey);

        // Setting Dialog Message
        alertDialog.setView(linearLayout);

        // Setting Icon to Dialog
        alertDialog.setIcon(R.drawable.ic_bluetooth_connect_white);

        // Setting Positive "Yes" Button
        alertDialog.setPositiveButton("Connect", onClickListener);

        // Setting Negative "NO" Button
        alertDialog.setNegativeButton("Cancel",
                (dialogInterface, i) -> dialogInterface.dismiss());

        // Showing Alert Message
        alertDialog.show();
    }

    /**
     * Display a dialog that can edit all information on the BNO055 BLE service
     */
    private void showBno055WriteDialog() {
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(new ContextThemeWrapper(
                getActivity(), R.style.AppTheme_Dialog_Alert));

        // Setting Dialog Title
        alertDialog.setTitle("Configure BNO055");

        View view = LayoutInflater.from(getContext()).inflate(R.layout.dialog_configure_bno055, null);
        EditText etBno055IntervalMs = view.findViewById(R.id.et_bno055_interval_ms);

        alertDialog.setView(view);

        // Setting Positive "Yes" Button
        alertDialog.setPositiveButton("Update", (dialogInterface, i) ->
                writeBleBno055MeasureInterval(Integer.parseInt(etBno055IntervalMs.getText().toString())));

        // Setting Negative "NO" Button
        alertDialog.setNegativeButton("Cancel",
                (dialogInterface, i) -> dialogInterface.dismiss());

        // Showing Alert Message
        alertDialog.show();
    }

    /**
     * Display a dialog that can edit all information on the Ble Scanner BLE service
     */
    private void showBleScannerWriteDialog() {
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(new ContextThemeWrapper(
                getActivity(), R.style.AppTheme_Dialog_Alert));

        // Setting Dialog Title
        alertDialog.setTitle("Configure Ble Scanner");

        View view = LayoutInflater.from(getContext()).inflate(R.layout.dialog_configure_ble, null);
        EditText etBleScanIntervalMs = view.findViewById(R.id.et_ble_scan_interval_ms);

        alertDialog.setView(view);

        // Setting Positive "Yes" Button
        alertDialog.setPositiveButton("Update", (dialogInterface, i) ->
                writeBleScannerWindow(Integer.parseInt(etBleScanIntervalMs.getText().toString())));

        // Setting Negative "NO" Button
        alertDialog.setNegativeButton("Cancel",
                (dialogInterface, i) -> dialogInterface.dismiss());

        // Showing Alert Message
        alertDialog.show();
    }


    /**
     * Display a dialog while the Ble Connection is in progress
     */
    private void showBleConnectionProgressDialog() {
        if (bleConnectionProgressDialog != null) {
            bleConnectionProgressDialog.dismiss();
        }

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

    /**
     * Extract all views from the current load layout
     *
     * @param view The current view displayed
     */
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

        cardBleBme680 = view.findViewById(R.id.card_ble_bme680);
        tvCvTitleBme680 = view.findViewById(R.id.tv_cv_title_bme680);
        rlBleBme680NotifsSwitch = view.findViewById(R.id.rl_ble_bme680_notifs_switch);
        swBme680EnableNotifications = view.findViewById(R.id.sw_bme680_enable_notifications);
        tvBme680MessageDevice = view.findViewById(R.id.tv_bme680_message_device);
        tvBme680ServiceIaqDevice = view.findViewById(R.id.tv_bme680_service_iaq_device);
        tvBme680ServiceIaqAccuracyDevice = view.findViewById(R.id.tv_bme680_service_iaq_accuracy_device);
        tvBme680ServiceTemperatureDevice = view.findViewById(R.id.tv_bme680_service_temperature_device);
        tvBme680ServiceHumidityDevice = view.findViewById(R.id.tv_bme680_service_humidity_device);
        tvBme680ServicePressureDevice = view.findViewById(R.id.tv_bme680_service_pressure_device);
        tvBme680ServiceRawTemperatureDevice = view.findViewById(R.id.tv_bme680_service_raw_temperature_device);
        tvBme680ServiceRawHumidityDevice = view.findViewById(R.id.tv_bme680_service_raw_humidity_device);
        tvBme680ServiceRawGasDevice = view.findViewById(R.id.tv_bme680_service_raw_gas_device);

        cardBleBno055 = view.findViewById(R.id.card_ble_bno055);
        tvCvTitleBno055 = view.findViewById(R.id.tv_cv_title_bno055);
        btnBno055BleWrite = view.findViewById(R.id.btn_bno055_ble_write);
        rlBleBno055NotifsSwitch = view.findViewById(R.id.rl_ble_bno055_notifs_switch);
        swBno055EnableNotifications = view.findViewById(R.id.sw_bno055_enable_notifications);
        tvBno055MessageDevice = view.findViewById(R.id.tv_bno055_message_device);
        tvBno055ServiceAccelerometerDevice = view.findViewById(R.id.tv_bno055_service_accelerometer_device);
        tvBno055ServiceGyroscopeDevice = view.findViewById(R.id.tv_bno055_service_gyroscope_device);
        tvBno055ServiceMagnetometerDevice = view.findViewById(R.id.tv_bno055_service_magnetometer_device);
        tvBno055ServiceMeasureDelayDevice = view.findViewById(R.id.tv_bno055_service_measure_delay_device);

        cardBleScan = view.findViewById(R.id.card_ble_scan);
        tvCvTitleBleScan = view.findViewById(R.id.tv_cv_title_ble_scan);
        btnBleScanBleWrite = view.findViewById(R.id.btn_ble_scan_ble_write);
        rlBleScanNotifsSwitch = view.findViewById(R.id.rl_ble_scan_notifs_switch);
        swBleScanEnableNotifications = view.findViewById(R.id.sw_ble_scan_enable_notifications);
        tvBleScanMessageDevice = view.findViewById(R.id.tv_ble_scan_message_device);
        tvBleScanServiceDevicesScannedDevice = view.findViewById(R.id.tv_ble_scan_service_devices_scanned_device);
        tvBleScanServiceScanWindowDevice = view.findViewById(R.id.tv_ble_scan_service_scan_window_device);


        /* Start the app by masking the 5 cards
         * If the device as been found on the server, the cards will be shown
         */
        setGpsCardFieldsVisibility(View.GONE);
        setLoraCardFieldsVisibility(View.GONE);
        setBme680CardFieldsVisibility(View.GONE);
        setBno055CardFieldsVisibility(View.GONE);
        setBleScannerCardFieldsVisibility(View.GONE);

    }

    /**
     * Change the visibility for all GPS fields
     *
     * @param visibility Visibility View.GONE or View.VISIBLE
     */
    private void setGpsCardFieldsVisibility(int visibility) {
        rlBleGpsNotifsSwitch.setVisibility(visibility);
        btnBleGpsDownloadDevice.setVisibility(visibility);
        tvGpsServicePositionDevice.setVisibility(visibility);
        tvGpsServiceSpeedDevice.setVisibility(visibility);
        tvGpsServiceCourseDevice.setVisibility(visibility);
        tvGpsServiceDateDevice.setVisibility(visibility);
        tvGpsServiceTimeDevice.setVisibility(visibility);
    }

    /**
     * Change the visibility for all BME680 fields
     *
     * @param visibility Visibility View.GONE or View.VISIBLE
     */
    private void setBme680CardFieldsVisibility(int visibility) {
        rlBleBme680NotifsSwitch.setVisibility(visibility);
        tvBme680ServiceIaqDevice.setVisibility(visibility);
        tvBme680ServiceIaqAccuracyDevice.setVisibility(visibility);
        tvBme680ServiceTemperatureDevice.setVisibility(visibility);
        tvBme680ServiceHumidityDevice.setVisibility(visibility);
        tvBme680ServicePressureDevice.setVisibility(visibility);
//        tvBme680ServiceRawTemperatureDevice.setVisibility(visibility);
//        tvBme680ServiceRawHumidityDevice.setVisibility(visibility);
        tvBme680ServiceRawGasDevice.setVisibility(visibility);
        tvBno055ServiceMeasureDelayDevice.setVisibility(visibility);
    }

    /**
     * Change the visibility for all BNO055 fields
     *
     * @param visibility Visibility View.GONE or View.VISIBLE
     */
    private void setBno055CardFieldsVisibility(int visibility) {
        rlBleBno055NotifsSwitch.setVisibility(visibility);
        btnBno055BleWrite.setVisibility(visibility);
        swBno055EnableNotifications.setVisibility(visibility);
        tvBno055ServiceAccelerometerDevice.setVisibility(visibility);
        tvBno055ServiceGyroscopeDevice.setVisibility(visibility);
        tvBno055ServiceMagnetometerDevice.setVisibility(visibility);
    }

    /**
     * Change the visibility for all Ble Scanner fields
     *
     * @param visibility Visibility View.GONE or View.VISIBLE
     */
    private void setBleScannerCardFieldsVisibility(int visibility) {

        rlBleScanNotifsSwitch.setVisibility(visibility);
        btnBleScanBleWrite.setVisibility(visibility);
        tvBleScanServiceDevicesScannedDevice.setVisibility(visibility);
        tvBleScanServiceScanWindowDevice.setVisibility(visibility);
    }

    private void setLoraCardFieldsVisibility(int visibility) {
        tvLoraDevEuiDevice.setVisibility(visibility);
        tvLoraAppEuiDevice.setVisibility(visibility);
        btnBleLoraUploadDevice.setVisibility(visibility);
        btnBleLoraDownloadDevice.setVisibility(visibility);
        tvLoraNetworkJoinStatusDevice.setVisibility(visibility);
        tvLoraDeviceAddressDevice.setVisibility(visibility);
    }

    private void setBtnBleConnectDeviceToConnectMode() {
        btnBleConnectDevice.setText(R.string.ble_btn_default_connect_msg);
        btnBleConnectDevice.setCompoundDrawablesWithIntrinsicBounds(0, 0,
                R.drawable.ic_bluetooth_connect_white, 0);
        btnBleConnectDevice.setOnClickListener(listenerConnectMode);
    }

    private void setBtnBleConnectDeviceToDisconnectMode() {
        btnBleConnectDevice.setText(R.string.ble_btn_disconnect_msg);
        btnBleConnectDevice.setCompoundDrawablesWithIntrinsicBounds(0, 0,
                R.drawable.ic_bluetooth_disconnect_white, 0);
        btnBleConnectDevice.setOnClickListener(listenerDisconnectMode);
    }

    private void setBtnBleConnectDeviceToDisabled() {
        btnBleConnectDevice.setAlpha(.5f);
        btnBleConnectDevice.setClickable(false);
    }

    private void setBtnBleConnectDeviceToEnable() {
        btnBleConnectDevice.setAlpha(1f);
        btnBleConnectDevice.setClickable(true);
    }

    private String extractDataStringFromTextView(TextView tv) {
        String tvText = tv.getText().toString();
        return tvText.substring(tvText.lastIndexOf(": ") + 2);
    }

    /**
     * Extract from the bundle the mac address of the device to connect.
     * With this mac address, context the BLE manager to retrieve a BleDevice object.
     */
    private void getDataFromArguments() {

        Bundle bundle = getArguments();
        String ble_mac_addr = bundle.getString(Constants.BLE_DEVICE_MAC);
        if (ble_mac_addr != null) {
            mBleDevice = mBleManager.getDevice(ble_mac_addr);
//            mBleDevice.unbond();
            mBleDevice.disconnect();
        }
    }

    /**
     * Request the Server the server with the information from the device selected previously.
     * The device is define by the attribute mBleDevice.
     */
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

    /**
     * Handle all correct responses from the rest API when requesting a SmartCantonDevBoxDevice
     *
     * @param device A SmartCantonDevBoxDevice from the server with all his information
     */
    private void handleResponse(SmartCantonDevBoxDevice device) {

        smartCantonDevBoxDevice = device;

        mProgressDialog.dismiss();

        tvMessageServ.setVisibility(View.GONE);

        tvBleMacAddrServ.setText(String.format("Mac Address : %s", device.getBleMacAddr()));
        tvBlePasskeyServ.setText(String.format("Passkey : %s", device.getBlePassKey()));
        tvDevEuiServ.setText(String.format("DevEUI : %s", device.getDevEui()));
        tvAppEuiServ.setText(String.format("AppEUI : %s", device.getAppEui()));
        tvAppKeyServ.setText(String.format("AppKey : %s", device.getAppKey()));
        tvPublicIdServ.setText(String.format("Owner ID : %s", device.getOwnerPublicId()));
    }

    /**
     * Handle error from the REST API throught the RetroFit interface
     *
     * @param error Error with the http exception
     */
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

    /**
     * Display a snack bar with a custom message
     *
     * @param message Message to be displayed on the snack bar
     */
    private void showSnackBarMessage(String message) {

        View view = getView();
        if (view != null) {
            Snackbar.make(getView(), message, Snackbar.LENGTH_LONG).show();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mBleDevice.disconnect();
    }

    /**
     * Enable all notifications from the service SMARTCANTON_DEVBOX_GPS_SERVICE
     */
    private void enableBleGpsNotifications() {
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_POSITION,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServicePositionDevice.setText(
                                    String.format("Position {Lat,Long} : %s [°]", e1.data_utf8()));
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SPEED,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServiceSpeedDevice.setText(
                                    String.format("Speed : %s [knots]", e1.data_utf8()));
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_COURSE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data_utf8().length() > 0)
                            tvGpsServiceCourseDevice.setText(
                                    String.format("Course : %s [°]", e1.data_utf8()));
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

    /**
     * Disable all notifications from the service SMARTCANTON_DEVBOX_GPS_SERVICE
     */
    private void disableBleGpsNotifications() {

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_POSITION);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SPEED);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_COURSE);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_DATE);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_TIME);
    }

    /**
     * Read all characteristics from the service SMARTCANTON_DEVBOX_GPS_SERVICE
     */
    private void readBleGpsCharacteristics() {
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_GPS_POSITION,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvGpsServicePositionDevice.setText(
                                String.format("Position {Lat,Long} : %s [°]", e1.data_utf8()));
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
                                String.format("Speed : %s [knots]", e1.data_utf8()));
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
                                String.format("Course : %s [°]", e1.data_utf8()));
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

    /**
     * Read LoRa characteristics from the service SMARTCANTON_DEVBOX_LORA_SERVICE
     */
    private void readBleLoraCharacteristics() {
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_APP_EUI,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraAppEuiDevice.setText(String.format("App EUI : %s",
                                FormatConversions.bytesArrayToHexString(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_DEVICE_EUI,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraDevEuiDevice.setText(String.format("Dev EUI : %s",
                                FormatConversions.bytesArrayToHexString(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_NETWORK_DEVICE_ADDRESS,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraDeviceAddressDevice.setText(String.format("Dev Addr : %s",
                                FormatConversions.bytesArrayToHexString(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_NETWORK_JOIN_STATUS,
                e1 -> {
                    if (e1.wasSuccess()) {
                        tvLoraNetworkJoinStatusDevice.setText(String.format("Network Join : %s",
                                FormatConversions.bytesArrayToHexString(e1.data())));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
    }

    /**
     * Write all the LoRa information from the Server to the SMARTCANTON_DEVBOX_LORA_SERVICE
     */
    private void writeBleLoraCharacteristicsFromServer() {

        mBleDevice.write(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_APP_EUI,
                FormatConversions.hexStringToByteArray(smartCantonDevBoxDevice.getAppEui()),
                e1 -> {
                    if (!e1.wasSuccess()) {
                        showSnackBarMessage(String.format("Error while writing characteristics %s",
                                "LORA APP EUI"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        mBleDevice.write(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_APP_KEY,
                FormatConversions.hexStringToByteArray(smartCantonDevBoxDevice.getAppKey()),
                e1 -> {
                    if (!e1.wasSuccess()) {
                        showSnackBarMessage(String.format("Error while writing characteristics %s",
                                "LORA APP KEY"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });

        byte[] cmdValidateConfig = {0x01};
        mBleDevice.write(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_LORA_NETWORK_VALIDATE_NEW_CONFIGURATION,
                cmdValidateConfig,
                e1 -> {
                    if (!e1.wasSuccess()) {
                        showSnackBarMessage(String.format("Error while writing characteristics %s",
                                "LORA VALIDATE CONFIG"));
                    }
                    bleReadWriteProgressDialog.incrementProgressBy(1);
                });
    }

    /**
     * Enable all notifications from the service SMARTCANTON_DEVBOX_BME680_SERVICE
     */
    private void enableBleBme680Notifications() {
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_IAQ_ACCURACY,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 1)
                            tvBme680ServiceIaqAccuracyDevice.setText(String.format(Locale.getDefault(),
                                    "IAQ accuracy : %d", e1.data_byte()));
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_IAQ,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 4) { // Only display if it can be transformed to float
                            tvBme680ServiceIaqDevice.setText(String.format(Locale.getDefault(),
                                    "IAQ : %.2f",
                                    ByteBuffer.wrap(e1.data()).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        }
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_TEMPERATURE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 4) { // Only display if it can be transformed to float
                            tvBme680ServiceTemperatureDevice.setText(
                                    String.format(Locale.getDefault(), "Temperature : %.2f [°C]",
                                            ByteBuffer.wrap(e1.data()).
                                                    order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        }
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_HUMIDITY,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 4) { // Only display if it can be transformed to float
                            tvBme680ServiceHumidityDevice.setText(
                                    String.format(Locale.getDefault(), "Humidity : %.2f [%%]",
                                            ByteBuffer.wrap(e1.data()).
                                                    order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        }
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_PRESSURE,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 4) { // Only display if it can be transformed to float
                            tvBme680ServicePressureDevice.setText(
                                    String.format(Locale.getDefault(), "Pressure : %.2f [hPa]",
                                            ByteBuffer.wrap(e1.data()).
                                                    order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        }
                    }
                });

        /*
         * These 2 lines are commented because it's not possible to save more than 16 characteristics
         * as notifications/indications on the KW41z. This limitation comes from a hardcoded value
         * inside the BLE stack. We disabled these 2 characteristics that can not only be accessed
         * by read operations.
         */
//        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
//                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_RAW_TEMPERATURE,
//                e1 -> {
//                    if (e1.wasSuccess()) {
//                        if (e1.data().length == 4) { // Only display if it can be transformed to float
//                            tvBme680ServiceRawTemperatureDevice.setText(
//                                    String.format(Locale.getDefault(), "Raw temperature : %.2f °C",
//                                            ByteBuffer.wrap(e1.data()).
//                                                    order(ByteOrder.LITTLE_ENDIAN).getFloat()));
//                        }
//                    }
//                });

//        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
//                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_RAW_HUDMITIY,
//                e1 -> {
//                    if (e1.wasSuccess()) {
//                        if (e1.data().length == 4) { // Only display if it can be transformed to float
//                            tvBme680ServiceRawHumidityDevice.setText(
//                                    String.format(Locale.getDefault(), "Raw humidity : %.2f %%",
//                                            ByteBuffer.wrap(e1.data()).
//                                                    order(ByteOrder.LITTLE_ENDIAN).getFloat()));
//                        }
//                    }
//                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_RAW_GAS,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 4) { // Only display if it can be transformed to float
                            tvBme680ServiceRawGasDevice.setText(
                                    String.format(Locale.getDefault(), "Gas resistance : %.0f [Ω]",
                                            ByteBuffer.wrap(e1.data()).
                                                    order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        }
                    }
                });
    }

    /**
     * Disable all notifications from the service SMARTCANTON_DEVBOX_BME680_SERVICE
     */
    private void disableBleBme680Notifications() {
        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_IAQ_ACCURACY);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_IAQ);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_TEMPERATURE);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_HUMIDITY);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_PRESSURE);

        /*
         * These 2 lines are commented because it's not possible to save more than 16 characteristics
         * as notifications/indications on the KW41z. This limitation comes from a hardcoded value
         * inside the BLE stack. We disabled these 2 characteristics that can not only be accessed
         * by read operations.
         */
//        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
//                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_RAW_TEMPERATURE);

//        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
//                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_RAW_HUDMITIY);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BME680_RAW_GAS);
    }

    /**
     * Write the measure delay from the the service SMARTCANTON_DEVBOX_BNO055_SERVICE
     */
    private void writeBleBno055MeasureInterval(int measureInterval) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(4);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putInt(measureInterval);
        byte[] bytes = byteBuffer.array();
        mBleDevice.write(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_MEASURE_INTERVAL,
                bytes,
                e1 -> {
                    if (!e1.wasSuccess()) {
                        showSnackBarMessage(String.format("Error while writing characteristics %s",
                                "BNO055 MEASURE INTERVAL"));

                    } else {
                        // Update the display with the new value
                        readBleBno055MeasureInterval();
                    }
                });
    }

    /**
     * Read the measure interval from the the service SMARTCANTON_DEVBOX_BNO055_SERVICE
     */
    private void readBleBno055MeasureInterval() {
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_MEASURE_INTERVAL,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (FormatConversions.unsignedIntToLong(e1.data()) > 0) {
                            tvBno055ServiceMeasureDelayDevice.setText(
                                    String.format(Locale.getDefault(),
                                            "Measure interval : %d [ms]",
                                            FormatConversions.unsignedIntToLong(e1.data())));
                        }
                    }
                });
    }

    /**
     * Enable all notifications from the service SMARTCANTON_DEVBOX_BNO055_SERVICE
     */
    private void enableBleBno055Notifications() {
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_ACCELEROMETER,
                e1 -> {
                    if (e1.data().length == 12) { // Only display if it can be transformed to float
                        tvBno055ServiceAccelerometerDevice.setText(
                                String.format(Locale.getDefault(), "Accel {x,y,z} : %.2f,%.2f,%.2f [g] ",
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 0, 4))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat(),
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 4, 8))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat(),
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 8, 12))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_GYROSCOPE,
                e1 -> {
                    if (e1.data().length == 12) { // Only display if it can be transformed to float
                        tvBno055ServiceGyroscopeDevice.setText(
                                String.format(Locale.getDefault(), "Gyro {x,y,z} : %.2f,%.2f,%.2f [deg/s] ",
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 0, 4))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat(),
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 4, 8))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat(),
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 8, 12))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                    }
                });

        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_MAGNETOMETER,
                e1 -> {
                    if (e1.data().length == 12) { // Only display if it can be transformed to float
                        tvBno055ServiceMagnetometerDevice.setText(
                                String.format(Locale.getDefault(), "Mag {x,y,z} : %.2f,%.2f,%.2f [μT] ",
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 0, 4))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat(),
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 4, 8))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat(),
                                        ByteBuffer.wrap(Arrays.copyOfRange(e1.data(), 8, 12))
                                                .order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                    }
                });
    }

    /**
     * Disable all notifications from the service SMARTCANTON_DEVBOX_BNO055_SERVICE
     */
    private void disableBleBno055Notifications() {
        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_ACCELEROMETER);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_GYROSCOPE);

        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BNO055_MAGNETOMETER);
    }

    /**
     * Enable all notifications from the service SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE
     */
    private void enableBleScannerNotifications() {
        bleScannedFirstBuggedDataRead = false;
        mBleDevice.enableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_DEVICES_SCANNED,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 2) {
                            if (bleScannedFirstBuggedDataRead) {
                                tvBleScanServiceDevicesScannedDevice.setText(
                                        String.format(Locale.getDefault(),
                                                "BLE beacons scanned : %d",
                                                FormatConversions.unsignedShortToInt(e1.data())));
                            }
                            bleScannedFirstBuggedDataRead = true;
                        }
                    }
                });
    }

    /**
     * Read the characteristic scan window form the SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE
     */
    private void readBleScannerWindow() {
        mBleDevice.read(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_SCAN_WINDOW,
                e1 -> {
                    if (e1.wasSuccess()) {
                        if (e1.data().length == 2) {
                            tvBleScanServiceScanWindowDevice.setText(
                                    String.format(Locale.getDefault(),
                                            "Scan interval : %d [s]",
                                            e1.data_short(true)));
                        }
                    }
                });
    }

    /**
     * Write the measure delay from the the service SMARTCANTON_DEVBOX_BNO055_SERVICE
     */
    private void writeBleScannerWindow(int interval) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(2);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putShort((short) interval);
        byte[] bytes = byteBuffer.array();
        mBleDevice.write(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_SCAN_WINDOW,
                bytes,
                e1 -> {
                    if (!e1.wasSuccess()) {
                        showSnackBarMessage(String.format("Error while writing characteristics %s",
                                "BLE_SCAN_WINDOW"));

                    } else {
                        // Update the display with the new value
                        readBleScannerWindow();
                    }
                });
    }

    /**
     * Disable all notifications from the service SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE
     */
    private void disableBleScannerNotifications() {
        mBleDevice.disableNotify(SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_SCAN_SERVICE,
                SmartCantonDevBoxBLEServices.SMARTCANTON_DEVBOX_BLE_DEVICES_SCANNED);
    }
}