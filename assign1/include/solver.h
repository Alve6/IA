#pragma once

#include "game.h"

#include <vector>
#include <string>

struct Action {
    RobotType robot;
    Direction dir;
};

struct SolverResult {
    std::vector<Action> actions;
    bool solved = false;

    int generatedStates = 0;
    int expandedStates = 0;
    int repeatedStates = 0;

    int solutionCost = 0;
    int solutionDepth = 0;

    size_t visitedStates = 0;
    size_t maxFrontierSize = 0;
    size_t approxMemoryBytes = 0;

    double elapsedMs = 0.0;
    std::string algorithmName = "";
};

std::string actionToString(const Action &action);

SolverResult solveBFS(const GameState &initState, const GameBoard &board);
SolverResult solveDFS(const GameState &initState, const GameBoard &board);
SolverResult solveIDS(const GameState &initState, const GameBoard &board);
SolverResult solveGreedy1(const GameState &initState, const GameBoard &board);
SolverResult solveGreedy2(const GameState &initState, const GameBoard &board);
SolverResult solveA(const GameState &initState, const GameBoard &board);
SolverResult solveWeightedA(const GameState &initState, const GameBoard &board);