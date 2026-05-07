#include "levels.h"

LevelData loadLevel(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file\n";
        exit(EXIT_FAILURE);
    }
    int width, height;
    if (!(file >> width && file >> height)) {
        std::cerr << "Failed to read board size\n";
        exit(EXIT_FAILURE);
    }

    GameState state;
    GameBoard board(width, height);
    
    if (!(file >> state.bluePos.x && file >> state.bluePos.y)) {
        std::cerr << "Failed to read blue position\n";
        exit(EXIT_FAILURE);
    }
    if (!(file >> state.redPos.x && file >> state.redPos.y)) {
        std::cerr << "Failed to read red position\n";
        exit(EXIT_FAILURE);
    }
    if (!(file >> state.greenPos.x && file >> state.greenPos.y)) {
        std::cerr << "Failed to read green position\n";
        exit(EXIT_FAILURE);
    }
    if (!(file >> state.orangePos.x && file >> state.orangePos.y)) {
        std::cerr << "Failed to read orange position\n";
        exit(EXIT_FAILURE);
    }
    std::string line;
    // skip the last \n
    std::getline(file, line);
    for (int y = 0; y < board.height; y++) {
        if (!std::getline(file, line)) {
            std::cerr << "Failed to read board data (row " << y << ")\n";
            exit(EXIT_FAILURE);
        }
        if (line.length() != board.width) {
            std::cerr << "Incorrect amount of characters in board data (row " << y << ")\n";
            exit(EXIT_FAILURE);
        }
        for (int x = 0; x < board.width; x++) {
            switch(line[x]) {
                case 'N':
                board.setTile({x, y}, 0);
                break;
                case 'S':
                board.setTile({x, y}, TILE_WALL_SOUTH);
                break;
                case 'E':
                board.setTile({x, y}, TILE_WALL_EAST);
                break;
                case 'L':
                board.setTile({x, y}, TILE_WALL_SOUTH | TILE_WALL_EAST);
                break;
                case 'G':
                board.setTile({x, y}, TILE_GOAL);
                break;
                case 'T':
                board.setTile({x, y}, TILE_GOAL | TILE_WALL_SOUTH);
                break;
                case 'D':
                board.setTile({x, y}, TILE_GOAL | TILE_WALL_EAST);
                break;
                case 'W':
                board.setTile({x, y}, TILE_GOAL | TILE_WALL_SOUTH | TILE_WALL_EAST);
                break;
                default:
                std::cerr << "Invalid character '" << line[x] << "' in board data\n";
                exit(EXIT_FAILURE);
            }
        }
    }
    return {state, board};
}