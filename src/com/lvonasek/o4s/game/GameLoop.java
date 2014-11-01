package com.lvonasek.o4s.game;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.util.AttributeSet;

import com.lvonasek.o4s.media.Settings;
import com.lvonasek.o4s.media.Sound;

import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Communication with native C++ code from java. It is also managing threads for graphical
 * and non-graphical processes. Main function of this class is calling C++ methods.
 * @author Lubos Vonasek
 */
public class GameLoop extends GLSurfaceView implements Renderer {

    public static final int CAR_INFO_SPEED = 0;
    public static final int CAR_INFO_PEDAL_GAS = 1;
    public static final int CAR_INFO_PEDAL_BRAKE = 2;
    public static final int CAR_INFO_REVERSE = 3;
    public static final int CAR_INFO_LASTSPEED = 4;
    public static final int CAR_INFO_GEARLOW = 5;
    public static final int CAR_INFO_GEARHIGH = 6;
    public static final int CAR_INFO_PEDAL_N2O = 7;
    public static final int CAR_INFO_GEARMIN = 8;
    public static final int CAR_INFO_GEARMAX = 9;
    public static final int CAR_INFO_N2OAMOUNT = 10;
    public static final int CAR_INFO_CAMERADST = 11;
    public static final int CAR_INFO_SNDCRASH = 12;
    public static final int CAR_INFO_SNDDIST = 13;
    public static final int CAR_INFO_SNDENGINE1 = 14;
    public static final int CAR_INFO_SNDENGINE2 = 15;
    public static final int CAR_INFO_SNDN2O = 16;
    public static final int CAR_INFO_SNDRATE = 17;

    //Game state
    public static int     paused  = 0;

    // instance of sounds
    private static ArrayList<Sound> sounds;

    //fps counter
    private int currentFPS;
    private long updateFPS;

    /**
     * Constructor which creates support for all incoming events
     * @param context
     * @param attrs
     */
    public GameLoop(Context context, AttributeSet attrs) {
        super(context);
        if (isInEditMode())
            return;
        System.loadLibrary("open4speed");
        sounds = new ArrayList<Sound>();
        for (int i = 0; i < 1; i++) {
            sounds.add(new Sound("sfx/crash.ogg", false));
            sounds.add(new Sound("sfx/engine.ogg", true));
            sounds.add(new Sound("sfx/engineplus.ogg", true));
        }
        //enable OpenGL ES 2.0 support
        setEGLConfigChooser(8, 8, 8, 0, 16, 1);
        setEGLContextClientVersion(2);
        setRenderer(this);
    }

    /**
     * Second init method - this is called when everything is ready
     * @param gl OpenGL object(unused because it is for java OpenGL interpretation)
     * @param config another OpenGL object(unused because it is for java OpenGL interpretation)
     */
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        //check if it is first time run
        if (!GameActivity.instance.init) {

            //get package assets object
            String apkFilePath;
            ApplicationInfo appInfo;
            PackageManager packMgmr = GameActivity.instance.getPackageManager();

            //try to get assets
            try {
                appInfo = packMgmr.getApplicationInfo("com.lvonasek.o4s", 0);
            } catch (Exception e) {
                e.printStackTrace();
                throw new RuntimeException("Unable to locate assets, aborting...");
            }

            //load game
            apkFilePath = appInfo.sourceDir;
            float quality = 0.01f * Settings.getConfig(GameActivity.instance, Settings.VISUAL_QUALITY);
            quality = 0.33f + 0.67f * quality;
            init(apkFilePath, quality);
            GameActivity.instance.finishLoading();
        }
        GameActivity.instance.init = true;
    }

    /**
     * Screen was resided
     * Game is forced into landscape mode so this method may not be called
     * @param gl is java OpenGL instance
     * @param w is width in pixels
     * @param h is height in pixels
     */
    public void onSurfaceChanged(GL10 gl, int w, int h) {
        //send new screen dimensions into C++ code
        if (!isInEditMode())
          resize(w, h);
    }

    /**
     * OpenGL thread
     * @param gl is java OpenGL instance
     */
    public void onDrawFrame(GL10 gl) {
        if (isInEditMode() || !GameActivity.instance.init)
            return;
        //check if game is not paused
        if (paused <= 0) {
            paused = 0;
            long time = System.currentTimeMillis();
            display();
            loop();
            update();

            currentFPS++;
            if (updateFPS + 1000 < System.currentTimeMillis()) {
                updateFPS = System.currentTimeMillis();
                final int FPS = currentFPS;
                GameActivity.instance.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        GameActivity.instance.fpsCounter.setText("FPS:" + FPS);
                    }
                });
                currentFPS = 0;
            }

            while (System.currentTimeMillis() - time < 50) {}
        } else
            display();
    }

    public void update() {
        int count = 1;//carCount();
        for (int i = 0; i < count; i++) {
            boolean crash = carState(i, CAR_INFO_SNDCRASH) > 0.5f;
            float dist = carState(i, CAR_INFO_SNDDIST);
            float engine1 = carState(i, CAR_INFO_SNDENGINE1);
            float engine2 = carState(i, CAR_INFO_SNDENGINE2);
            float rate = carState(i, CAR_INFO_SNDRATE);
            if (dist > 0.1f) {
                if (crash)
                    sounds.get(0 + i * 3).play();
                sounds.get(1 + i * 3).setFreq(rate);
                sounds.get(1 + i * 3).setVolume(dist * engine1);
                sounds.get(1 + i * 3).play();
                sounds.get(2 + i * 3).setFreq(rate);
                sounds.get(2 + i * 3).setVolume(dist * engine2);
                sounds.get(2 + i * 3).play();
            } else {
                for (int j = 0 + i * 4; j < 4 + i * 4; j++)
                    sounds.get(j).stop();
            }
        }
    }

    //C++ methods
    public synchronized native int carCount();
    public synchronized native float carState(int index, int type);
    public synchronized native void init(String str, float aliasing);
    public synchronized native void key(int code);
    public synchronized native void keyUp(int code);
    public synchronized native void display();
    public synchronized native void loop();
    public synchronized native void resize(int w, int h);
    public synchronized native void unload();
    public synchronized native void unlock();
}