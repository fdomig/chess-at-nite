# Makefile for the chess project
# written by Franziskus Domig

CC=g++
CFLAGS=-O3 -c -Wall -fmessage-length=0
LDFLAGS=
RM=rm -rf

COMMON_SOURCES=common/utils.cpp
CONTROL_SOURCES=control/CLI.cpp
MODEL_SOURCES=model/Board.cpp model/evaluate.cpp model/Game.cpp model/MoveGenerator.cpp model/OpeningBook.cpp
PLAYER_SOURCES=player/ComputerPlayer.cpp player/HumanPlayer.cpp player/Player.cpp
SOURCES=chess.cpp $(COMMON_SOURCES) $(CONTROL_SOURCES) $(MODEL_SOURCES) $(PLAYER_SOURCES)

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=chess-at-nite

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o */*.o $(EXECUTABLE)
