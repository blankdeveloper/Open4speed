package com.lvonasek.o4s.ui.menus;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.game.GameActivity;
import com.lvonasek.o4s.game.GameLoop;
import com.lvonasek.o4s.media.Settings;
import com.lvonasek.o4s.media.Sound;
import com.lvonasek.o4s.ui.common.SeekBar;

/**
 * Implementation of pause menu dialog with all actions
 * @author Lubos Vonasek
 */
public class PauseMenu extends DialogFragment {

    public PauseMenu() {
        // Empty constructor required for DialogFragment
    }

    public int currentDialog = 0;
    public Dialog dialog = null;
    public RelativeLayout mainDialog;
    public RelativeLayout optionsDialog;
    public boolean ready = false;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle bundle) {
        int fs = WindowManager.LayoutParams.FLAG_FULLSCREEN;
        final Context c = getActivity();
        ready = false;
        dialog = getDialog();
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.getWindow().setFlags(fs, fs);
        dialog.setCancelable(false);
        dialog.setCanceledOnTouchOutside(false);
        dialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface dialogInterface, int keyCode, KeyEvent event) {
                if (event.getAction() == KeyEvent.ACTION_DOWN)
                    if (keyCode == KeyEvent.KEYCODE_BACK) {
                        if (currentDialog != 0)
                            openDialog(0);
                        else {
                            if (ready) {
                                GameLoop.paused--;
                                if (Sound.snd != null)
                                    Sound.snd.autoResume();
                            }
                            dismiss();
                        }
                    }
                return true;
            }
        });
        new Thread() {
            @Override
            public void run() {
                try {
                    sleep(500);
                    ready = true;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }.start();
        View view = inflater.inflate(R.layout.menu_pause, container);
        mainDialog = (RelativeLayout) view.findViewById(R.id.pause_main);
        optionsDialog = (RelativeLayout) view.findViewById(R.id.pause_options);
        optionsDialog.setVisibility(View.GONE);

        //main pause menu
        view.findViewById(R.id.menu_pause_resume).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainMenu.playButtonSound();
                GameLoop.paused--;
                if (Sound.snd != null)
                    Sound.snd.autoResume();
                dismiss();
            }
        });
        view.findViewById(R.id.menu_pause_options).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainMenu.playButtonSound();
                openDialog(1);
            }
        });
        view.findViewById(R.id.menu_pause_restart).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainMenu.playButtonSound();
                GameActivity.instance.overridePendingTransition(0, 0);
                startActivity(new Intent(GameActivity.instance.getApplicationContext(), GameActivity.class));
                GameActivity.instance.quit();
                dismiss();
            }
        });
        view.findViewById(R.id.menu_pause_exit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainMenu.playButtonSound();
                GameActivity.instance.quit();
                dismiss();
            }
        });

        //options menu
        view.findViewById(R.id.menu_pause_options_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainMenu.playButtonSound();
                openDialog(0);
            }
        });
        ((SeekBar)view.findViewById(R.id.options_music)).setProgress(Settings.getConfig(c, Settings.MUSIC_VOLUME));
        ((SeekBar)view.findViewById(R.id.options_music)).setOnSeekBarChangeListener(new android.widget.SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(android.widget.SeekBar seekBar, int value, boolean b) {
                if (b) {
                    Settings.setConfig(c, Settings.MUSIC_VOLUME, value);
                    float volume = value * 0.01f;
                    GameActivity.music.setVolume(volume, volume);
                }
            }

            @Override
            public void onStartTrackingTouch(android.widget.SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(android.widget.SeekBar seekBar) {}
        });
        ((SeekBar)view.findViewById(R.id.options_sound)).setProgress(Settings.getConfig(c, Settings.SOUND_VOLUME));
        ((SeekBar)view.findViewById(R.id.options_sound)).setOnSeekBarChangeListener(new android.widget.SeekBar.OnSeekBarChangeListener() {
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
        return view;
    }

    public void openDialog(int value) {
        if (value == 0) //main
        {
            mainDialog.setVisibility(View.VISIBLE);
            optionsDialog.setVisibility(View.GONE);
        } else if (value == 1) //options
        {
            mainDialog.setVisibility(View.GONE);
            optionsDialog.setVisibility(View.VISIBLE);
        }
        currentDialog = value;
    }

    @Override
    public void onPause() {
        dismiss();
        GameLoop.paused--;
        super.onPause();
    }
}
