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

import android.app.DialogFragment;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
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
import com.master_hesso.smartcantonmanager.ProfileActivity;
import com.master_hesso.smartcantonmanager.R;
import com.master_hesso.smartcantonmanager.model.Response;
import com.master_hesso.smartcantonmanager.model.User;
import com.master_hesso.smartcantonmanager.network.NetworkUtil;
import com.master_hesso.smartcantonmanager.utils.Constants;

import java.io.IOException;

import retrofit2.adapter.rxjava.HttpException;
import rx.android.schedulers.AndroidSchedulers;
import rx.schedulers.Schedulers;
import rx.subscriptions.CompositeSubscription;

import static com.master_hesso.smartcantonmanager.utils.Validation.validateFields;

/**
 * Display a dialog to change the password currently connected
 */
public class ChangePasswordDialog extends DialogFragment {

    public interface Listener {

        void onPasswordChanged();
    }

    public static final String TAG = ChangePasswordDialog.class.getSimpleName();

    private EditText mEtOldPassword;
    private EditText mEtNewPassword;
    private Button mBtChangePassword;
    private Button mBtCancel;
    private TextView mTvMessage;
    private TextInputLayout mTiOldPassword;
    private TextInputLayout mTiNewPassword;
    private ProgressBar mProgressBar;

    private CompositeSubscription mSubscriptions;

    private String mToken;
    private String mEmail;

    private Listener mListener;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.dialog_change_password, container, false);
        mSubscriptions = new CompositeSubscription();
        getData();
        initViews(view);
        return view;
    }

    /**
     * Retrieve user information from the bundle of data
     */
    private void getData() {

        Bundle bundle = getArguments();

        mToken = bundle.getString(Constants.TOKEN);
        mEmail = bundle.getString(Constants.USERNAME);
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        mListener = (ProfileActivity) context;
    }

    /**
     * Init all views present on the layout displayed
     *
     * @param v Current view displayed
     */
    private void initViews(View v) {

        mEtOldPassword = v.findViewById(R.id.et_old_password);
        mEtNewPassword = v.findViewById(R.id.et_new_password);
        mTiOldPassword = v.findViewById(R.id.ti_old_password);
        mTiNewPassword = v.findViewById(R.id.ti_new_password);
        mTvMessage = v.findViewById(R.id.tv_message);
        mBtChangePassword = v.findViewById(R.id.btn_change_password);
        mBtCancel = v.findViewById(R.id.btn_cancel);
        mProgressBar = v.findViewById(R.id.progress);

        mBtChangePassword.setOnClickListener(view -> changePassword());
        mBtCancel.setOnClickListener(view -> dismiss());
    }

    /**
     * Verify the validity of the password entered and send it to the server
     */
    private void changePassword() {

        setError();

        String oldPassword = mEtOldPassword.getText().toString();
        String newPassword = mEtNewPassword.getText().toString();

        int err = 0;

        if (!validateFields(oldPassword)) {

            err++;
            mTiOldPassword.setError("Password should not be empty !");
        }

        if (!validateFields(newPassword)) {

            err++;
            mTiNewPassword.setError("Password should not be empty !");
        }

        if (err == 0) {

            User user = new User();
            user.setPassword(oldPassword);
            user.setNewPassword(newPassword);
            changePasswordProgress(user);
            mProgressBar.setVisibility(View.VISIBLE);

        }
    }

    /**
     * Clear error on the text input layout
     */
    private void setError() {

        mTiOldPassword.setError(null);
        mTiNewPassword.setError(null);
    }

    /**
     * Contact the server to change the password on the user. Use the local token to request the
     * change.
     *
     * @param user The user with the new password
     */
    private void changePasswordProgress(User user) {

        mSubscriptions.add(NetworkUtil.getRetrofit(mToken).changePassword(mEmail, user)
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.io())
                .subscribe(this::handleResponse, this::handleError));
    }

    /**
     * Handle the response from the server after changing the password
     *
     * @param response Response from the server
     */
    private void handleResponse(Response response) {

        mProgressBar.setVisibility(View.GONE);
        mListener.onPasswordChanged();
        dismiss();
    }

    /**
     * If the server throw an HTTP error, handle it here
     *
     * @param error Error to be handled
     */
    private void handleError(Throwable error) {

        mProgressBar.setVisibility(View.GONE);

        if (error instanceof HttpException) {

            Gson gson = new GsonBuilder().create();

            try {

                String errorBody = ((HttpException) error).response().errorBody().string();
                Response response = gson.fromJson(errorBody, Response.class);
                showMessage(response.getMessage());

            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {

            showMessage("Network Error !");
        }
    }

    /**
     * Show a message on the TextView present on the dialog fragment
     *
     * @param message Message to be displayed
     */
    private void showMessage(String message) {

        mTvMessage.setVisibility(View.VISIBLE);
        mTvMessage.setText(message);

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mSubscriptions.unsubscribe();
    }
}
