#include "game.h"

#include <cstring>

bool iVector2::operator==(const iVector2 &other) const {
    return x == other.x && y == other.y;
}

bool GameState::operator==(const GameState &other) const {
    return  bluePos     ==  other.bluePos   &&
            redPos      ==  other.redPos    &&
            greenPos    ==  other.greenPos  && 
            orangePos   ==  other.orangePos ;
}

bool GameState::operator!=(const GameState &other) const {
    return !(*this == other);
}

GameBoard::GameBoard(int width, int height) {
    board = (char*)malloc(sizeof(char) * (width * height));
    if (!board) {
        std::cerr << "Failed to allocate memory for the board\n";
        exit(EXIT_FAILURE);
    }
    memset(board, 0, width * height);
    this->width = width;
    this->height = height;
}

GameBoard::~GameBoard() {
    free(board);
}

char GameBoard::getTile(iVector2 pos) const {
    return board[pos.y * this->width + pos.x];
}

void GameBoard::setTile(iVector2 pos, char flags) {
    board[pos.y * this->width + pos.x] = flags;
}

void GameBoard::addTileFlag(iVector2 pos, TileFlag flag) {
    board[pos.y * this->width + pos.x] |= flag;
}

void GameBoard::removeTileFlag(iVector2 pos, TileFlag flag) {
    board[pos.y * this->width + pos.x] &= ~flag;
}

bool GameBoard::checkFlag(iVector2 pos, TileFlag flag) const {
    return board[pos.y * this->width + pos.x] & flag;
}

GameState slideMove(RobotType robotType, Direction dir, const GameState &state, const GameBoard &board) {  
    iVector2 pos;
    switch (robotType) {
        case ROBOT_BLUE:
        pos = state.bluePos;
        break;
        case ROBOT_RED:
        pos = state.redPos;
        break;
        case ROBOT_GREEN:
        pos = state.greenPos;
        break;
        case ROBOT_ORANGE:
        pos = state.orangePos;
        break;
    }

    bool blocked;
    iVector2 newPos;
    bool done = false;
    while (!done) {
        switch (dir) {
            ////////////////////////////////////////////////////////////////////////////////////////////
            case DIR_NORTH:
            if (pos.y == 0) {
                done = true;
                break;
            }
            newPos = {pos.x, pos.y - 1};
            blocked = board.checkFlag(pos, TILE_WALL_NORTH) || board.checkFlag(newPos, TILE_WALL_SOUTH);
            if (blocked) {
                done = true;
                break;
            }
            break;
            ////////////////////////////////////////////////////////////////////////////////////////////
            case DIR_EAST:
            if (pos.x == board.width - 1) {
                done = true;
                break;
            }
            newPos = {pos.x + 1, pos.y};
            blocked = board.checkFlag(pos, TILE_WALL_EAST) || board.checkFlag(newPos, TILE_WALL_WEST);
            if (blocked) {
                done = true;
                break;
            }
            break;
            ////////////////////////////////////////////////////////////////////////////////////////////
            case DIR_SOUTH:
            if (pos.y == board.height - 1) {
                done = true;
                break;
            }
            newPos = {pos.x, pos.y + 1};
            blocked = board.checkFlag(pos, TILE_WALL_SOUTH) || board.checkFlag(newPos, TILE_WALL_NORTH);
            if (blocked) {
                done = true;
                break;
            }
            break;
            ////////////////////////////////////////////////////////////////////////////////////////////
            case DIR_WEST:
            if (pos.x == 0) {
                done = true;
                break;
            }
            newPos = {pos.x - 1, pos.y};
            blocked = board.checkFlag(pos, TILE_WALL_WEST) || board.checkFlag(newPos, TILE_WALL_EAST);
            if (blocked) {
                done = true;
                break;
            }
            break;
            default:
            std::cerr << "INVALID DIRECTION IN slideMove\n";
            break;
        }

        if (newPos == state.bluePos ||
            newPos == state.redPos ||
            newPos == state.greenPos || 
            newPos == state.orangePos)
            break;
        if (!blocked)
            pos = newPos;
    }
    GameState newState = state;
    switch (robotType) {
        case ROBOT_BLUE:
        newState.bluePos = pos;
        break;
        case ROBOT_RED:
        newState.redPos = pos;
        break;
        case ROBOT_GREEN:
        newState.greenPos = pos;
        break;
        case ROBOT_ORANGE:
        newState.orangePos = pos;
        break;
    }
    return newState;
}

// bool SameCell(const Cell &a, const Cell &b) {
//     return a.x == b.x && a.y == b.y;
// }


// bool HasWall(const std::vector<Wall> &walls, int x, int y, bool horizontal) {
//     for (const Wall &wall : walls) {
//         if (wall.x == x && wall.y == y && wall.horizontal == horizontal) {
//             return true;
//         }
//     }
//     return false;
// }

// bool IsBlockedByWall(const Cell &from, int dx, int dy, const std::vector<Wall> &walls) {
//     if (dx == 1) {
//         return HasWall(walls, from.x + 1, from.y, false);
//     }
//     if (dx == -1) {
//         return HasWall(walls, from.x, from.y, false);
//     }
//     if (dy == 1) {
//         return HasWall(walls, from.x, from.y + 1, true);
//     }
//     if (dy == -1) {
//         return HasWall(walls, from.x, from.y, true);
//     }
//     return false;
// }


// bool SameCellPlayer(const Cell player, const std::vector<Cell> &otherPlayer) {
//     for (int i = 0; i < (int)otherPlayer.size(); i++) {
//         if (player.x==otherPlayer[i].x && player.y == otherPlayer[i].y) return true;
//     }
//     return false;
// }

// Cell SlideMove(Cell start, int dx, int dy, int cols, int rows, const std::vector<Wall> &walls, const std::vector<Cell> otherPlayer) {
//     Cell current = start;
//     while (true) {
//         Cell next = {current.x + dx, current.y + dy};
//         bool outside = next.x < 0 || next.x >= cols || next.y < 0 || next.y >= rows;
//         bool blockedByObstacle = IsBlockedByWall(current, dx, dy, walls);
//         bool blockedByPlayer = SameCellPlayer(next, otherPlayer);
//         if (outside || blockedByObstacle || blockedByPlayer) {
//             break;
//         }
//         current = next;
//     }
//     return current;
// }
