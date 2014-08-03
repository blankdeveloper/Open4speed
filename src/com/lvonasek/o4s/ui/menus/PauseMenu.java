package com.lvonasek.o4s.ui.menus;

import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentManager;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import com.lvonasek.o4s.R;
import com.lvonasek.o4s.Sounds;
import com.lvonasek.o4s.game.GameActivity;
import com.lvonasek.o4s.game.GameLoop;

/**
 * Created by lubos on 31.7.14.
 */
public class PauseMenu extends DialogFragment {

    public PauseMenu() {
        // Empty constructor required for DialogFragment
    }

    public Dialog dialog = null;
    public boolean ready = false;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle bundle) {
        int fs = WindowManager.LayoutParams.FLAG_FULLSCREEN;
        ready = false;
        dialog = getDialog();
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.getWindow().setFlags(fs, fs);
        dialog.setCanceledOnTouchOutside(false);
        dialog.setOnKeyListener(listener);
        Thread thread = new Thread()
        {
            @Override
            public void run() {
                try {
                    sleep(500);
                    ready = true;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        };
        thread.start();
        View view = inflater.inflate(R.layout.menu_pause, container);
        view.findViewById(R.id.menu_pause_resume).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                GameLoop.paused = false;
                if (Sounds.snd != null)
                    Sounds.snd.autoResume();
                dismiss();
            }
        });
        view.findViewById(R.id.menu_pause_options).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FragmentManager fm = GameActivity.instance.getSupportFragmentManager();
                PauseOptions pauseOptions = new PauseOptions();
                pauseOptions.show(fm, "menu_pause_options");
            }
        });
        view.findViewById(R.id.menu_pause_exit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                GameActivity.instance.quit();
                dismiss();
            }
        });
        return view;
    }

    DialogInterface.OnKeyListener listener = new DialogInterface.OnKeyListener() {
        @Override
        public boolean onKey(DialogInterface dialogInterface, int i, KeyEvent event) {
            int keyCode = event.getKeyCode();
            if (ready)
                if ((keyCode == KeyEvent.KEYCODE_BACK) || (keyCode == KeyEvent.KEYCODE_MENU)) {
                    GameLoop.paused = false;
                    if (Sounds.snd != null)
                        Sounds.snd.autoResume();
                }
            return false;
        }
    };
}
