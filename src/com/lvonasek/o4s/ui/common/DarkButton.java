package com.lvonasek.o4s.ui.common;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Rect;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.Button;

import com.lvonasek.o4s.R;

/**
 * Created by lubos on 1.8.14.
 */
public class DarkButton extends Button {
    public DarkButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                Color.WHITE, Color.YELLOW, Shader.TileMode.CLAMP));
        if (!isInEditMode()) {
            getPaint().setShadowLayer(5, 0, 0, Color.BLACK);
            String font = getResources().getString(R.string.ui_font);
            setTypeface(Typeface.createFromAsset(getContext().getAssets(), font));
        }
        setBackgroundResource(R.drawable.button_normal);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            setBackgroundResource(R.drawable.button_focus);
            getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                    Color.WHITE, Color.LTGRAY, Shader.TileMode.CLAMP));
        } else if (event.getAction() == MotionEvent.ACTION_UP) {
            setBackgroundResource(R.drawable.button_normal);
            getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                    Color.WHITE, Color.YELLOW, Shader.TileMode.CLAMP));
        }
        return super.onTouchEvent(event);
    }

    @Override
    public void onFocusChanged(boolean focused, int dir, Rect rect) {
        if (focused) {
            setBackgroundResource(R.drawable.button_focus);
            getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                    Color.WHITE, Color.LTGRAY, Shader.TileMode.CLAMP));
        } else {
            setBackgroundResource(R.drawable.button_normal);
            getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                    Color.WHITE, Color.YELLOW, Shader.TileMode.CLAMP));
        }
        super.onFocusChanged(focused, dir, rect);
    }

    @Override
    public boolean onHoverEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_HOVER_ENTER) {
            setBackgroundResource(R.drawable.button_focus);
            getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                    Color.WHITE, Color.LTGRAY, Shader.TileMode.CLAMP));
        } else if (event.getAction() == MotionEvent.ACTION_HOVER_EXIT) {
            setBackgroundResource(R.drawable.button_normal);
            getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                    Color.WHITE, Color.YELLOW, Shader.TileMode.CLAMP));
        }
        return super.onHoverEvent(event);
    }


    @Override
    protected synchronized void onDraw(Canvas canvas) {
        Shader sh = getPaint().getShader();
        getPaint().setShader(null);
        super.onDraw(canvas);
        getPaint().setShader(sh);
        super.onDraw(canvas);
    }
}
