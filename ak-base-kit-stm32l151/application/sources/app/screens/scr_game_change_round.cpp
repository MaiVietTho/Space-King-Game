#include "scr_game_change_round.h"

/*****************************************************************************/
/* Variable Declaration - game change round */
/*****************************************************************************/
uint8_t current_round = SC_GAME_ROUND_1;

/*****************************************************************************/
/* View - game change round */
/*****************************************************************************/
static void view_scr_game_change_round();

view_dynamic_t dyn_view_item_game_change_round = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_change_round
};

view_screen_t scr_game_change_round = {
	&dyn_view_item_game_change_round,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_game_change_round() {
	if (sc_game_state != GAME_FINISHED) {
		// Screen
		view_render.fillScreen(WHITE);
		view_render.setTextSize(2);
		view_render.setTextColor(BLACK);
		view_render.setCursor(25, 10);
		view_render.print("ROUND");
		view_render.setCursor(90, 10);
		view_render.print(save_data.current_round);
		view_render.setTextSize(1);
		view_render.setTextColor(BLACK);
		view_render.setCursor(32, 28);
		view_render.print("Good Luck!");
		view_render.setCursor(20, 40);
		view_render.print("You'll need it!");
	}
	else {
		view_render.clear();
		view_render.fillScreen(WHITE);
		view_render.drawBitmap(35, 5, bitmap_icon_charts, 60, 20, 0);
		view_render.setTextSize(1);
		view_render.setTextColor(BLACK);
		view_render.setCursor(15, 30);
		view_render.print("YOU ARE THE KING!");
	}
	// Icon
	view_render.drawBitmap(10, 	48,	icon_restart,	15,	15,	0);
	view_render.drawBitmap(100,	48,	icon_go_home,	16,	16,	0);
}

/*****************************************************************************/
/* Handle - game change round */
/*****************************************************************************/

void scr_game_change_round_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		// View render
		view_render.initialize();
		view_render_display_on();
		timer_set(SC_GAME_CHANGE_ROUND_ID, SC_GAME_SETUP_NEXT_ROUND, 2000, TIMER_ONE_SHOT);
	} break;

	case SC_GAME_SETUP_NEXT_ROUND: {
		APP_DBG_SIG("SC_GAME_SETUP_NEXT_ROUND\n");
		if (sc_game_state != GAME_FINISHED) {
			SCREEN_TRAN(scr_spacecraft_game_handle, &scr_spacecraft_game);
		}
	} break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		timer_remove_attr(SC_GAME_CHANGE_ROUND_ID, SC_GAME_SETUP_NEXT_ROUND);
		SCREEN_TRAN(scr_info_handle, &scr_info);
		BUZZER_PlayTones(tones_cc);
	} break;

	case AC_DISPLAY_BUTTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_RELEASED\n");
		BUZZER_PlayTones(tones_cc);
	} break;

	case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_RELEASED\n");
		timer_remove_attr(SC_GAME_CHANGE_ROUND_ID, SC_GAME_SETUP_NEXT_ROUND);
		SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
		BUZZER_PlayTones(tones_cc);
	} break;

	default:
		break;
	}
}