#include "sc_game_spacecraft.h"

sc_game_spacecraft_t spacecraft;
bool is_invincible = false;
bool is_hit_showing = false;
static int32_t last_check_y_enemy = AXIS_Y_SPACECRAFT;
static int32_t last_check_y_energy_ball = AXIS_Y_SPACECRAFT;

static void scGameSpacecraftSetup() {
    spacecraft.x = AXIS_X_SPACECRAFT; 
    spacecraft.y = AXIS_Y_SPACECRAFT; 
    spacecraft.visible = WHITE; 
    spacecraft.hearts = save_data.spacecraft_hearts;
    spacecraft.arrow_power = save_data.spacecraft_arrow_power;
    spacecraft.action_image = 1; 
    last_check_y_enemy = spacecraft.y;
    last_check_y_energy_ball = spacecraft.y;
    APP_DBG("spacecraft y: %d\n", spacecraft.y);
}

static void scGameSpacecraftUp() {
    if (spacecraft.visible == WHITE && !is_hit_showing) {
        spacecraft.y -= SPACECRAFT_MOVE_STEP;
        if (spacecraft.y <= 0) {
            spacecraft.y = 0;
        }
    }
}

static void scGameSpacecraftDown() {
    if (spacecraft.visible == WHITE && !is_hit_showing) {
        spacecraft.y += SPACECRAFT_MOVE_STEP;
        if (spacecraft.y >= 40) {
            spacecraft.y = 40;
        }
    }
}

static void scGameSpacecraftForward() {
    if (spacecraft.visible == WHITE && !is_hit_showing) {
        spacecraft.x += SPACECRAFT_MOVE_STEP;
        if (spacecraft.x >= 110) {
            spacecraft.x = 110;
        }
    }
}

static void scGameSpacecraftBackward() {
    if (spacecraft.visible == WHITE && !is_hit_showing) {
        spacecraft.x -= SPACECRAFT_MOVE_STEP;
        if (spacecraft.x <= 0) {
            spacecraft.x = 0;
        }
    }
}

static void scGameSpacecraftReset() {
    spacecraft.x = AXIS_X_SPACECRAFT;
    spacecraft.y = AXIS_Y_SPACECRAFT;
    spacecraft.visible = BLACK;
    is_invincible = false;
    is_hit_showing = false;
}

// Hit: keep the ship drawn where it was hit (over the shot) for a moment (300ms), then HitDone() removes it.
static void scGameSpacecraftHandleDeath() {
    if (spacecraft.visible == BLACK || is_hit_showing) {
        return;
    }
    is_hit_showing = true;
    timer_set(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_HIT_DONE, SC_GAME_SPACECRAFT_HIT_SHOW_INTERVAL, TIMER_ONE_SHOT);
}

static void scGameSpacecraftHitDone() {
    is_hit_showing = false;
    spacecraft.visible = BLACK;
    save_data.spacecraft_hearts--;
    if (save_data.spacecraft_arrow_power > 1) {
        save_data.spacecraft_arrow_power = save_data.spacecraft_arrow_power / 2;
    }
    spacecraft.x = AXIS_X_SPACECRAFT;
    spacecraft.y = AXIS_Y_SPACECRAFT;
    if (save_data.spacecraft_hearts <= 0) {
        task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_CHECK_GAME_OVER);
        return;
    }
    is_invincible = true;
    timer_set(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_SETUP, SC_GAME_SPACECRAFT_RESPAWN_INTERVAL, TIMER_ONE_SHOT);
    timer_set(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_RESET_INVINCIBLE, SC_GAME_SPACECRAFT_RESET_INVINCIBLE_INTERVAL, TIMER_ONE_SHOT);
}

/*The spaceship is destroyed when a bullet penetrates 10 px into its body (direct hit). 
If the spaceship is moving and collides with a bullet, 
it is also considered destroyed if the bullet's tail has not passed the 2/3 point of the spaceship's body*/
static bool scGameSpacecraftShotHitX(int32_t shot_x, int32_t shot_w, int32_t step, bool head_on) {
    int32_t nose_x = spacecraft.x + SIZE_BITMAP_SPACECRAFT_X;
    int32_t pass_x = nose_x - SPACECRAFT_SHOT_PASS_X;
    // Direct hit
    if (head_on) {
        return (shot_x <= pass_x) && (shot_x + step > pass_x);
    }
    //Collides with a bullet while moving
    return (shot_x + shot_w > pass_x) && (shot_x < nose_x);
}

static bool scGameSpacecraftArrowEnemyOverlapY(int32_t ship_y, int32_t arrow_y) {
    return ship_y <= (arrow_y + SIZE_BITMAP_ARROW_ENEMY_Y - 3) && (ship_y + SIZE_BITMAP_SPACECRAFT_Y) >= arrow_y + 3;
}

static bool scGameSpacecraftEnergyBallOverlapY(int32_t ship_y, int32_t ball_y) {
    return ship_y < (ball_y + SIZE_BITMAP_ENERGY_BALL_Y - 1) && (ship_y + SIZE_BITMAP_SPACECRAFT_Y - 1) > ball_y;
}

static void scGameSpacecraftCheckGameOverByEnemy() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        for (uint8_t j = 0; j < MAX_ARROW_ENEMY; j++) {
            if (enemy[i].arrow[j].visible == WHITE && !is_invincible &&
                scGameSpacecraftArrowEnemyOverlapY(spacecraft.y, enemy[i].arrow[j].y)) {
                bool head_on = scGameSpacecraftArrowEnemyOverlapY(last_check_y_enemy, enemy[i].arrow[j].y);
                if (scGameSpacecraftShotHitX(enemy[i].arrow[j].x, SIZE_BITMAP_ARROW_ENEMY_X, settingsetup.arrow_speed, head_on)) {
                    scGameSpacecraftHandleDeath();
                }
            }
        }
    }
    last_check_y_enemy = spacecraft.y;
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
            if (scGameSpacecraftEnergyBallOverlapY(spacecraft.y, energy_ball[i].y)) {
                bool head_on = scGameSpacecraftEnergyBallOverlapY(last_check_y_energy_ball, energy_ball[i].y);
                if (scGameSpacecraftShotHitX(energy_ball[i].x, SIZE_BITMAP_ENERGY_BALL_X, SC_ENERGY_BALL_SPEED_X, head_on)) {
                    scGameSpacecraftHandleDeath();
                }
            }
        }
    }
    last_check_y_energy_ball = spacecraft.y;
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

    case SC_GAME_SPACECRAFT_HIT_DONE: {
        APP_DBG_SIG("SC_GAME_SPACECRAFT_HIT_DONE\n");
        scGameSpacecraftHitDone();
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