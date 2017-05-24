package com.oguzdev.mountaineer;

import java.nio.charset.StandardCharsets;
import java.text.ParseException;
import java.util.Locale;

/**
 * Copyright 2017 Oğuz Bilgener
 * Mountaineer
 */

public class SensorData {

    public float temperature;
    public int altitude;

    public SensorData(byte[] payload) throws ParseException {
        StringBuilder tempBuilder = new StringBuilder();
        StringBuilder altBuilder  = new StringBuilder();
        boolean tempDone = false;
        for (int i=0;i<payload.length;i++) {
            if (!tempDone && payload[i] == '#') {
                tempDone = true;
            }
            else {
                if (!tempDone) {
                    tempBuilder.append(payload[i]);
                } else {
                    altBuilder.append(payload[i]);
                }
            }
        }

        try {
            temperature = Float.parseFloat(tempBuilder.toString());
        } catch (NumberFormatException e) {
            throw new ParseException(String.format("Invalid temperature: '%s'", tempBuilder.toString()), 0);
        }
        try {
            altitude = Integer.parseInt(altBuilder.toString());
        }
        catch (NumberFormatException e) {
            throw new ParseException(String.format("Invalid altitude: '%s'", altBuilder.toString()), 0);
        }

        if (!tempDone) {
            throw new ParseException("No data separator", payload.length-1);
        }
    }

    public byte[] serialize() {
        return String.format(Locale.US, "%.1f#%d", temperature, altitude).getBytes(StandardCharsets.UTF_8);
    }

    public float getTemperature() {
        return temperature;
    }

    public void setTemperature(float temperature) {
        this.temperature = temperature;
    }

    public int getAltitude() {
        return altitude;
    }

    public void setAltitude(int altitude) {
        this.altitude = altitude;
    }

    @Override
    public String toString() {
        return "sensor:" + temperature +
                ", altitude=" + altitude;
    }
}
