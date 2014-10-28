package com.lvonasek.o4s.ui.menus;

import android.app.Dialog;
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
import com.lvonasek.o4s.media.Sound;

/**
 * Created by lubos on 31.7.14.
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
