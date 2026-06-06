#ifndef __SC_GAME_ENEMY_H__
#define __SC_GAME_ENEMY_H__

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
#include "sc_game_arrow_enemy.h"
#include "scr_spacecraft_game.h"

#define TOTAL_ENEMY                   (10)
#define MAX_ARROW_ENEMY               (3)
#define SIZE_BITMAP_ENEMIES_X         (16)
#define SIZE_BITMAP_ENEMIES_Y         (11)
#define POSITION_ENEMY_SETUP_X        (141)

#define AXIS_Y_ENEMY_COLUM_0          (2)
#define AXIS_Y_ENEMY_COLUM_1          (12)
#define AXIS_Y_ENEMY_COLUM_2          (22)
#define AXIS_Y_ENEMY_COLUM_3          (32)
#define AXIS_Y_ENEMY_COLUM_4          (42)

typedef struct {
    bool visible;
    int32_t x, y;
    uint8_t action_image;
    game_arrow_enemy_t arrow[MAX_ARROW_ENEMY];
    int8_t hp;
    int8_t hearts;
    bool has_gift;
} sc_game_enemy_t;

extern sc_game_enemy_t enemy[TOTAL_ENEMY ];

#endif //__SC_GAME_ENEMY_H__