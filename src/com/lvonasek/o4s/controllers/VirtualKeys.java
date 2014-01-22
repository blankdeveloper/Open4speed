package com.lvonasek.o4s.controllers;

import android.view.MotionEvent;
import com.lvonasek.o4s.O4SJNI;

/**
 * Manage virtual keys. It is fully multi touched so there are no limits in key pressings. This is
 * the most universal way to detect virtual buttons.
 * @author Lubos Vonasek
 */
public class VirtualKeys {

    //define buttons
    private static Button left = new Button(0, 0.761f, 0.112f, 1, 100);
    private static Button right = new Button(0.156f, 0.761f, 0.263f, 1, 102);
    private static Button n2o = new Button(0.886f, 0.221f, 1, 0.454f, 160);
    private static Button up = new Button(0.886f, 0.452f, 1, 0.695f, 101);
    private static Button down = new Button(0.886f, 0.763f, 1, 1, 103);

    /**
     * Button structure
     */
    static class Button {

        float x1, y1, x2, y2;
        int code;
        boolean first, last;

        /**
         * Create button as a virtual key
         * @param x1 is top left x coordinate(values from 0 to 1)
         * @param y1 is top left y coordinate(values from 0 to 1)
         * @param x2 is bottm right x coordinate(values from 0 to 1)
         * @param y2 is bottm right y coordinate(values from 0 to 1)
         * @param code is keycode to send to C++ code after press/release
         */
        public Button(float x1, float y1, float x2, float y2, int code) {
            this.x1 = x1;
            this.y1 = y1;
            this.x2 = x2;
            this.y2 = y2;
            this.code = code;
            first = true;
        }

        /**
         * Method to call C++ code on press
         * @param down is true when button is pressed
         */
        public void action(boolean down) {
            //filter method multi calling
            if (first || (last != down)) {
                //call C++ code
                if (down) {
                    O4SJNI.nativeKey(code);
                } else {
                    O4SJNI.nativeKeyUp(code);
                }
                //save state
                last = down;
                first = false;
            }
        }

        /**
         * Check is position is inside the button
         * @param x is x coordinate in screen space(from 0 to 1)
         * @param y is y coordinate in screen space(from 0 to 1)
         * @return true if it is inside
         */
        public boolean isInside(float x, float y) {
            return (x >= x1) & (y >= y1) & (x <= x2) & (y <= y2);
        }
    }

    /**
     * Apply click on the screen button
     * @param e is information about touch
     * @param screenWidth is screen width
     * @param screenHeight is screen height
     */
    public static void click(final MotionEvent e, float screenWidth, float screenHeight) {

        //set status unpressed
        boolean l = false;
        boolean r = false;
        boolean u = false;
        boolean d = false;
        boolean n = false;

        for (int i = 0; i < e.getPointerCount(); i++) {
            //get finger position(from 0 to 1)
            float x = e.getX(i) / screenWidth;
            float y = e.getY(i) / screenHeight;
            boolean pressed = true;

            //filter last released pointer
            if ((e.getActionMasked() == MotionEvent.ACTION_POINTER_UP) ||
                (e.getActionMasked() == MotionEvent.ACTION_UP)) {
                if (e.getActionIndex() == e.findPointerIndex(e.getPointerId(i))) {
                    pressed = false;
                }
            }

            //get press state
            l = l || (left.isInside(x, y) && pressed);
            r = r || (right.isInside(x, y) && pressed);
            u = u || (up.isInside(x, y) && pressed);
            d = d || (down.isInside(x, y) && pressed);
            n = n || (n2o.isInside(x, y) && pressed);
        }

        //apply key actions
        left.action(l);
        right.action(r);
        up.action(u);
        down.action(d);
        n2o.action(n);
    }
}
