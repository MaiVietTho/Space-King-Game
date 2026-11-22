#!/usr/bin/env python3
"""Generate the sequence diagrams of the docs as editable `.drawio.svg` files.

Usage (from the repo root):
    python3 tools/gen_diagrams.py

Open any output file in draw.io / diagrams.net to edit it; GitHub renders it as an image.
"""
import os

from seqdraw import Seq

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "resources/diagrams")

BTN = ("btn", "Buttons\n(app_bsp)", "actor")
TMR = ("tmr", "AK Timer", "kernel")
DISP = ("disp", "Display Task\n(screen manager)", "screen")
GAME = ("game", "Game Screen\n(scr_spacecraft_game)", "screen")
CR = ("cr", "Change Round\n(scr_game_change_round)", "screen")
EEP = ("eep", "EEPROM", "store")
SC = ("sc", "Spacecraft", "object")
ROUND = ("round", "Enemy / Meteoroid\n/ Boss", "object")


def P(seq, *parts):
    for p in parts:
        seq.part(*p)
    return seq


# ---------------------------------------------------------------- overview (README)

def overview():
    d = P(Seq("Space King - game sequence overview"), BTN, TMR, DISP, GAME,
          ("obj", "Game Objects\n(one task each)", "object"))
    d.msg("tmr", "disp", "AC_DISPLAY_INITIAL", "async")
    d.self_("disp", "Startup: eeprom_read(settings), apply Silent\nInfo: eeprom_read(settings, save_data)\n-> Menu")
    d.msg("btn", "disp", "MODE: Continue / Newbie", "async")
    d.self_("disp", "srand(millis)\nSCREEN_TRAN(change round)")
    d.msg("tmr", "disp", "SC_GAME_SETUP_NEXT_ROUND (2 s)", "async")
    d.msg("disp", "game", "SCREEN_ENTRY")
    d.self_("game", "eeprom_read(settings)\nround setup")
    d.msg("game", "obj", "*_SETUP", "async")
    d.msg("game", "tmr", "SC_GAME_TIME_TICK (100 ms)\n+ round timers")
    d.frag("loop", "every 100 ms")
    d.msg("tmr", "disp", "SC_GAME_TIME_TICK", "async")
    d.msg("disp", "game", "dispatch")
    d.msg("game", "obj", "update + collision signals", "async")
    d.msg("game", "disp", "SC_GAME_RENDER (after the updates)", "async")
    d.self_("disp", "view_render_screen()")
    d.msg("btn", "obj", "shoot / move", "async")
    d.end()
    d.frag("alt", "round cleared")
    d.msg("obj", "game", "SC_GAME_ROUND_COMPLETE", "async")
    d.msg("game", "obj", "SC_GAME_RESET", "async")
    d.msg("tmr", "disp", "SC_GAME_NEXT_ROUND (2 s)", "async")
    d.msg("disp", "game", "dispatch")
    d.self_("game", "round + 1, eeprom_write\nSCREEN_TRAN(change round)")
    d.else_("out of hearts")
    d.msg("obj", "game", "SC_GAME_CHECK_GAME_OVER", "async")
    d.self_("game", "save: round 1, eeprom_write")
    d.msg("game", "obj", "SC_GAME_RESET", "async")
    d.msg("tmr", "disp", "SC_GAME_EXIT_GAME (2 s)", "async")
    d.msg("disp", "game", "dispatch")
    d.self_("game", "SCREEN_TRAN(game over)")
    d.end()
    return d


# ---------------------------------------------------------------- runtime

def rt_boot_menu():
    d = P(Seq("Boot and menu"), BTN, DISP, TMR, EEP)
    d.msg("tmr", "disp", "AC_DISPLAY_INITIAL (100 ms)", "async")
    d.self_("disp", "scr_startup: LCD on, draw logo")
    d.msg("disp", "eep", "eeprom_read(setting)")
    d.self_("disp", "validate setting\nBUZZER_Sleep(silent)")
    d.msg("disp", "tmr", "AC_DISPLAY_SHOW_LOGO (2 s)")
    d.msg("tmr", "disp", "AC_DISPLAY_SHOW_LOGO", "async")
    d.self_("disp", "SCREEN_TRAN(scr_info)")
    d.msg("disp", "eep", "eeprom_read(setting, save_data)")
    d.self_("disp", "validate setting, save_data\nBUZZER_Sleep(silent)")
    d.msg("disp", "tmr", "AC_DISPLAY_SHOW_IDLE (10 s)")
    d.frag("alt", "MODE pressed")
    d.msg("btn", "disp", "BUTTON_MODE_RELEASED", "async")
    d.self_("disp", "SCREEN_TRAN(scr_menu_game)")
    d.else_("no input for 10 s")
    d.msg("tmr", "disp", "AC_DISPLAY_SHOW_IDLE", "async")
    d.self_("disp", "SCREEN_TRAN(scr_idle)")
    d.msg("btn", "disp", "MODE (or DOWN until no ball left)", "async")
    d.self_("disp", "SCREEN_TRAN(scr_menu_game)")
    d.end()
    d.msg("disp", "tmr", "AC_DISPLAY_SHOW_IDLE (20 s)")
    d.frag("loop", "choose item")
    d.msg("btn", "disp", "BUTTON_UP / DOWN_RELEASED", "async")
    d.self_("disp", "move cursor, restart 20 s timer")
    d.end()
    d.msg("btn", "disp", "BUTTON_MODE_RELEASED", "async")
    d.frag("alt", "Continue / Newbie")
    d.self_("disp", "Newbie: save_data = round 1, 3 hearts\nsrand(sys_ctrl_millis())\nSCREEN_TRAN(scr_game_change_round)")
    d.else_("Setting")
    d.self_("disp", "SCREEN_TRAN(scr_game_setting)")
    d.else_("Exit")
    d.self_("disp", "SCREEN_TRAN(scr_idle)")
    d.end()
    return d


def rt_round_start():
    d = P(Seq("Round start"), CR, TMR, GAME, ROUND,
          ("common", "Spacecraft, Arrow,\nBang, Gift, Starfield", "object"))
    d.msg("cr", "tmr", "SC_GAME_SETUP_NEXT_ROUND (2 s)")
    d.msg("tmr", "cr", "SC_GAME_SETUP_NEXT_ROUND", "async")
    d.msg("cr", "game", "SCREEN_TRAN -> SCREEN_ENTRY")
    d.self_("game", "eeprom_read(settingsetup)\nsc_game_setting_validate()\nscGameSpacecraftRoundSetup()")
    d.msg("game", "game", "SC_GAME_SETUP_COMMOM", "async")
    d.frag("alt", "round 1")
    d.msg("game", "round", "ARROW_ENEMY_SETUP\nENEMY_SETUP_MOVE_X", "async")
    d.msg("game", "tmr", "GIFT_RANDOM_FOR_ENEMY (5 s)")
    d.else_("round 2 / 3")
    d.msg("game", "round", "ENEMY_SETUP_MOVE_Y\n+ first move / shoot", "async")
    d.msg("game", "tmr", "ENEMY_RANDOM_MOVE_* (2 s)\nARROW_ENEMY_SHOOT (1.5 s)\nGIFT_RANDOM_FOR_ENEMY (5 s)")
    d.else_("round 4")
    d.msg("game", "round", "METEOROID_SETUP", "async")
    d.msg("game", "tmr", "METEOROID_RANDOM (2 s)\nGIFT_RANDOM_FOR_METEOROID (1 s)")
    d.else_("round 5")
    d.msg("game", "round", "BOSS_SETUP\nBOSS_SKILL_SETUP", "async")
    d.msg("game", "tmr", "SHOOT_ENERGY_BALL (4 s)\nSHOOT_LASER (10 s)")
    d.end()
    d.msg("game", "tmr", "SC_GAME_TIME_TICK (100 ms)")
    d.self_("game", "state = GAME_PLAY")
    d.note("game", "common", "SC_GAME_SETUP_COMMOM handled (after the round SETUPs)")
    d.msg("game", "common", "*_SETUP", "async")
    return d


def rt_tick():
    d = P(Seq("Game tick 100 ms"), TMR, DISP, GAME,
          ("common", "Spacecraft, Arrow,\nBang, Gift", "object"), ROUND)
    d.frag("loop", "every 100 ms while GAME_PLAY")
    d.msg("tmr", "disp", "SC_GAME_TIME_TICK", "async")
    d.msg("disp", "game", "dispatch")
    d.msg("game", "common", "ARROW_RUN\nBANG_UPDATE\nGIFT_CHECK_RECEIVE\nGIFT_RUN", "async")
    d.frag("alt", "round 1-3")
    d.msg("game", "round", "ENEMY_MOVE_Y | _ZIGZAG_Y (round 2 | 3)\nARROW_ENEMY_RUN\nENEMY_UPDATE\nENEMY_DETONATOR\nENEMY_CHECK_WIN", "async")
    d.msg("game", "common", "CHECK_GAME_OVER_BY_ENEMY", "async")
    d.else_("round 4")
    d.msg("game", "round", "METEOROID_DETONATOR\nMETEOROID_CHECK_WIN\nMETEOROID_MOVE", "async")
    d.msg("game", "common", "CHECK_GAME_OVER_BY_METEOROID", "async")
    d.else_("round 5")
    d.msg("game", "round", "MOVE_ENERGY_BALL\nBOSS_UPDATE\nBOSS_MOVE\nBOSS_HIT\nBOSS_CHECK_WIN", "async")
    d.msg("game", "common", "CHECK_GAME_OVER_BY_ENERGY_BALL\nCHECK_GAME_OVER_BY_LASER", "async")
    d.end()
    d.msg("game", "disp", "SC_GAME_RENDER", "async")
    d.self_("disp", "no redraw for TIME_TICK\n(SCREEN_NONE_UPDATE_MASK)")
    d.note("disp", "round", "FIFO: the updates above run first, then SC_GAME_RENDER")
    d.msg("disp", "game", "dispatch SC_GAME_RENDER")
    d.self_("disp", "view_render_screen()")
    d.end()
    return d


def rt_round_end():
    d = P(Seq("Round end"), ("obj", "Game Objects", "object"), GAME, DISP, TMR)
    d.frag("alt", "win")
    d.msg("obj", "game", "SC_GAME_ROUND_COMPLETE\n(Enemy / Meteoroid / Boss)", "async")
    d.self_("game", "ignore if state != GAME_PLAY\nor ship showing a hit\nstate = GAME_ROUND_COMPLETED")
    d.msg("game", "game", "SC_GAME_RESET", "async")
    d.msg("game", "tmr", "SC_GAME_NEXT_ROUND (2 s)")
    d.else_("lose")
    d.msg("obj", "game", "SC_GAME_CHECK_GAME_OVER\n(Spacecraft out of hearts)", "async")
    d.self_("game", "ignore if already GAME_OVER\nremove timer SC_GAME_NEXT_ROUND\nsave_data = round 1, 3 hearts\neeprom_write(save_data)\nstate = GAME_OVER")
    d.msg("game", "game", "SC_GAME_RESET", "async")
    d.msg("game", "tmr", "SC_GAME_EXIT_GAME (2 s)")
    d.end()
    d.note("obj", "tmr", "SC_GAME_RESET handled")
    d.msg("game", "obj", "*_RESET", "async")
    d.msg("game", "tmr", "timer_remove_attr(tick, spawn,\nmove, shoot, build, respawn)")
    d.frag("alt", "win")
    d.msg("tmr", "disp", "SC_GAME_NEXT_ROUND", "async")
    d.msg("disp", "game", "dispatch")
    d.self_("game", "round++ (after round 5: GAME_FINISHED)\neeprom_write(save_data)\nSCREEN_TRAN(scr_game_change_round)")
    d.else_("lose")
    d.msg("tmr", "disp", "SC_GAME_EXIT_GAME", "async")
    d.msg("disp", "game", "dispatch")
    d.self_("game", "SCREEN_TRAN(scr_game_over)")
    d.end()
    return d


def rt_buttons():
    d = P(Seq("Button handling"), BTN, TMR, SC, ("ar", "Arrow", "object"), DISP)
    d.frag("alt", "MODE pressed, in game")
    d.msg("btn", "ar", "SC_GAME_ARROW_SHOOT", "async")
    d.else_("MODE released, outside game")
    d.msg("btn", "disp", "BUTTON_MODE_RELEASED", "async")
    d.end()
    d.frag("alt", "UP pressed, in game (round 4: on release)")
    d.msg("btn", "sc", "SC_GAME_SPACECRAFT_UP", "async")
    d.else_("UP held, in game")
    d.msg("btn", "sc", "first step at once", "async")
    d.msg("btn", "tmr", "SPACECRAFT_UP (100 ms)\nround 4: FORWARD")
    d.frag("loop", "while held")
    d.msg("tmr", "sc", "SPACECRAFT_UP | FORWARD", "async")
    d.end()
    d.msg("btn", "tmr", "release: remove timer")
    d.else_("outside game")
    d.msg("btn", "disp", "BUTTON_UP_RELEASED\n/ _LONG_PRESSED", "async")
    d.end()
    d.note("btn", "disp", "DOWN mirrors UP: SPACECRAFT_DOWN, held in round 4: BACKWARD")
    return d


# ---------------------------------------------------------------- objects

def ob_spacecraft():
    d = P(Seq("Spacecraft"), BTN, GAME, SC, TMR)
    d.msg("game", "sc", "SPACECRAFT_SETUP", "async")
    d.self_("sc", "x = 0, y = 20, visible\nhearts, arrow_power <- save_data")
    d.msg("btn", "sc", "UP / DOWN / FORWARD / BACKWARD", "async")
    d.self_("sc", "move spacecraft.x/y 5 px\ny: 0..40, x: 0..110")
    d.msg("game", "sc", "CHECK_GAME_OVER_BY_*", "async")
    d.note("game", "sc", "head-on: hit when tip 10 px deep; moved into: hit\nunless the tail is 2/3 past the nose")
    d.frag("opt", "hit and not invincible")
    d.self_("sc", "HandleDeath(): ignore if hidden / showing a hit\nis_hit_showing: frozen over the shot")
    d.msg("sc", "tmr", "SPACECRAFT_HIT_DONE (300 ms)")
    d.msg("tmr", "sc", "SPACECRAFT_HIT_DONE", "async")
    d.self_("sc", "hide, hearts--, arrow_power /= 2")
    d.frag("alt", "hearts == 0")
    d.msg("sc", "game", "SC_GAME_CHECK_GAME_OVER", "async")
    d.else_("hearts left")
    d.msg("sc", "tmr", "SETUP (1 s)\nRESET_INVINCIBLE (3 s)")
    d.msg("tmr", "sc", "SPACECRAFT_SETUP (respawn)", "async")
    d.note("sc", "tmr", "ship blinks while invincible (2 frames on, 2 off)")
    d.msg("tmr", "sc", "RESET_INVINCIBLE", "async")
    d.end()
    d.end()
    d.msg("game", "sc", "SPACECRAFT_RESET", "async")
    d.self_("sc", "back to start, hide\ninvincible off, is_hit_showing off")
    return d


def ob_arrow():
    d = P(Seq("Arrow"), BTN, GAME, ("ar", "Arrow", "object"), ROUND)
    d.msg("game", "ar", "ARROW_SETUP", "async")
    d.self_("ar", "hide 10 arrows")
    d.msg("btn", "ar", "ARROW_SHOOT (MODE)", "async")
    d.frag("alt", "num_arrow > 0, spacecraft visible, not hit showing")
    d.self_("ar", "show 1 arrow at the ship (x, y + 5)\nnum_arrow--, tones_cc")
    d.else_("num_arrow == 0")
    d.self_("ar", "tones_3beep")
    d.end()
    d.msg("game", "ar", "ARROW_RUN", "async")
    d.self_("ar", "x += arrow_speed\nfully off screen (x >= 128):\nhide, num_arrow++")
    d.msg("game", "round", "*_DETONATOR / BOSS_HIT", "async")
    d.self_("round", "hit: hp -= arrow_power\nhide arrow, num_arrow++")
    d.msg("game", "ar", "ARROW_RESET", "async")
    return d


def ob_enemy_r1():
    d = P(Seq("Enemy - round 1"), GAME, ("en", "Enemy", "object"), TMR,
          ("ae", "Arrow Enemy", "object"), ("fx", "Bang, Gift", "object"))
    d.msg("game", "en", "ENEMY_SETUP_MOVE_X", "async")
    d.self_("en", "10 enemies at x = 141\nhp = round * 2, hearts = level")
    d.msg("en", "en", "ENEMY_BUILD_TEAM_3", "async")
    d.self_("en", "mark team as flying in")
    d.frag("loop", "every tick: team 3 -> 2 -> 1 -> 0")
    d.msg("game", "en", "ENEMY_UPDATE", "async")
    d.self_("en", "flying teams: x -= 6, stop at target_x\nteam in place: next team starts")
    d.end()
    d.msg("en", "ae", "ARROW_ENEMY_RANDOM_SHOOT", "async")
    d.msg("en", "tmr", "ARROW_ENEMY_RANDOM_SHOOT (2 s)")
    d.msg("game", "en", "ENEMY_UPDATE\nENEMY_DETONATOR\nENEMY_CHECK_WIN", "async")
    d.frag("opt", "enemy hp <= 0")
    d.self_("en", "hearts--, hide")
    d.msg("en", "fx", "show bang, drop gift")
    d.frag("opt", "hearts > 0")
    d.self_("en", "restore hp, x = 141, visible")
    d.msg("en", "en", "ENEMY_RESPAWN(id)", "async")
    d.msg("en", "tmr", "BUILD_TEAM of its team (1 s)")
    d.end()
    d.end()
    d.msg("en", "game", "SC_GAME_ROUND_COMPLETE\n(all enemies out of hp and hearts)", "async")
    return d


def ob_enemy_r23():
    d = P(Seq("Enemy - round 2 / 3"), GAME, ("en", "Enemy", "object"), TMR)
    d.msg("game", "en", "ENEMY_SETUP_MOVE_Y", "async")
    d.self_("en", "4 columns, hidden off screen\nhp = round * 2, hearts = level")
    d.frag("loop", "every 2 s")
    d.msg("tmr", "en", "ENEMY_RANDOM_MOVE_Y\n(round 3: _ZIGZAG_Y)", "async")
    d.self_("en", "2 columns (1 even, 1 odd): show 1 enemy\nwith hearts at a random off-screen y")
    d.end()
    d.frag("loop", "every tick")
    d.msg("game", "en", "ENEMY_MOVE_Y\n(round 3: _ZIGZAG_Y)", "async")
    d.self_("en", "y += 2 per tick (round 3: even\ncolumns go up), off screen: hide")
    d.end()
    d.msg("game", "en", "ENEMY_DETONATOR", "async")
    d.self_("en", "hp <= 0: hearts--, hide, restore hp")
    d.msg("game", "en", "ENEMY_CHECK_WIN", "async")
    d.msg("en", "game", "SC_GAME_ROUND_COMPLETE", "async")
    return d


def ob_arrow_enemy():
    d = P(Seq("Arrow Enemy"), GAME, ("ae", "Arrow Enemy", "object"), TMR, SC)
    d.msg("game", "ae", "ARROW_ENEMY_SETUP", "async")
    d.frag("alt", "round 1, every 2 s")
    d.msg("tmr", "ae", "ARROW_ENEMY_RANDOM_SHOOT", "async")
    d.self_("ae", "3 random visible enemies fire 1 shot each\n(all of them when fewer than 3 left)")
    d.else_("round 2 / 3, every 1.5 s")
    d.msg("tmr", "ae", "ARROW_ENEMY_SHOOT", "async")
    d.self_("ae", "every visible enemy fires 1 shot\n(max 3 shots per enemy)")
    d.end()
    d.msg("game", "ae", "ARROW_ENEMY_RUN", "async")
    d.self_("ae", "x -= arrow_speed\nfully off screen: hide")
    d.msg("game", "sc", "CHECK_GAME_OVER_BY_ENEMY", "async")
    d.msg("game", "ae", "ARROW_ENEMY_RESET", "async")
    return d


def ob_meteoroid():
    d = P(Seq("Meteoroid - round 4"), GAME, ("mt", "Meteoroid", "object"), TMR, ("fx", "Bang, Gift", "object"))
    d.msg("game", "mt", "METEOROID_SETUP", "async")
    d.self_("mt", "hide 10 meteoroids\nremaining = level * 20")
    d.frag("loop", "every 2 s")
    d.msg("tmr", "mt", "METEOROID_RANDOM", "async")
    d.self_("mt", "Spawn(id, 1 of 5 x positions)\ny = -20, hp = round * 2")
    d.end()
    d.msg("game", "mt", "METEOROID_DETONATOR", "async")
    d.frag("opt", "hp <= 0")
    d.self_("mt", "hide, remaining--")
    d.msg("mt", "fx", "show bang, drop gift")
    d.end()
    d.msg("game", "mt", "METEOROID_CHECK_WIN", "async")
    d.msg("mt", "game", "SC_GAME_ROUND_COMPLETE\n(remaining == 0)", "async")
    d.msg("game", "mt", "METEOROID_MOVE", "async")
    d.self_("mt", "x += 2, y += 2\ny >= 64: hide, remaining--")
    return d


def ob_gift():
    d = P(Seq("Gift"), GAME, ("gf", "Gift", "object"), TMR, ROUND, SC)
    d.msg("game", "gf", "GIFT_SETUP", "async")
    d.frag("loop", "until 4 gifts assigned")
    d.msg("tmr", "gf", "GIFT_RANDOM_FOR_ENEMY (5 s)\n| _FOR_METEOROID (1 s)", "async")
    d.self_("gf", "assign HEART | ARROW_POWER\nto 1 object + 1 free slot")
    d.end()
    d.msg("round", "gf", "gift carrier destroyed: show gift")
    d.msg("round", "gf", "GIFT_OF_BOSS (every 20% hp)", "async")
    d.msg("game", "gf", "GIFT_RUN", "async")
    d.self_("gf", "x -= 2, x <= 0: hide")
    d.msg("game", "gf", "GIFT_CHECK_RECEIVE", "async")
    d.frag("opt", "touches spacecraft")
    d.msg("gf", "sc", "HEART: hearts++\nARROW_POWER: arrow_power++")
    d.end()
    return d


def ob_boss():
    d = P(Seq("Boss - round 5"), GAME, ("bs", "Boss", "object"), ("gf", "Gift", "object"))
    d.msg("game", "bs", "BOSS_SETUP", "async")
    d.self_("bs", "x = 80, y = 30, hp = level * 50")
    d.frag("loop", "every tick")
    d.msg("game", "bs", "BOSS_UPDATE", "async")
    d.self_("bs", "next frame")
    d.msg("game", "bs", "BOSS_MOVE", "async")
    d.self_("bs", "move 2 px, x: 48..108, y: 0..34\nlaser follows the boss")
    d.msg("bs", "bs", "BOSS_RANDOM_DIRECTION (every 5 steps)", "async")
    d.msg("game", "bs", "BOSS_HIT", "async")
    d.self_("bs", "hit: hp -= arrow_power, bang\nhp <= 50%: boss_enraged")
    d.msg("bs", "gf", "GIFT_OF_BOSS (every 20% hp)", "async")
    d.msg("game", "bs", "BOSS_CHECK_WIN", "async")
    d.end()
    d.msg("bs", "game", "SC_GAME_ROUND_COMPLETE (hp <= 0)", "async")
    return d


def ob_boss_skill():
    d = P(Seq("Boss Skill - round 5"), GAME, ("sk", "Boss Skill", "object"), TMR, SC)
    d.msg("game", "sk", "BOSS_SKILL_SETUP", "async")
    d.frag("loop", "every tick")
    d.msg("game", "sk", "MOVE_ENERGY_BALL", "async")
    d.self_("sk", "x -= 3 (SC_ENERGY_BALL_SPEED_X),\nfully off screen: hide")
    d.end()
    d.frag("loop", "every 4 s (enraged: 2 s)")
    d.msg("tmr", "sk", "SHOOT_ENERGY_BALL", "async")
    d.self_("sk", "fire 1 energy ball (not during laser)")
    d.msg("sk", "tmr", "re-arm SHOOT_ENERGY_BALL")
    d.end()
    d.frag("loop", "every 10 s (enraged: 5 s)")
    d.msg("tmr", "sk", "SHOOT_LASER", "async")
    d.self_("sk", "laser on, follows the boss")
    d.msg("sk", "tmr", "RESET_LASER (1 s)")
    d.msg("tmr", "sk", "RESET_LASER", "async")
    d.msg("sk", "tmr", "re-arm SHOOT_LASER")
    d.end()
    d.msg("game", "sc", "CHECK_GAME_OVER_BY_ENERGY_BALL\nCHECK_GAME_OVER_BY_LASER", "async")
    return d


def ob_bang_starfield():
    d = P(Seq("Bang and Starfield"), GAME, ("st", "Starfield", "object"), ("bg", "Bang", "object"), ROUND)
    d.msg("game", "st", "STARFIELD_SETUP", "async")
    d.self_("st", "show 128x54 background")
    d.msg("game", "bg", "BANG_SETUP", "async")
    d.msg("round", "bg", "destroyed / hit: show bang[i]")
    d.msg("game", "bg", "BANG_UPDATE", "async")
    d.self_("bg", "frame 1 -> 2 -> 3, then hide")
    d.msg("game", "bg", "BANG_RESET", "async")
    d.msg("game", "st", "STARFIELD_RESET", "async")
    return d


DIAGRAMS = {
    "sc_game_sequence_overview": overview,
    "runtime_01_boot_menu": rt_boot_menu,
    "runtime_02_round_start": rt_round_start,
    "runtime_03_game_tick": rt_tick,
    "runtime_04_round_end": rt_round_end,
    "runtime_05_buttons": rt_buttons,
    "object_01_spacecraft": ob_spacecraft,
    "object_02_arrow": ob_arrow,
    "object_03_enemy_round1": ob_enemy_r1,
    "object_04_enemy_round23": ob_enemy_r23,
    "object_05_arrow_enemy": ob_arrow_enemy,
    "object_06_meteoroid": ob_meteoroid,
    "object_07_gift": ob_gift,
    "object_08_boss": ob_boss,
    "object_09_boss_skill": ob_boss_skill,
    "object_10_bang_starfield": ob_bang_starfield,
}


def main():
    os.makedirs(OUT, exist_ok=True)
    for name, build in DIAGRAMS.items():
        build().save(os.path.join(OUT, f"{name}.drawio.svg"))
    print(f"{len(DIAGRAMS)} diagrams written to {OUT}")


if __name__ == "__main__":
    main()
