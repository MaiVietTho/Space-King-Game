#ifndef __APP_EEPROM_H__
#define __APP_EEPROM_H__

#include <stdint.h>
#include "app.h"

/**
  *****************************************************************************
  * EEPROM define address.
  *
  *****************************************************************************
  */
#define EEPROM_START_ADDR								(0X0000)
#define EEPROM_END_ADDR									(0X1000)


#define EEPROM_SCORE_START_ADDR     (0X0010)
#define EEPROM_SCORE_PLAY_ADDR      (0X00FA)

#define EEPROM_SETTING_START_ADDR   (0X0100)

#define EEPROM_SAVE_DATA_START_ADDR   (0x0118)

/******************************************************************************/
/* Archey game */
/******************************************************************************/
typedef struct {
  /* score data */
  uint32_t score_now;
  uint32_t score_1st;
  uint32_t score_2nd;
  uint32_t score_3rd;
} ar_game_score_t;

typedef struct {
  /* setting data */
  bool silent;
  uint8_t num_arrow;
  uint8_t arrow_speed;
  uint8_t level;
} ar_game_setting_t;

#define SETTING_DEFAULT_NUM_ARROW     (5)
#define SETTING_DEFAULT_ARROW_SPEED   (5)
#define SETTING_DEFAULT_LEVEL         (1)

static inline void sc_game_setting_validate(ar_game_setting_t* setting) {
  uint8_t* silent_raw = (uint8_t*)&setting->silent;
  if (*silent_raw > 1) {
    *silent_raw = 0;
  }
  if (setting->num_arrow < 1 || setting->num_arrow > 10) {
    setting->num_arrow = SETTING_DEFAULT_NUM_ARROW;
  }
  if (setting->arrow_speed != SETTING_DEFAULT_ARROW_SPEED) {
    setting->arrow_speed = SETTING_DEFAULT_ARROW_SPEED;
  }
  if (setting->level < 1 || setting->level > 3) {
    setting->level = SETTING_DEFAULT_LEVEL;
  }
}

typedef struct{
  uint8_t spacecraft_hearts;
  uint8_t spacecraft_arrow_power;
  uint8_t current_round;
} sc_game_save_data_t;

#endif //__APP_EEPROM_H__
