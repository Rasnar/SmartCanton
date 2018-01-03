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
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.auth0.android.jwt.DecodeException;
import com.auth0.android.jwt.JWT;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.master_hesso.smartcantonmanager.fragments.ChangePasswordDialog;
import com.master_hesso.smartcantonmanager.model.Response;
import com.master_hesso.smartcantonmanager.model.User;
import com.master_hesso.smartcantonmanager.network.NetworkUtil;
import com.master_hesso.smartcantonmanager.utils.Constants;

import java.io.IOException;

import retrofit2.adapter.rxjava.HttpException;
import rx.android.schedulers.AndroidSchedulers;
import rx.schedulers.Schedulers;
import rx.subscriptions.CompositeSubscription;

/**
 * Activity displaying the profile of the current user with his JWT token, the possibility to change
 * the password and a logout button
 */
public class ProfileActivity extends AppCompatActivity implements ChangePasswordDialog.Listener {

    public static final String TAG = ProfileActivity.class.getSimpleName();

    private TextView tvJwtToken;
    private TextView tvJwtType;
    private TextView tvJwtAlg;
    private TextView tvJwtExpiresAt;

    private TextView tvAdmin;
    private TextView tvUsername;
    private TextView tvPublicId;

    private Button btnChangePassword;
    private Button btnLogout;

    private ProgressBar mProgressbar;

    private SharedPreferences mSharedPreferences;
    private String mToken;
    private String mUsername;
    private String mUserPublicId;
    private JWT mJwt;

    private CompositeSubscription mSubscriptions;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);
        mSubscriptions = new CompositeSubscription();
        initViews();
        initSharedPreferences();
        extractTokenInformation();
        showTokenInformation();

        loadProfile();

        try {
            this.getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        } catch (Exception ignored) {
        }
    }

    private void initViews() {

        tvUsername = findViewById(R.id.tv_username);
        tvAdmin = findViewById(R.id.tv_admin);
        tvPublicId = findViewById(R.id.tv_public_id);


        tvJwtToken = findViewById(R.id.tv_jwt_token);
        tvJwtType = findViewById(R.id.tv_jwt_type);
        tvJwtAlg = findViewById(R.id.tv_jwt_alg);
        tvJwtExpiresAt = findViewById(R.id.tv_jwt_expires_at);

        btnChangePassword = findViewById(R.id.btn_change_password);
        btnLogout = findViewById(R.id.btn_logout);
        mProgressbar = findViewById(R.id.progress);

        btnChangePassword.setOnClickListener(view -> showDialog());
        btnLogout.setOnClickListener(view -> logoutAndRestartApp());
    }

    /**
     * Clear the shared preferences
     */
    private void initSharedPreferences() {

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mToken = mSharedPreferences.getString(Constants.TOKEN,"");
        mUsername = mSharedPreferences.getString(Constants.USERNAME,"");
    }

    /**
     * Extract token information from the token string
     */
    private void extractTokenInformation() {
        try {
            mJwt = new JWT(mToken);
        } catch (DecodeException exception){
            showSnackBarMessage("Token not valid !");
            logoutAndRestartApp();
        }

        mUserPublicId = mJwt.getClaim("public_id").asString();
    }

    /**
     * Display the token information on the token card view
     */
    private void showTokenInformation(){

        tvJwtToken.setText(mJwt.toString());
        tvJwtType.setText("Type : JWT");
        tvJwtAlg.setText("Algorithm : HS256");
        try {
            tvJwtExpiresAt.setText(String.format("Expires at : %s", mJwt.getExpiresAt().toString()));
        } catch (Exception ignored){
            tvJwtExpiresAt.setText("Expired time not available");
        }

    }

    /**
     * Logout and restart the app to ask a new login
     */
    private void logoutAndRestartApp() {

        SharedPreferences.Editor editor = mSharedPreferences.edit();
        editor.putString(Constants.USERNAME,"");
        editor.putString(Constants.TOKEN,"");
        editor.apply();

        Intent intent = new Intent(getApplicationContext(), MainActivity.class);
        getApplicationContext().startActivity(intent);
        if (getApplicationContext() instanceof AppCompatActivity) {
            ((AppCompatActivity) getApplicationContext()).finish();
        }

        Runtime.getRuntime().exit(0);
    }

    /**
     * Show a dialog to change the password
     */
    private void showDialog(){

        ChangePasswordDialog fragment = new ChangePasswordDialog();

        Bundle bundle = new Bundle();
        bundle.putString(Constants.USERNAME, mUsername);
        bundle.putString(Constants.TOKEN, mToken);
        fragment.setArguments(bundle);

        fragment.show(getFragmentManager(), ChangePasswordDialog.TAG);
    }

    /**
     * Contact the server to ask more information about the logged user
     */
    private void loadProfile() {

        mSubscriptions.add(NetworkUtil.getRetrofit(mToken).getProfile(mUserPublicId)
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.io())
                .subscribe(this::handleResponse,this::handleError));
    }

    /**
     * Handle a correct response from the server
     * @param user User object with all his information
     */
    private void handleResponse(User user) {

        mProgressbar.setVisibility(View.GONE);

        tvUsername.setText(String.format("Username : %s", user.getUsername()));
        tvPublicId.setText(String.format("Public ID : %s", user.getPublicId()));
        tvAdmin.setText(String.format("Is Admin : %b", user.getAdmin()));

    }

    /**
     * Handle errors from the server (eg. token not valid)
     * @param error HTTP error
     */
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

    /**
     * Display a snackbar with a custom message
     * @param message Message to display
     */
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
}

