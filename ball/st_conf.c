/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERBALL is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include "gui.h"
#include "hud.h"
#include "back.h"
#include "geom.h"
#include "item.h"
#include "ball.h"
#include "part.h"
#include "audio.h"
#include "config.h"
#include "video.h"
#include "common.h"

#include "game_common.h"
#include "game_client.h"
#include "game_server.h"

#include "st_conf.h"
#include "st_title.h"
#include "st_resol.h"
#include "st_name.h"
#include "st_ball.h"
#include "st_shared.h"

extern const char TITLE[];
extern const char ICON[];

/*---------------------------------------------------------------------------*/

static void conf_slider(int id, const char *text,
                        int token, int value,
                        int *ids, int num)
{
    int jd, kd, i;

    if ((jd = gui_harray(id)) && (kd = gui_harray(jd)))
    {
        /* A series of empty buttons forms a "slider". */

        for (i = num - 1; i >= 0; i--)
            ids[i] = gui_state(kd, NULL, GUI_SML, token + i, (i == value));

        gui_label(jd, text, GUI_SML, GUI_ALL, 0, 0);
    }
}

static int conf_state(int id, const char *label, const char *text, int token)
{
    int jd, kd, rd = 0;

    if ((jd = gui_harray(id)) && (kd = gui_harray(jd)))
    {
        rd = gui_state(kd, text, GUI_SML, token, 0);
        gui_label(jd, label, GUI_SML, GUI_ALL, 0, 0);
    }

    return rd;
}

static void conf_toggle(int id, const char *label, int value,
                        const char *text1, int token1, int value1,
                        const char *text0, int token0, int value0)
{
    int jd, kd;

    if ((jd = gui_harray(id)) && (kd = gui_harray(jd)))
    {
        gui_state(kd, text0, GUI_SML, token0, (value == value0));
        gui_state(kd, text1, GUI_SML, token1, (value == value1));

        gui_label(jd, label, GUI_SML, GUI_ALL, 0, 0);
    }
}

/*---------------------------------------------------------------------------*/

enum
{
    CONF_FULL = 1,
    CONF_WIN,
    CONF_TEXHI,
    CONF_TEXLO,
    CONF_REFON,
    CONF_REFOF,
    CONF_BACON,
    CONF_BACOF,
    CONF_SHDON,
    CONF_SHDOF,
    CONF_BACK,
    CONF_RES,
    CONF_PLAYER,
    CONF_BALL
};

static int music_id[11];
static int sound_id[11];

static int conf_action(int i)
{
    int w = config_get_d(CONFIG_WIDTH);
    int h = config_get_d(CONFIG_HEIGHT);
    int s = config_get_d(CONFIG_SOUND_VOLUME);
    int m = config_get_d(CONFIG_MUSIC_VOLUME);
    int r = 1;

    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case CONF_FULL:
        goto_state(&st_null);
        r = video_mode(1, w, h);
        goto_state(&st_conf);
        break;

    case CONF_WIN:
        goto_state(&st_null);
        r = video_mode(0, w, h);
        goto_state(&st_conf);
        break;

    case CONF_TEXHI:
        goto_state(&st_null);
        config_set_d(CONFIG_TEXTURES, 1);
        goto_state(&st_conf);
        break;

    case CONF_TEXLO:
        goto_state(&st_null);
        config_set_d(CONFIG_TEXTURES, 2);
        goto_state(&st_conf);
        break;

    case CONF_REFON:
        goto_state(&st_null);
        config_set_d(CONFIG_REFLECTION, 1);
        r = video_init(TITLE, ICON);
        goto_state(&st_conf);
        break;

    case CONF_REFOF:
        goto_state(&st_null);
        config_set_d(CONFIG_REFLECTION, 0);
        goto_state(&st_conf);
        break;

    case CONF_BACON:
        goto_state(&st_null);
        config_set_d(CONFIG_BACKGROUND, 1);
        goto_state(&st_conf);
        break;

    case CONF_BACOF:
        goto_state(&st_null);
        config_set_d(CONFIG_BACKGROUND, 0);
        goto_state(&st_conf);
        break;

    case CONF_SHDON:
        goto_state(&st_null);
        config_set_d(CONFIG_SHADOW, 1);
        goto_state(&st_conf);
        break;

    case CONF_SHDOF:
        goto_state(&st_null);
        config_set_d(CONFIG_SHADOW, 0);
        goto_state(&st_conf);
        break;

    case CONF_BACK:
        goto_state(&st_title);
        break;

    case CONF_RES:
        goto_state(&st_resol);
        break;

    case CONF_PLAYER:
        goto_name(&st_conf, &st_conf, 1);
        break;

    case CONF_BALL:
        goto_state(&st_ball);
        break;

    default:
        if (100 <= i && i <= 110)
        {
            int n = i - 100;

            config_set_d(CONFIG_SOUND_VOLUME, n);
            audio_volume(n, m);
            audio_play(AUD_BUMPM, 1.f);

            gui_toggle(sound_id[n]);
            gui_toggle(sound_id[s]);
        }
        if (200 <= i && i <= 210)
        {
            int n = i - 200;

            config_set_d(CONFIG_MUSIC_VOLUME, n);
            audio_volume(s, n);
            audio_play(AUD_BUMPM, 1.f);

            gui_toggle(music_id[n]);
            gui_toggle(music_id[m]);
        }
    }

    return r;
}

static int conf_gui(void)
{
    int id, jd;

    /* Initialize the configuration GUI. */

    if ((id = gui_vstack(0)))
    {
        int f = config_get_d(CONFIG_FULLSCREEN);
        int t = config_get_d(CONFIG_TEXTURES);
        int r = config_get_d(CONFIG_REFLECTION);
        int b = config_get_d(CONFIG_BACKGROUND);
        int h = config_get_d(CONFIG_SHADOW);
        int s = config_get_d(CONFIG_SOUND_VOLUME);
        int m = config_get_d(CONFIG_MUSIC_VOLUME);

        const char *player = config_get_s(CONFIG_PLAYER);
        const char *ball   = config_get_s(CONFIG_BALL_FILE);

        char resolution[20];

        int name_id = 0, ball_id = 0;

        sprintf(resolution, "%d x %d",
                config_get_d(CONFIG_WIDTH),
                config_get_d(CONFIG_HEIGHT));

        if ((jd = gui_harray(id)))
        {
            gui_label(jd, _("Options"), GUI_SML, GUI_ALL, 0, 0);
            gui_space(jd);
            gui_start(jd, _("Back"),    GUI_SML, CONF_BACK, 0);
        }

        gui_space(id);

        conf_toggle(id, _("Fullscreen"), f,
                    _("Yes"), CONF_FULL, 1,
                    _("No"), CONF_WIN, 0);

        conf_state(id, _("Resolution"), resolution, CONF_RES);

        gui_space(id);

        conf_toggle(id, _("Textures"), t,
                    _("High"), CONF_TEXHI, 1,
                    _("Low"), CONF_TEXLO, 2);

        conf_toggle(id, _("Reflection"), r,
                    _("On"), CONF_REFON, 1,
                    _("Off"), CONF_REFOF, 0);

        conf_toggle(id, _("Background"), b,
                    _("On"), CONF_BACON, 1,
                    _("Off"), CONF_BACOF, 0);

        conf_toggle(id, _("Shadow"), h,
                    _("On"), CONF_SHDON, 1,
                    _("Off"), CONF_SHDOF, 0);

        gui_space(id);

        conf_slider(id, _("Sound Volume"), 100, s,
                    sound_id, ARRAYSIZE(sound_id));
        conf_slider(id, _("Music Volume"), 200, m,
                    music_id, ARRAYSIZE(music_id));

        gui_space(id);

        name_id = conf_state(id, _("Player Name"), " ", CONF_PLAYER);
        ball_id = conf_state(id, _("Ball Model"), " ", CONF_BALL);

        gui_layout(id, 0, 0);

        gui_set_trunc(name_id, TRUNC_TAIL);
        gui_set_trunc(ball_id, TRUNC_TAIL);

        gui_set_label(name_id, player);
        gui_set_label(ball_id, base_name(ball));
    }

    return id;
}

static int conf_enter(struct state *st, struct state *prev)
{
    game_client_free();
    back_init("back/gui.png");
    audio_music_fade_to(0.5f, "bgm/inter.ogg");

    return conf_gui();
}

static void conf_leave(struct state *st, struct state *next, int id)
{
    back_free();
    gui_delete(id);
}

static void conf_paint(int id, float t)
{
    video_push_persp((float) config_get_d(CONFIG_VIEW_FOV), 0.1f, FAR_DIST);
    {
        back_draw(0);
    }
    video_pop_matrix();
    gui_paint(id);
}

static int conf_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return conf_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return conf_action(CONF_BACK);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int null_enter(struct state *st, struct state *prev)
{
    hud_free();
    gui_free();
    swch_free();
    jump_free();
    goal_free();
    item_free();
    ball_free();
    shad_free();
    part_free();

    return 0;
}

static void null_leave(struct state *st, struct state *next, int id)
{
    part_init(GOAL_HEIGHT, JUMP_HEIGHT);
    shad_init();
    ball_init();
    item_init();
    goal_init();
    jump_init();
    swch_init();
    gui_init();
    hud_init();
}

/*---------------------------------------------------------------------------*/

struct state st_conf = {
    conf_enter,
    conf_leave,
    conf_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    shared_click,
    NULL,
    conf_buttn
};

struct state st_null = {
    null_enter,
    null_leave,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
