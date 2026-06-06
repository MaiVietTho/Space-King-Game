/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   13/08/2016
 ******************************************************************************
**/

#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ak.h"
#if defined (IF_NETWORK_NRF24_EN)
#include "nrf_nwk_sig.h"
#endif

#include "app_if.h"
#include "app_eeprom.h"
#include "app_data.h"

/*****************************************************************************/
/* SYSTEM task define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	SYSTEM_AK_FLASH_UPDATE_REQ = AK_USER_DEFINE_SIG,
};

/*****************************************************************************/
/* FIRMWARE task define
 */
/*****************************************************************************/
/* define timer */
#define FW_PACKED_TIMEOUT_INTERVAL			(5000)
#define FW_UPDATE_REQ_INTERVAL				(5000)

/* define signal */
enum {
	FW_CRENT_APP_FW_INFO_REQ = AK_USER_DEFINE_SIG,
	FW_CRENT_BOOT_FW_INFO_REQ,
	FW_UPDATE_REQ,
	FW_UPDATE_SM_OK,
	FW_TRANSFER_REQ,
	FW_INTERNAL_UPDATE_APP_RES_OK,
	FW_INTERNAL_UPDATE_BOOT_RES_OK,
	FW_SAFE_MODE_RES_OK,
	FW_UPDATE_SM_BUSY,
	FW_PACKED_TIMEOUT,
	FW_CHECKING_REQ
};

/*****************************************************************************/
/*  LIFE task define
 */
/*****************************************************************************/
/* define timer */
#define AC_LIFE_TASK_TIMER_LED_LIFE_INTERVAL		(1000)

/* define signal */
enum {
	AC_LIFE_SYSTEM_CHECK = AK_USER_DEFINE_SIG,
};

/*****************************************************************************/
/*  SHELL task define
 */
/*****************************************************************************/
/* define timer */

/* define signal */
enum {
	AC_SHELL_LOGIN_CMD = AK_USER_DEFINE_SIG,
	AC_SHELL_REMOTE_CMD,
};

/*****************************************************************************/
/*  RF24 task define
 */
/*****************************************************************************/
/* private define */
/* define timer */
/* define signal */
enum {
	AC_RF24_IF_INIT_NETWORK = AK_USER_DEFINE_SIG,
	AC_RF24_IF_PURE_MSG_OUT,
	AC_RF24_IF_COMMON_MSG_OUT,
	AC_RF24_IF_PURE_MSG_IN,
	AC_RF24_IF_COMMON_MSG_IN,
};

/*****************************************************************************/
/* IF task define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	AC_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
	AC_IF_PURE_MSG_OUT,
	AC_IF_COMMON_MSG_IN,
	AC_IF_COMMON_MSG_OUT,
	AC_IF_DYNAMIC_MSG_IN,
	AC_IF_DYNAMIC_MSG_OUT,
};

/*****************************************************************************/
/* UART_IF task define
 */
/*****************************************************************************/
/* timer signal */
/* define signal */

enum {
	AC_UART_IF_INIT = AK_USER_DEFINE_SIG,
	AC_UART_IF_PURE_MSG_OUT,
	AC_UART_IF_COMMON_MSG_OUT,
	AC_UART_IF_DYNAMIC_MSG_OUT,
	AC_UART_IF_PURE_MSG_IN,
	AC_UART_IF_COMMON_MSG_IN,
	AC_UART_IF_DYNAMIC_MSG_IN,
};

/*****************************************************************************/
/*  LIFE task define
 */
/*****************************************************************************/
/* define timer */
#define AC_DISPLAY_INITIAL_INTERVAL									(100)
#define AC_DISPLAY_STARTUP_INTERVAL									(2000)
#define AC_DISPLAY_LOGO_INTERVAL									(10000)
#define AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL				(150)
#define AC_DISPLAY_SHOW_MERRY_CHRISTMAS_SNOW_MOVING_UPDATE_INTERAL	(150)
#define AC_DISPLAY_SHOW_MERRY_CHRISTMAS_SLEEP_INTERVAL				(15000)
#define AC_DISPLAY_SHOW_MODBUS_PULL_INTERVAL						(1500)
#define AC_DISPLAY_SHOW_MODBUS_PULL_SLEEP_INTERVAL					(30000)
#define AC_DISPLAY_IDLE_INTERVAL									(20000)

/* define signal */
enum {
	AC_DISPLAY_INITIAL = AK_USER_DEFINE_SIG,
	AC_DISPLAY_BUTTON_MODE_RELEASED,
	AC_DISPLAY_BUTTON_UP_RELEASED,
	AC_DISPLAY_BUTTON_DOWN_RELEASED,
	AC_DISPLAY_BUTTON_MODE_PRESSED,						
	AC_DISPLAY_BUTTON_MODE_LONG_PRESSED,					
	AC_DISPLAY_BUTTON_UP_PRESSED,							
	AC_DISPLAY_BUTTON_UP_LONG_PRESSED,					
	AC_DISPLAY_BUTTON_DOWN_PRESSED,						
	AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED,
	AC_DISPLAY_SHOW_LOGO,
	AC_DISPLAY_SHOW_IDLE,
	AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE,
	AC_DISPLAY_SHOW_FW_UPDATE,
	AC_DISPLAY_SHOW_FW_UPDATE_ERR,
	AC_DISPLAY_SHOW_MERRY_CHRISTMAS_SNOW_MOVING_UPDATE,
	AC_DISPLAY_SHOW_MERRY_CHRISTMAS_SLEEP,
	AC_DISPLAY_SHOW_MODBUS_PULL_UPDATE,
	AC_DISPLAY_SHOW_MODBUS_PULL_SLEEP
};

/*****************************************************************************/
/*  ZIGBEE task define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	AC_ZIGBEE_INIT = AK_USER_DEFINE_SIG,
	AC_ZIGBEE_FORCE_START_COODINATOR,
	AC_ZIGBEE_START_COODINATOR,
	AC_ZIGBEE_PERMIT_JOINING_REQ,
	AC_ZIGBEE_ZCL_CMD_HANDLER
};

/*****************************************************************************/
/* DBG task define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	AC_DBG_TEST_1 = AK_USER_DEFINE_SIG,
};

/*****************************************************************************/
/*  Spacecraft game 'SCREEN HANDLE' task define
 */
/*****************************************************************************/
/* define timer */
#define SC_GAME_TIME_TICK_INTERVAL									(100)
#define SC_GAME_WAIT_RESET_INTERVAL									(2000)
/* define signal */
enum {
	SC_GAME_INITIAL_SETUP = AK_USER_DEFINE_SIG,
	SC_GAME_TIME_TICK = SC_GAME_DEFINE_SIG,
	SC_GAME_SETUP_COMMOM,
	SC_GAME_RESET,
	SC_GAME_EXIT_GAME,
	SC_GAME_ROUND_COMPLETE,
	SC_GAME_NEXT_ROUND,
	SC_GAME_CHECK_GAME_OVER,
};

/*****************************************************************************/
/*  Spacecraft game 'SCREEN CHANGE ROUND' task define
 */
/*****************************************************************************/
/* define signal */
enum {
	SC_GAME_CHANGE_ROUND_INITIAL_SETUP = AK_USER_DEFINE_SIG,
	SC_GAME_SETUP_NEXT_ROUND = SC_GAME_DEFINE_SIG,
};

/*****************************************************************************/
/*  Spacecraft game 'SPACECRAFT' task define
 */
/*****************************************************************************/
/* define timer */
#define SC_GAME_SPACECRAFT_RESPAWN_INTERVAL					(1000)
#define SC_GAME_SPACECRAFT_RESET_INVINCIBLE_INTERVAL		(3000)
/* define signal */
enum {
	SC_GAME_SPACECRAFT_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_SPACECRAFT_UPDATE,
	SC_GAME_SPACECRAFT_UP,
	SC_GAME_SPACECRAFT_DOWN,
	SC_GAME_SPACECRAFT_FORWARD,
	SC_GAME_SPACECRAFT_BACKWARD,
	SC_GAME_SPACECRAFT_RESET,
	SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENEMY,
	SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_METEOROID,
	SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_ENERGY_BALL,
	SC_GAME_SPACECRAFT_CHECK_GAME_OVER_BY_LASER,
	SC_GAME_SPACECRAFT_RESET_INVINCIBLE,
};

/*****************************************************************************/
/*  Spacecraft game 'ARROW' task define
 */
/*****************************************************************************/
/* define signal */
enum {
	SC_GAME_ARROW_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_ARROW_RUN,
	SC_GAME_ARROW_SHOOT,
	SC_GAME_ARROW_RESET,
};

/*****************************************************************************/
/*  Spacecraft game 'BANG' task define
 */
/*****************************************************************************/
/* define signal */
enum {
	SC_GAME_BANG_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_BANG_UPDATE,
	SC_GAME_BANG_RESET,
};

/*****************************************************************************/
/*  Spacecraft game 'STARFIELD' task define
 */
/*****************************************************************************/
/* define signal */
enum {
	SC_GAME_STARFIELD_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_STARFIELD_RESET,
};

/*****************************************************************************/
/*  Spacecraft game 'METEOROID' task define
 */
/*****************************************************************************/
/* define timer */
#define SC_GAME_METEOROID_SPAWN_INTERVAL					(2000)
#define SC_GAME_METEOROID_MOVE_INTERVAL						(200)
/* define signal */
enum {
	SC_GAME_METEOROID_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_METEOROID_MOVE,
	SC_GAME_METEOROID_DETONATOR,
	SC_GAME_METEOROID_CHECK_WIN,
	SC_GAME_METEOROID_RANDOM,
	SC_GAME_METEOROID_RESET,
};

/*****************************************************************************/
/*  Spacecraft game 'ENEMY' task define
 */
/*****************************************************************************/
/* define timer */
#define SC_GAME_ENEMY_MOVE_INTERVAL						(200)
#define SC_GAME_ENEMY_SPAWN_INTERVAL					(2000)
#define SC_GAME_ENEMY_RESPAWN_INTERVAL					(1000)
/* define signal */
enum {
	SC_GAME_ENEMY_SETUP_MOVE_X = SC_GAME_DEFINE_SIG,
	SC_GAME_ENEMY_RUN,
	SC_GAME_ENEMY_RESET,
	SC_GAME_ENEMY_UPDATE,
	SC_GAME_ENEMY_BUILD_TEAM_3,
	SC_GAME_ENEMY_BUILD_TEAM_2,
	SC_GAME_ENEMY_BUILD_TEAM_1,
	SC_GAME_ENEMY_BUILD_TEAM_0,
	SC_GAME_ENEMY_DETONATOR,
	SC_GAME_ENEMY_RESPAWN,
	SC_GAME_ENEMY_CHECK_WIN,
	SC_GAME_ENEMY_SETUP_MOVE_Y,
	SC_GAME_ENEMY_MOVE_Y,
	SC_GAME_ENEMY_MOVE_ZIGZAG_Y,
	SC_GAME_ENEMY_RANDOM_MOVE_Y,
	SC_GAME_ENEMY_RANDOM_MOVE_ZIGZAG_Y,
	SC_GAME_ENEMY_TEST,
};

/*****************************************************************************/
/*  Spacecraft game 'ARROW_ENEMY' task define
 */
/*****************************************************************************/
/* define timer */
#define SC_GAME_RANDOM_ENEMY_SHOOT_INTERVAL						(2000)
#define SC_GAME_ENEMY_SHOOT_INTERVAL							(1500)
/* define signal */
enum {
	SC_GAME_ARROW_ENEMY_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_ARROW_ENEMY_RUN,
	SC_GAME_ARROW_ENEMY_RANDOM_SHOOT,
	SC_GAME_ARROW_ENEMY_RESET,
	SC_GAME_ARROW_ENEMY_SHOOT,
};

/*****************************************************************************/
/*  Spacecraft game 'GIFT' task define
 */
/*****************************************************************************/
/* define timer */
#define SC_GAME_ENEMY_GIFT_SPAWN_INTERVAL					(5000)
#define SC_GAME_METEOROID_GIFT_SPAWN_INTERVAL				(1000)
/* define signal */
enum {
	SC_GAME_GIFT_RESET = SC_GAME_DEFINE_SIG,
	SC_GAME_GIFT_SETUP,
	SC_GAME_GIFT_RANDOM_FOR_ENEMY,
	SC_GAME_GIFT_RANDOM_FOR_METEOROID,
	SC_GAME_GIFT_RUN,
	SC_GAME_GIFT_CHECK_RECEIVE,
	SC_GAME_GIFT_OF_BOSS,
};

/*****************************************************************************/
/*  Spacecraft game 'BOSS' task define
 */
/*****************************************************************************/
/* define signal */
enum {
	SC_GAME_BOSS_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_BOSS_RANDOM_DIRECTION,
	SC_GAME_BOSS_MOVE,
	SC_GAME_BOSS_CHECK_WIN,
	SC_GAME_BOSS_UPDATE,
	SC_GAME_BOSS_HIT,
	SC_GAME_BOSS_RESET,
};

/*****************************************************************************/
/*  Spacecraft game 'BOSS SKILL' task define
 */
/*****************************************************************************/
/* define timer */
#define SC_GAME_ENERGY_BALL_SHOOT_INTERVAL					(4000)
#define SC_GAME_ENERGY_BALL_MOVE_INTERVAL					(200)
#define SC_GAME_LASER_SHOOT_INTERVAL						(10000)
#define SC_GAME_LASER_RESET_INTERVAL						(1000)
/* define signal */
enum {
	SC_GAME_BOSS_SKILL_SETUP = SC_GAME_DEFINE_SIG,
	SC_GAME_BOSS_SKILL_MOVE_ENERGY_BALL,
	SC_GAME_BOSS_SKILL_SHOOT_ENERGY_BALL,
	SC_GAME_BOSS_SKILL_RESET,
	SC_GAME_BOSS_SKILL_SHOOT_LASER,
	SC_GAME_BOSS_SKILL_RESET_LASER,
};

/*****************************************************************************/
/*  app function declare
 */
/*****************************************************************************/
#define APP_MAGIC_NUMBER	0xAABBCCDD
#define APP_VER				{0, 0, 0, 3}

typedef struct {
	uint32_t magic_number;
	uint8_t version[4];
} app_info_t;

extern const app_info_t app_info;

extern void* app_get_boot_share_data();
extern int  main_app();

#ifdef __cplusplus
}
#endif

#endif //__APP_H__
