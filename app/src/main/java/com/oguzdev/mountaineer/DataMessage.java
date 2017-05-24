package com.oguzdev.mountaineer;

import java.nio.charset.StandardCharsets;
import java.text.ParseException;
import java.util.Arrays;
import java.util.Date;

/**
 * Copyright 2017 Oğuz Bilgener
 * Mountaineer
 */

public class DataMessage {

    public static final int TYPE_HUMAN = 1;
    public static final int TYPE_SOS = 2;
    public static final int TYPE_SENSOR = 3;
    public static final int TYPE_ONLINE = 4;

    public static final int SENDER_MY_PHONE = 1;
    public static final int SENDER_MY_DEVICE = 2;
    public static final int SENDER_OTHER = 3;

    private int type;
    private int sender;
    private long date;
    private byte[] payload;
    private String textMessage;
    private SensorData sensorData;

    public byte[] serialize() {
        if (payload != null) {
            return serializeForPayload(this, payload);
        }
        byte[] serializedPayload;

        if (type == TYPE_HUMAN) {
            serializedPayload = textMessage.getBytes(StandardCharsets.UTF_8);
        }
        else if (type == TYPE_SENSOR) {
            if (sensorData == null) {
                throw new IllegalStateException("SensorData cannot be null.");
            }
            serializedPayload = sensorData.serialize();
        }
        else {
            serializedPayload = new byte[0];
        }
        return serializeForPayload(this, serializedPayload);

    }

    public static byte[] serializeForPayload(DataMessage dataMessage, byte[] payload) {
        byte[] data = new byte[payload.length + 8];
        data[0] = '$';
        data[1] = '$';
        data[2] = '$';
        data[data.length - 1] = '$';
        data[data.length - 2] = '$';
        data[data.length - 3] = '$';
        data[3] = (byte) (dataMessage.getType() + '0');
        data[4] = (byte) (dataMessage.getSender() + '0');
        for (int i = 0; i < payload.length; i++) {
            data[5 + i] = payload[i];
        }
        return data;
    }

    public DataMessage(String text) {
        this.type = TYPE_HUMAN;
        this.sender = SENDER_MY_PHONE;
        this.date = new Date().getTime();
        this.payload = text.getBytes(StandardCharsets.UTF_8);
        this.textMessage = null;
        this.sensorData = null;
    }

    public DataMessage() {
        this.type = TYPE_ONLINE;
        this.sender = SENDER_MY_PHONE;
        this.date = new Date().getTime();
        this.textMessage = null;
        this.sensorData = null;
    }

    public DataMessage(byte[] serialized) throws ParseException {
        final int length = serialized.length;
        if (length < 8) {
            throw new ParseException("Message way too short: "+length, 0);
        }

        if (serialized[0] == '$' && serialized[1] == '$' && serialized[2] == '$' &&
                serialized[length - 1] == '$' && serialized[length - 2] == '$' && serialized[length - 3] == '$') {

            type = ((char) serialized[3]) - '0';
            sender = ((char) serialized[4]) - '0';

            if (type < 1 || type > 4) {
                throw new ParseException("Invalid type: "+type, 3);
            }

            if (sender < 1 || sender > 3) {
                throw new ParseException("Invalid sender: "+sender, 4);
            }

            payload = Arrays.copyOfRange(serialized, 5, length - 3);

            if (type == TYPE_HUMAN) {
                textMessage = new String(payload);
            }
            else if (type == TYPE_SENSOR) {
                sensorData = new SensorData(payload);
            }
        }
        else {
            throw new ParseException("Message start and end delimiters are invalid", 0);
        }
        this.date = new Date().getTime();
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getSender() {
        return sender;
    }

    public void setSender(int sender) {
        this.sender = sender;
    }

    public long getDate() {
        return date;
    }

    public void setDate(long date) {
        this.date = date;
    }

    public byte[] getPayload() {
        return payload;
    }

    public void setPayload(byte[] payload) {
        this.payload = payload;
    }

    public String getTextMessage() {
        return textMessage;
    }

    public void setTextMessage(String textMessage) {
        this.textMessage = textMessage;
    }

    public SensorData getSensorData() {
        return sensorData;
    }

    public void setSensorData(SensorData sensorData) {
        this.sensorData = sensorData;
    }


}
