#ifndef __AR_GAME_STARFIELD_H__
#define __AR_GAME_STARFIELD_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#define SIZE_BITMAP_STARFIELD_X           (128)
#define SIZE_BITMAP_STARFIELD_Y           (54)

typedef struct {
    bool visible;
    uint32_t x, y;
    uint8_t action_image;
} ar_game_starfield_t;

extern ar_game_starfield_t starfield;

#endif //__AR_GAME_STARFIELD_H__