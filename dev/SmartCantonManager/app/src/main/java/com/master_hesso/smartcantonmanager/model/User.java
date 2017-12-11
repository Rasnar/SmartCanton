package com.master_hesso.smartcantonmanager.model;


public class User {

    //private String name;
    private String username;
    private String password;
    private String public_id;
    private Boolean admin;
    //private String created_at;
    private String newPassword;
    //private String token;

//    public void setName(String name) {
//        this.name = name;
//    }

    public void setUsername(String username) {
        this.username = username;
    }

    public void setPassword(String password) {
        this.password = password;
    }

//    public String getName() {
//        return name;
//    }

    public String getUsername() {
        return username;
    }

    public String getPublicId() {
        return public_id;
    }

    public Boolean getAdmin() {
        return admin;
    }

//    public String getCreated_at() {
//        return created_at;
//    }

    public void setNewPassword(String newPassword) {
        this.newPassword = newPassword;
    }

//    public void setToken(String token) {
//        this.token = token;
//    }
}
