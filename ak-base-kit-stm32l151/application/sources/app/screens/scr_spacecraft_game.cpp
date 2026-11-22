#include "scr_spacecraft_game.h"

#include "screens.h"
#include "scr_game_change_round.h"

/*****************************************************************************/
/* Variable Declaration - Spacecraft game screen */
/*****************************************************************************/
sc_game_state_t sc_game_state; 
ar_game_setting_t settingsetup;
sc_game_save_data_t save_data;

/*****************************************************************************/
/* View - Spacecraft game screen*/
/*****************************************************************************/

void sc_game_frame_display() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(2,55);
	view_render.print("Ar:");
	view_render.print(settingsetup.num_arrow);
	view_render.setCursor(35,55);
	view_render.print("Pw:");
	view_render.print(spacecraft.arrow_power);
	view_render.setCursor(66,55);
	view_render.print("L:");
	view_render.print(spacecraft.hearts);
	view_render.drawRect(0,  LCD_HEIGHT - 10, 128, 10, WHITE);
	view_render.drawFastVLine(88, LCD_HEIGHT - 10, 10, WHITE);

	switch (save_data.current_round)
	{
	case SC_GAME_ROUND_1:
	case SC_GAME_ROUND_2:
	case SC_GAME_ROUND_3: {
		view_render.setCursor(99,55);
		view_render.print("L:");
		view_render.print(settingsetup.level);
	} break;

	case SC_GAME_ROUND_4: {
		view_render.setCursor(94,55);
		view_render.print("MT:");
		view_render.print(remaining_meteoroids);
	} break;

	case SC_GAME_ROUND_5: {
		view_render.setCursor(91,55);
		view_render.print("HP:");
		view_render.print(boss.hp);
	} break;

	default:
		break;
	}
	
}

void sc_game_spacecraft_display() {
	static uint8_t blink_frame = 0;
	blink_frame++;
	if (is_invincible && (blink_frame & 0x02)) {
		return;
	}

	if (spacecraft.visible == WHITE) {
		view_render.drawBitmap(	spacecraft.x, \
								spacecraft.y, \
								bitmap_spacecraft, \
								SIZE_BITMAP_SPACECRAFT_X, \
								SIZE_BITMAP_SPACECRAFT_Y, \
								WHITE);
	}
}

void sc_game_arrow_display() {
	for (uint8_t i = 0; i < MAX_NUM_ARROW; i++) {
		if (arrow[i].visible == WHITE) {
			view_render.drawBitmap(	arrow[i].x, \
									arrow[i].y, \
									bitmap_arrow, \
									SIZE_BITMAP_ARROW_X, \
									SIZE_BITMAP_ARROW_Y, \
									WHITE);
		}
	}
}

void sc_game_meteoroid_display() {
	for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
		if (meteoroid[i].visible == WHITE) {
			view_render.drawBitmap(	meteoroid[i].x, \
									meteoroid[i].y, \
									bitmap_meteoroid, \
									SIZE_BITMAP_METEOROIDS_X, \
									SIZE_BITMAP_METEOROIDS_Y, \
									WHITE);
		}
	}
}

void sc_game_bang_display() {
	for (uint8_t i = 0; i < NUM_BANG; i++) {
		if (bang[i].visible == WHITE) {
			if (bang[i].action_image == 1) {
				view_render.drawBitmap(	bang[i].x, \
										bang[i].y, \
										bitmap_bang_I, \
										SIZE_BITMAP_BANG_I_X, \
										SIZE_BITMAP_BANG_I_Y, \
										WHITE);
			}
			else if (bang[i].action_image == 2) {
				view_render.drawBitmap(	bang[i].x, \
										bang[i].y, \
										bitmap_bang_II, \
										SIZE_BITMAP_BANG_I_X, \
										SIZE_BITMAP_BANG_I_Y, \
				 						WHITE);
			}
			else if (bang[i].action_image == 3) {
				view_render.drawBitmap( bang[i].x + 2, \
										bang[i].y - 1, \
										bitmap_bang_III, \
										SIZE_BITMAP_BANG_II_X, \
										SIZE_BITMAP_BANG_II_Y, \
				 						WHITE);
			}
		}
	}
}

void sc_game_gift_heart_display() {
	for (uint8_t i = 0; i < GIFT_MAX; i++) {
		if (gift[i].visible == WHITE && gift[i].type == GIFT_HEART) {
			view_render.drawBitmap(	gift[i].x, \
									gift[i].y, \
									bitmap_gift_heart, \
									SIZE_BITMAP_GIFT_X, \
									SIZE_BITMAP_GIFT_Y, \
									WHITE);
		}
	}
}

void sc_game_gift_arrow_power_display() {
	for (uint8_t i = 0; i < GIFT_MAX; i++) {
		if (gift[i].visible == WHITE && gift[i].type == GIFT_ARROW_POWER) {
			view_render.drawBitmap(	gift[i].x, \
									gift[i].y, \
									bitmap_gift_arrow_power, \
									SIZE_BITMAP_GIFT_X, \
									SIZE_BITMAP_GIFT_Y, \
									WHITE);	
		}
	}
}

void sc_game_starfield_display() {
	if (starfield.visible == WHITE) {
		view_render.drawBitmap(	starfield.x, \
								starfield.y, \
								bitmap_starfield_I, \
								SIZE_BITMAP_STARFIELD_X, \
								SIZE_BITMAP_STARFIELD_Y, \
								WHITE);
	}
}

void sc_game_enemy_display() {
	for (uint8_t i = 0; i < TOTAL_ENEMY ; i++) {
		if (enemy[i].visible == WHITE) {
			if (enemy[i].action_image == 1 || enemy[i].action_image == 2) {
				view_render.drawBitmap(	enemy[i].x, \
										enemy[i].y, \
										bitmap_enemy_I, \
										SIZE_BITMAP_ENEMIES_X, \
										SIZE_BITMAP_ENEMIES_Y, \
				 						WHITE);
			}
			else if (enemy[i].action_image == 3 || enemy[i].action_image == 4) {
				view_render.drawBitmap(	enemy[i].x, \
										enemy[i].y, \
										bitmap_enemy_II, \
										SIZE_BITMAP_ENEMIES_X, \
										SIZE_BITMAP_ENEMIES_Y, \
				 						WHITE);			
			}
			else if (enemy[i].action_image == 5 || enemy[i].action_image == 6) {
				view_render.drawBitmap(	enemy[i].x, \
										enemy[i].y, \
										bitmap_enemy_III, \
										SIZE_BITMAP_ENEMIES_X, \
										SIZE_BITMAP_ENEMIES_Y, \
				 						WHITE);			
			}
		}
	}
}

void sc_game_arrow_enemy_display() {
    for (uint8_t i = 0; i < TOTAL_ENEMY; i++) {
		for (uint8_t j = 0; j < MAX_ARROW_ENEMY; j++) {
			if (enemy[i].arrow[j].visible == WHITE) {
				view_render.drawBitmap( enemy[i].arrow[j].x, \
										enemy[i].arrow[j].y, \
										bitmap_arrow_enemy, \
										SIZE_BITMAP_ARROW_ENEMY_X, \
										SIZE_BITMAP_ARROW_ENEMY_Y, \
										WHITE);
			}
		}
    }
}

void sc_game_skill_energy_ball_display() {
	for (uint8_t i = 0; i < MAX_NUM_BOSS_SKILL ; i++) {
		if (energy_ball[i].visible == WHITE) {
				view_render.drawBitmap(	energy_ball[i].x, \
										energy_ball[i].y, \
										bitmap_energy_ball, \
										SIZE_BITMAP_ENERGY_BALL_X, \
										SIZE_BITMAP_ENERGY_BALL_Y, \
				 						WHITE);
		}
	}
}

void sc_game_skill_laser_display() {
	if (laser.visible == WHITE) {
		view_render.drawBitmap(	laser.x, \
								laser.y, \
								bitmap_Laser, \
								SIZE_BITMAP_LASER_X, \
								SIZE_BITMAP_LASER_Y, \
								WHITE);
	}
}

void sc_game_boss_display() {
	if (boss.visible == WHITE) {
		if (boss.action_image == 1 || boss.action_image == 2) {
			view_render.drawBitmap(	boss.x, \
									boss.y, \
									bitmap_Boss_I, \
									SIZE_BITMAP_BOSS_X, \
									SIZE_BITMAP_BOSS_Y, \
									WHITE);
		}
		else if (boss.action_image == 3 || boss.action_image == 4) {
				view_render.drawBitmap(	boss.x, \
										boss.y, \
										bitmap_Boss_II, \
										SIZE_BITMAP_BOSS_X, \
										SIZE_BITMAP_BOSS_Y, \
										WHITE);			
		}
		else if (boss.action_image == 6) {
				view_render.drawBitmap(	boss.x, \
										boss.y, \
										bitmap_Boss_III, \
										SIZE_BITMAP_BOSS_X, \
										SIZE_BITMAP_BOSS_Y, \
										WHITE);			
		}
	}
}

static void view_scr_spacecraft_game();

view_dynamic_t dyn_view_item_spacecraft_game = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_spacecraft_game
};

view_screen_t scr_spacecraft_game = {
	&dyn_view_item_spacecraft_game,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

static void render_objects_by_round() {
	switch (save_data.current_round) {
	case SC_GAME_ROUND_1:
	case SC_GAME_ROUND_2:
	case SC_GAME_ROUND_3: {
		sc_game_enemy_display();
		sc_game_arrow_enemy_display();
	} break;

	case SC_GAME_ROUND_4: {
		sc_game_meteoroid_display();
	} break;

	case SC_GAME_ROUND_5: {
		sc_game_boss_display();
		sc_game_skill_energy_ball_display();
		sc_game_skill_laser_display();
	} break;

	default:
		break;
	}
}

void view_scr_spacecraft_game() {
	if (sc_game_state == GAME_PLAY) {
		sc_game_frame_display();
		sc_game_spacecraft_display();
		sc_game_arrow_display();
		sc_game_bang_display();
		sc_game_starfield_display();
		sc_game_gift_heart_display();
		sc_game_gift_arrow_power_display();
		render_objects_by_round();
	}
	else if (sc_game_state == GAME_ROUND_COMPLETED) {
		view_render.clear();
		view_render.setTextSize(2);
		view_render.setTextColor(WHITE);
		view_render.setCursor(17, 24);
		view_render.print("COMPLETED");
	}
	else if (sc_game_state == GAME_OVER) {
		view_render.clear();
		view_render.setTextSize(2);
		view_render.setTextColor(WHITE);
		view_render.setCursor(17, 24);
		view_render.print("YOU LOSE");
	}
}

/*****************************************************************************/
/* Handle - Spacecraft game screen */
/*****************************************************************************/

static void scGameSpacecraftRoundSetup(uint8_t round) {
    switch (round) {
    case SC_GAME_ROUND_1: {
        APP_DBG("ROUND 1\n");
		task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_SETUP_COMMOM);
		task_post_pure_msg(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SETUP);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_SETUP_MOVE_X);
		timer_set(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_ENEMY, SC_GAME_ENEMY_GIFT_SPAWN_INTERVAL, TIMER_ONE_SHOT);
	} break;

    case SC_GAME_ROUND_2: {
        APP_DBG("ROUND 2\n");
		task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_SETUP_COMMOM);
		task_post_pure_msg(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SETUP);
		task_post_pure_msg(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SHOOT);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_SETUP_MOVE_Y);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_Y);
		timer_set(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SHOOT, SC_GAME_ENEMY_SHOOT_INTERVAL, TIMER_PERIODIC);
		timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_Y, SC_GAME_ENEMY_SPAWN_INTERVAL, TIMER_PERIODIC);
		timer_set(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_ENEMY, SC_GAME_ENEMY_GIFT_SPAWN_INTERVAL, TIMER_ONE_SHOT);
	} break;

    case SC_GAME_ROUND_3: {
		APP_DBG("ROUND 3\n");
		task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_SETUP_COMMOM);
		task_post_pure_msg(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SHOOT);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_SETUP_MOVE_Y);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y);
		timer_set(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SHOOT, SC_GAME_ENEMY_SHOOT_INTERVAL, TIMER_PERIODIC);
		timer_set(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y, SC_GAME_ENEMY_SPAWN_INTERVAL, TIMER_PERIODIC);
		timer_set(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_ENEMY, SC_GAME_ENEMY_GIFT_SPAWN_INTERVAL, TIMER_ONE_SHOT);
	} break;

    case SC_GAME_ROUND_4: {
		APP_DBG("ROUND 4\n");
		task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_SETUP_COMMOM);
		task_post_pure_msg(SC_GAME_METEOROID_ID, SC_GAME_METEOROID_SETUP);
		timer_set(SC_GAME_METEOROID_ID, SC_GAME_METEOROID_RANDOM, SC_GAME_METEOROID_SPAWN_INTERVAL, TIMER_PERIODIC);
		timer_set(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_METEOROID, SC_GAME_METEOROID_GIFT_SPAWN_INTERVAL, TIMER_ONE_SHOT);
	} break;

    case SC_GAME_ROUND_5: {
		APP_DBG("ROUND 5\n");
		task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_SETUP_COMMOM);
		task_post_pure_msg(SC_GAME_BOSS_ID, SC_GAME_BOSS_SETUP);
		task_post_pure_msg(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_SETUP);
		timer_set(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_SHOOT_ENERGY_BALL, SC_GAME_ENERGY_BALL_SHOOT_INTERVAL, TIMER_ONE_SHOT);
		timer_set(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_SHOOT_LASER, SC_GAME_LASER_SHOOT_INTERVAL, TIMER_ONE_SHOT);
	} break;

    default:
        break;
    }

	timer_set(	AC_TASK_DISPLAY_ID, SC_GAME_TIME_TICK, SC_GAME_TIME_TICK_INTERVAL, TIMER_PERIODIC);
}

static void scGameSpacecraftTimeTick(uint8_t round) {
	task_post_pure_msg(SC_GAME_ARROW_ID, SC_GAME_ARROW_RUN);
	task_post_pure_msg(SC_GAME_BANG_ID, SC_GAME_BANG_UPDATE);
	task_post_pure_msg(SC_GAME_GIFT_ID, SC_GAME_GIFT_CHECK_RECEIVE);
	task_post_pure_msg(SC_GAME_GIFT_ID, SC_GAME_GIFT_RUN);

	switch (round)
	{
	case SC_GAME_ROUND_1: 
	case SC_GAME_ROUND_2:
	case SC_GAME_ROUND_3: {
		APP_DBG("SC_GAME_ROUND_%d\n", round);
		if (round == SC_GAME_ROUND_2) {
			task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_MOVE_Y);
		}
		else if (round == SC_GAME_ROUND_3) {
			task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_MOVE_ZIGZAG_Y);
		}
		task_post_pure_msg(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_RUN);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_UPDATE);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_DETONATOR);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_CHECK_WIN);
		task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENEMY);
	} break;

	case SC_GAME_ROUND_4: {
		APP_DBG("SC_GAME_ROUND_%d\n", round);
		task_post_pure_msg(SC_GAME_METEOROID_ID, SC_GAME_METEOROID_DETONATOR);
		task_post_pure_msg(SC_GAME_METEOROID_ID, SC_GAME_METEOROID_CHECK_WIN);
		task_post_pure_msg(SC_GAME_METEOROID_ID, SC_GAME_METEOROID_MOVE);
		task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_METEOROID);
	} break;

	case SC_GAME_ROUND_5: {
		APP_DBG("SC_GAME_ROUND_%d\n", round);
		task_post_pure_msg(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_MOVE_ENERGY_BALL);
		task_post_pure_msg(SC_GAME_BOSS_ID, SC_GAME_BOSS_UPDATE);
		task_post_pure_msg(SC_GAME_BOSS_ID, SC_GAME_BOSS_MOVE);
		task_post_pure_msg(SC_GAME_BOSS_ID, SC_GAME_BOSS_HIT);
		task_post_pure_msg(SC_GAME_BOSS_ID, SC_GAME_BOSS_CHECK_WIN);
		task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENERGY_BALL);
		task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_LASER);
	} break;
	
	default:
		break;
	}

	task_post_pure_msg(AC_TASK_DISPLAY_ID, SC_GAME_RENDER);
}

void scr_spacecraft_game_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		// Setting setup
		eeprom_read( EEPROM_SETTING_START_ADDR, \
				(uint8_t*)&settingsetup, \
				sizeof(settingsetup));
		sc_game_setting_validate(&settingsetup);
		scGameSpacecraftRoundSetup(save_data.current_round);
		// State update
		sc_game_state = GAME_PLAY;
	} break;

	case SC_GAME_SETUP_COMMOM: {
		APP_DBG_SIG("SC_GAME_SETUP_COMMOM\n");
		task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_SETUP);
		task_post_pure_msg(SC_GAME_ARROW_ID, SC_GAME_ARROW_SETUP);
		task_post_pure_msg(SC_GAME_BANG_ID, SC_GAME_BANG_SETUP);
		task_post_pure_msg(SC_GAME_GIFT_ID, SC_GAME_GIFT_SETUP);
		task_post_pure_msg(SC_GAME_STARFIELD_ID, SC_GAME_STARFIELD_SETUP);
	} break;

	case SC_GAME_TIME_TICK: {
		APP_DBG_SIG("SC_GAME_TIME_TICK\n");
		scGameSpacecraftTimeTick(save_data.current_round);
		SCREEN_NONE_UPDATE_MASK();
	} break;

	case SC_GAME_RENDER: {
		// nothing to do: scr_mng_dispatch() draws the screen after this message
	} break;

	case SC_GAME_ROUND_COMPLETE: {
		APP_DBG_SIG("SC_GAME_ROUND_COMPLETE\n");
		if (sc_game_state != GAME_PLAY) {
			break;
		}
		if (is_hit_showing) {
			break;
		}
		sc_game_state = GAME_ROUND_COMPLETED;
		task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_RESET);
		timer_set(AC_TASK_DISPLAY_ID, SC_GAME_NEXT_ROUND, SC_GAME_WAIT_RESET_INTERVAL, TIMER_ONE_SHOT);
	} break;

	case SC_GAME_NEXT_ROUND: {
		APP_DBG_SIG("SC_GAME_NEXT_ROUND\n");
		save_data.current_round++;
		if (save_data.current_round > SC_GAME_ROUND_5) {
			save_data.current_round = SC_GAME_ROUND_5;
			sc_game_state = GAME_FINISHED;
		}
		eeprom_write(	EEPROM_SAVE_DATA_START_ADDR, \
						(uint8_t*)&save_data, \
						sizeof(save_data));
		SCREEN_TRAN(scr_game_change_round_handle, &scr_game_change_round);
	} break;

	case SC_GAME_CHECK_GAME_OVER: {
		APP_DBG_SIG("SC_GAME_CHECK_GAME_OVER\n");
		if (sc_game_state == GAME_OVER) {
			break;
		}
		timer_remove_attr(AC_TASK_DISPLAY_ID, SC_GAME_NEXT_ROUND);
		save_data.spacecraft_hearts = 3; 
		save_data.spacecraft_arrow_power = 1;
		save_data.current_round = SC_GAME_ROUND_1;
		eeprom_write(	EEPROM_SAVE_DATA_START_ADDR, \
						(uint8_t*)&save_data, \
						sizeof(save_data));

		sc_game_state = GAME_OVER;
		task_post_pure_msg(SC_GAME_SCREEN_ID, SC_GAME_RESET);
		timer_set(AC_TASK_DISPLAY_ID, SC_GAME_EXIT_GAME, SC_GAME_WAIT_RESET_INTERVAL, TIMER_ONE_SHOT);
	} break;

	case SC_GAME_RESET: {
		APP_DBG_SIG("SC_GAME_RESET\n");
		// Reset game Object
		task_post_pure_msg(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_RESET);
		task_post_pure_msg(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RESET);
		task_post_pure_msg(SC_GAME_ARROW_ID, SC_GAME_ARROW_RESET);
		task_post_pure_msg(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_RESET);
		task_post_pure_msg(SC_GAME_METEOROID_ID, SC_GAME_METEOROID_RESET);
		task_post_pure_msg(SC_GAME_GIFT_ID, SC_GAME_GIFT_RESET);
		task_post_pure_msg(SC_GAME_BANG_ID, SC_GAME_BANG_RESET);
		task_post_pure_msg(SC_GAME_BOSS_ID, SC_GAME_BOSS_RESET);
		task_post_pure_msg(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_RESET);
		task_post_pure_msg(SC_GAME_STARFIELD_ID, SC_GAME_STARFIELD_RESET);
		timer_remove_attr(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_RANDOM_SHOOT);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_Y);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_MOVE_Y);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_MOVE_ZIGZAG_Y);
		timer_remove_attr(SC_GAME_ARROW_ENEMY_ID, SC_GAME_ARROW_ENEMY_SHOOT);
		timer_remove_attr(SC_GAME_METEOROID_ID, SC_GAME_METEOROID_RANDOM);
		timer_remove_attr(AC_TASK_DISPLAY_ID, SC_GAME_TIME_TICK);
		timer_remove_attr(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_ENEMY);
		timer_remove_attr(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_SHOOT_ENERGY_BALL);
		timer_remove_attr(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_MOVE_ENERGY_BALL);
		timer_remove_attr(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_RESET_LASER);
		timer_remove_attr(SC_GAME_BOSS_SKILL_ID, SC_GAME_BOSS_SKILL_SHOOT_LASER);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_0);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_1);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_2);
		timer_remove_attr(SC_GAME_ENEMY_ID, SC_GAME_ENEMY_BUILD_TEAM_3);
		timer_remove_attr(SC_GAME_GIFT_ID, SC_GAME_GIFT_RANDOM_FOR_METEOROID);
		timer_remove_attr(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_SETUP);
		timer_remove_attr(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_RESET_INVINCIBLE);
		timer_remove_attr(SC_GAME_SPACECRAFT_ID, SC_GAME_SPACECRAFT_HIT_DONE);
	} break;

	case SC_GAME_EXIT_GAME: {
		APP_DBG_SIG("SC_GAME_EXIT_GAME\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, SC_GAME_TIME_TICK);
		SCREEN_TRAN(scr_game_over_handle, &scr_game_over);		
	} break;

	default:
		break;
	}
}
