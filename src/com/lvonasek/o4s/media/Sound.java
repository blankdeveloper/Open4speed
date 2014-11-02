package com.lvonasek.o4s.media;


import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.SoundPool;

import com.lvonasek.o4s.game.GameActivity;
import com.lvonasek.o4s.game.GameLoop;

import java.io.IOException;

/**
 * Sound FX player and holder
 * @author Lubos Vonasek
 */
public class Sound {

    public static SoundPool snd = new SoundPool(16, AudioManager.STREAM_MUSIC, 0);

    public static float globalVolume = 1;

    int id;
    boolean loop;
    boolean playing;
    float rate;
    float volume;

    public Sound(String filename, boolean loop) {
        //unzip file(Android java access)
        try {
            AssetFileDescriptor afd = GameActivity.instance.getAssets().openFd(filename);
            id = snd.load(afd, 1);
        } catch (IOException e) {
            e.printStackTrace();
            this.id = -1;
        }
        this.loop = loop;
        this.rate = 1;
        this.volume = 0;
        playing = false;
    }

    public void play() {
        if (GameLoop.paused == 0) {
            //play sound
            if (playing)
                snd.pause(id);
            snd.play(id, volume, volume, 1, loop ? -1 : 1, rate);
            playing = true;
        }
    }

    /**
     * Change frequency of sound(engine, n2o)
     * @param speed is play rate(from 0.5 to 2.0)
     */
    public void setFreq(float speed) {
        //clamp rate speed
        rate = speed;
        if (speed < 0.5) {
            rate = 0.5f;
        }
        if (speed > 2.0) {
            rate = 2.0f;
        }
    }

    /**
     * Set sound volume
     * @param volume is sound volume(from 0 to 1)
     */
    public void setVolume(float volume) {
        //audio clip
        if (GameLoop.paused == 0) {
            volume *= 0.25f;
            this.volume = volume * globalVolume;
        }
    }

    /**
     * Stops playing sound
     */
    public void stop() {
        //audio clip
        if ((GameLoop.paused == 0) && playing) {
            snd.stop(id);
            playing = false;
        }
    }
}