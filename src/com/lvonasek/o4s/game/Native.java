package com.lvonasek.o4s.game;

import com.lvonasek.o4s.media.Sound;

import java.util.ArrayList;

/**
 * Created by lubos on 15.6.14.
 */
public class Native {

    // instance of sounds
    private static ArrayList<Sound> list;

    public static final int CAR_INFO_SPEED = 0;
    public static final int CAR_INFO_PEDAL_GAS = 1;
    public static final int CAR_INFO_PEDAL_BRAKE = 2;
    public static final int CAR_INFO_REVERSE = 3;
    public static final int CAR_INFO_LASTSPEED = 4;
    public static final int CAR_INFO_GEARLOW = 5;
    public static final int CAR_INFO_GEARHIGH = 6;
    public static final int CAR_INFO_PEDAL_N2O = 7;
    public static final int CAR_INFO_GEARMIN = 8;
    public static final int CAR_INFO_GEARMAX = 9;
    public static final int CAR_INFO_N2OAMOUNT = 10;
    public static final int CAR_INFO_CAMERADST = 11;
    public static final int CAR_INFO_SNDCRASH = 12;
    public static final int CAR_INFO_SNDDIST = 13;
    public static final int CAR_INFO_SNDENGINE1 = 14;
    public static final int CAR_INFO_SNDENGINE2 = 15;
    public static final int CAR_INFO_SNDN2O = 16;
    public static final int CAR_INFO_SNDRATE = 17;

    //include native C++ library
    public static void init() {
        System.loadLibrary("open4speed");
        list = new ArrayList<Sound>();
        for (int i = 0; i < 1; i++) {
            list.add(new Sound("sfx/crash.ogg", false));
            list.add(new Sound("sfx/engine.ogg", true));
            list.add(new Sound("sfx/engineplus.ogg", true));
        }
    }

    public static void update() {
        int count = 1;//carCount();
        for (int i = 0; i < count; i++) {
            boolean crash = carState(i, CAR_INFO_SNDCRASH) > 0.5f;
            float dist = carState(i, CAR_INFO_SNDDIST);
            float engine1 = carState(i, CAR_INFO_SNDENGINE1);
            float engine2 = carState(i, CAR_INFO_SNDENGINE2);
            float rate = carState(i, CAR_INFO_SNDRATE);
            if (dist > 0.1f) {
                if (crash)
                    list.get(0 + i * 3).play();
                list.get(1 + i * 3).setFreq(rate);
                list.get(1 + i * 3).setVolume(dist * engine1);
                list.get(1 + i * 3).play();
                list.get(2 + i * 3).setFreq(rate);
                list.get(2 + i * 3).setVolume(dist * engine2);
                list.get(2 + i * 3).play();
            } else {
                for (int j = 0 + i * 4; j < 4 + i * 4; j++)
                    list.get(j).stop();
            }
        }
    }

    //C++ methods
    public static native int carCount();
    public static native float carState(int index, int type);
    public static native void init(String str);
    public static native void key(int code);
    public static native void keyUp(int code);
    public static native void display();
    public static native void loop();
    public static native void resize(int w, int h);
}
