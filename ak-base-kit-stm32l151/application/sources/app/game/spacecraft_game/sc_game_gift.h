#ifndef __SC_GAME_GIFT_H__
#define __SC_GAME_GIFT_H__

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

#define SC_GIFT_MOVE_SPEED_X  (2)

#define SIZE_BITMAP_GIFT_X     (10)
#define SIZE_BITMAP_GIFT_Y     (10)
#define GIFT_MAX               (4)

typedef enum {
    GIFT_HEART,
    GIFT_ARROW_POWER,
    GIFT_NONE,
} gift_type_t;

typedef struct {
    bool visible;
    int32_t x, y;
    gift_type_t type;
    uint8_t object_id;
} sc_game_gift_t;

extern sc_game_gift_t gift[GIFT_MAX];

#endif //__SC_GAME_GIFT_H__