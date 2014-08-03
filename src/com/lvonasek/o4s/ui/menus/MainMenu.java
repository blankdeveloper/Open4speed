package com.lvonasek.o4s.ui.menus;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.widget.VideoView;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.game.GameActivity;

/**
 * Created by lubos on 31.7.14.
 */
public class MainMenu extends Activity {

    private VideoView view = null;

    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        setContentView(R.layout.menu_main);
        view = (VideoView)findViewById(R.id.menu_background);

        final Context c = this;

        findViewById(R.id.menu_main_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(c, GameActivity.class));
            }
        });

        findViewById(R.id.menu_main_exit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                finish();
            }
        });
    }


    @Override
    protected void onPause() {
        view.pause();
        super.onPause();
    }

    @Override
    protected void onStart() {
        super.onStart();
        String path = "android.resource://" + getPackageName() + "/" + R.raw.menu_background;
        view.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                mp.setLooping(true);
                mp.start();
            }
        });
        view.setVideoURI(Uri.parse(path));
    }
}
