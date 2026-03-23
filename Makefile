SRC := src/main.cpp src/game.cpp src/solver.cpp
RAYLIB_PREFIX := $(HOME)/.local

CXX := g++
CXXFLAGS := -g -Iinclude -I$(RAYLIB_PREFIX)/include
LDFLAGS := -L$(RAYLIB_PREFIX)/lib
LDLIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

run: game
	./game

game: $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o game

clean:
	rm -f game