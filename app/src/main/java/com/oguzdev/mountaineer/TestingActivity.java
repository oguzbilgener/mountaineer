package com.oguzdev.mountaineer;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class TestingActivity extends AppCompatActivity {

    BluetoothAdapter bluetoothAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_testing);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        BluetoothDevice device = bluetoothAdapter.getRemoteDevice("20:16:11:14:45:76");


        BluetoothSocket tmp = null;
        BluetoothSocket mmSocket = null;

        // Get a BluetoothSocket for a connection with the
        // given BluetoothDevice
        try {
            tmp = device.createRfcommSocketToServiceRecord(Constants.myUUID);
            Method m = device.getClass().getMethod("createRfcommSocket", new Class[] {int.class});
            tmp = (BluetoothSocket) m.invoke(device, 1);
            
        } catch (IOException e) {
            Log.e(Constants.TAG, "create() failed", e);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        mmSocket = tmp;
    }

    private void startSearching() {
        if (bluetoothAdapter.startDiscovery()) {
            Toast.makeText(this, "Searching", Toast.LENGTH_SHORT).show();
        } else {

//            Snackbar.make(coordinatorLayout, "Failed to start searching", Snackbar.LENGTH_INDEFINITE)
//                    .setAction("Try Again", new View.OnClickListener() {
//                        @Override public void onClick(View v) {
//                            startSearching();
//                        }
//                    }).show();
        }
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // Get the BluetoothDevice object from the Intent
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                device.fetchUuidsWithSdp();

                /*if (bluetoothDevicesAdapter.getPosition(device) == -1) {
                    // -1 is returned when the item is not in the adapter
                    bluetoothDevicesAdapter.add(device);
                    bluetoothDevicesAdapter.notifyDataSetChanged();
                }*/

            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
//                toolbarProgressCircle.setVisibility(View.INVISIBLE);
//                setStatus("None");

            } else if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
                switch (state) {
                    case BluetoothAdapter.STATE_OFF:
//                        Snackbar.make(coordinatorLayout, "Bluetooth turned off", Snackbar.LENGTH_INDEFINITE)
//                                .setAction("Turn on", new View.OnClickListener() {
//                                    @Override public void onClick(View v) {
//                                        enableBluetooth();
//                                    }
//                                }).show();
                        break;
                }
            }
        }
    };
}
