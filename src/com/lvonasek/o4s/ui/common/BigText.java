package com.lvonasek.o4s.ui.common;

import android.content.Context;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.Gravity;
import android.widget.TextView;

import com.lvonasek.o4s.R;

public class BigText extends TextView {

    public BigText(Context context, AttributeSet attrs) {
        super(context, attrs);
        getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                Color.WHITE, Color.YELLOW, Shader.TileMode.CLAMP));
        if (!isInEditMode()) {
            getPaint().setShadowLayer(5, 0, 0, Color.BLACK);
            String font = getResources().getString(R.string.ui_font);
            setTypeface(Typeface.createFromAsset(getContext().getAssets(), font));
        }
        setGravity(Gravity.CENTER_HORIZONTAL);
    }
}