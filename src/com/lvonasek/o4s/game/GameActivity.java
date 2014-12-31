package com.lvonasek.o4s.game;

import android.content.Intent;
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
import com.lvonasek.o4s.media.Settings;
import com.lvonasek.o4s.media.Sound;
import com.lvonasek.o4s.ui.common.BigText;
import com.lvonasek.o4s.ui.common.HudText;
import com.lvonasek.o4s.ui.menus.MainMenu;
import com.lvonasek.o4s.ui.menus.PauseMenu;

import java.io.IOException;

/**
 * Runnable class - it sets environment and run everything needed. It also manages interruptions of
 * sounds and game loop(e.g.during incoming call)
 * @author Lubos Vonasek
 */
public class GameActivity extends FragmentActivity {

    //various instances
    public TextView            fpsCounter = null;
    public GameLoop            gameLoop   = null;
    public static boolean      init       = false;
    public static HudText[]    infopanel  = null;
    public static GameActivity instance   = null;
    private ImageView          loadingBkg = null;
    private ImageView          loadingImg = null;
    public static MediaPlayer  music      = new MediaPlayer();
    public static BigText      place      = null;
    private ProgressBar        progressBar= null;
    public static BigText      restart    = null;
    public static int          restartable= 0;
    public static boolean      started    = false;
    private int[]              startSnd   = null;

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
        infopanel = new HudText[3];
        infopanel[0] = (HudText) findViewById(R.id.infopanel1);
        infopanel[1] = (HudText) findViewById(R.id.infopanel2);
        infopanel[2] = (HudText) findViewById(R.id.infopanel3);
        loadingBkg = (ImageView) findViewById(R.id.loadingBkg);
        loadingImg = (ImageView) findViewById(R.id.loading);
        place = (BigText) findViewById(R.id.place);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);
        restart = (BigText) findViewById(R.id.restart);

        //keep screen on
        final Window win = getWindow();
        win.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        win.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);

        //set the hardware buttons to control the game sound
        setVolumeControlStream(AudioManager.STREAM_MUSIC);

        //set loading splash
        int event = Settings.getConfig(this, Settings.RACE_EVENT);
        if (getIntent().getAction() == Intent.ACTION_VIEW) {
            String path = getIntent().getDataString();
            path = path.substring(path.indexOf("file://") + 7);
            Settings.setConfig(this, Settings.RACE_CUSTOM_EVENT, path);
            event = -1;
            Settings.setConfig(this, Settings.RACE_EVENT, event);
            loadingImg.setVisibility(View.GONE);
        }
        if (event >= 0)
            loadingImg.setBackground(getResources().getDrawable(RaceInfo.EVENT[event].splash));
        else
            loadingImg.setVisibility(View.GONE);

        //start sounds
        startSnd = new int[2];
        for (int i = 1; i <= startSnd.length; i++)
            try {
                startSnd[i - 1] = Sound.snd.load(getAssets().openFd("sfx/start" + i + ".wav"), 1);
            } catch (IOException e) {
                e.printStackTrace();
            }

        //reload button click sound
        try {
            AssetFileDescriptor afd = getAssets().openFd("sfx/button.wav");
            MainMenu.buttonClick = Sound.snd.load(afd, 1);
        } catch (IOException e) {
            e.printStackTrace();
        }

        restart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainMenu.playButtonSound();
                restartable = 0;
                gameLoop.restart();
            }
        });
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
                loadingBkg.setVisibility(View.GONE);
                loadingImg.setVisibility(View.GONE);
                progressBar.setVisibility(View.GONE);
            }
        });

        try {
            music.setAudioStreamType(AudioManager.STREAM_MUSIC);
            int event = Settings.getConfig(this, Settings.RACE_EVENT);
            if (event >= 0) {
                AssetFileDescriptor afd = getAssets().openFd(RaceInfo.EVENT[event].music);
                music.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
                final float volume = Settings.getConfig(this, Settings.MUSIC_VOLUME) * 0.01f;
                if (GameLoop.paused <= 0) {
                    music.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                        @Override
                        public void onPrepared(MediaPlayer mp) {
                            mp.setVolume(volume, volume);
                            mp.setLooping(true);
                            mp.start();
                        }
                    });
                }
                music.prepare();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        final ImageView[] start = {(ImageView) findViewById(R.id.start0),
                                   (ImageView) findViewById(R.id.start1),
                                   (ImageView) findViewById(R.id.start2),
                                   (ImageView) findViewById(R.id.start3)};
        new Thread(new Runnable() {

            private void playSnd(int index) {
                Sound.snd.play(startSnd[index], 1, 1, 1, 0, 1);
            }

            private void show(final ImageView[] start, final int index) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        for (int i = 0; i < start.length; i++) {
                            if (i == index)
                                start[i].setVisibility(View.VISIBLE);
                            else
                                start[i].setVisibility(View.GONE);
                        }
                    }
                });
            }

            private void sleep() {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void run() {
                for (int i = 0; i < start.length; i++) {
                    sleep();
                    if ((i == 1) || (i == 2))
                        playSnd(0);
                    else if (i == 3)
                        playSnd(1);
                    show(start, i);
                }
                gameLoop.unlock();
                sleep();
                show(start, -1);
                started = true;
            }
        }).start();
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