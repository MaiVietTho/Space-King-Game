#include "sc_game_enemy.h"

#define AXIS_X_ENEMY_ROW_0          (115)
#define AXIS_X_ENEMY_ROW_1          (102)
#define AXIS_X_ENEMY_ROW_2          (89)
#define AXIS_X_ENEMY_ROW_3          (76)

#define SC_ENEMY_0                  (0)
#define SC_ENEMY_1                  (1)
#define SC_ENEMY_2                  (2)
#define SC_ENEMY_3                  (3)
#define SC_ENEMY_4                  (4)
#define SC_ENEMY_5                  (5)
#define SC_ENEMY_6                  (6)
#define SC_ENEMY_7                  (7)
#define SC_ENEMY_8                  (8)
#define SC_ENEMY_9                  (9)

#define SC_ENEMY_MOVE_SPEED_X       (13)
#define SC_ENEMY_MOVE_SPEED_Y       (3)

#define SC_RANDOM_NEG_Y_POS_1       (-8)
#define SC_RANDOM_NEG_Y_POS_2       (-18)
#define SC_RANDOM_NEG_Y_POS_3       (-28)
#define SC_RANDOM_NEG_Y_POS_4       (-38)
#define SC_RANDOM_NEG_Y_POS_5       (-48)

#define SC_RANDOM_POS_Y_POS_1       (62)
#define SC_RANDOM_POS_Y_POS_2       (72)
#define SC_RANDOM_POS_Y_POS_3       (82)
#define SC_RANDOM_POS_Y_POS_4       (92)
#define SC_RANDOM_POS_Y_POS_5       (102)

#define ENEMY_OUT_BOTTOM_Y          (54)
#define ENEMY_OUT_TOP_Y             (-8)

sc_game_enemy_t enemy[TOTAL_ENEMY ];

typedef struct {
    uint8_t start;
    uint8_t end;
    uint32_t target_x;
    uint8_t next_sig;
} enemy_team_t;

static enemy_team_t enemy_team[] = {
    { SC_ENEMY_8, SC_ENEMY_9, 0, SC_GAME_ENEMY_BUILD_TEAM_2 },
    { SC_ENEMY_5, SC_ENEMY_7, 0, SC_GAME_ENEMY_BUILD_TEAM_1 },
    { SC_ENEMY_3, SC_ENEMY_4, 0, SC_GAME_ENEMY_BUILD_TEAM_0 },
    { SC_ENEMY_0, SC_ENEMY_2, 0, 0 }
};

static const uint8_t enemy_y[TOTAL_ENEMY] = {
    AXIS_Y_ENEMY_COLUM_0,
    AXIS_Y_ENEMY_COLUM_2,
    AXIS_Y_ENEMY_COLUM_4,
    AXIS_Y_ENEMY_COLUM_1,
    AXIS_Y_ENEMY_COLUM_3,
    AXIS_Y_ENEMY_COLUM_0,
    AXIS_Y_ENEMY_COLUM_2,
    AXIS_Y_ENEMY_COLUM_4,
    AXIS_Y_ENEMY_COLUM_1,
    AXIS_Y_ENEMY_COLUM_3,
};

static const int8_t move_neg_y_pos[] = {
    SC_RANDOM_NEG_Y_POS_1,
    SC_RANDOM_NEG_Y_POS_2,
    SC_RANDOM_NEG_Y_POS_3,
    SC_RANDOM_NEG_Y_POS_4,
    SC_RANDOM_NEG_Y_POS_5
};

static const int8_t move_pos_y_pos[] = {
    SC_RANDOM_POS_Y_POS_1,
    SC_RANDOM_POS_Y_POS_2,
    SC_RANDOM_POS_Y_POS_3,
    SC_RANDOM_POS_Y_POS_4,
    SC_RANDOM_POS_Y_POS_5
};

static void scGameEnemySetTargetXByRound()
{
    switch (save_data.current_round) {

    case SC_GAME_ROUND_1: {
        enemy_team[0].target_x = AXIS_X_ENEMY_ROW_3;
        enemy_team[1].target_x = AXIS_X_ENEMY_ROW_2;
        enemy_team[2].target_x = AXIS_X_ENEMY_ROW_1;
        enemy_team[3].target_x = AXIS_X_ENEMY_ROW_0;
    } break;

    case SC_GAME_ROUND_2: {
        enemy_team[0].target_x = AXIS_X_ENEMY_ROW_3 - 13;
        enemy_team[1].target_x = AXIS_X_ENEMY_ROW_2 - 13;
        enemy_team[2].target_x = AXIS_X_ENEMY_ROW_1 - 13;
        enemy_team[3].target_x = AXIS_X_ENEMY_ROW_0 - 13;
    } break;

    case SC_GAME_ROUND_3: {
        enemy_team[0].target_x = AXIS_X_ENEMY_ROW_3 - 26;
        enemy_team[1].target_x = AXIS_X_ENEMY_ROW_2 - 26;
        enemy_team[2].target_x = AXIS_X_ENEMY_ROW_1 - 26;
        enemy_team[3].target_x = AXIS_X_ENEMY_ROW_0 - 26;      
    } break;

    default:
        break;
    }
}

/*----------------------GAME ROUND 1---------------------------*/

static void scGameEnemySetupMoveX() {
    scGameEnemySetTargetXByRound();

    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        enemy[i].visible = WHITE;
        enemy[i].x = POSITION_ENEMY_SETUP_X;
        enemy[i].y = enemy_y[i];
        enemy[i].hp = save_data.current_round * 2;
        enemy[i].hearts = settingsetup.level;
        enemy[i].action_image = 1;
    }

    task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_3);
}

static void scGameEnemyBuild(uint8_t team_id, uint8_t current_sig) {
    bool build_done = true;
    enemy_team_t* team = &enemy_team[team_id];

    for (uint8_t i = team->start; i <= team->end; i++) {
        if (enemy[i].visible == WHITE && enemy[i].x > team->target_x) {
            enemy[i].x -= SC_ENEMY_MOVE_SPEED_X;
            // X-axis setup position not reached yet
            if (enemy[i].x > team->target_x) {
                build_done = false;
            }
        }
    }

    if (!build_done) {
        timer_set(SC_GAME_ENEMY_ID, current_sig, 100, TIMER_ONE_SHOT);
    }
    else if (team->next_sig != 0) {
        timer_set(SC_GAME_ENEMY_ID, team->next_sig, 100, TIMER_ONE_SHOT);
    }
    else {
        task_post_pure_msg(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_RANDOM_SHOOT);
    }
}

/*----------------------GAME ROUND 2 / 3---------------------------*/

static void scGameEnemySetupMoveY() {
    scGameEnemySetTargetXByRound();

    for (uint8_t i = 0; i < sizeof(enemy_team) / sizeof(enemy_team[0]); i++) {
        enemy_team_t* team = &enemy_team[i];
        for (uint8_t j = team->start; j <= team->end; j++) {
            enemy[j].visible = BLACK;
            enemy[j].y = SC_RANDOM_NEG_Y_POS_4;
            enemy[i].hp = save_data.current_round * 2;
            enemy[i].hearts = settingsetup.level;
            enemy[i].action_image = 1;
            enemy[j].x = team->target_x;
        }
    }
}

static void scGameEnemySetupSpawnPosY(uint8_t id_row, bool is_staggered) {
    enemy_team_t* team = &enemy_team[id_row];

    for (uint8_t i = team->start; i <= team->end; i++) {
        if (enemy[i].hearts > 0 && enemy[i].visible == BLACK) {
            // Enemies move in a staggered pattern between rows.
            // Enemy spawns are randomized on even-numbered rows.
            if (is_staggered && (id_row % 2 == 0)) {
                enemy[i].y = move_pos_y_pos[rand() % 5];
            }
            else {
                enemy[i].y = move_neg_y_pos[rand() % 5];
            }
            enemy[i].visible = WHITE;
            break;
        }
    }
}

static void scGameEnemyRandomMoveY(bool is_staggered_move) {
    uint8_t row_1;
    uint8_t row_2;

    row_1 = rand() % 4;
    do {
        row_2 = rand() % 4;
    }
    while (row_1 == row_2 || ((row_1 % 2) == (row_2 % 2)));

    scGameEnemySetupSpawnPosY(row_1, is_staggered_move);
    scGameEnemySetupSpawnPosY(row_2, is_staggered_move);
}

static void scGameEnemyMoveY(bool is_staggered_move) {
    for (uint8_t i = 0; i < sizeof(enemy_team) / sizeof(enemy_team[0]); i++) {
        enemy_team_t* team = &enemy_team[i];

        // Enemies move from top to bottom.
        // If staggered movement is enabled, enemies in even rows move from bottom to top.
        int8_t direction = 1;
        if (is_staggered_move && (i % 2 == 0)) {
            direction = -1;
        }

        for (uint8_t j = team->start; j <= team->end; j++) {
            if (enemy[j].visible == WHITE) {
                enemy[j].y += (SC_ENEMY_MOVE_SPEED_Y * direction);
            }

            if (direction > 0) {
                if (enemy[j].y >= ENEMY_OUT_BOTTOM_Y) {
                    enemy[j].visible = BLACK;
                }
            }
            else {
                if (enemy[j].y <= ENEMY_OUT_TOP_Y) {
                    enemy[j].visible = BLACK;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------*/

static void scGameEnemyUpdate() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        if (enemy[i].visible == WHITE) {
            enemy[i].action_image ++;
            if (enemy[i].action_image > 6) {
                enemy[i].action_image = 1;
            }
        }
    }
}

static void scGameEnemyCheckWin() {
    bool win = true;
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        if (enemy[i].hp > 0 && enemy[i].hearts > 0) {
            win = false;
        }
    }

    if (win) {
        task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_ROUND_COMPLETE);
    }
}

static void scGameEnemyDetonation() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        if (enemy[i].visible != WHITE) continue;

        for (uint8_t j = 0; j < MAX_NUM_ARROW; j++) {
            if (arrow[j].visible != WHITE) continue;

            if ((enemy[i].x < (arrow[j].x + SIZE_BITMAP_ARROW_X - 3)) &&
                ((enemy[i].y - 2) <= arrow[j].y) && ((enemy[i].y + 8) >= arrow[j].y)) {

                enemy[i].hp -= spacecraft.arrow_power;
                arrow[j].visible = BLACK;
                arrow[j].x = 0;
                settingsetup.num_arrow++;

                /*-----------Enemy Destroyed------------*/
                if (enemy[i].hp <= 0) {
                    enemy[i].hearts--;
                    enemy[i].visible = BLACK;

                    bang[i].visible = WHITE;
                    bang[i].x = enemy[i].x - 5;
                    bang[i].y = enemy[i].y + 2;

                    /*--------Gift Drop--------*/
                    if (enemy[i].has_gift) {
                        for (uint8_t k = 0; k < GIFT_MAX; k++) {
                            if (gift[k].object_id == i) {
                                gift[k].x = enemy[i].x;
                                gift[k].y = enemy[i].y;
                                gift[k].visible = WHITE;
                            }
                        }
                        enemy[i].has_gift = false;
                    }

                    /*--------Enemy respawns--------*/
                    if (enemy[i].hearts > 0) {
                        enemy[i].hp = save_data.current_round * 2;
                        switch (save_data.current_round){
                            case SC_GAME_ROUND_1: {
                                enemy[i].x = POSITION_ENEMY_SETUP_X;
                                enemy[i].visible = WHITE;
                                uint8_t enemy_id = i;
                                task_post_common_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RESPAWN, &enemy_id, sizeof(enemy_id));
                            } break;

                            case SC_GAME_ROUND_2:
                            case SC_GAME_ROUND_3: {
                                enemy[i].y = SC_RANDOM_NEG_Y_POS_4;
                                enemy[i].visible = BLACK;
                            } break;
                            
                            default:
                                break;
                        }
                    }

                    BUZZER_PlayTones(tones_BUM);
                    break;
                }
            }
        }
    }
}

static void scGameEnemyReset() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
        enemy[i].visible = BLACK;
        enemy[i].x = POSITION_ENEMY_SETUP_X;
        enemy[i].y = enemy_y[i];
        enemy[i].hp = 2;
        enemy[i].hearts = 1;
    }
}

void sc_game_enemy_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_ENEMY_SETUP_MOVE_X: {
        APP_DBG_SIG("SC_GAME_ENEMY_SETUP_MOVE_X\n");
        scGameEnemySetupMoveX();
    } break;

    case SC_GAME_ENEMY_BUILD_TEAM_3: {
        APP_DBG_SIG("SC_GAME_ENEMY_BUILD_TEAM_3\n");
        scGameEnemyBuild(0, SC_GAME_ENEMY_BUILD_TEAM_3);
    } break;

    case SC_GAME_ENEMY_BUILD_TEAM_2: {
        APP_DBG_SIG("SC_GAME_ENEMY_BUILD_TEAM_2\n");
        scGameEnemyBuild(1, SC_GAME_ENEMY_BUILD_TEAM_2);
    } break;

    case SC_GAME_ENEMY_BUILD_TEAM_1: {
        APP_DBG_SIG("SC_GAME_ENEMY_BUILD_TEAM_1\n");
        scGameEnemyBuild(2, SC_GAME_ENEMY_BUILD_TEAM_1);
    } break;

    case SC_GAME_ENEMY_BUILD_TEAM_0: {
        APP_DBG_SIG("SC_GAME_ENEMY_BUILD_TEAM_0\n");
        scGameEnemyBuild(3, SC_GAME_ENEMY_BUILD_TEAM_0);
    } break;

    case SC_GAME_ENEMY_UPDATE: {
        APP_DBG_SIG("SC_GAME_ENEMY_UPDATE\n");
        scGameEnemyUpdate();
    } break;

    case SC_GAME_ENEMY_DETONATOR: {
        APP_DBG_SIG("SC_GAME_ENEMY_DETONATOR\n");
        scGameEnemyDetonation();
    } break;

    case SC_GAME_ENEMY_RESPAWN: {
        APP_DBG_SIG("SC_GAME_ENEMY_RESPAWN\n");
        uint8_t enemy_id = *get_data_common_msg(msg);
        APP_DBG_SIG("enemy respawn id: %d\n", enemy_id);
        if (enemy_id >= SC_ENEMY_0 && enemy_id <= SC_ENEMY_2) {
            timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_0, SC_GAME_ENEMY_RESPAWN_INTERVAL, TIMER_ONE_SHOT);
        }
        else if (enemy_id == SC_ENEMY_3 || enemy_id == SC_ENEMY_4) {
            timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_1, SC_GAME_ENEMY_RESPAWN_INTERVAL, TIMER_ONE_SHOT);
        }
        else if (enemy_id >= SC_ENEMY_5 && enemy_id <= SC_ENEMY_7) {
            timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_2, SC_GAME_ENEMY_RESPAWN_INTERVAL, TIMER_ONE_SHOT);
        }
        else if (enemy_id == SC_ENEMY_8 || enemy_id == SC_ENEMY_9) {
            timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_3, SC_GAME_ENEMY_RESPAWN_INTERVAL, TIMER_ONE_SHOT);
        }
    } break;

    case SC_GAME_ENEMY_CHECK_WIN: {
        APP_DBG_SIG("SC_GAME_ENEMY_CHECK_WIN\n");
        scGameEnemyCheckWin();
    } break;

    case SC_GAME_ENEMY_SETUP_MOVE_Y: {
        APP_DBG_SIG("SC_GAME_ENEMY_SETUP_MOVE_Y\n");
        scGameEnemySetupMoveY();
    } break;

    case SC_GAME_ENEMY_MOVE_Y: {
        APP_DBG_SIG("SC_GAME_ENEMY_MOVE_Y\n");
        scGameEnemyMoveY(false);
        timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_MOVE_Y, SC_GAME_ENEMY_MOVE_INTERVAL, TIMER_ONE_SHOT);
    } break;

    case SC_GAME_ENEMY_MOVE_ZIGZAG_Y: {
        APP_DBG_SIG("SC_GAME_ENEMY_MOVE_ZIGZAG_Y\n");
        scGameEnemyMoveY(true);
        timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_MOVE_ZIGZAG_Y, SC_GAME_ENEMY_MOVE_INTERVAL, TIMER_ONE_SHOT);
    } break;

    case SC_GAME_ENEMY_RANDOM_MOVE_Y: {
        APP_DBG_SIG("SC_GAME_ENEMY_RANDOM_MOVE_Y\n");
        scGameEnemyRandomMoveY(false);
        timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_Y, SC_GAME_ENEMY_SPAWN_INTERVAL, TIMER_ONE_SHOT);
    } break;

    case SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y: {
        APP_DBG_SIG("SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y\n");
        scGameEnemyRandomMoveY(true);
        timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y, SC_GAME_ENEMY_SPAWN_INTERVAL, TIMER_ONE_SHOT);
    } break;

    case SC_GAME_ENEMY_RESET: {
        APP_DBG_SIG("SC_GAME_ENEMY_RESET\n");
        scGameEnemyReset();
    } break;

    default:
        break;
    }
}