#ifndef __SCR_GAME_CHANGE_ROUND_H__
#define __SCR_GAME_CHANGE_ROUND_H__

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

#include "buzzer.h"

#include "screens.h"
#include "screens_bitmap.h"

typedef enum {
    SC_GAME_ROUND_1 = 1,
    SC_GAME_ROUND_2,
    SC_GAME_ROUND_3,    
    SC_GAME_ROUND_4,
    SC_GAME_ROUND_5,
} sc_game_round_t;

extern view_dynamic_t dyn_view_item_game_change_round;
extern view_screen_t scr_game_change_round;
extern void scr_game_change_round_handle(ak_msg_t* msg);

#endif //__SCR_GAME_CHANGE_ROUND_H__

