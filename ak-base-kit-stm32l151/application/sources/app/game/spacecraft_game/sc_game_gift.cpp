#include "sc_game_gift.h"

static uint8_t total_gift_spawned = 0;
sc_game_gift_t gift[GIFT_MAX];

static void scGameGiftSetup() {
    total_gift_spawned = 0;
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        enemy[i].has_gift = false;
        meteoroid[i].has_gift = false;
    }
    for (uint8_t i = 0; i < GIFT_MAX; i++) {
        gift[i].x = 0;
        gift[i].y = 0;
        gift[i].visible = BLACK;
        gift[i].type = GIFT_NONE;
    }
}

static void scGameGiftRandomForEnemy() {
    if (total_gift_spawned >= GIFT_MAX) {
        return;
    }

    uint8_t count_retry = 0;
    uint8_t id;
    do {
        id = rand() % TOTAL_ENEMY;
        count_retry++;
    } while ((enemy[id].hearts <= 0 || enemy[id].has_gift == true) && count_retry < 50);

    if (enemy[id].hearts <= 0 || enemy[id].has_gift == true) {
        return;
    }

    for (uint8_t i = 0; i < GIFT_MAX; i++) {
        if (gift[i].visible == BLACK && gift[i].type == GIFT_NONE) {
            gift[i].object_id = id;
            gift[i].type =  (gift_type_t)(rand() % GIFT_NONE);
            enemy[id].has_gift = true;
            total_gift_spawned++;
            break;
        }
    } 
}

static void scGameGiftRandomForMeteoroid() {
    if (total_gift_spawned >= GIFT_MAX) {
        return;
    }

    uint8_t count_retry = 0;
    uint8_t id;
    do {
        id = rand() % NUM_METEOROIDS;
        count_retry++;
    } while (meteoroid[id].has_gift  && count_retry < 50);

    if (meteoroid[id].has_gift) {
        return;
    }

    for (uint8_t i = 0; i < GIFT_MAX; i++) {
        if (gift[i].visible == BLACK && gift[i].type == GIFT_NONE) {
            gift[i].object_id = id;
            gift[i].type =  (gift_type_t)(rand() % GIFT_NONE);
            meteoroid[id].has_gift = true;
            total_gift_spawned++;
            break;
        }
    }
}

static void scGameGiftRun() {
    for (uint8_t i = 0; i < GIFT_MAX; i++) {
        if (gift[i].visible == WHITE) {
            gift[i].x -= SC_GIFT_MOVE_SPEED_X;
            if (gift[i].x <= 0) {
                gift[i].visible = BLACK;
                gift[i].type= GIFT_NONE;
            }
        }
    }
}

static void scGameGiftCheckReceive() {
    for (uint8_t j = 0; j < GIFT_MAX; j++) {
        if (gift[j].visible == WHITE && gift[j].x < (spacecraft.x + SIZE_BITMAP_SPACECRAFT_X - 5) && 
            spacecraft.y < (gift[j].y + SIZE_BITMAP_GIFT_Y) && (spacecraft.y + SIZE_BITMAP_SPACECRAFT_Y) > gift[j].y) { 
            if (gift[j].type == GIFT_HEART) {
                save_data.spacecraft_hearts++;
                spacecraft.hearts = save_data.spacecraft_hearts;
            }
            else if (gift[j].type == GIFT_ARROW_POWER) {
                save_data.spacecraft_arrow_power++;
                spacecraft.arrow_power = save_data.spacecraft_arrow_power;
            }
            gift[j].visible = BLACK;
            gift[j].type= GIFT_NONE;          
        }
    }
}

static void scGameGiftOfBoss() {
    for (uint8_t i = 0; i < GIFT_MAX; i++) {
        if (gift[i].visible == BLACK) {
            gift[i].type = (gift_type_t)(rand() % GIFT_NONE);
            gift[i].x = boss.x - 5;
            gift[i].y = boss.y + 8; 
            gift[i].visible = WHITE;
            break;
        }
    }
}

void sc_game_gift_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_GIFT_SETUP: {
        APP_DBG_SIG("SC_GAME_GIFT_SETUP\n");
        scGameGiftSetup();
    } break;

    case SC_GAME_GIFT_RANDOM_FOR_ENEMY: {
        APP_DBG_SIG("SC_GAME_GIFT_RANDOM_FOR_ENEMY\n");
        if (total_gift_spawned < GIFT_MAX) {
            scGameGiftRandomForEnemy();
            timer_set(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_ENEMY, SC_GAME_ENEMY_GIFT_SPAWN_INTERVAL, TIMER_ONE_SHOT);
        }
    } break;

    case SC_GAME_GIFT_RANDOM_FOR_METEOROID: {
        APP_DBG_SIG("SC_GAME_GIFT_RANDOM_FOR_METEOROID");
        if (total_gift_spawned < GIFT_MAX) {
            scGameGiftRandomForMeteoroid();
            timer_set(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_METEOROID, SC_GAME_METEOROID_GIFT_SPAWN_INTERVAL, TIMER_ONE_SHOT);
        }
    } break;

    case SC_GAME_GIFT_RUN: {
        APP_DBG_SIG("SC_GAME_GIFT_RUN\n");
        scGameGiftRun();
    } break;

    case SC_GAME_GIFT_CHECK_RECEIVE: {
        APP_DBG_SIG("SC_GAME_GIFT_CHECK_RECEIVE\n");
        scGameGiftCheckReceive();
    } break;

    case SC_GAME_GIFT_RESET: {
        APP_DBG_SIG("SC_GAME_GIFT_RESET\n");
        scGameGiftSetup();
    } break;

    case SC_GAME_GIFT_OF_BOSS: {
        APP_DBG_SIG("SC_GAME_GIFT_OF_BOSS\n");
        scGameGiftOfBoss();
    } break;
    
    default:
        break;
    }
}