package com.lvonasek.o4s.controllers;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.lvonasek.o4s.Native;
import com.lvonasek.o4s.R;

/**
 * Manage virtual keys. It is fully multi touched so there are no limits in key pressings. This is
 * the most universal way to detect virtual buttons.
 * @author Lubos Vonasek
 */
public class VirtualKey extends View {

    public VirtualKey(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        int code = 0;
        switch (getId()) {
            case R.id.arrow_left:
                code = 100;
                break;
            case R.id.arrow_up:
                code = 101;
                break;
            case R.id.arrow_right:
                code = 102;
                break;
            case R.id.arrow_down:
                code = 103;
                break;
            case R.id.arrow_n2o:
                code = 160;
                break;
        }
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            Native.key(code);
        } else if (event.getAction() == MotionEvent.ACTION_UP) {
            Native.keyUp(code);
        }

        return true;
    }
}
