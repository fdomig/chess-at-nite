# This file is part of the chess-at-nite project [chess-at-nite.googlecode.com]
#
# Copyright (c) 2009-2010 by
#   Franziskus Domig
#   Panayiotis Lipiridis
#   Radoslav Petrik
#   Thai Gia Tuong
#
# For the full copyright and license information, please visit:
#   http://chess-at-nite.googlecode.com/svn/trunk/doc/LICENSE

CC=g++
#to use it:
#  $ make mode=debug
ifeq ($(mode),debug)
    CFLAGS=-O0 -g -c -Wall -fmessage-length=0 -DDEBUG
else
    CFLAGS=-O3 -c -Wall -fmessage-length=0
endif

LDFLAGS=
RM=rm -rf

PRE=/usr/local
BIN=$(PRE)/bin
TEMP_BIN=bin

SRC_DIR=src
COMMON_SOURCES=$(SRC_DIR)/common/utils.cpp $(SRC_DIR)/common/extra_utils.cpp
CONTROL_SOURCES=$(SRC_DIR)/control/CLI.cpp $(SRC_DIR)/control/PGN.cpp $(SRC_DIR)/control/XBoard.cpp
MODEL_SOURCES=$(SRC_DIR)/model/Board.cpp $(SRC_DIR)/model/evaluate.cpp $(SRC_DIR)/model/Game.cpp $(SRC_DIR)/model/MoveGenerator.cpp $(SRC_DIR)/model/OpeningBook.cpp
PLAYER_SOURCES=$(SRC_DIR)/player/ComputerPlayer.cpp $(SRC_DIR)/player/HumanPlayer.cpp $(SRC_DIR)/player/Player.cpp
SOURCES=$(SRC_DIR)/chess.cpp $(COMMON_SOURCES) $(CONTROL_SOURCES) $(MODEL_SOURCES) $(PLAYER_SOURCES)

OBJECTS=$(SOURCES:.cpp=.o)

ifeq ($(mode),debug)
   EXECUTABLE=$(TEMP_BIN)/chess-at-nite-debug
else
   EXECUTABLE=$(TEMP_BIN)/chess-at-nite
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(SRC_DIR)/*.o $(SRC_DIR)/*/*.o $(EXECUTABLE)

install:
	mkdir -p $(BIN)
	cp -f $(EXECUTABLE) $(BIN)

uninstall:
	$(RM) $(BIN)/$(EXECUTABLE)