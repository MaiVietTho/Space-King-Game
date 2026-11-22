#include "button.h"

#include "sys_dbg.h"

#include "app.h"
#include "app_bsp.h"
#include "app_dbg.h"
#include "app_if.h"

#include "task_list.h"
#include "task_list_if.h"
#include "scr_spacecraft_game.h"

static bool is_hold_up;
static bool is_hold_down;
button_t btn_mode;
button_t btn_up;
button_t btn_down;

static void btn_hold_move(uint8_t sig) {
	task_post_pure_msg(SC_GAME_SPACECRAFT_ID, sig);
	timer_set(SC_GAME_SPACECRAFT_ID, sig, SC_GAME_SPACECRAFT_MOVE_INTERVAL, TIMER_PERIODIC);
}

void btn_mode_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		APP_DBG("[btn_mode_callback] BUTTON_SW_STATE_PRESSED\n");
		if (sc_game_state == GAME_PLAY) {
			task_post_pure_msg(SC_GAME_ARROW_ID, SC_GAME_ARROW_SHOOT);
		}
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		APP_DBG("[btn_mode_callback] BUTTON_SW_STATE_LONG_PRESSED\n");
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_MODE_LONG_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
		APP_DBG("[btn_mode_callback] BUTTON_SW_STATE_RELEASED\n");
		if (sc_game_state != GAME_PLAY) {
			task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_MODE_RELEASED);
		}
	}
		break;

	default:
		break;
	}
}

void btn_up_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		APP_DBG("[btn_up_callback] BUTTON_SW_STATE_PRESSED\n");
		is_hold_up = false;
		if (sc_game_state == GAME_PLAY && save_data.current_round != SC_GAME_ROUND_4) {
			task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_UP);
		}
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		APP_DBG("[btn_up_callback] BUTTON_SW_STATE_LONG_PRESSED\n");
		is_hold_up = true;
		if (sc_game_state == GAME_PLAY && save_data.current_round == SC_GAME_ROUND_4) {
			btn_hold_move(SC_GAME_SPACECRAFT_FORWARD);
		}
		else if (sc_game_state == GAME_PLAY) {
			btn_hold_move(SC_GAME_SPACECRAFT_UP);
		}
		else {
			task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_UP_LONG_PRESSED);
		}
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
		APP_DBG("[btn_up_callback] BUTTON_SW_STATE_RELEASED\n");
		timer_remove_attr(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_UP);
		timer_remove_attr(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_FORWARD);
		if (is_hold_up) {
			is_hold_up = false;
		}
		else if (sc_game_state == GAME_PLAY) {
			if (save_data.current_round == SC_GAME_ROUND_4) {
				task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_UP);
			}
		}
		else {
			task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_UP_RELEASED);
		}
	}
		break;

	default:
		break;
	}
}

void btn_down_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		APP_DBG("[btn_down_callback] BUTTON_SW_STATE_PRESSED\n");
		is_hold_down = false;
		if (sc_game_state == GAME_PLAY && save_data.current_round != SC_GAME_ROUND_4) {
			task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_DOWN);
		}
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		APP_DBG("[btn_down_callback] BUTTON_SW_STATE_LONG_PRESSED\n");
		is_hold_down = true;
		if (sc_game_state == GAME_PLAY && save_data.current_round == SC_GAME_ROUND_4) {
			btn_hold_move(SC_GAME_SPACECRAFT_BACKWARD);
		}
		else if (sc_game_state == GAME_PLAY) {
			btn_hold_move(SC_GAME_SPACECRAFT_DOWN);
		}
		else {
			task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED);
		}
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
		APP_DBG("[btn_down_callback] BUTTON_SW_STATE_RELEASED\n");
		timer_remove_attr(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_DOWN);
		timer_remove_attr(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_BACKWARD);
		if (is_hold_down) {
			is_hold_down = false;
		}
		else if (sc_game_state == GAME_PLAY) {
			if (save_data.current_round == SC_GAME_ROUND_4) {
				task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_DOWN);
			}
		}
		else {
			task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_DOWN_RELEASED);
		}
	}
		break;

	default:
		break;
	}
}
