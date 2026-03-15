#pragma once

#include "game.h"

#include <vector>

struct Action {
    RobotType robot;
    Direction dir;
};

std::vector<Action> solveBFS(const GameState &initState, const GameBoard &board);