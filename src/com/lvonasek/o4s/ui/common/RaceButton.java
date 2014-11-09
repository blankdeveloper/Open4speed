package com.lvonasek.o4s.ui.common;

import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.game.GameActivity;
import com.lvonasek.o4s.game.RaceInfo;
import com.lvonasek.o4s.media.Settings;

import static com.lvonasek.o4s.ui.menus.MainMenu.playButtonSound;

/**
 * GUI component for race event button rendering
 * @author Lubos Vonasek
 */
public class RaceButton extends ImageView {
    public RaceButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        setBackgroundResource(R.drawable.button_normal);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            setBackgroundResource(R.drawable.button_focus);
        } else if (event.getAction() == MotionEvent.ACTION_UP) {
            setBackgroundResource(R.drawable.button_normal);
        }
        return super.onTouchEvent(event);
    }

    @Override
    public void onFocusChanged(boolean focused, int dir, Rect rect) {
        if (focused) {
            setBackgroundResource(R.drawable.button_focus);
        } else {
            setBackgroundResource(R.drawable.button_normal);
        }
        super.onFocusChanged(focused, dir, rect);
    }

    @Override
    public boolean onHoverEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_HOVER_ENTER) {
            setBackgroundResource(R.drawable.button_focus);
        } else if (event.getAction() == MotionEvent.ACTION_HOVER_EXIT) {
            setBackgroundResource(R.drawable.button_normal);
        }
        return super.onHoverEvent(event);
    }


    @Override
    protected synchronized void onDraw(Canvas canvas) {
        super.onDraw(canvas);
    }

    public void setRace(final Context c, final int race) {
        setImageDrawable(c.getResources().getDrawable(RaceInfo.EVENT[race].splash));
        setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                playButtonSound();
                Settings.setConfig(c, Settings.RACE_EVENT, race);
                c.startActivity(new Intent(c, GameActivity.class));
            }
        });
    }
}
