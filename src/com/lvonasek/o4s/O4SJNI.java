package com.lvonasek.o4s;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.view.MotionEvent;
import com.lvonasek.o4s.controllers.VirtualKeys;
import java.util.Timer;
import java.util.TimerTask;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Communication with native C++ code from java. It is also managing threads for graphical
 * and non-graphical processes. Main function of this class is calling C++ methods.
 * @author Lubos Vonasek
 */
public class O4SJNI extends GLSurfaceView implements Renderer {

    //Thread objects
    public Timer timer;
    public TimerTask game;
    //Thread timing
    private final long DEFAULT_SCHEDULE = 1000 / 17;
    private long schedule = DEFAULT_SCHEDULE;
    private long lastSchedule = DEFAULT_SCHEDULE;
    //Game state
    boolean init = false;
    boolean paused = false;

    /**
     * Constructor which creates support for all incoming events
     * @param context
     */
    public O4SJNI(Context context) {
        super(context);
        //enable OpenGL ES 2.0 support
        setEGLContextClientVersion(2);
    }

    @Override
    /**
     * Method automatically called on any touch event
     */
    public boolean onTouchEvent(final MotionEvent e) {
        super.onTouchEvent(e);

        // click to GUI objects(work as same as mouse)
        if (e.getAction() == MotionEvent.ACTION_DOWN) {
            O4SJNI.nativeClick((int) e.getX(), (int) e.getY());
        }

        //click on virtual buttons
        VirtualKeys.click(e, getWidth(), getHeight());

        return true;
    }

    /**
     * Second init method - this is called when everything is ready
     * @param gl OpenGL object(unused because it is for java OpenGL interpretation)
     * @param config another OpenGL object(unused because it is for java OpenGL interpretation)
     */
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        //check if it is ran first time
        if (!init) {
            //get package assets object
            String apkFilePath = null;
            ApplicationInfo appInfo = null;
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
            nativeInit(apkFilePath);
        }
        init = true;
    }

    /**
     * Screen resized(or rotated)
     * Game is forced into landscape mode so this method may not be called
     * @param gl is java OpenGL instance
     * @param w is width in pixels
     * @param h is height in pixels
     */
    public void onSurfaceChanged(GL10 gl, int w, int h) {
        //send new screen dimensions into C++ code
        nativeResize(w, h);
    }

    /**
     * OpenGL thread
     * @param gl is java OpenGL instance
     */
    public void onDrawFrame(GL10 gl) {
        //check if game is not paused
        if (!paused) {

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
            nativeDisplay();

            //reschedule non-graphical thread(it is needed to end it and start again)
            if (timer != null) {
                if (Math.abs(Math.max(lastSchedule, DEFAULT_SCHEDULE) - schedule) > 2) {
                    schedule = Math.max(lastSchedule, DEFAULT_SCHEDULE);
                    timer.cancel();
                    game = new Game();
                    timer = new Timer();
                    timer.schedule(game, 1, schedule);
                }
            }
        }
    }

    /**
     * Thread for non-graphical processes
     */
    class Game extends TimerTask {
        @Override
        public void run() {
            //start time
            long time = System.currentTimeMillis();
            //set priority(helps only on few devices)
            Thread.currentThread().setPriority(Thread.MAX_PRIORITY);
            //run C++ code
            nativeLoop();
            //request OpenGL thread to run
            requestRender();
            //get execution time
            lastSchedule = (System.currentTimeMillis() - time);
        }
    }

    //include native C++ library
    static {
        System.loadLibrary("open4speed");
    }

    //C++ methods
    public static native void nativeBack();
    private static native void nativeClick(int x, int y);
    private static native void nativeDisplay();
    private static native void nativeInit(String str);
    public static native void nativeKey(int code);
    public static native void nativeKeyUp(int code);
    private static native void nativeLoop();
    private static native void nativeResize(int w, int h);
}