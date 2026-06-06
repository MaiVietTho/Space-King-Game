#ifndef __SCR_SPACECRAFT_GAME_H__
#define __SCR_SPACECRAFT_GAME_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "eeprom.h"
#include "app_eeprom.h"

#include "buzzer.h"

#include "screens.h"
#include "screens_bitmap.h"

#include "sc_game_spacecraft.h"
#include "sc_game_arrow.h"
#include "sc_game_bang.h"
#include "sc_game_starfield.h"
#include "sc_game_meteoroid.h"
#include "sc_game_enemy.h"
#include "sc_game_arrow_enemy.h"
#include "sc_game_gift.h"
#include "sc_game_boss.h"
#include "sc_game_boss_skill.h"

typedef enum {
    GAME_OFF,
    GAME_ROUND_COMPLETED,
    GAME_PLAY,
    GAME_OVER,
    GAME_FINISHED,
    GAME_RESET,
} sc_game_state_t;

extern sc_game_state_t sc_game_state;
extern ar_game_setting_t settingsetup;
extern sc_game_save_data_t save_data;

extern view_dynamic_t dyn_view_item_spacecraft_game;
extern view_screen_t scr_spacecraft_game;
extern void scr_spacecraft_game_handle(ak_msg_t* msg);

#endif //__SCR_SPACECRAFT_GAME_H__
