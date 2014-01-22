package com.lvonasek.o4s.controllers;

/**
 * Support another controllers and control devices
 * @author Lubos Vonasek
 */
public class HWKeys {

    /**
     * Remap Android key codes into GLUT key codes. This method is used for gamepads and USB keybs.
     * @param code is received code from controller
     * @return code for GLUT
     */
    public static int mapKeyCode(int code) {
        switch (code) {
            case (21):
                return 100;
            case (22):
                return 102;
            case (19):
                return 101;
            case (20):
                return 103;
            case (62):
                return 160;
            default:
                return code;
        }
    }
}
