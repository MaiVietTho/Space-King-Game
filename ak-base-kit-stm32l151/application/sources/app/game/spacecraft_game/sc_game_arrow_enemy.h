#ifndef __GAME_ARROW_ENEMY_H__
#define __GAME_ARROW_ENEMY_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
// #include "scr_spacecraft_game.h"

#define MIN_AXIS_X_ARROW_2          (8)
#define SIZE_BITMAP_ARROW_ENEMY_X   (10)
#define SIZE_BITMAP_ARROW_ENEMY_Y   (5)

#define SHOOT_COUNT 3

typedef struct {
    bool visible;
    int32_t x, y;
    uint8_t action_image;
} game_arrow_enemy_t;

#endif // __GAME_ARROW_ENEMY_H__