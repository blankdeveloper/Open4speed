package com.lvonasek.o4s.game;

import com.lvonasek.o4s.R;

/**
 * Configuration of race events. It holds track info for native code, music path, splash drawable
 * and race event type.
 * @author Lubos Vonasek
 */
public class RaceInfo {

    private static final String MUSIC_MENU          = "sfx/01-danosongs.com-dublin-forever-instr.mp3";
    private static final String MUSIC_ENERGY1       = "sfx/02-danosongs.com-megacosm.mp3";
    private static final String MUSIC_ENERGY2       = "sfx/03-danosongs.com-gem-droids.mp3";
    private static final String MUSIC_ENERGY3       = "sfx/04-danosongs.com-crazyfromthemessage.mp3";
    private static final String MUSIC_AMBIENT1      = "sfx/05-danosongs.com-inkarnation.mp3";
    private static final String MUSIC_AMBIENT2      = "sfx/06-danosongs.com-undiscovered-oceans.mp3";
    private static final String MUSIC_AMBIENT3      = "sfx/07-danosongs.com-cellular-faith.mp3";

    private static final String RACE_WINTER_DAY     = "tracks/winter-day";
    private static final String RACE_WINTER_FOG     = "tracks/winter-fog";
    private static final String RACE_WINTER_NIGHT   = "tracks/winter-night";

    private static final int    TYPE_CLASSIC        = 0;

    public static final RaceInfo[] EVENT = {
            new RaceInfo(MUSIC_ENERGY1, RACE_WINTER_DAY, R.drawable.winter_day, TYPE_CLASSIC),
            new RaceInfo(MUSIC_AMBIENT1, RACE_WINTER_NIGHT, R.drawable.winter_night, TYPE_CLASSIC),
            new RaceInfo(MUSIC_ENERGY2, RACE_WINTER_FOG, R.drawable.winter_fog, TYPE_CLASSIC),
    };

    public String music;
    public String race;
    public int    splash;
    public int    type;

    public RaceInfo(String music, String race, int splash, int type) {
        this.music = music;
        this.race = race;
        this.splash = splash;
        this.type = type;
    }
}
