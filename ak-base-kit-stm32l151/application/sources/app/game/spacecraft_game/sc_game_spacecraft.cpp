#include "sc_game_spacecraft.h"

sc_game_spacecraft_t spacecraft;
static int32_t spacecraft_y = AXIS_Y_SPACECRAFT;
static int32_t spacecraft_x = AXIS_X_SPACECRAFT;
static bool is_invincible = false;

static void scGameSpacecraftSetup() {
    spacecraft.x = AXIS_X_SPACECRAFT; 
    spacecraft.y = AXIS_Y_SPACECRAFT; 
    spacecraft.visible = WHITE; 
    spacecraft.hearts = save_data.spacecraft_hearts;
    spacecraft.arrow_power = save_data.spacecraft_arrow_power;
    spacecraft.action_image = 1; 
    APP_DBG("spacecraft y: %d\n", spacecraft.y);
}

static void scGameSpacecraftUp() {
    if (spacecraft.visible == WHITE) {
        spacecraft_y -= SPACECRAFT_MOVE_STEP;
        if (spacecraft_y <= 0) {
            spacecraft_y = 0;
        }
    }
}

static void scGameSpacecraftDown() {
    if (spacecraft.visible == WHITE) {
        spacecraft_y += SPACECRAFT_MOVE_STEP;
        if (spacecraft_y >= 40) {
            spacecraft_y = 40;
        }
    }
}

static void scGameSpacecraftForward() {
    if (spacecraft.visible == WHITE) {
        spacecraft_x += SPACECRAFT_MOVE_STEP;
        if (spacecraft_x >= 110) {
            spacecraft_x = 110;
        }
    }
}

static void scGameSpacecraftBackward() {
    if (spacecraft.visible == WHITE) {
        spacecraft_x -= SPACECRAFT_MOVE_STEP;
        if (spacecraft_x <= 0) {
            spacecraft_x = 0;
        }
    }
}

static void scGameSpacecraftReset() {
    spacecraft.x = AXIS_X_SPACECRAFT;
    spacecraft.y = AXIS_Y_SPACECRAFT;
    spacecraft.visible = BLACK;
    spacecraft_y = AXIS_Y_SPACECRAFT;
    spacecraft_x = AXIS_X_SPACECRAFT;
    is_invincible = false;
}

static void scGameSpacecraftHandleDeath() {
    spacecraft.visible = BLACK;
    save_data.spacecraft_hearts--;
    if (save_data.spacecraft_arrow_power > 1) {
        save_data.spacecraft_arrow_power = save_data.spacecraft_arrow_power / 2;
    }
    spacecraft_x = AXIS_X_SPACECRAFT;
    spacecraft_y = AXIS_Y_SPACECRAFT;
    if (save_data.spacecraft_hearts <= 0) {
        task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_CHECK_GAME_OVER);
        return;
    }
    is_invincible = true;
    timer_set(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_SETUP, SC_GAME_SPACECRAFT_RESPAWN_INTERVAL, TIMER_ONE_SHOT);
    timer_set(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_RESET_INVINCIBLE, SC_GAME_SPACECRAFT_RESET_INVINCIBLE_INTERVAL, TIMER_ONE_SHOT);
}

static void scGameSpacecraftCheckGameOverByEnemy() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        for (uint8_t j = 0; j < MAX_ARROW_ENEMY; j++) {
            if (enemy[i].arrow[j].visible == WHITE && !is_invincible &&
                enemy[i].arrow[j].x + 10 <= spacecraft.x + SIZE_BITMAP_SPACECRAFT_X && spacecraft.y <= (enemy[i].arrow[j].y + SIZE_BITMAP_ARROW_ENEMY_Y - 3) && (spacecraft.y + SIZE_BITMAP_SPACECRAFT_Y) >= enemy[i].arrow[j].y + 3) {
                scGameSpacecraftHandleDeath();
            }
        }
    }
}

static void scGameSpacecraftCheckGameOverByMeteoroid() {
    for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
        if (meteoroid[i].visible == WHITE && !is_invincible) {
            if (spacecraft.x < (meteoroid[i].x + SIZE_BITMAP_METEOROIDS_X - 4) && (spacecraft.x + SIZE_BITMAP_SPACECRAFT_X - 4) > meteoroid[i].x &&
                spacecraft.y < (meteoroid[i].y + SIZE_BITMAP_METEOROIDS_Y + 1) && (spacecraft.y + SIZE_BITMAP_SPACECRAFT_Y + 1) > meteoroid[i].y ) {
                scGameSpacecraftHandleDeath();
            }
        }
    }
}

static void scGameSpacecraftCheckGameOverByEnergyBall() {
    for (uint8_t i = 0; i < MAX_NUM_BOSS_SKILL; i++) {
        if (energy_ball[i].visible == WHITE && !is_invincible) {
            if (energy_ball[i].x + 5 < spacecraft.x + SIZE_BITMAP_SPACECRAFT_X && 
                spacecraft.y < (energy_ball[i].y + SIZE_BITMAP_ENERGY_BALL_Y - 1) && (spacecraft.y + SIZE_BITMAP_SPACECRAFT_Y - 1) > energy_ball[i].y) {
                scGameSpacecraftHandleDeath();
            }
        }
    }
}

static void scGameSpacecraftCheckGameOverByLaser() {
    if (laser.visible == WHITE && !is_invincible) {
        if (spacecraft.y < (laser.y + SIZE_BITMAP_LASER_Y - 1) && (spacecraft.y + SIZE_BITMAP_SPACECRAFT_Y - 1) > laser.y) {
            scGameSpacecraftHandleDeath();
        }
    }
}


void sc_game_spacecraft_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_SPACECRAFT_SETUP: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_SETUP\n");
        scGameSpacecraftSetup();
    } break;

    case SC_GAME_SPACECRAFT_UPDATE: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_UPDATE\n");
        spacecraft.y = spacecraft_y;
        spacecraft.x = spacecraft_x;
        APP_DBG("spacecraft x: %d, y: %d\n", spacecraft.x, spacecraft.y);
    } break;

    case SC_GAME_SPACECRAFT_FORWARD: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_FORWARD\n");
        scGameSpacecraftForward();
    } break;

    case SC_GAME_SPACECRAFT_BACKWARD: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_BACKWARD\n");
        scGameSpacecraftBackward();
    } break;

    case SC_GAME_SPACECRAFT_UP: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_UP\n");
        scGameSpacecraftUp();
    } break;

    case SC_GAME_SPACECRAFT_DOWN: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_DOWN\n");
        scGameSpacecraftDown();
    } break;

    case SC_GAME_SPACECRAFT_RESET: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_RESET\n");
        scGameSpacecraftReset();
    } break;

    case SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENEMY: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENEMY\n");
        scGameSpacecraftCheckGameOverByEnemy();
    } break;

    case SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_METEOROID: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_METEOROID\n");
        scGameSpacecraftCheckGameOverByMeteoroid();
    } break;

    case SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENERGY_BALL: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENERGY_BALL\n");
        scGameSpacecraftCheckGameOverByEnergyBall();
    } break;

    case SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_LASER: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_LASER\n");
        scGameSpacecraftCheckGameOverByLaser();
    } break;

    case SC_GAME_SPACECRAFT_RESET_INVINCIBLE: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_RESET_INVINCIBLE\n");
        is_invincible = false;
    } break;

    default:
        break;
    }
}