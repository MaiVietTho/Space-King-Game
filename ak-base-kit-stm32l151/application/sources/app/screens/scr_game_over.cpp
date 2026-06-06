#include "scr_game_over.h"

/*****************************************************************************/
/* Variable Declaration - game over */
/*****************************************************************************/

/*****************************************************************************/
/* View - game over */
/*****************************************************************************/
static void view_scr_game_over();

view_dynamic_t dyn_view_item_game_over = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_over
};

view_screen_t scr_game_over = {
	&dyn_view_item_game_over,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_game_over() {
	// Screen
	view_render.fillScreen(WHITE);
	view_render.setTextSize(2);
	view_render.setTextColor(BLACK);
	view_render.setCursor(11, 10);
	view_render.print("GAME OVER");
	view_render.setTextSize(1);
	view_render.setCursor(20, 30);
	view_render.print("Hello Chicken!");
	// Icon
	view_render.drawBitmap(10, 	48,	icon_restart,	15,	15,	0);
	view_render.drawBitmap(100,	48,	icon_go_home,	16,	16,	0);
}

/*****************************************************************************/
/* Handle - game over */
/*****************************************************************************/

void scr_game_over_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		// View render
		view_render.initialize();
		view_render_display_on();
	} break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
		BUZZER_PlayTones(tones_cc);
	} break;

	case AC_DISPLAY_BUTTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_RELEASED\n");
		BUZZER_PlayTones(tones_cc);
	} break;

	case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_RELEASED\n");
		SCREEN_TRAN(scr_spacecraft_game_handle, &scr_spacecraft_game );
		BUZZER_PlayTones(tones_cc);
	} break;

	default:
		break;
	}
}