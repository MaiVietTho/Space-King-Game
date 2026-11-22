#include "sc_game_boss_skill.h"

bool is_laser_shooting;
sc_game_skill_energy_ball_t energy_ball[MAX_NUM_BOSS_SKILL];
sc_game_skill_laser_t laser;

static void scGameBossSkillSetup() {
    for (uint8_t i = 0; i < MAX_NUM_BOSS_SKILL; i++) { 
        energy_ball[i].x = 130; 
        energy_ball[i].y = 0;
        energy_ball[i].visible = BLACK; 
        energy_ball[i].action_image = 1; 
    } 
    laser.x = 130;
    laser.y = 0;
    laser.visible = BLACK;
    laser.action_image = 1;
    is_laser_shooting = false;
}

static void scGameBossSkillMoveEnergyBall() {
    // Called every tick
    for (uint8_t i = 0; i < MAX_NUM_BOSS_SKILL; i++) {
        if (energy_ball[i].visible == WHITE) {
            energy_ball[i].x -= SC_ENERGY_BALL_SPEED_X;
            if (energy_ball[i].x + SIZE_BITMAP_ENERGY_BALL_X <= 0) {
                energy_ball[i].visible = BLACK;
            }
        }
    }
}

static void scGameBossSKillShootEnergyBall() {
    if (is_laser_shooting) return;
    
    for (uint8_t i = 0; i < MAX_NUM_BOSS_SKILL; i++) { 
        if (energy_ball[i].visible == BLACK) { 
            energy_ball[i].x = boss.x - 5;
            energy_ball[i].y = boss.y + 6;
            energy_ball[i].visible = WHITE; 
            BUZZER_PlayTones(tones_cc); 
            break; 
        } 
    } 
}

static void scGameBossSKillShootLaser() {
    laser.x = (boss.x + 11) - SIZE_BITMAP_LASER_X;
    laser.y = boss.y + 9;
    laser.visible = WHITE;
    is_laser_shooting = true;
    BUZZER_PlayTones(tones_cc);
}

void sc_game_boss_skill_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_BOSS_SKILL_SETUP: {
        APP_DBG_SIG("SC_GAME_BOSS_SKILL_SETUP\n");
        scGameBossSkillSetup();
    } break;

    case SC_GAME_BOSS_SKILL_MOVE_ENERGY_BALL: {
        APP_DBG_SIG("SC_GAME_BOSS_SKILL_MOVE_ENERGY_BALL\n");
        scGameBossSkillMoveEnergyBall();
    } break;

    case SC_GAME_BOSS_SKILL_SHOOT_ENERGY_BALL: {
        APP_DBG_SIG("SC_GAME_BOSS_SKILL_SHOOT_ENERGY_BALL\n");
        scGameBossSKillShootEnergyBall();
        timer_set(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_SHOOT_ENERGY_BALL, boss_enraged? SC_GAME_ENERGY_BALL_SHOOT_INTERVAL / 2 : SC_GAME_ENERGY_BALL_SHOOT_INTERVAL, TIMER_ONE_SHOT);
    } break;

    case SC_GAME_BOSS_SKILL_RESET: {
        APP_DBG_SIG("SC_GAME_BOSS_SKILL_RESET\n");
        scGameBossSkillSetup();
    } break;

    case SC_GAME_BOSS_SKILL_SHOOT_LASER: {
        APP_DBG_SIG("SC_GAME_BOSS_SKILL_SHOOT_LASER\n");
        scGameBossSKillShootLaser();
        timer_set(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_RESET_LASER, SC_GAME_LASER_RESET_INTERVAL, TIMER_ONE_SHOT);
    } break;

    case SC_GAME_BOSS_SKILL_RESET_LASER: {
        APP_DBG_SIG("SC_GAME_BOSS_SKILL_RESET_LASER\n");
        is_laser_shooting = false;
        laser.x = 130;
        laser.y = 0;
        laser.visible = BLACK;
        timer_set(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_SHOOT_LASER, boss_enraged? SC_GAME_LASER_SHOOT_INTERVAL / 2 : SC_GAME_LASER_SHOOT_INTERVAL, TIMER_ONE_SHOT);
    } break;

    default:
        break;
    }
}