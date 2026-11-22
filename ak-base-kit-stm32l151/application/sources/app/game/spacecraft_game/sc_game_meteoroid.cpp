#include "sc_game_meteoroid.h"

uint16_t remaining_meteoroids;
sc_game_meteoroid_t meteoroid[NUM_METEOROIDS];

static const int8_t meteoroid_x[NUM_METEOROIDS] = {
    AXIS_x_METEOROID_0,
    AXIS_x_METEOROID_1,
    AXIS_x_METEOROID_2,
    AXIS_x_METEOROID_3,
    AXIS_x_METEOROID_4,
};

static void scGameMeteoroidSetup() {
    for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
        meteoroid[i].x = AXIS_x_METEOROID_0;
        meteoroid[i].y = -20;
        meteoroid[i].hp = save_data.current_round * 2;
        meteoroid[i].has_gift = false;
        meteoroid[i].visible = BLACK;
    }
    remaining_meteoroids = settingsetup.level * 20;
}

static void scGameMeteoroidSpawn(uint8_t id, uint8_t pos) {
    if (meteoroid[id].visible == BLACK) {
        meteoroid[id].x = meteoroid_x[pos];
        meteoroid[id].y  = -20;
        meteoroid[id].hp = save_data.current_round * 2;
        meteoroid[id].visible = WHITE;
    }
}

static void scGameMeteoroidRandom() {
    scGameMeteoroidSpawn(rand() % NUM_METEOROIDS, rand() % 5);
}

static void scGameMeteoroidMove() {
    for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
        if (meteoroid[i].visible == WHITE) {
            meteoroid[i].x += SC_METEOROID_MOVE_SPEED;
            meteoroid[i].y += SC_METEOROID_MOVE_SPEED;
            if (meteoroid[i].y >= 64) {
                meteoroid[i].visible = BLACK;
                meteoroid[i].hp = save_data.current_round * 2;
                if (remaining_meteoroids > 0) {
                    remaining_meteoroids--;
                }
            }
        }
    } 
}

static void sc_gameMeteoroidDetonation() {
    for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
        if (meteoroid[i].visible != WHITE) continue;
        // arrows now fly to the screen edge: do not hit a meteoroid that has drifted off screen
        if (meteoroid[i].x >= MAX_AXIS_X_ARROW) continue;

        for (uint8_t j = 0; j < MAX_NUM_ARROW; j++) {
            if (arrow[j].visible != WHITE) continue;

            if ((meteoroid[i].x < (arrow[j].x + SIZE_BITMAP_ARROW_X - 3)) && (meteoroid[i].x + SIZE_BITMAP_METEOROIDS_X - 3) > arrow[j].x &&
                (meteoroid[i].y <= arrow[j].y + SIZE_BITMAP_ARROW_Y -3) && ((meteoroid[i].y + SIZE_BITMAP_METEOROIDS_Y) >= arrow[j].y + 3)) {
                
                meteoroid[i].hp -= spacecraft.arrow_power;
                arrow[j].visible = BLACK;
                arrow[j].x = 0;
                settingsetup.num_arrow++;

                /*-----------Meteoroid Destroyed------------*/
                if (meteoroid[i].hp <= 0) {
                    meteoroid[i].visible = BLACK;
                    if (remaining_meteoroids > 0) {
                        remaining_meteoroids--;
                    }

                    bang[i].visible = WHITE;
                    bang[i].x = meteoroid[i].x - 5;
                    bang[i].y = meteoroid[i].y + 8;

                    /*--------Gift Drop--------*/
                    if (meteoroid[i].has_gift) {
                        for (uint8_t k = 0; k < GIFT_MAX; k++) {
                            if (gift[k].object_id == i && gift[k].visible == BLACK && gift[k].type != GIFT_NONE) {
                                gift[k].x = meteoroid[i].x + 5;
                                gift[k].y = meteoroid[i].y  + 7;
                                gift[k].visible = WHITE;
                                break;
                            }
                        }
                        meteoroid[i].has_gift = false;
                    }

                    BUZZER_PlayTones(tones_BUM);
                    break;
                }
            }
        }
    }
}


void sc_game_meteoroid_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_METEOROID_SETUP: {
        APP_DBG_SIG("SC_GAME_METEOROID_SETUP\n");
        scGameMeteoroidSetup();
    } break;

    case SC_GAME_METEOROID_RANDOM: {
        APP_DBG("SC_GAME_METEOROID_RANDOM\n");
        scGameMeteoroidRandom();
    } break;

    case SC_GAME_METEOROID_MOVE: {
        APP_DBG_SIG("SC_GAME_METEOROID_MOVE\n");
        scGameMeteoroidMove();
    } break;

    case SC_GAME_METEOROID_DETONATOR: {
        APP_DBG_SIG("SC_GAME_METEOROID_DETONATOR\n");
        sc_gameMeteoroidDetonation();
    } break;

    case SC_GAME_METEOROID_CHECK_WIN: {
        APP_DBG_SIG("SC_GAME_METEOROID_CHECK_WIN\n");
        if (remaining_meteoroids <= 0) {
            task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_ROUND_COMPLETE);
        }
    } break; 

    case SC_GAME_METEOROID_RESET: {
        APP_DBG_SIG("SC_GAME_METEOROID_RESET\n");
        scGameMeteoroidSetup();
    } break;

    default:
        break;
    }
}