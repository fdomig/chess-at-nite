# Makefile for the chess project
# written by Franziskus Domig

CC=g++
CFLAGS=-O3 -c -Wall -fmessage-length=0
LDFLAGS=
RM=rm -rf

PRE=/usr/local
BIN=$(PRE)/bin
TEMP_BIN=bin

SRC_DIR=src
COMMON_SOURCES=$(SRC_DIR)/common/utils.cpp
CONTROL_SOURCES=$(SRC_DIR)/control/CLI.cpp
MODEL_SOURCES=$(SRC_DIR)/model/Board.cpp $(SRC_DIR)/model/evaluate.cpp $(SRC_DIR)/model/Game.cpp $(SRC_DIR)/model/MoveGenerator.cpp $(SRC_DIR)/model/OpeningBook.cpp
PLAYER_SOURCES=$(SRC_DIR)/player/ComputerPlayer.cpp $(SRC_DIR)/player/HumanPlayer.cpp $(SRC_DIR)/player/Player.cpp
SOURCES=$(SRC_DIR)/chess.cpp $(COMMON_SOURCES) $(CONTROL_SOURCES) $(MODEL_SOURCES) $(PLAYER_SOURCES)

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(TEMP_BIN)/chess-at-nite

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