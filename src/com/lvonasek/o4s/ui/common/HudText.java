package com.lvonasek.o4s.ui.common;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.Gravity;
import android.widget.TextView;

import com.lvonasek.o4s.R;

/**
 * GUI component for nice and fast text rendering
 * @author Lubos Vonasek
 */
public class HudText extends TextView {

    private String lastText;

    public HudText(Context context, AttributeSet attrs) {
        super(context, attrs);
        getPaint().setShader(new LinearGradient(0, 0, 0, getTextSize(),
                Color.WHITE, Color.YELLOW, Shader.TileMode.CLAMP));
        if (!isInEditMode()) {
            String font = getResources().getString(R.string.ui_font);
            setTypeface(Typeface.createFromAsset(getContext().getAssets(), font));
        }
        setGravity(Gravity.CENTER_HORIZONTAL);
    }

    @Override
    public void setText(CharSequence text, BufferType type) {
        if (isInEditMode())
            return;
        if (lastText == null) {
            lastText = text.toString();
            super.setText(text, type);
            return;
        }
        if (lastText.compareTo(text.toString()) != 0) {
            lastText = text.toString();
            super.setText(text, type);
        }
    }
}