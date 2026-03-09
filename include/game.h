#pragma once

#include <vector>

struct Cell {
    int x;
    int y;
};

struct Wall {
    int x;
    int y;
    bool horizontal;
};

bool SameCell(const Cell &a, const Cell &b);

bool SameCellPlayer(const Cell player, const std::vector<Cell> &otherPlayer);

bool HasWall(const std::vector<Wall> &walls, int x, int y, bool horizontal);

bool IsBlockedByWall(const Cell &from, int dx, int dy, const std::vector<Wall> &walls);

Cell SlideMove(Cell start, int dx, int dy, int cols, int rows, const std::vector<Wall> &walls, const std::vector<Cell> otherPlayer);
