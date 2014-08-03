package com.lvonasek.o4s.media;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.VideoView;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.ui.menus.MainMenu;

/**
 * Created by lubos on 15.7.14.
 */
public class VideoActivity extends Activity {

    private VideoView view= null;

    @Override
    /**
     * Init method
     */
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.video);
        view = (VideoView)findViewById(R.id.videoView);
    }

    @Override
    protected void onPause() {
        view.pause();
        super.onPause();
    }

    @Override
    protected void onStart() {
        super.onStart();
        final Activity activity = this;
        String path = "android.resource://" + getPackageName() + "/" + R.raw.intro;
        view.setVideoURI(Uri.parse(path));
        view.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mediaPlayer) {
                finish();
                startActivity(new Intent(activity, MainMenu.class));
            }
        });
        view.start();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            finish();
            startActivity(new Intent(this, MainMenu.class));
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        if (e.getAction() == MotionEvent.ACTION_DOWN) {
            finish();
            startActivity(new Intent(this, MainMenu.class));
            return true;
        }
        return super.onTouchEvent(e);
    }
}
