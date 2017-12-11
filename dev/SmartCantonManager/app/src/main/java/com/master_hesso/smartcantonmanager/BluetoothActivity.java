package com.master_hesso.smartcantonmanager;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.auth0.android.jwt.DecodeException;
import com.auth0.android.jwt.JWT;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.idevicesinc.sweetblue.BleDevice;
import com.idevicesinc.sweetblue.BleDeviceState;
import com.idevicesinc.sweetblue.BleManager;
import com.idevicesinc.sweetblue.BleManagerConfig;
import com.idevicesinc.sweetblue.utils.BluetoothEnabler;
import com.idevicesinc.sweetblue.utils.Uuids;
import com.master_hesso.smartcantonmanager.fragments.ChangePasswordDialog;
import com.master_hesso.smartcantonmanager.model.Response;
import com.master_hesso.smartcantonmanager.model.User;
import com.master_hesso.smartcantonmanager.network.NetworkUtil;
import com.master_hesso.smartcantonmanager.utils.Constants;

import java.io.IOException;
import java.util.Date;

import retrofit2.adapter.rxjava.HttpException;
import rx.android.schedulers.AndroidSchedulers;
import rx.schedulers.Schedulers;
import rx.subscriptions.CompositeSubscription;


public class BluetoothActivity extends AppCompatActivity implements ChangePasswordDialog.Listener {

    public static final String TAG = BluetoothActivity.class.getSimpleName();

    private TextView mTvName;
    private TextView mTvUsername;
    private TextView mTvDate;
    private Button mBtChangePassword;
    private Button mBtLogout;

    private ProgressBar mProgressbar;

    private SharedPreferences mSharedPreferences;
    private String mToken;
    private String mUsername;
    private String mUserId;
    private Date mTokenExpiresAt;
    private JWT mJwt;

    private CompositeSubscription mSubscriptions;

    // A ScanFilter decides whether a BleDevice instance will be created from a
    // BLE advertisement and passed to the DiscoveryListener implementation below.
    final BleManagerConfig.ScanFilter scanFilter = e ->
            BleManagerConfig.ScanFilter.Please.acknowledgeIf(e.name_normalized().
                    contains("SmartCantonDevBox")).thenStopScan();

    // New BleDevice instances are provided through this listener.
    // Nested listeners then listen for connection and read results.
    // Obviously you will want to structure your actual code a little better.
    // The deep nesting simply demonstrates the async-callback-based nature of the API.
    final BleManager.DiscoveryListener discoveryListener = new BleManager.DiscoveryListener()
    {
        @Override public void onEvent(DiscoveryEvent e)
        {
            if( e.was(LifeCycle.DISCOVERED) )
            {
                e.device().connect(e1 -> {
                    if( e1.didEnter(BleDeviceState.INITIALIZED) )
                    {
                        e1.device().read(Uuids.BATTERY_LEVEL, e11 -> {
                            if( e11.wasSuccess() )
                            {
                                Log.i("", "Battery level is " + e11.data_byte() + "%");
                            }
                        });
                    }
                });
            }
        }
    };



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);
        mSubscriptions = new CompositeSubscription();
        initViews();
        initSharedPreferences();
        extractTokenInformation();
        loadProfile();

        // This class helps you navigate the treacherous waters of Android M Location requirements for scanning.
        // First it enables bluetooth itself, then location permissions, then location services. The latter two
        // are only needed in Android M. This must be called from an Activity instance.
        BluetoothEnabler.start(this, new BluetoothEnabler.DefaultBluetoothEnablerFilter()
        {
            @Override public Please onEvent(BluetoothEnablerEvent e)
            {
                if( e.isDone() )
                {
                    e.bleManager().startScan(scanFilter, discoveryListener);
                }

                return super.onEvent(e);
            }
        });
    }

    private void initViews() {

        mTvName = (TextView) findViewById(R.id.tv_name);
        mTvUsername = (TextView) findViewById(R.id.tv_username);
        mTvDate = (TextView) findViewById(R.id.tv_date);
        mBtChangePassword = (Button) findViewById(R.id.btn_change_password);
        mBtLogout = (Button) findViewById(R.id.btn_logout);
        mProgressbar = (ProgressBar) findViewById(R.id.progress);

        mBtChangePassword.setOnClickListener(view -> showDialog());
        mBtLogout.setOnClickListener(view -> logout());
    }

    private void initSharedPreferences() {

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mToken = mSharedPreferences.getString(Constants.TOKEN,"");
        mUsername = mSharedPreferences.getString(Constants.USERNAME,"");
    }

    private void extractTokenInformation() {
        try {
            mJwt = new JWT(mToken);
        } catch (DecodeException exception){
            logout();
        }

        mUserId = mJwt.getClaim("public_id").asString();
        mTokenExpiresAt = mJwt.getExpiresAt();
    }

    private void logout() {

        SharedPreferences.Editor editor = mSharedPreferences.edit();
        editor.putString(Constants.USERNAME,"");
        editor.putString(Constants.TOKEN,"");
        editor.apply();
        finish();
    }

    private void showDialog(){

        ChangePasswordDialog fragment = new ChangePasswordDialog();

        Bundle bundle = new Bundle();
        bundle.putString(Constants.USERNAME, mUsername);
        bundle.putString(Constants.TOKEN, mToken);
        fragment.setArguments(bundle);

        fragment.show(getFragmentManager(), ChangePasswordDialog.TAG);
    }

    private void loadProfile() {

        mSubscriptions.add(NetworkUtil.getRetrofit(mToken).getProfile(mUserId)
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.io())
                .subscribe(this::handleResponse,this::handleError));
    }

    private void handleResponse(User user) {

        mProgressbar.setVisibility(View.GONE);
        mTvName.setText(user.getUsername());
        mTvUsername.setText(user.getPublicId());

        //mTvDate.setText(user.getCreated_at());
    }

    private void handleError(Throwable error) {

        mProgressbar.setVisibility(View.GONE);

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

    private void showSnackBarMessage(String message) {

        Snackbar.make(findViewById(R.id.activity_profile),message,Snackbar.LENGTH_SHORT).show();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mSubscriptions.unsubscribe();
    }

    @Override
    public void onPasswordChanged() {

        showSnackBarMessage("Password Changed Successfully !");
    }
}

