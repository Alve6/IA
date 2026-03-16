#pragma once

#include "game.h"

#include <vector>

struct Action {
    RobotType robot;
    Direction dir;
};

// Guarantees to give the shortest solution, but may take a long while to compute
std::vector<Action> solveBFS(const GameState &initState, const GameBoard &board);
// Gives solution quickly, but it will probably be extremely inefficient (e.g. 615 moves)
std::vector<Action> solveDFS(const GameState &initState, const GameBoard &board);
//
std::vector<Action> solveIDS(const GameState &initState, const GameBoard &board);