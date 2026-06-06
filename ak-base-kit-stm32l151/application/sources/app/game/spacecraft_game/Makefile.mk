CFLAGS		+= -I./sources/app/game/spacecraft_game
CPPFLAGS	+= -I./sources/app/game/spacecraft_game

VPATH += sources/app/game/spacecraft_game

# CPP source files 
# Archery game 
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_spacecraft.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_arrow.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_bang.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_starfield.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_meteoroid.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_enemy.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_arrow_enemy.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_gift.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_boss.cpp
SOURCES_CPP += sources/app/game/spacecraft_game/sc_game_boss_skill.cpp