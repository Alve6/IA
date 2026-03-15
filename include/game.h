#pragma once

#include <vector>
#include <stdlib.h>
#include <iostream>

enum Direction {
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_INVALID
};

std::string directionToString(Direction dir);

enum RobotType {
    ROBOT_BLUE,
    ROBOT_RED,
    ROBOT_GREEN,
    ROBOT_ORANGE
};
std::string robotTypeToString(RobotType robot);

enum TileFlag {
    TILE_WALL_NORTH = (1 << 0),
    TILE_WALL_EAST  = (1 << 1),
    TILE_WALL_SOUTH = (1 << 2),
    TILE_WALL_WEST  = (1 << 3),
    TILE_GOAL       = (1 << 4)
};

struct iVector2 {
    int x;
    int y;
    bool operator==(const iVector2 &other) const;
};

struct GameState {
    iVector2 bluePos;
    iVector2 redPos;
    iVector2 greenPos;
    iVector2 orangePos;
    bool operator==(const GameState &other) const;
    bool operator!=(const GameState &other) const;
};

struct GameBoard {
    char* board;
    int width;
    int height;
    
    GameBoard(int width, int height);

    ~GameBoard();

    // Get all flags of a specific tile
    char getTile(iVector2 pos) const;
    // Overwrite entire tile with a set of flags
    void setTile(iVector2 pos, char flags);

    // Add a single flag to a tile (e.g. one wall or goal)
    void addTileFlag(iVector2 pos, TileFlag flag);
    // Remove a single flag from a tile
    void removeTileFlag(iVector2 pos, TileFlag flag);

    // Check if a specific flag is present on a tile
    bool checkFlag(iVector2 pos, TileFlag flag) const;
};

bool isWinningState(const GameState &state, const GameBoard &board);

// Attempts to move a specific robot in a specific direction
// Returns the new GameState (may be the same as initial, if movement is not possible)
GameState slideMove(RobotType robotType, Direction dir, const GameState &state, const GameBoard &board);
