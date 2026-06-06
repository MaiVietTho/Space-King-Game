#include "sc_game_starfield.h"

ar_game_starfield_t starfield;

static void scGameStarfieldSetup() {
    starfield.x = 0;
    starfield.y = 0;
    starfield.visible = WHITE;
    starfield.action_image = 1;
}

static void scGameStarfieldReset() {
    starfield.visible = BLACK;
    starfield.action_image = 1;
}

void sc_game_starfield_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SC_GAME_STARFIELD_SETUP: {
        APP_DBG_SIG("SC_GAME_STARFIELD_SETUP\n");
        scGameStarfieldSetup();
    } break;

    case SC_GAME_STARFIELD_RESET: {
        APP_DBG_SIG("SC_GAME_STARFIELD_RESET\n");
        scGameStarfieldReset();
    } break;
    
    default:
        break;
    }
}