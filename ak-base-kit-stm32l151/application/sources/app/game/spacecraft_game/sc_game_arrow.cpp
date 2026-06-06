#include "sc_game_arrow.h"

sc_game_arrow_t arrow[MAX_NUM_ARROW];

static void scGameArrowSetup() {
    for (uint8_t i = 0; i < MAX_NUM_ARROW; i++) { 
        arrow[i].x = 0; 
        arrow[i].y = 0;
        arrow[i].visible = BLACK; 
        arrow[i].action_image = 1; 
    } 
}

static void scGameArrowRun() {
    for (uint8_t i = 0; i < MAX_NUM_ARROW; i++) { 
        if (arrow[i].visible == WHITE) { 
            arrow[i].x += settingsetup.arrow_speed; 
            if (arrow[i].x == MAX_AXIS_X_ARROW) { 
                arrow[i].visible = BLACK; 
                arrow[i].x = 0; 
                settingsetup.num_arrow++; 
            } 
        } 
    }  
}

static void scGameArrowShoot() {
    for (uint8_t i = 0; i < MAX_NUM_ARROW; i++) { 
        if (arrow[i].visible == BLACK && settingsetup.num_arrow != 0 && spacecraft.visible == WHITE) { 
            settingsetup.num_arrow--; 
            arrow[i].visible = WHITE; 
            arrow[i].x = spacecraft.x;
            arrow[i].y = spacecraft.y + 5;
            BUZZER_PlayTones(tones_cc); 
            break; 
        } 
        else if (settingsetup.num_arrow == 0) { 
            BUZZER_PlayTones(tones_3beep); 
            break; 
        } 
    } 
}

static void scGameArrowReset() {
    for (uint8_t i = 0; i < MAX_NUM_ARROW; i++) { 
        arrow[i].x = 0; 
        arrow[i].y = 0; 
        arrow[i].visible = BLACK; 
        arrow[i].action_image = 1; 
    } 
}

void sc_game_arrow_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_ARROW_SETUP: {
        APP_DBG_SIG("SC_GAME_ARROW_SETUP\n");
        scGameArrowSetup();
    } break;

    case SC_GAME_ARROW_RUN: {
        APP_DBG_SIG("SC_GAME_ARROW_RUN\n");
        scGameArrowRun();
    } break;

    case SC_GAME_ARROW_SHOOT: {
        APP_DBG_SIG("SC_GAME_ARROW_SHOOT\n");
        scGameArrowShoot();
    } break;

    case SC_GAME_ARROW_RESET: {
        APP_DBG_SIG("SC_GAME_ARROW_RESET\n");
        scGameArrowReset();
    } break;

    default:
        break;
    }
}