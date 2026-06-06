#ifndef __SC_GAME_BOSS_H__
#define __SC_GAME_BOSS_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "scr_spacecraft_game.h"

#define SC_METEOROID_MOVE_SPEED     (2)
#define SIZE_BITMAP_BOSS_X          (20)
#define SIZE_BITMAP_BOSS_Y          (20)

typedef struct {
    bool visible;
    int32_t x, y;
    uint8_t action_image;
    int32_t hp;
} sc_game_boss_t;

extern bool boss_enraged;
extern sc_game_boss_t boss;

#endif //__SC_GAME_METEOROID_H__