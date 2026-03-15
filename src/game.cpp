#include "game.h"

#include <cstring>

std::string directionToString(Direction dir) {
    switch(dir) {
        case DIR_NORTH:
        return "DIR_NORTH";
        case DIR_EAST:
        return "DIR_EAST";
        case DIR_SOUTH:
        return "DIR_SOUTH";
        case DIR_WEST:
        return "DIR_WEST";
        case DIR_INVALID:
        return "DIR_INVALID";
    }
}

std::string robotTypeToString(RobotType robot) {
    switch(robot) {
        case ROBOT_BLUE:
        return "ROBOT_BLUE";
        case ROBOT_RED:
        return "ROBOT_RED";
        case ROBOT_GREEN:
        return "ROBOT_GREEN";
        case ROBOT_ORANGE:
        return "ROBOT_ORANGE";
    }
}
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

bool isWinningState(const GameState &state, const GameBoard &board) {
    
    // std::cout << state.bluePos.x << ' ' << state.bluePos.y << ',' <<state.redPos.x << ' ' << state.redPos.y << ',' <<
    // state.greenPos.x << ' ' << state.greenPos.y << ',' <<state.orangePos.x << ' ' << state.orangePos.y << '\n';
    return board.checkFlag(state.bluePos, TILE_GOAL) ||
            board.checkFlag(state.redPos, TILE_GOAL) ||
            board.checkFlag(state.greenPos, TILE_GOAL) ||
            board.checkFlag(state.orangePos, TILE_GOAL);
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
    iVector2 newPos = {-1, -1};
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
            
        if (!done)
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
