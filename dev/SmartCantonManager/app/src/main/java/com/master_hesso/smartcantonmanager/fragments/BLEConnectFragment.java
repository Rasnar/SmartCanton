/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.master_hesso.smartcantonmanager.fragments;

import android.app.Fragment;
import android.app.ListFragment;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import com.idevicesinc.sweetblue.BleDevice;
import com.idevicesinc.sweetblue.BleManager;
import com.idevicesinc.sweetblue.BleManager.DiscoveryListener.LifeCycle;
import com.idevicesinc.sweetblue.BleManagerConfig;
import com.idevicesinc.sweetblue.BleManagerConfig.ScanFilter;
import com.idevicesinc.sweetblue.BleScanApi;
import com.idevicesinc.sweetblue.utils.BluetoothEnabler;
import com.idevicesinc.sweetblue.utils.BluetoothEnabler.DefaultBluetoothEnablerFilter;
import com.idevicesinc.sweetblue.utils.Interval;
import com.master_hesso.smartcantonmanager.R;
import com.master_hesso.smartcantonmanager.adapters.ScanResultAdapter;

import java.util.Timer;
import java.util.TimerTask;


/**
 * Scans for Bluetooth Low Energy Advertisements matching a filter and displays them to the user.
 */
public class BLEConnectFragment extends Fragment {

    private static final String TAG = BLEConnectFragment.class.getSimpleName();

    /**
     * SweetBlue BLE configuration
     */
    private final BleManagerConfig mBleManagerConfig = new BleManagerConfig() {{

        //this.loggingEnabled = true;


        //this.infiniteScanInterval = Interval.ONE_SEC;
        //this.autoScanActiveTime =
        this.scanApi = BleScanApi.POST_LOLLIPOP;
        this.stopScanOnPause = true;
    }};

    //Handler object declaration
    final Handler h = new Handler();
    // Timer object creation and Staring;
    final Timer timer = new Timer();
    // Timer task creation which will be executed on timer schedule
    TimerTask timerTask = new TimerTask() {
        @Override
        public void run() {
            h.post(() -> mAdapter.notifyDataSetChanged());
        }
    };

    // A ScanFilter decides whether a BleDevice instance will be created from a
    // BLE advertisement and passed to the DiscoveryListener implementation below.
    final ScanFilter scanFilter = e ->
            ScanFilter.Please.acknowledgeIf(e.name_normalized().
                    contains("SmartCantonDevBox")).thenStopScan();

    // New BleDevice instances are provided through this listener.
    // Nested listeners then listen for connection and read results.
    // Obviously you will want to structure your actual code a little better.
    // The deep nesting simply demonstrates the async-callback-based nature of the API.
    final BleManager.DiscoveryListener discoveryListener = e -> {
        if( e.was(LifeCycle.DISCOVERED) || e.was(LifeCycle.REDISCOVERED) )
        {
            mAdapter.add(e.device());
            mAdapter.notifyDataSetChanged();
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);

        // Use getActivity().getApplicationContext() instead of just getActivity() because this
        // object lives in a fragment and needs to be kept separate from the Activity lifecycle.
        //
        // We could get a LayoutInflater from the ApplicationContext but it messes with the
        // default theme, so generate it from getActivity() and pass it in separately.
        mAdapter = new ScanResultAdapter(getActivity().getApplicationContext(),
                LayoutInflater.from(getActivity()));

        // This class helps you navigate the treacherous waters of Android M Location requirements for scanning.
        // First it enables bluetooth itself, then location permissions, then location services. The latter two
        // are only needed in Android M. This must be called from an Activity instance.
        BluetoothEnabler.start(getActivity(), new DefaultBluetoothEnablerFilter()
        {
            @Override public Please onEvent(BluetoothEnablerEvent e)
            {
                if( e.isDone() )
                {
                    e.bleManager().setConfig(mBleManagerConfig);
                    e.bleManager().startScan(Interval.INFINITE, discoveryListener);
                }

                return super.onEvent(e);
            }
        });

        timer.schedule(timerTask, 1000, 1000);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        final View view = super.onCreateView(inflater, container, savedInstanceState);

        setListAdapter(mAdapter);
        setHasOptionsMenu(true);

        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

//        getListView().setDivider(null);
//        getListView().setDividerHeight(0);

        setEmptyText(getString(R.string.empty_list));
    }

    @Override
    public void onListItemClick(ListView l, View v, int position, long id) {
        BleDevice device = (BleDevice) mAdapter.getItem(position);
        Log.d(TAG, "onListItemClick: " + device.getMacAddress());

        try {
            ((OnScannerDeviceSelected) getActivity()).onScannerDevicePicked(device);
        } catch (ClassCastException cce) {
        }
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super.onCreateOptionsMenu(menu, inflater);
    }


    @Override
    public void onResume() {
        super.onResume();
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

    public interface OnScannerDeviceSelected {
        public void onScannerDevicePicked(BleDevice device);
    }
}
