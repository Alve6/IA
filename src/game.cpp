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
