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

import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.Nullable;
import android.support.design.widget.Snackbar;
import android.support.design.widget.TextInputLayout;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.master_hesso.smartcantonmanager.BluetoothActivity;
import com.master_hesso.smartcantonmanager.R;
import com.master_hesso.smartcantonmanager.model.Response;
import com.master_hesso.smartcantonmanager.network.NetworkUtil;
import com.master_hesso.smartcantonmanager.utils.Constants;

import retrofit2.adapter.rxjava.HttpException;
import rx.android.schedulers.AndroidSchedulers;
import rx.schedulers.Schedulers;
import rx.subscriptions.CompositeSubscription;

import static com.master_hesso.smartcantonmanager.utils.Validation.validateUsername;
import static com.master_hesso.smartcantonmanager.utils.Validation.validateFields;

/**
 * Fragment to login on the SmartCanton APP Key server. This fragment will connect to the
 * server and if the connection is successful, go to the next fragment
 */
public class LoginFragment extends Fragment {

    public static final String TAG = LoginFragment.class.getSimpleName();

    private EditText mEtUsername;
    private EditText mEtPassword;
    private Button mBtLogin;
    private TextView mTvRegister;
    private TextView mTvForgotPassword;
    private TextInputLayout mTiUsername;
    private TextInputLayout mTiPassword;
    private ProgressBar mProgressBar;

    private CompositeSubscription mSubscriptions;
    private SharedPreferences mSharedPreferences;



    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_login,container,false);
        mSubscriptions = new CompositeSubscription();
        initViews(view);
        initSharedPreferences();
        return view;
    }

    /**
     * Extract all views from the current load layout
     *
     * @param v The current view displayed
     */
    private void initViews(View v) {

        mEtUsername = v.findViewById(R.id.et_username);
        mEtPassword = v.findViewById(R.id.et_password);
        mBtLogin = v.findViewById(R.id.btn_login);
        mTiUsername = v.findViewById(R.id.ti_username);
        mTiPassword = v.findViewById(R.id.ti_password);
        mProgressBar = v.findViewById(R.id.progress);
        mTvRegister = v.findViewById(R.id.tv_register);
        mTvForgotPassword = v.findViewById(R.id.tv_forgot_password);

        mBtLogin.setOnClickListener(view -> login());
        mTvRegister.setOnClickListener(view -> goToRegister());
        mTvForgotPassword.setOnClickListener(view -> showDialog());
    }

    private void initSharedPreferences() {

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(getActivity());
    }

    /**
     * Extract the information entered by the user and ask a login to the server
     */
    private void login() {

        setError();

        String username = mEtUsername.getText().toString();
        String password = mEtPassword.getText().toString();

        int err = 0;

        if (!validateUsername(username)) {

            err++;
            mTiUsername.setError(getString(R.string.ti_username_should_not_be_empty));
        }

        if (!validateFields(password)) {

            err++;
            mTiPassword.setError(getString(R.string.ti_password_should_not_be_empty));
        }

        if (err == 0) {

            loginProcess(username,password);
            mProgressBar.setVisibility(View.VISIBLE);

        } else {

            showSnackBarMessage(getString(R.string.snack_enter_valid_details));
        }
    }

    /**
     * Clear all errors messages on the TextInputLayouts
     */
    private void setError() {

        mTiUsername.setError(null);
        mTiPassword.setError(null);
    }

    /**
     * Contact the server to request a login
     * @param username The username of the user
     * @param password The password matching the username
     */
    private void loginProcess(String username, String password) {

        mSubscriptions.add(NetworkUtil.getRetrofit(username, password).login()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.io())
                .subscribe(this::handleResponse,this::handleError));
    }

    /**
     * Handle a correct response from the server (code 200)
     * @param response The response from the server
     */
    private void handleResponse(Response response) {

        mProgressBar.setVisibility(View.GONE);

        SharedPreferences.Editor editor = mSharedPreferences.edit();
        editor.putString(Constants.TOKEN,response.getToken());
        editor.putString(Constants.USERNAME, String.valueOf(mEtUsername.getText()));
        editor.apply();

        mEtUsername.setText(null);
        mEtPassword.setText(null);

        //Intent intent = new Intent(getActivity(), ProfileActivity.class);
        Intent intent = new Intent(getActivity(), BluetoothActivity.class);
        startActivity(intent);

    }

    /**
     * Handle errors when contacting the server
     * @param error The error throw by the server with this HTTP code
     */
    private void handleError(Throwable error) {

        mProgressBar.setVisibility(View.GONE);

        if (error instanceof HttpException) {

            Gson gson = new GsonBuilder().create();

            try {

                String errorBody = ((HttpException) error).response().errorBody().string();
                Response response = gson.fromJson(errorBody,Response.class);

                switch (((HttpException) error).response().code()) {
                    case 401 :
                        mTiUsername.setError(getString(R.string.tx_username_login_invalid));
                        mTiPassword.setError(getString(R.string.tx_password_login_invalid));
                        showSnackBarMessage(getString(R.string.snack_password_or_username_not_valid));
                        break;
                    default:
                        showSnackBarMessage(((HttpException) error).response().code() + " : " +
                                response.getMessage());
                }

            } catch (Exception e) {
                showSnackBarMessage(getString(R.string.snack_unexpected_response_from_server));
                e.printStackTrace();
            }
        } else {
            showSnackBarMessage(getString(R.string.snack_network_error));
        }
    }


    /**
     * Display a message on the SnackBar
     * @param message Message to display
     */
    private void showSnackBarMessage(String message) {

        if (getView() != null) {

            Snackbar.make(getView(),message,Snackbar.LENGTH_LONG).show();
        }
    }

    private void goToRegister(){

        FragmentTransaction ft = getFragmentManager().beginTransaction();
        RegisterFragment fragment = new RegisterFragment();
        ft.replace(R.id.fragmentFrame,fragment,RegisterFragment.TAG);
        ft.commit();
    }

    private void showDialog(){

        ResetPasswordDialog fragment = new ResetPasswordDialog();

        fragment.show(getFragmentManager(), ResetPasswordDialog.TAG);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mSubscriptions.unsubscribe();
    }
}
