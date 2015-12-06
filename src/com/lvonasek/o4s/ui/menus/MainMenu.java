package com.lvonasek.o4s.ui.menus;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.VideoView;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.media.Settings;
import com.lvonasek.o4s.media.Sound;
import com.lvonasek.o4s.ui.common.RaceButton;
import com.lvonasek.o4s.ui.common.SeekBar;

import java.io.IOException;

/**
 * Implementation of main menu activity with all actions, background player, etc.
 * @author Lubos Vonasek
 */
public class MainMenu extends Activity {

    public static  int          buttonClick = 0;
    private static MediaPlayer  music       = new MediaPlayer();
    private        int          currentMenu = 0;
    private        LinearLayout aboutMenu   = null;
    private        LinearLayout mainMenu    = null;
    private        LinearLayout optionsMenu = null;
    private        LinearLayout startMenu   = null;
    private        VideoView    view        = null;

    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(null);
        try {
            setContentView(R.layout.menu_main);
        } catch(Exception e) {
            startActivity(new Intent(this, MainMenu.class));
            System.exit(0);
        }
        view = (VideoView) findViewById(R.id.menu_background);
        Settings.init(this);
        final Context c = this;
        aboutMenu = (LinearLayout) findViewById(R.id.menu_about);
        mainMenu = (LinearLayout) findViewById(R.id.menu_main);
        optionsMenu = (LinearLayout) findViewById(R.id.menu_options);
        startMenu = (LinearLayout) findViewById(R.id.menu_start);
        Sound.globalVolume = Settings.getConfig(c, Settings.SOUND_VOLUME) * 0.01f;
        try {
            buttonClick = Sound.snd.load(getAssets().openFd("sfx/button.wav"), 1);

        } catch (IOException e) {
            e.printStackTrace();
        }

        //main menu
        findViewById(R.id.menu_main_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                playButtonSound();
                openMenu(1);
            }
        });
        findViewById(R.id.menu_main_options).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                playButtonSound();
                openMenu(2);
            }
        });
        findViewById(R.id.menu_main_about).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                playButtonSound();
                openMenu(3);
            }
        });
        findViewById(R.id.menu_main_exit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                playButtonSound();
                System.exit(0);
            }
        });

        //start menu
        ((RaceButton)findViewById(R.id.menu_start_race0)).setRace(c, 0);
        ((RaceButton)findViewById(R.id.menu_start_race1)).setRace(c, 1);

        //options menu
        ((SeekBar)findViewById(R.id.options_music)).setOnSeekBarChangeListener(new android.widget.SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(android.widget.SeekBar seekBar, int value, boolean b) {
                if (b) {
                    Settings.setConfig(c, Settings.MUSIC_VOLUME, value);
                    float volume = value * 0.01f;
                    music.setVolume(volume, volume);
                }
            }

            @Override
            public void onStartTrackingTouch(android.widget.SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(android.widget.SeekBar seekBar) {}
        });
        ((SeekBar)findViewById(R.id.options_sound)).setOnSeekBarChangeListener(new android.widget.SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(android.widget.SeekBar seekBar, int value, boolean b) {
                if (b) {
                    Settings.setConfig(c, Settings.SOUND_VOLUME, value);
                    Sound.globalVolume = value * 0.01f;
                }
            }

            @Override
            public void onStartTrackingTouch(android.widget.SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(android.widget.SeekBar seekBar) {}
        });
        ((SeekBar)findViewById(R.id.options_visual)).setOnSeekBarChangeListener(new android.widget.SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(android.widget.SeekBar seekBar, int value, boolean b) {
                if (b)
                  Settings.setConfig(c, Settings.VISUAL_QUALITY, value);
            }

            @Override
            public void onStartTrackingTouch(android.widget.SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(android.widget.SeekBar seekBar) {}
        });
    }

    public void openMenu(int value) {
        if (value == 0) //main
        {
            mainMenu.setVisibility(View.VISIBLE);
            startMenu.setVisibility(View.GONE);
            optionsMenu.setVisibility(View.GONE);
            aboutMenu.setVisibility(View.GONE);
        } else if (value == 1) //start
        {
            mainMenu.setVisibility(View.GONE);
            startMenu.setVisibility(View.VISIBLE);
            optionsMenu.setVisibility(View.GONE);
            aboutMenu.setVisibility(View.GONE);
        } else if (value == 2) //options
        {
            ((SeekBar)findViewById(R.id.options_music)).setProgress(Settings.getConfig(this, Settings.MUSIC_VOLUME));
            ((SeekBar)findViewById(R.id.options_sound)).setProgress(Settings.getConfig(this, Settings.SOUND_VOLUME));
            ((SeekBar)findViewById(R.id.options_visual)).setProgress(Settings.getConfig(this, Settings.VISUAL_QUALITY));
            mainMenu.setVisibility(View.GONE);
            startMenu.setVisibility(View.GONE);
            optionsMenu.setVisibility(View.VISIBLE);
            aboutMenu.setVisibility(View.GONE);
        } else if (value == 3) //about
        {
            mainMenu.setVisibility(View.GONE);
            startMenu.setVisibility(View.GONE);
            optionsMenu.setVisibility(View.GONE);
            aboutMenu.setVisibility(View.VISIBLE);
        }
        currentMenu = value;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            if (currentMenu != 0) {
                openMenu(0);
                return true;
            }
            else
                System.exit(0);
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onPause() {
        view.pause();
        music.pause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        music.start();
    }

    @Override
    protected void onStart() {
        super.onStart();
        final Context c = this;
        final String path = "android.resource://" + getPackageName() + "/" + R.raw.menu_background;
        view.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                mp.setAudioStreamType(AudioManager.STREAM_MUSIC);
                //mp.setLooping(true);
                mp.start();
            }
        });
        view.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            public void onCompletion(MediaPlayer mp) {
                mp.reset();
                view.setVideoURI(Uri.parse(path));
            }
        });
        view.setVideoURI(Uri.parse(path));
        try {
            AssetFileDescriptor afd = getAssets().openFd("sfx/01-danosongs.com-dublin-forever-instr.mp3");
            music.setAudioStreamType(AudioManager.STREAM_MUSIC);
            music.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
            music.prepare();
            music.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                @Override
                public void onPrepared(MediaPlayer mp) {
                    float volume = Settings.getConfig(c, Settings.MUSIC_VOLUME) * 0.01f;
                    mp.setVolume(volume, volume);
                    mp.setLooping(true);
                    mp.start();
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void playButtonSound() {
        float volume = Sound.globalVolume * 0.5f;
        Sound.snd.play(buttonClick, volume, volume, 1, 0, 1);
    }
}
