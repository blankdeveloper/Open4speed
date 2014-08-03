package com.lvonasek.o4s.game;

/**
 * Created by lubos on 15.6.14.
 */
public class Native {

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

    //include native C++ library
    public static void init() {
        System.loadLibrary("open4speed");
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
