package com.lvonasek.o4s;

/**
 * Created by lubos on 15.6.14.
 */
public class Native {

    //include native C++ library
    static {
        System.loadLibrary("open4speed");
    }

    //C++ methods
    public static native void back();
    public static native void display();
    public static native void init(String str);
    public static native void key(int code);
    public static native void keyUp(int code);
    public static native void loop();
    public static native void resize(int w, int h);
}
