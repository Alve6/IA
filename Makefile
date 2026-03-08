src := src/main.cpp src/game.cpp

run: game
	./game

game:
	g++ $(src) -lraylib -Iinclude -o game
