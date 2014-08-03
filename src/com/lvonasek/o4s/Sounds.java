package com.lvonasek.o4s;

import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;

import com.lvonasek.o4s.game.GameActivity;
import com.lvonasek.o4s.game.GameLoop;

import java.io.IOException;
import java.util.ArrayList;

/**
 * Sound bridge - it enables me to use Android API from native C++ code(Soundpool MediaPlayer).
 * This access to sounds is also better for game loop interruption(eg.on incoming call)
 * NOTE: if this class is renamed/moved it have to be rewritten in C++ code
 * @author Lubos Vonasek
 */
public class Sounds {
    // instance of sounds
    private static ArrayList<Sound> list = new ArrayList<Sound>();
    // instance of sound APIs
    public static MediaPlayer music;
    public static SoundPool snd;

    /**
     * Structure for sound
     */
    static class Sound {

        int id;
        boolean playing;
        float rate;
        float volume;

        Sound(int id, float rate, float volume) {
            this.id = id;
            this.rate = rate;
            this.volume = volume;
            playing = false;
        }
    }

    public static void init() {
        music = new MediaPlayer();
        snd = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
    }

    /**
     * Change frequency of sound(engine, n2o)
     * @param index is index of sound
     * @param speed is play rate(from 0.5 to 2.0)
     */
    public static void soundFreq(int index, float speed) {
        //check index validity(it is not a stream)
        if (list.get(index).id != Integer.MAX_VALUE) {
            //clamp rate speed
            list.get(index).rate = speed;
            if (speed < 0.5) {
                list.get(index).rate = 0.5f;
            }
            if (speed > 2.0) {
                list.get(index).rate = 2.0f;
            }
        }
    }

    /**
     * Load sound into memory
     * @param filename is filename of sound with full path(/ is root of the APK package)
     */
    public static void soundLoad(String filename) {
        //load short audio clip(max 15seconds)
        if (filename.endsWith(".ogg") && !GameLoop.paused) {
            try {
                //unzip file(Android java access)
                AssetFileDescriptor afd = GameActivity.instance.getAssets().openFd(filename);
                //use 1 channel per sample - change of channels amount have to be done in C++ too
                for (int i = 0; i < 1; i++) {
                    //get id
                    int id = snd.load(afd, 1);
                    //add into list of sounds
                    list.add(new Sound(id, 1, 0));
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        //stream long audio clip
        else {
            //reset previous music
            if (music != null) {
                music.reset();
            }
            try {
                //unzip file(Android java access)
                AssetFileDescriptor afd = GameActivity.instance.getAssets().openFd(filename);
                //prepare audio clip
                music = new MediaPlayer();
                music.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
                music.prepare();
                //mark sound as a stream
                list.add(new Sound(Integer.MAX_VALUE, 1, 0));
            } catch (IOException e) {
                music = null;
            }
        }
    }

    /**
     * Play sound
     * @param index is index of sound
     * @param loop true for looping
     */
    public static void soundPlay(int index, int loop) {
        //sound is a stream
        if (index == Integer.MAX_VALUE) {
            //set looping
            music.setLooping(loop == 1);
            //start playing
            music.start();
        }

        //audio clip
        else if (!GameLoop.paused) {
            //play sound(note:looping is solved in C++ code)
            Sound s = list.get(index);
            if (s.playing)
                snd.pause(s.id);
            snd.play(s.id, s.volume, s.volume, 1, loop, s.rate);
            s.playing = true;
        }
    }

    /**
     * Stops playing sound
     * @param index is index of sound
     */
    public static void soundStop(int index) {
        //sound is a stream
        if (index == Integer.MAX_VALUE) {
            music.pause();
        }

        //audio clip
        else if (!GameLoop.paused) {
            snd.stop(list.get(index).id);
            list.get(index).playing = false;
        }
    }

    /**
     * Set sound volume
     * @param index is index of sound
     * @param volume is sound volume(from 0 to 1)
     */
    public static void soundVolume(int index, float volume) {
        //sound is a stream
        if (index == Integer.MAX_VALUE) {
            music.setVolume(volume, volume);
        }

        //audio clip
        else if (!GameLoop.paused) {
            volume *= 0.25f;
            snd.setVolume(list.get(index).id, volume, volume);
            list.get(index).volume = volume;
        }
    }
}
