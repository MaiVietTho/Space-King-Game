#include "qrcode.h"
#include "scr_info.h"
#include "scr_menu_game.h"
#include "screens_bitmap.h"

static void view_scr_game_info();

view_dynamic_t dyn_view_info = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_info
};

view_screen_t scr_info = {
	&dyn_view_info,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

class line_segment {
public:
	int x0, x1;
	int distance() {
		return (x1 - x0);
	}
};

void view_scr_info() {
	/* link base kit */
	QRCode qrcode;
	uint8_t qrcodeData[qrcode_getBufferSize(8)];
	qrcode_initText(&qrcode, qrcodeData, 8, 0, "https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu");

	for (uint8_t y = 0; y < qrcode.size; y++) {
		for (uint8_t x = 0; x < qrcode.size; x++) {
			view_render.drawPixel(x+40, y+2, qrcode_getModule(&qrcode, x, y) ? 1 : 0);
		}
	}
}

void view_scr_game_info() {
	view_render.clear();
	view_render.drawBitmap(0, 0, bitmap_poster,	128, 64, WHITE);
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(0, 25);
	view_render.print("SPACE");
	view_render.setCursor(2, 35);
	view_render.print("KING");
}

void scr_info_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		// Read setting
		eeprom_read(	EEPROM_SETTING_START_ADDR, \
						(uint8_t*)&settingdata, \
						sizeof(settingdata));
		sc_game_setting_validate(&settingdata);
		BUZZER_Sleep(settingdata.silent);
						
		eeprom_read(	EEPROM_SAVE_DATA_START_ADDR, \
						(uint8_t*)&save_data, \
						sizeof(save_data));
		if (save_data.spacecraft_hearts <= 0 || save_data.spacecraft_arrow_power <= 0 || save_data.current_round < SC_GAME_ROUND_1  || save_data.current_round > SC_GAME_ROUND_5) {
			save_data.spacecraft_hearts = 3; 
			save_data.spacecraft_arrow_power = 1;
			save_data.current_round = SC_GAME_ROUND_1;
		}
		APP_DBG("READ EEPROM SAVE DATA hearts: %d, arrow_power: %d, round: %d\n",save_data.spacecraft_hearts, save_data.spacecraft_arrow_power, save_data.current_round);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_LOGO_INTERVAL, TIMER_ONE_SHOT);	
	} break;

	case AC_DISPLAY_SHOW_IDLE: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
		SCREEN_TRAN(scr_idle_handle, &scr_idle);
	} break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
	} break;

	default:
		break;
	}
}
