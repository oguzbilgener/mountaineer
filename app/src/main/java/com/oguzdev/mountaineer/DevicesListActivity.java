package com.oguzdev.mountaineer;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.annimon.stream.Stream;

import java.util.Set;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import butterknife.OnItemClick;

public class DevicesListActivity extends AppCompatActivity {

    BluetoothAdapter bluetoothAdapter;

    BluetoothDevicesAdapter bluetoothDevicesAdapter;

    @BindView(R.id.toolbar)
    Toolbar toolbar;
    @BindView(R.id.devices_list_view)
    ListView devicesListView;
    @BindView(R.id.empty_list_item)
    TextView emptyListTextView;
    @BindView(R.id.toolbar_progress_bar)
    ProgressBar toolbarProgressCircle;
    @BindView(R.id.coordinator_layout_main)
    CoordinatorLayout coordinatorLayout;

    /*
    @OnClick(R.id.search_button) void search() {

        if (bluetoothAdapter.isEnabled()) {
            Log.d("oguz", "startSearching");
            // Bluetooth enabled
            startSearching();
        } else {

            enableBluetooth();
        }
    }
    */

    private void enableBluetooth() {
        setStatus("Enabling Bluetooth");
        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(enableBtIntent, Constants.REQUEST_ENABLE_BT);
    }

    @OnItemClick(R.id.devices_list_view) void onItemClick(int position) {
        final BluetoothDevice device = bluetoothDevicesAdapter.getItem(position);

        new AlertDialog.Builder(DevicesListActivity.this)
                .setCancelable(false)
                .setTitle("Connect")
                .setMessage("Do you want to connect to: " + device.getName() + " - " + device.getAddress())
                .setPositiveButton("Connect", (dialog, which) -> {
                    Log.d(Constants.TAG, "Opening new Activity");
                    bluetoothAdapter.cancelDiscovery();
                    toolbarProgressCircle.setVisibility(View.INVISIBLE);

                    Intent intent = new Intent(DevicesListActivity.this, ChatActivity.class);

                    intent.putExtra(Constants.EXTRA_DEVICE, device);

                    startActivity(intent);
                })
                .setNegativeButton("Cancel", (dialog, which) -> {
                    Log.d(Constants.TAG, "Cancelled ");
                }).show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ButterKnife.bind(this);
        setSupportActionBar(toolbar);

        setStatus("Choose a paired device");

        bluetoothDevicesAdapter = new BluetoothDevicesAdapter(this);

        devicesListView.setAdapter(bluetoothDevicesAdapter);
        devicesListView.setEmptyView(emptyListTextView);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        Log.d("oguz", "paired device count: "+pairedDevices.size());

        Stream.of(pairedDevices).forEach((device) -> {
//            Log.d("oguz", device.getAddress()+" : "+device.getName()+" ");
            if (bluetoothDevicesAdapter.getPosition(device) == -1) {
                // -1 is returned when the item is not in the adapter
                bluetoothDevicesAdapter.add(device);
                bluetoothDevicesAdapter.notifyDataSetChanged();
            }
        });

        if (pairedDevices.isEmpty()) {
            new AlertDialog.Builder(DevicesListActivity.this)
                    .setCancelable(false)
                    .setTitle("No paired devices")
                    .setMessage("There aren't any paired Bluetooth devices.")
                    .setPositiveButton("Close app", (dialog, which) -> {
                        Log.d(Constants.TAG, "App closed");
                        finish();
                    }).show();
        }

        if (bluetoothAdapter == null) {
            new AlertDialog.Builder(DevicesListActivity.this)
                    .setCancelable(false)
                    .setTitle("No Bluetooth")
                    .setMessage("Your device has no bluetooth")
                    .setPositiveButton("Close app", (dialog, which) -> {
                        Log.d(Constants.TAG, "App closed");
                        finish();
                    }).show();

        }
    }

    @Override protected void onStart() {
        super.onStart();
    }

    @Override protected void onStop() {
        super.onStop();
    }


    private void setStatus(String status) {
        toolbar.setSubtitle(status);
    }

    @Override protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == Constants.REQUEST_ENABLE_BT) {
            if (resultCode == RESULT_OK) {
                startSearching();
            } else {
                setStatus("Error");
                Snackbar.make(coordinatorLayout, "Failed to enable bluetooth", Snackbar.LENGTH_INDEFINITE)
                        .setAction("Try Again", v -> enableBluetooth()).show();
            }
        }

    }

    private void startSearching() {
        if (bluetoothAdapter.startDiscovery()) {
            toolbarProgressCircle.setVisibility(View.VISIBLE);
            setStatus("Searching for devices");
        } else {
            setStatus("Error");
            Snackbar.make(coordinatorLayout, "Failed to start searching", Snackbar.LENGTH_INDEFINITE)
                    .setAction("Try Again", v -> startSearching()).show();
        }
    }

}
