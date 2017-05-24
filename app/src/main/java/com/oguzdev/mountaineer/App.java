package com.oguzdev.mountaineer;

import android.app.Application;

import butterknife.ButterKnife;

/**
 * Copyright 2017 Oğuz Bilgener
 * Mountaineer
 */

public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        ButterKnife.setDebug(true);
    }
}
