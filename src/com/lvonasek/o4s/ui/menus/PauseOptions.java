package com.lvonasek.o4s.ui.menus;

import android.app.Dialog;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import com.lvonasek.o4s.R;

/**
 * Created by lubos on 31.7.14.
 */
public class PauseOptions extends DialogFragment {

    public PauseOptions() {
        // Empty constructor required for DialogFragment
    }

    public Dialog dialog = null;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle bundle) {
        int fs = WindowManager.LayoutParams.FLAG_FULLSCREEN;
        dialog = getDialog();
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.getWindow().setFlags(fs, fs);
        dialog.setCanceledOnTouchOutside(false);
        View view = inflater.inflate(R.layout.menu_pause_options, container);

        view.findViewById(R.id.menu_pause_options_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
              dismiss();
            }
        });
        return view;
    }
}
