package com.lvonasek.o4s.ui.common;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.Gravity;
import android.widget.TextView;

public class SimpleText extends TextView {

    public SimpleText(Context context, AttributeSet attrs) {
        super(context, attrs);
        setTextColor((Color.YELLOW + Color.WHITE) / 2);
        setLinkTextColor(Color.WHITE);
        if (!isInEditMode()) {
            getPaint().setShadowLayer(5, 0, 0, Color.BLACK);
        }
        setGravity(Gravity.CENTER);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        for (int i = 0; i < 3; i++)
          super.onDraw(canvas);
    }
}