SFML_PREFIX := $(shell brew --prefix sfml 2>/dev/null)

CXX      := g++
CXXFLAGS := -std=c++17 -I src
LDFLAGS  := -lsfml-graphics -lsfml-window -lsfml-system

ifneq ($(SFML_PREFIX),)
CXXFLAGS += -I $(SFML_PREFIX)/include
LDFLAGS  += -L $(SFML_PREFIX)/lib
endif

SRC := src/main.cpp src/Chess/chessBoard.cpp src/move/move-generator.cpp
BIN := chess

.PHONY: all run clean

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)
