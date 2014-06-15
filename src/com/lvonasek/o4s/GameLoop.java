package com.lvonasek.o4s;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.util.AttributeSet;
import java.util.Timer;
import java.util.TimerTask;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Communication with native C++ code from java. It is also managing threads for graphical
 * and non-graphical processes. Main function of this class is calling C++ methods.
 * @author Lubos Vonasek
 */
public class GameLoop extends GLSurfaceView implements Renderer {

    //Thread objects
    public Timer timer;
    public TimerTask game;
    //Thread timing
    private final long DEFAULT_SCHEDULE = 1000 / 15;
    private long schedule = DEFAULT_SCHEDULE;
    //Game state
    boolean init = false;
    boolean paused = false;

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
            setRenderMode(RENDERMODE_WHEN_DIRTY);
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
            PackageManager packMgmr = O4SActivity.mO4SActivity.getPackageManager();

            //try to get assets
            try {
                appInfo = packMgmr.getApplicationInfo("com.lvonasek.o4s", 0);
            } catch (Exception e) {
                e.printStackTrace();
                throw new RuntimeException("Unable to locate assets, aborting...");
            }

            //send APK file path into C++ code
            apkFilePath = appInfo.sourceDir;
            Native.init(apkFilePath);
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
        //check if game is not paused
        if (!paused && init && !isInEditMode()) {

            //if non-graphical thread is not exist then create it and start it
            if (game == null) {
                game = new Game();
                //first run in graphical thread(it solve problem with null pointer exceptions)
                game.run();
                //periodically run non-graphical thread
                timer = new Timer();
                timer.schedule(game, 1, schedule);
            }

            //run graphical code
            Native.display();
        }
    }

    /**
     * Thread for non-graphical processes
     */
    class Game extends TimerTask {
        @Override
        public void run() {
            //set priority(helps only on few devices)
            Thread.currentThread().setPriority(Thread.MAX_PRIORITY);
            //run C++ code
            Native.loop();
            //request OpenGL thread to run
            requestRender();
        }
    }
}