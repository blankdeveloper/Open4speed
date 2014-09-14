package com.lvonasek.o4s.game;

import android.media.AudioManager;
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
import com.lvonasek.o4s.ui.menus.PauseMenu;

/**
 * Runnable class - it sets environment and run everything needed. It also manages interruptions of
 * sounds and game loop(e.g.during incoming call)
 *
 * @author Lubos Vonasek
 */
public class GameActivity extends FragmentActivity {

    //various instances
    private GameLoop gameLoop;
    public static GameActivity instance;

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

        setContentView(R.layout.race);
        instance = this;
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

        //new instance
        if (gameLoop == null) {
            //Main screen
            gameLoop = new GameLoop(this, null);
            //Ad banner
            //mAdView = new AdView(this, AdSize.SMART_BANNER, "a152250049be91b");
        }

        //create screen layout
        /*RelativeLayout layout = new RelativeLayout(instance);
        //add main view
        layout.addView(gameLoop);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams
                (RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_TOP |
                RelativeLayout.CENTER_HORIZONTAL, RelativeLayout.TRUE);

        //enable renderer
        setContentView(layout);*/
    }

    @Override
    /**
     * Integration of HW keys(or USB/nonUSB controller) into game
     */
    public boolean dispatchKeyEvent(KeyEvent event) {
        //press
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            Native.key(HWKeys.mapKeyCode(event.getKeyCode()));
            if (HWKeys.mapKeyCode(event.getKeyCode()) != event.getKeyCode())
                return true;
        }
        //release
        if (event.getAction() == KeyEvent.ACTION_UP) {
            Native.keyUp(HWKeys.mapKeyCode(event.getKeyCode()));
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
    protected synchronized void onPause() {
        //pause game
        if (gameLoop != null)
            gameLoop.paused = true;
        if (Native.snd != null)
            Native.snd.autoPause();
        super.onPause();
    }

    @Override
    /**
     * Game resume
     */
    protected void onResume() {
        //resume game
        super.onResume();
        instance = this;
        GameLoop.paused = false;
        if (Native.snd != null)
            Native.snd.autoResume();
    }

    @Override
    /**
     * Implement Android back button into game
     */
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        //change back key function
        if ((keyCode == KeyEvent.KEYCODE_BACK) || (keyCode == KeyEvent.KEYCODE_MENU)) {
            if (GameLoop.init)
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
    }

    public void pause() {
        GameLoop.paused = true;
        if (Native.snd != null)
            Native.snd.autoPause();
        FragmentManager fm = getSupportFragmentManager();
        PauseMenu pauseMenu = new PauseMenu();
        pauseMenu.show(fm, "menu_pause");
    }

    public void quit() {
        GameLoop.paused = true;
        Native.snd.autoPause();
        Native.snd = null;
        gameLoop = null;
        System.exit(0);
    }
}