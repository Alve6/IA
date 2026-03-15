src := src/main.cpp src/game.cpp src/solver.cpp

run: game
	./game

game: $(src)
	g++ $(src) -g -lraylib -Iinclude -o game

clean:
	rm game