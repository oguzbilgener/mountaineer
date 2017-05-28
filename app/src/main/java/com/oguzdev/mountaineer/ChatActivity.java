package com.oguzdev.mountaineer;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.nio.charset.StandardCharsets;
import java.text.ParseException;
import java.util.Date;

import butterknife.BindView;
import butterknife.ButterKnife;
import co.intentservice.chatui.ChatView;
import co.intentservice.chatui.models.ChatMessage;

public class ChatActivity extends AppCompatActivity {

    BluetoothService bluetoothService;
    BluetoothDevice device;

    @BindView(R.id.toolbar)
    Toolbar toolbar;
    @BindView(R.id.coordinator_layout_bluetooth)
    CoordinatorLayout coordinatorLayout;
    @BindView(R.id.toolbar_progress_bar)
    ProgressBar toolbalProgressBar;
    @BindView(R.id.chat_view)
    ChatView chatView;
    @BindView(R.id.my_sensor_data)
    TextView mySensorLabel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth);

        ButterKnife.bind(this);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setSupportActionBar(toolbar);

        assert getSupportActionBar() != null; // won't be null, lint error
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        device = getIntent().getExtras().getParcelable(Constants.EXTRA_DEVICE);

        BtHandler handler = new BtHandler(this);

        bluetoothService = new BluetoothService(handler, device);

        setTitle(device.getName());

        chatView.setOnSentMessageListener(chatMessage -> {
            // perform actual message sending
            DataMessage message = new DataMessage(chatMessage.getMessage());
            bluetoothService.write(message.serialize());

            return true;
        });
    }

    @Override protected void onStart() {
        super.onStart();
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mReceiver, filter);

        bluetoothService.connect();
        Log.d(Constants.TAG, "Connecting");
    }

    @Override protected void onStop() {
        super.onStop();
        if (bluetoothService != null) {
            bluetoothService.stop();
            Log.d(Constants.TAG, "Stopping");
        }

        unregisterReceiver(mReceiver);
    }

    private static class BtHandler extends Handler {
        private final WeakReference<ChatActivity> mActivity;

        public BtHandler(ChatActivity activity) {
            mActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg) {

            final ChatActivity activity = mActivity.get();

            switch (msg.what) {
                case Constants.MESSAGE_STATE_CHANGE:
                    switch (msg.arg1) {
                        case Constants.STATE_CONNECTED:
                            activity.setStatus("Connected");
//                            activity.reconnectButton.setVisible(false);
                            activity.toolbalProgressBar.setVisibility(View.GONE);
                            activity.bluetoothService.write(
                                    new DataMessage().serialize()
                            );
                            break;
                        case Constants.STATE_CONNECTING:
                            activity.setStatus("Connecting");
                            activity.toolbalProgressBar.setVisibility(View.VISIBLE);
                            break;
                        case Constants.STATE_NONE:
                            activity.setStatus("Not Connected");
                            activity.toolbalProgressBar.setVisibility(View.GONE);
                            break;
                        case Constants.STATE_ERROR:
                            activity.setStatus("Error");
//                            activity.reconnectButton.setVisible(true);
                            activity.toolbalProgressBar.setVisibility(View.GONE);
                            break;
                    }
                    break;
                case Constants.MESSAGE_WRITE:
                    byte[] writeBuf = (byte[]) msg.obj; 
                    // construct a string from the buffer
                    String writeMessage = new String(writeBuf);
                    Log.d("oguz", "writem: "+writeMessage);
//                    ChatMessage message = new ChatMessage(writeMessage, new Date().getTime(), ChatMessage.Type.SENT);
//                    activity.chatView.addMessage(message);
//                    ChatMessage messageWrite = new ChatMessage("Me", writeMessage);
//                    activity.addMessageToAdapter(messageWrite);
                    break;
                case Constants.MESSAGE_READ:

                    String readMessage = (String) msg.obj;
//                    readMessage = readMessage.substring(0, readMessage.length() - 1);
                    Log.d("oguz", "msg: `"+readMessage+"`");


                    try {
                        DataMessage message = new DataMessage(readMessage.getBytes(StandardCharsets.UTF_8));
                        ChatMessage chatMessage;
                        String senderStr = message.getSender() == DataMessage.SENDER_MY_DEVICE ? "my" : "other";

                        // TODO: check sender
                        if (message.getType() == DataMessage.TYPE_HUMAN) {
                            chatMessage = new ChatMessage(message.getTextMessage(), message.getDate(), ChatMessage.Type.RECEIVED);
                            activity.chatView.addMessage(chatMessage);
                        }
                        else if (message.getType() == DataMessage.TYPE_SOS) {
                            chatMessage = new ChatMessage("SOS!!!!!!! "+senderStr, message.getDate(), ChatMessage.Type.RECEIVED);
                            activity.chatView.addMessage(chatMessage);
                        }
                        else if (message.getType() == DataMessage.TYPE_SENSOR) {
                            if (message.getSender() == DataMessage.SENDER_MY_DEVICE) {
                                activity.mySensorLabel.setText(
                                        message.getSensorData().getTemperature()+" °C\n" +
                                        message.getSensorData().getAltitude()+" m\n"
                                );
                            }
                            else {
                                chatMessage = new ChatMessage(message.getSensorData().toString()+" "+senderStr, message.getDate(), ChatMessage.Type.RECEIVED);
                                activity.chatView.addMessage(chatMessage);
                            }

                        }
                        else if (message.getType() == DataMessage.TYPE_ONLINE) {
                            chatMessage = new ChatMessage("Online "+senderStr, message.getDate(), ChatMessage.Type.RECEIVED);
                            activity.chatView.addMessage(chatMessage);
                        }


                    } catch (ParseException e) {
                        Log.e("oguz", "broken message `"+readMessage+"`");
                        e.printStackTrace();
//                        ChatMessage chatMessage = new ChatMessage("[Broken message]", new Date().getTime(), ChatMessage.Type.RECEIVED);
//                        activity.chatView.addMessage(chatMessage);
                    }


//                    if (readMessage != null && activity.showMessagesIsChecked) {
//                        ChatMessage messageRead = new ChatMessage(activity.device.getName(), readMessage.trim());
//                        activity.addMessageToAdapter(messageRead);
//
//                    }
                    break;

                case Constants.MESSAGE_SNACKBAR:
                    Snackbar.make(activity.coordinatorLayout, msg.getData().getString(Constants.SNACKBAR), Snackbar.LENGTH_LONG)
                            .setAction("Connect", v -> activity.reconnect()).show();

                    break;
            }
        }
    }

    private void reconnect() {
//        reconnectButton.setVisible(false);
        bluetoothService.stop();
        bluetoothService.connect();
    }

        private void setStatus(String status) {
            toolbar.setSubtitle(status);
        }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
                switch (state) {
                    case BluetoothAdapter.STATE_OFF:
                        break;
                    case BluetoothAdapter.STATE_TURNING_ON:

                        break;
                    case BluetoothAdapter.STATE_ON:
                        reconnect();
                }
            }
        }
    };

    @Override
    public boolean onSupportNavigateUp() {
        onBackPressed();
        return true;
    }
}
