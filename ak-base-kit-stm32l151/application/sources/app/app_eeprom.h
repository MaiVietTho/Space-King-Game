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

typedef struct{
  uint8_t spacecraft_hearts;
  uint8_t spacecraft_arrow_power;
  uint8_t current_round;
} sc_game_save_data_t;

#endif //__APP_EEPROM_H__
