package com.master_hesso.smartcantonmanager.network;

import com.google.gson.annotations.SerializedName;

public class Token implements NetworkRequest.ApiResponse {
    @SerializedName("access_token")
    private String accessToken;

    public String getAccessToken() {
        return accessToken;
    }

    @Override
    public String string() {
        return accessToken;
    }
}
