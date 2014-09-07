package com.lvonasek.o4s.game;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.util.AttributeSet;

import com.lvonasek.o4s.Sounds;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Communication with native C++ code from java. It is also managing threads for graphical
 * and non-graphical processes. Main function of this class is calling C++ methods.
 * @author Lubos Vonasek
 */
public class GameLoop extends GLSurfaceView implements Renderer {

    //Game state
    public static boolean init    = false;
    public static boolean paused  = false;

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
        //enable OpenGL ES 2.0 support
        if (!isInEditMode()) {
            setEGLContextClientVersion(2);
            setRenderer(this);
        }
    }

    /**
     * Second init method - this is called when everything is ready
     * @param gl OpenGL object(unused because it is for java OpenGL interpretation)
     * @param config another OpenGL object(unused because it is for java OpenGL interpretation)
     */
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        //check if it is ran first time
        if (!init && !isInEditMode()) {
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

            //send APK file path into C++ code
            apkFilePath = appInfo.sourceDir;
            Sounds.init();
            Native.init();
            Native.init(apkFilePath);
            Sounds.soundLoad("sfx/02-danosongs.com-megacosm.mp3");
            Sounds.soundPlay(Integer.MAX_VALUE, 1);
            Sounds.soundVolume(Integer.MAX_VALUE, 1);
            GameActivity.instance.finishLoading();
        }

        init = true;
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
          Native.resize(w, h);
    }

    /**
     * OpenGL thread
     * @param gl is java OpenGL instance
     */
    public void onDrawFrame(GL10 gl) {
        if (isInEditMode() || !init)
            return;
        //check if game is not paused
        if (!paused) {
            long time = System.currentTimeMillis();
            Native.loop();

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
            Native.display();
    }
}