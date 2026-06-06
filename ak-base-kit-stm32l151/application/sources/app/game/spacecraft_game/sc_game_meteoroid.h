#ifndef __SC_GAME_METEOROID_H__
#define __SC_GAME_METEOROID_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "app_eeprom.h"
#include "buzzer.h"
#include "scr_spacecraft_game.h"

#define NUM_METEOROIDS              (10)
#define SC_METEOROID_MOVE_SPEED     (2)
#define SIZE_BITMAP_METEOROIDS_X    (20)
#define SIZE_BITMAP_METEOROIDS_Y    (20)

#define AXIS_x_METEOROID_0          (-45)
#define AXIS_x_METEOROID_1          (-10)
#define AXIS_x_METEOROID_2          (25)
#define AXIS_x_METEOROID_3          (60)
#define AXIS_x_METEOROID_4          (95)

typedef struct {
    bool visible;
    int32_t x, y;
    uint8_t action_image;
    int8_t hp;
    bool has_gift;
} sc_game_meteoroid_t;

extern uint16_t remaining_meteoroids;
extern sc_game_meteoroid_t meteoroid[NUM_METEOROIDS];

#endif //__SC_GAME_METEOROID_H__