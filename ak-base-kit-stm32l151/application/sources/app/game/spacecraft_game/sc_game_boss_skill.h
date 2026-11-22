#ifndef __SC_GAME_BOSS_SKILL_H__
#define __SC_GAME_BOSS_SKILL_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "scr_spacecraft_game.h"

#define MAX_NUM_BOSS_SKILL       (3)
#define SIZE_BITMAP_ENERGY_BALL_X (8)
#define SIZE_BITMAP_ENERGY_BALL_Y (8)
#define SC_ENERGY_BALL_SPEED_X   (3)     // px per tick
#define SIZE_BITMAP_LASER_X (120)
#define SIZE_BITMAP_LASER_Y (3)

typedef struct {
    bool visible;
    int32_t x, y;
    uint8_t action_image;
} sc_game_skill_energy_ball_t;

typedef struct {
    bool visible;
    int32_t x, y;
    uint8_t action_image;
} sc_game_skill_laser_t;

extern sc_game_skill_energy_ball_t energy_ball[MAX_NUM_BOSS_SKILL];
extern sc_game_skill_laser_t laser;
extern bool is_laser_shooting;

#endif //__SC_GAME_BOSS_SKILL_H__