package com.lvonasek.o4s;

import android.app.Activity;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.lvonasek.o4s.controllers.HWKeys;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Runnable class - it sets environment and run everything needed. It also manages interruptions of
 * sounds and game loop(e.g.during incoming call)
 *
 * @author Lubos Vonasek
 */
public class O4SActivity extends Activity {

    //paths(NOTE: static path work on restricted profile but sounds not)
    private static final String path = "/data/data/com.lvonasek.o4s/";
    private static final String configFile = path + "config";
    private static final String versionFile = path + "open4speed.1.20";
    //various instances
    private O4SJNI mO4SJNI;
    public static O4SActivity mO4SActivity;

    @Override
    /**
     * Init method
     */
    protected void onCreate(Bundle savedInstanceState) {
        //init
        super.onCreate(savedInstanceState);
        mO4SActivity = this;

        //keep screen on
        final Window win = getWindow();
        win.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        win.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);

        //set the hardware buttons to control the game sound
        setVolumeControlStream(AudioManager.STREAM_MUSIC);

        //check if the game is installed
        if (!new File(versionFile).exists()) {
            //install config file on sd card
            try {
                //unpack initial configuration
                if (!new File(configFile).exists()) {
                    InputStream i = getAssets().open("init.zip");
                    new File(path).mkdirs();
                    Utils.unzip(i, path);
                    i.close();
                }

                //create version file
                FileOutputStream fout = new FileOutputStream(versionFile);
                fout.write("Created by Luboš Vonásek".getBytes());
                fout.close();
            } catch (IOException ex) {
            }
        }

        //new instance
        if (mO4SJNI == null) {
            //Main screen
            mO4SJNI = new O4SJNI(this);
            //Ad banner
            //mAdView = new AdView(this, AdSize.SMART_BANNER, "a152250049be91b");
        }

        //create screen layout
        RelativeLayout layout = new RelativeLayout(mO4SActivity);
        //add main view
        layout.addView(mO4SJNI);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams
                (RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_TOP |
                RelativeLayout.CENTER_HORIZONTAL, RelativeLayout.TRUE);

        //enable renderer
        setContentView(layout);
        mO4SJNI.setRenderer(mO4SJNI);
        mO4SJNI.setRenderMode(mO4SJNI.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    /**
     * Integration of HW keys(or USB/nonUSB controller) into game
     */
    public boolean dispatchKeyEvent(KeyEvent event) {
        //press
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            O4SJNI.nativeKey(HWKeys.mapKeyCode(event.getKeyCode()));
            if (HWKeys.mapKeyCode(event.getKeyCode()) != event.getKeyCode())
                return true;
        }
        //release
        if (event.getAction() == KeyEvent.ACTION_UP) {
            O4SJNI.nativeKeyUp(HWKeys.mapKeyCode(event.getKeyCode()));
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
        super.onPause();
        if (mO4SJNI != null) {
            if (mO4SJNI.init) {
                //pause
                mO4SJNI.paused = true;
                //pause sounds
                Sounds.snd.autoPause();
                Sounds.music.pause();
            }
        }
    }

    @Override
    /**
     * Game resume
     */
    protected void onResume() {
        //resume game
        super.onResume();
        mO4SActivity = this;
        if (mO4SJNI != null) {
            if (mO4SJNI.init) {
                //unpause
                mO4SJNI.paused = false;
                //resume sounds
                Sounds.snd.autoResume();
                Sounds. music.start();
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
            O4SJNI.nativeBack();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }
}