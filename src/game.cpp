#include "game.h"



bool SameCell(const Cell &a, const Cell &b) {
    return a.x == b.x && a.y == b.y;
}


bool HasWall(const std::vector<Wall> &walls, int x, int y, bool horizontal) {
    for (const Wall &wall : walls) {
        if (wall.x == x && wall.y == y && wall.horizontal == horizontal) {
            return true;
        }
    }
    return false;
}

bool IsBlockedByWall(const Cell &from, int dx, int dy, const std::vector<Wall> &walls) {
    if (dx == 1) {
        return HasWall(walls, from.x + 1, from.y, false);
    }
    if (dx == -1) {
        return HasWall(walls, from.x, from.y, false);
    }
    if (dy == 1) {
        return HasWall(walls, from.x, from.y + 1, true);
    }
    if (dy == -1) {
        return HasWall(walls, from.x, from.y, true);
    }
    return false;
}


bool SameCellPlayer(const Cell player, const std::vector<Cell> &otherPlayer) {
    for (int i = 0; i < (int)otherPlayer.size(); i++) {
        if (player.x==otherPlayer[i].x && player.y == otherPlayer[i].y) return true;
    }
    return false;
}

Cell SlideMove(Cell start, int dx, int dy, int cols, int rows, const std::vector<Wall> &walls, const std::vector<Cell> otherPlayer) {
    Cell current = start;
    while (true) {
        Cell next = {current.x + dx, current.y + dy};
        bool outside = next.x < 0 || next.x >= cols || next.y < 0 || next.y >= rows;
        bool blockedByObstacle = IsBlockedByWall(current, dx, dy, walls);
        bool blockedByPlayer = SameCellPlayer(next, otherPlayer);
        if (outside || blockedByObstacle || blockedByPlayer) {
            break;
        }
        current = next;
    }
    return current;
}
