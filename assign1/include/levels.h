#pragma once

#include "game.h"

#include <string>
#include <iostream>
#include <fstream>

// N - nothing
// S - south
// E - east
// L - south and east

// G - just goal
// T - south and goal
// D - east and goal
// W - south, east, and goal

struct LevelData {
    GameState state;
    GameBoard board;
};
LevelData loadLevel(const std::string &filepath);