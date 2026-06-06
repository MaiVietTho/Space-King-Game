#include "sc_game_arrow_enemy.h"

#include "sc_game_enemy.h"

static void scGameArrowEnemySetup() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        for (uint8_t j = 0; j < MAX_ARROW_ENEMY; j++) {
            enemy[i].arrow[j].x = enemy[i].x;
            enemy[i].arrow[j].y = enemy[i].y + 3;
            enemy[i].arrow[j].visible = BLACK;
            enemy[i].arrow[j].action_image = 1;
        }
    }
}

void scGameEnemyShoot(uint8_t enemyId) {
    for (uint8_t i = 0; i < MAX_ARROW_ENEMY; i++) {
        if (enemy[enemyId].visible == WHITE && enemy[enemyId].arrow[i].visible == BLACK) {
            enemy[enemyId].arrow[i].x = enemy[enemyId].x;
            enemy[enemyId].arrow[i].y = enemy[enemyId].y + 3;
            enemy[enemyId].arrow[i].visible = WHITE;
            BUZZER_PlayTones(tones_cc);
            break;
        }
    }
}

void scGameArrowEnemyRandomShoot() {
    bool selected[TOTAL_ENEMY] = {false};
    int count = 0;

    while (count < SHOOT_COUNT) {
        int id = rand() % TOTAL_ENEMY;

        if (!selected[id]) {
            selected[id] = true;
            scGameEnemyShoot(id);
            count++;
        }
    }
}

void scGameArrowEnemyShoot() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        if (enemy[i].visible == WHITE) {
            for (uint8_t j = 0; j < MAX_ARROW_ENEMY; j++) {
                if (enemy[i].arrow[j].visible == BLACK && enemy[i].y >= AXIS_Y_ENEMY_COLUM_0) {
                    enemy[i].arrow[j].x = enemy[i].x;
                    enemy[i].arrow[j].y = enemy[i].y + 3;
                    enemy[i].arrow[j].visible = WHITE;
                    BUZZER_PlayTones(tones_cc);
                    break;
                }
            }  
        }
    }
}

void scGameArrowEnemyRun() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        for (uint8_t j = 0; j < MAX_ARROW_ENEMY; j++) {
            if (enemy[i].arrow[j].visible == WHITE) {
                enemy[i].arrow[j].x -= settingsetup.arrow_speed;
                if (enemy[i].arrow[j].x < 0) {
                    enemy[i].arrow[j].visible = BLACK;
                }
            }
        }
    }
}

void scGameArrowEnemyReset() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        for (uint8_t j = 0; j < MAX_ARROW_ENEMY; j++) {
            enemy[i].arrow[j].x = POSITION_ENEMY_SETUP_X;
            enemy[i].arrow[j].y = enemy[i].y + 3;
            enemy[i].arrow[j].visible = BLACK;
            enemy[i].arrow[j].action_image = 1;
        }
    }
}

void sc_game_arrow_enemy_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_ARROW_ENEMY_SETUP: {
        APP_DBG_SIG("SC_GAME_ARROW_ENEMY_SETUP\n");
        scGameArrowEnemySetup();
    } break;

    case SC_GAME_ARROW_ENEMY_RANDOM_SHOOT: {
        APP_DBG_SIG("SC_GAME_ARROW_ENEMY_RANDOM_SHOOT\n");
        scGameArrowEnemyRandomShoot();
        timer_set(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_RANDOM_SHOOT, SC_GAME_RANDOM_ENEMY_SHOOT_INTERVAL, TIMER_ONE_SHOT);
    } break;

    case SC_GAME_ARROW_ENEMY_RUN: {
        APP_DBG_SIG("SC_GAME_ARROW_ENEMY_RUN\n");
        scGameArrowEnemyRun();
    } break;

    case SC_GAME_ARROW_ENEMY_RESET: {
        APP_DBG_SIG("SC_GAME_ARROW_ENEMY_RESET\n");
        scGameArrowEnemyReset();
    } break;

    case SC_GAME_ARROW_ENEMY_SHOOT: {
        APP_DBG_SIG("SC_GAME_ARROW_ENEMY_SHOOT\n");
        scGameArrowEnemyShoot();
        timer_set(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SHOOT, SC_GAME_ENEMY_SHOOT_INTERVAL, TIMER_ONE_SHOT);
    }

    default:
        break;
    }
}