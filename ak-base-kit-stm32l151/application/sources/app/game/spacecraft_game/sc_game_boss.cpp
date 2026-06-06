#include "sc_game_boss.h"

#define BOSS_MOVE_LIMIT_LEFT      48
#define BOSS_MOVE_LIMIT_RIGHT     108
#define BOSS_MOVE_LIMIT_TOP       0
#define BOSS_MOVE_LIMIT_BOTTOM    34

bool boss_enraged;
int32_t next_gift_hp;
int32_t hp_max;

sc_game_boss_t boss;
static uint8_t count_step_move = 0;
typedef enum {
    BOSS_MOVE_UP,
    BOSS_MOVE_DOWN,
    BOSS_MOVE_FORWARD,
    BOSS_MOVE_BACKWARD,  
} direction_boss_t;

direction_boss_t dir_boss = BOSS_MOVE_FORWARD;

static void sc_gameBossSetup() {
    boss.x = 80;
    boss.y = 30;
    boss.hp = settingsetup.level * 50;
    boss.visible = WHITE;
    boss.action_image = 1; 
    hp_max = boss.hp;
    next_gift_hp = hp_max - (hp_max * 20 / 100);
    boss_enraged = false;
}

static void sc_gameBossRandomDirection() {
    uint8_t direction;
    direction = rand() % 4;
    if (direction == BOSS_MOVE_UP) {
        dir_boss = BOSS_MOVE_UP;
    }
    else if (direction == BOSS_MOVE_DOWN) {
        dir_boss = BOSS_MOVE_DOWN;
    }
    else if (direction == BOSS_MOVE_FORWARD) {
        dir_boss = BOSS_MOVE_FORWARD;
    }
    else {
        dir_boss = BOSS_MOVE_BACKWARD;
    }
}

static void sc_gameBossdMove() {
    switch (dir_boss) {
        case BOSS_MOVE_UP: {
            boss.y -= SC_METEOROID_MOVE_SPEED;
            if(is_laser_shooting) {
                laser.y -= SC_METEOROID_MOVE_SPEED;
            }
        } break;

        case BOSS_MOVE_DOWN: {
            boss.y += SC_METEOROID_MOVE_SPEED;
            if(is_laser_shooting) {
                laser.y += SC_METEOROID_MOVE_SPEED;
            }
        } break;

        case BOSS_MOVE_FORWARD: {
            boss.x -= SC_METEOROID_MOVE_SPEED;
            if (is_laser_shooting) {
                laser.x -= SC_METEOROID_MOVE_SPEED;
            }
        } break;

        case BOSS_MOVE_BACKWARD: {
            boss.x += SC_METEOROID_MOVE_SPEED;
            if (is_laser_shooting) {
                laser.x += SC_METEOROID_MOVE_SPEED;
            }          
        } break;

        default:
            break;
    }

    if (boss.y <= BOSS_MOVE_LIMIT_TOP) {
        boss.y = BOSS_MOVE_LIMIT_TOP;
        if (is_laser_shooting) {
            laser.y = boss.y + 9; 
        }
    }
    else if (boss.y >= BOSS_MOVE_LIMIT_BOTTOM) {
        boss.y = BOSS_MOVE_LIMIT_BOTTOM;
        if (is_laser_shooting) {
            laser.y = boss.y + 9; 
        }
    }

    if (boss.x >= BOSS_MOVE_LIMIT_RIGHT) {
        boss.x = BOSS_MOVE_LIMIT_RIGHT;
        if (is_laser_shooting) {
            laser.x = (boss.x + 11) - SIZE_BITMAP_LASER_X;
        }  
    }
    else if (boss.x <= BOSS_MOVE_LIMIT_LEFT) {
        boss.x = BOSS_MOVE_LIMIT_LEFT;
        if (is_laser_shooting) {
            laser.x = (boss.x + 11) - SIZE_BITMAP_LASER_X;
        }  
    }

    count_step_move++;
    if (count_step_move >= 5) {
        count_step_move = 0;
        task_post_pure_msg(SC_GAME_BOSS_ID, SC_GAME_BOSS_RANDOM_DIRECTION);
    }
}

static void sc_gameBossHit() {
    for (uint8_t i = 0; i < MAX_NUM_ARROW; i++) {
        if (arrow[i].visible != WHITE)
            continue;

        if ((boss.x < (arrow[i].x + SIZE_BITMAP_ARROW_X - 5)) &&
            boss.y <= (arrow[i].y + SIZE_BITMAP_ARROW_Y - 3) && (boss.y + SIZE_BITMAP_BOSS_Y) >= arrow[i].y + 3) {

            boss.hp -= spacecraft.arrow_power;
            arrow[i].visible = BLACK;
            arrow[i].x = 0;
            settingsetup.num_arrow++;

            bang[i].visible = WHITE;
            bang[i].x = boss.x;
            bang[i].y = boss.y + 2;
            BUZZER_PlayTones(tones_BUM);

            if (boss.hp <= (hp_max / 2)) {
                boss_enraged = true;
            }

            if (boss.hp <= next_gift_hp) {
                APP_DBG("boss hp: %d, next gift hp: %d\n", boss.hp, next_gift_hp);
                task_post_pure_msg(SC_GAME_GIFT_ID, SC_GAME_GIFT_OF_BOSS);
                while (boss.hp <= next_gift_hp) {
                    next_gift_hp -= hp_max * 20 / 100;
                }
            }
        }
    }  
}

void sc_game_boss_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_BOSS_SETUP: {
        APP_DBG_SIG("SC_GAME_BOSS_SETUP\n");
        sc_gameBossSetup();
    } break;

    case SC_GAME_BOSS_RANDOM_DIRECTION: {
        APP_DBG_SIG("SC_GAME_BOSS_RANDOM_DIRECTION\n");
        sc_gameBossRandomDirection();

    } break;

    case SC_GAME_BOSS_MOVE: {
        APP_DBG_SIG("SC_GAME_BOSS_MOVE\n");
        sc_gameBossdMove();
    } break;

    case SC_GAME_BOSS_UPDATE: {
        APP_DBG_SIG("SC_GAME_BOSS_UPDATE\n");
        boss.action_image++;
        if (boss.action_image == 5) {
            boss.action_image = 1;
        }
    } break;

    case SC_GAME_BOSS_CHECK_WIN: {
        APP_DBG_SIG("SC_GAME_BOSS_CHECK_WIN\n");
        if (boss.hp <= 0) {
            boss.action_image = 6;
            task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_ROUND_COMPLETE);
        }
    } break;

    case SC_GAME_BOSS_HIT: {
        APP_DBG_SIG("SC_GAME_BOSS_HIT\n");
        sc_gameBossHit();
    } break;

    case SC_GAME_BOSS_RESET: {
        APP_DBG_SIG("SC_GAME_BOSS_RESET\n");
        boss.visible = BLACK;
    } break;

    default:
        break;
    }
}