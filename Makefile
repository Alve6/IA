src := src/main.cpp src/game.cpp

run: game
	./game

game: $(src)
	g++ $(src) -lraylib -Iinclude -o game

clean:
	rm game