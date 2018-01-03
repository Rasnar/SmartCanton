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

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.ProgressBar;

import com.auth0.android.jwt.DecodeException;
import com.auth0.android.jwt.JWT;
import com.master_hesso.smartcantonmanager.fragments.ChangePasswordDialog;
import com.master_hesso.smartcantonmanager.fragments.LoginFragment;
import com.master_hesso.smartcantonmanager.fragments.ScannerFragment;
import com.master_hesso.smartcantonmanager.utils.Constants;

import java.util.Date;

/**
 * Principal activity that manager a fragment holder
 * Switch between the Scanner fragment to the BleFragment when a device is selected on the scanner
 * list.
 */
public class BluetoothActivity extends AppCompatActivity implements ChangePasswordDialog.Listener {

    public static final String TAG = BluetoothActivity.class.getSimpleName();

    private ScannerFragment mScannerFragment;

    private ProgressBar mProgressbar;

    private SharedPreferences mSharedPreferences;
    private String mToken;
    private String mUsername;
    private String mUserId;
    private Date mTokenExpiresAt;
    private JWT mJwt;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth);
        initViews();
        initSharedPreferences();
        extractTokenInformation();
        loadFragment();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu items for use in the action bar
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.bluetooth_menu, menu);

        // Update Action title with current username
        MenuItem profileMenuItem = menu.findItem(R.id.action_profile);
        profileMenuItem.setTitle(" " + mUsername);

        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case R.id.action_profile:

                Intent intentProfile = new Intent(getApplicationContext(), ProfileActivity.class);
                startActivity(intentProfile);
                return true;
            case R.id.action_beacon_broadcast:

                Intent intentBeacon = new Intent(getApplicationContext(), BeaconActivity.class);
                startActivity(intentBeacon);
                return true;
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    /**
     * Display the action bar to be able to return to the last view
     */
    private void initViews() {
        try {
            this.getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        } catch (Exception ignored) {
        }
    }

    /**
     * Reset user and token information on the shared preferences
     */
    private void initSharedPreferences() {

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mToken = mSharedPreferences.getString(Constants.TOKEN, "");
        mUsername = mSharedPreferences.getString(Constants.USERNAME, "");
    }

    /**
     * Extract token information from the token
     */
    private void extractTokenInformation() {
        try {
            mJwt = new JWT(mToken);
        } catch (DecodeException exception) {
            logout();
        }

        mUserId = mJwt.getClaim("public_id").asString();
        mTokenExpiresAt = mJwt.getExpiresAt();
    }

    /**
     * Load the Scanner Fragment
     */
    private void loadFragment() {

        if (mScannerFragment == null) {

            mScannerFragment = new ScannerFragment();
        }

        getSupportFragmentManager().beginTransaction().replace(R.id.fragmentFrame, mScannerFragment, LoginFragment.TAG).commit();
    }

    /**
     * Logout from the current activity
     */
    private void logout() {

        SharedPreferences.Editor editor = mSharedPreferences.edit();
        editor.putString(Constants.USERNAME, "");
        editor.putString(Constants.TOKEN, "");
        editor.apply();
        finish();
    }

    /**
     * Display a snack bar with a custom message
     * @param message Message to display
     */
    private void showSnackBarMessage(String message) {

        Snackbar.make(findViewById(R.id.activity_bluetooth), message, Snackbar.LENGTH_SHORT).show();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onPasswordChanged() {

        showSnackBarMessage("Password Changed Successfully !");
    }
}

