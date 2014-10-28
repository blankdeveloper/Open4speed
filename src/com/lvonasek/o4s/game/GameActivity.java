package com.lvonasek.o4s.game;

import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.controllers.HWKeys;
import com.lvonasek.o4s.media.Sound;
import com.lvonasek.o4s.ui.menus.MainMenu;
import com.lvonasek.o4s.ui.menus.PauseMenu;

import java.io.IOException;

/**
 * Runnable class - it sets environment and run everything needed. It also manages interruptions of
 * sounds and game loop(e.g.during incoming call)
 *
 * @author Lubos Vonasek
 */
public class GameActivity extends FragmentActivity {

    //various instances
    public GameLoop            gameLoop   = null;
    public static boolean      init       = false;
    public static GameActivity instance   = null;
    private static MediaPlayer music      = new MediaPlayer();

    //splash items
    private ImageView loadingImg;
    private ProgressBar progressBar;
    public TextView fpsCounter;

    @Override
    /**
     * Init method
     */
    protected void onCreate(Bundle savedInstanceState) {
        //init
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_PROGRESS);
        setProgressBarVisibility(true);

        instance = this;
        setContentView(R.layout.race);
        fpsCounter = (TextView) findViewById(R.id.fpsCounter);
        gameLoop = (GameLoop) findViewById(R.id.game_screen);
        loadingImg = (ImageView) findViewById(R.id.loading);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);

        //keep screen on
        final Window win = getWindow();
        win.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        win.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);

        //set the hardware buttons to control the game sound
        setVolumeControlStream(AudioManager.STREAM_MUSIC);

        //reload button click sound
        try {
            AssetFileDescriptor afd = getAssets().openFd("sfx/button.wav");
            MainMenu.buttonClick = Sound.snd.load(afd, 1);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    /**
     * Integration of HW keys(or USB/nonUSB controller) into game
     */
    public boolean dispatchKeyEvent(KeyEvent event) {
        //press
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            gameLoop.key(HWKeys.mapKeyCode(event.getKeyCode()));
            if (HWKeys.mapKeyCode(event.getKeyCode()) != event.getKeyCode())
                return true;
        }
        //release
        if (event.getAction() == KeyEvent.ACTION_UP) {
            gameLoop.keyUp(HWKeys.mapKeyCode(event.getKeyCode()));
            if (HWKeys.mapKeyCode(event.getKeyCode()) != event.getKeyCode())
                return true;
        }
        //another keys(e.g.volume buttons)
        return super.dispatchKeyEvent(event);
    }

    @Override
    /**
     * Game pause
     */
    protected void onPause() {
        //pause game
        if (init)
            music.pause();
        else
            quit();
        if (gameLoop != null)
            gameLoop.paused++;
        if (Sound.snd != null)
            Sound.snd.autoPause();
        super.onPause();
    }

    @Override
    /**
     * Game resume
     */
    protected void onResume() {
        //new instance of game
        if (gameLoop == null) {
            //Main screen
            gameLoop = new GameLoop(this, null);
        }

        //resume game
        super.onResume();
        instance = this;
        gameLoop.paused--;
        if (Sound.snd != null)
            Sound.snd.autoResume();
        if (init) {
            music.start();
            if (GameLoop.paused <= 0)
                GameLoop.paused = 0;
            else
                pause();
        }
    }

    @Override
    public void onStart() {
        super.onStart();
        overridePendingTransition(0, 0);
    }

    @Override
    /**
     * Implement Android back button into game
     */
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        //change back key function
        if ((keyCode == KeyEvent.KEYCODE_BACK) || (keyCode == KeyEvent.KEYCODE_MENU)) {
            if (init)
                pause();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    public void finishLoading() {
        setProgressBarVisibility(false);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                loadingImg.setVisibility(View.INVISIBLE);
                progressBar.setVisibility(View.INVISIBLE);
            }
        });

        try {
            AssetFileDescriptor afd = getAssets().openFd("sfx/02-danosongs.com-megacosm.mp3");
            music = new MediaPlayer();
            music.setAudioStreamType(AudioManager.STREAM_MUSIC);
            music.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
            music.setLooping(true);
            music.prepare();
            if (GameLoop.paused <= 0) {
                music.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                    @Override
                    public void onPrepared(MediaPlayer mp) {
                        mp.start();
                    }
                });
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void pause() {
        GameLoop.paused++;
        if (Sound.snd != null)
            Sound.snd.autoPause();
        FragmentManager fm = getSupportFragmentManager();
        PauseMenu pauseMenu = new PauseMenu();
        pauseMenu.show(fm, "menu_pause");
    }


    public void quit() {
        Sound.snd.autoPause();
        if (init)
            gameLoop.unload();
        Sound.snd = null;
        gameLoop = null;
        GameLoop.paused = 0;
        System.exit(0);
    }
}