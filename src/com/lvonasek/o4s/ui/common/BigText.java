package com.lvonasek.o4s.ui.common;

import android.content.Context;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.widget.TextView;

import com.lvonasek.o4s.R;

public class BigText extends TextView {

    public BigText(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        if (changed && !isInEditMode()) {
            getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                    Color.WHITE, Color.YELLOW, Shader.TileMode.CLAMP));
            getPaint().setShadowLayer(5, 0, 0, Color.BLACK);
            String font = getResources().getString(R.string.ui_font);
            setTypeface(Typeface.createFromAsset(getContext().getAssets(), font));
        }
    }
}