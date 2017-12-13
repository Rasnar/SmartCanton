package com.master_hesso.smartcantonmanager.network;

import com.master_hesso.smartcantonmanager.model.Response;
import com.master_hesso.smartcantonmanager.model.SmartCantonDevBoxDevice;
import com.master_hesso.smartcantonmanager.model.User;

import java.util.List;

import retrofit2.http.Body;
import retrofit2.http.DELETE;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.PUT;
import retrofit2.http.Path;
import rx.Observable;

public interface RetrofitInterface {

    @POST("user")
    Observable<Response> register(@Body User user);

    @POST("auth")
    Observable<Response> login();

    @GET("user/{userId}")
    Observable<User> getProfile(@Path("userId") String userId);

    @PUT("user/{userId}")
    Observable<Response> changePassword(@Path("userId") String userId, @Body User user);

    @POST("user/{userId}/password")
    Observable<Response> resetPasswordInit(@Path("userId") String userId);

    @POST("user/{userId}/password")
    Observable<Response> resetPasswordFinish(@Path("userId") String userId, @Body User user);

    @GET("device")
    Observable<List<SmartCantonDevBoxDevice>> getAllDevices(@Path("userId") String userId, @Body User user);

    @GET("device/{bleMacAddr}")
    Observable<SmartCantonDevBoxDevice> getDevice(@Path("bleMacAddr") String bleMacAddr);

    @POST("device")
    Observable<Response> createDevice(@Path("userId") String userId, @Body SmartCantonDevBoxDevice device);

    @PUT("device/{bleMacAddr}")
    Observable<Response> modifyDevice(@Path("userId") String userId, @Body User user);

    @DELETE("device/{bleMacAddr}")
    Observable<Response> deleteDevice(@Path("userId") String userId, @Body User user);
}
