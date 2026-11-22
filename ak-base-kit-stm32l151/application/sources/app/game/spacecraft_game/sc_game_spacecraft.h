#ifndef __SC_GAME_SPACECRAFT_H__
#define __SC_GAME_SPACECRAFT_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "scr_spacecraft_game.h"

#define SPACECRAFT_MOVE_STEP     (5)
#define SIZE_BITMAP_SPACECRAFT_X   (15)
#define SIZE_BITMAP_SPACECRAFT_Y   (15)
#define SPACECRAFT_SHOT_PASS_X     (SIZE_BITMAP_SPACECRAFT_X * 2 / 3)
#define AXIS_X_SPACECRAFT          (0)
#define AXIS_Y_SPACECRAFT          (20)

typedef struct {
    bool visible;
    int32_t x, y;
    uint8_t action_image;
    uint8_t hearts;
    uint8_t arrow_power;
} sc_game_spacecraft_t;

extern sc_game_spacecraft_t spacecraft;
extern bool is_invincible;
extern bool is_hit_showing;

#endif //__SC_GAME_SPACECRAFT_H__
