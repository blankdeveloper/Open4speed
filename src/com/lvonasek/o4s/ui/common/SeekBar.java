package com.lvonasek.o4s.ui.common;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.Gravity;
import android.widget.TextView;

import com.lvonasek.o4s.R;

public class SeekBar extends android.widget.SeekBar {

    public SeekBar(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        Paint p = new Paint();
        p.setStyle(Paint.Style.FILL);
        p.setShader(new LinearGradient(0, 0, 0, getHeight(),
                Color.BLACK, Color.GRAY, Shader.TileMode.CLAMP));
        canvas.drawRect(0, 0, getWidth(), getHeight(), p);
        p.setShader(new LinearGradient(0, 0, 0, getHeight(),
                Color.YELLOW, Color.BLACK, Shader.TileMode.CLAMP));
        canvas.drawRect(0, 0, getProgress() * getWidth() / getMax(), getHeight(), p);
        p.setStyle(Paint.Style.STROKE);
        p.setStrokeWidth(5);
        p.setShader(new LinearGradient(0, 0, 0, getHeight(),
                Color.WHITE, Color.GRAY, Shader.TileMode.CLAMP));
        canvas.drawRect(0, 0, getWidth(), getHeight(), p);
    }
}