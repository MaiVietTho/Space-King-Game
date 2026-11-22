#include "sc_game_bang.h"

sc_game_bang_t bang[NUM_BANG];

static void scGameBangSetup() {
    for (uint8_t i = 0; i < NUM_BANG; i++) { 
        bang[i].x = 0; 
        bang[i].y = 0;
        bang[i].visible = BLACK; 
        bang[i].action_image = 1; 
    }  
}

static void scGameBangUpdate() {
    for (uint8_t i = 0; i < NUM_BANG; i++) { 
        if (bang[i].visible == WHITE) { 
            bang[i].action_image++; 
        } 
        if (bang[i].action_image == 4) {
            bang[i].action_image = 1; 
            bang[i].visible = BLACK; 
        } 
    }
}

static void scGameBangReset() {
    for (uint8_t i = 0; i < NUM_BANG; i++) { 
        bang[i].visible = BLACK; 
        bang[i].action_image = 1; 
    }     
}

void sc_game_bang_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_BANG_SETUP: {
        APP_DBG_SIG("SC_GAME_BANG_SETUP\n");
        scGameBangSetup();
    } break;

    case SC_GAME_BANG_UPDATE: {
        APP_DBG_SIG("SC_GAME_BANG_UPDATE\n");
        scGameBangUpdate();
    } break;

    case SC_GAME_BANG_RESET: {
        APP_DBG_SIG("SC_GAME_BANG_RESET\n");
        scGameBangReset();
    } break;

    default:
        break;
    }
}