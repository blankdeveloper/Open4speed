package com.lvonasek.o4s.game;

import android.app.Activity;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.Sounds;
import com.lvonasek.o4s.controllers.HWKeys;

/**
 * Runnable class - it sets environment and run everything needed. It also manages interruptions of
 * sounds and game loop(e.g.during incoming call)
 *
 * @author Lubos Vonasek
 */
public class GameActivity extends Activity {

    //various instances
    private static GameLoop gameLoop;
    public static GameActivity instance;

    @Override
    /**
     * Init method
     */
    protected void onCreate(Bundle savedInstanceState) {
        //init
        super.onCreate(savedInstanceState);
        setContentView(R.layout.race);
        instance = this;
        gameLoop = (GameLoop) findViewById(R.id.game_screen);

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
    protected void onPause() {
        //pause game
        if (gameLoop != null)
            if (gameLoop.init) {
                gameLoop.paused = true;
                Sounds.snd.autoPause();
            }
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
        if (gameLoop != null) {
            if (gameLoop.init) {
                gameLoop.paused = false;
                Sounds.snd.autoResume();
            }
        }
    }

    @Override
    /**
     * Implement Android back button into game
     */
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        //change back key function
        if ((keyCode == KeyEvent.KEYCODE_BACK) || (keyCode == KeyEvent.KEYCODE_MENU)) {
            GameLoop.init = false;
            finish();
        }
        return super.onKeyDown(keyCode, event);
    }
}