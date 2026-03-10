#include "game.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <queue>


// std::vector<Cell> breadth_first_search(Cell initial_state, Cell goal_state, int cols, int rows, const std::vector<Wall> &walls, const std::vector<Cell> otherPlayer) {
//     std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
//     std::vector<std::vector<Cell>> parent(rows, std::vector<Cell>(cols, {-1, -1}));
//     std::queue<Cell> q;

//     visited[initial_state.y][initial_state.x] = true;
//     q.push(initial_state);

//     const int dirs[4][2] = {
//         {0, -1},
//         {-1, 0},
//         {0, 1},
//         {1, 0}
//     };
//     while (!q.empty()) {
//         Cell current = q.front();
//         q.pop();

//         if (SameCell(current, goal_state)) {
//             break;
//         }

//         for (int i = 0; i < 4; i++) {
//             Cell next = SlideMove(current, dirs[i][0], dirs[i][1], cols, rows, walls, otherPlayer);
//             if (SameCell(next, current)) {
//                 continue;
//             }
//             if (!visited[next.y][next.x]) {
//                 visited[next.y][next.x] = true;
//                 parent[next.y][next.x] = current;
//                 q.push(next);
//             }
//         }
//     }

//     if (!visited[goal_state.y][goal_state.x]) {
//         return {};
//     }

//     std::vector<Cell> path;
//     Cell cur = goal_state;
//     path.push_back(cur);
//     while (!SameCell(cur, initial_state)) {
//         Cell prev = parent[cur.y][cur.x];
//         if (prev.x == -1 && prev.y == -1) {
//             return {};
//         }
//         cur = prev;
//         path.push_back(cur);
//     }

//     std::reverse(path.begin(), path.end());
//     return path;
// }

int main() {
    const int screenWidth = 900;
    const int screenHeight = 720;
    const int cellSize = 48;
    const int cols = 8;
    const int rows = 8;
    const int gridX = (screenWidth - cols * cellSize) / 2;
    const int gridY = 90;

    // std::vector<Wall> walls;
    // auto addWall = [&](int x, int y, bool horizontal) {
    //     if (!HasWall(walls, x, y, horizontal)) {
    //         walls.push_back({x, y, horizontal});
    //     }
    // };

    // auto addCellWall = [&](int cellNumber, char side) {
    //     int index = cellNumber - 1;
    //     int x = index % cols;
    //     int y = index / cols;

    //     if (side == 'R') addWall(x + 1, y, false);
    //     if (side == 'L') addWall(x, y, false);
    //     if (side == 'T') addWall(x, y, true);
    //     if (side == 'B') addWall(x, y + 1, true);
    // };

    // addCellWall(5, 'R');
    // addCellWall(12, 'R');
    // addCellWall(12, 'B');
    // addCellWall(15, 'L');
    // addCellWall(15, 'B');
    // addCellWall(26, 'L');
    // addCellWall(26, 'B');
    // addCellWall(37, 'L');
    // addCellWall(37, 'T');
    // addCellWall(39, 'R');
    // addCellWall(39, 'T');
    // addCellWall(50, 'T');
    // addCellWall(50, 'R');
    // addCellWall(54, 'B');
    // addCellWall(54, 'R');
    // addCellWall(56, 'T');
    // addCellWall(59, 'R');

    InitWindow(screenWidth, screenHeight, "Ricochet");
    SetTargetFPS(60);

    enum ScreenState { MENU, PLAYING };
    ScreenState state = MENU;

    Rectangle playButton = {(float)(screenWidth / 2 - 100), 340.0f, 200.0f, 60.0f};
    Rectangle hintButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY + 20), 120.0f, 42.0f};

    // Cell p1 = {0, 0};
    // Cell p2 = {0,7};
    // Cell p3 = {7,0};
    // Cell p4 = {7,7};
    // Cell coin = {5, 6};
    int score = 0;
    // int selectedPlayer = 0;
    // Cell hintNext = {-1, -1};
    // bool hasHint = false;

    // auto respawnCoin = [&]() {
    //     do {
    //         coin = {GetRandomValue(0, cols - 1), GetRandomValue(0, rows - 1)};
    //     } while (SameCell(coin, p1) || SameCell(coin, p2) || SameCell(coin, p3) || SameCell(coin, p4));
    // };
    
    GameBoard gameBoard(cols, rows);
    GameState gameState;
    gameState.bluePos = {0,0};
    gameState.redPos = {7,0};
    gameState.greenPos = {4,0};
    gameState.orangePos = {3,0};
    gameBoard.addTileFlag({3,0}, TILE_WALL_EAST);
    gameBoard.addTileFlag({5,0}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({0,1}, TILE_WALL_EAST);
    gameBoard.addTileFlag({3,1}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({6,1}, TILE_WALL_EAST);
    gameBoard.addTileFlag({0,2}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({1,2}, TILE_WALL_EAST);
    gameBoard.addTileFlag({4,2}, TILE_WALL_EAST);
    gameBoard.addTileFlag({7,2}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({2,3}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({3,3}, TILE_WALL_EAST);
    gameBoard.addTileFlag({5,3}, TILE_WALL_EAST);
    gameBoard.addTileFlag({1,4}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({2,4}, TILE_GOAL);
    gameBoard.addTileFlag({3,4}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({4,4}, TILE_WALL_EAST);
    gameBoard.addTileFlag({6,4}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({2,5}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({5,5}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({7,5}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({3,6}, TILE_WALL_SOUTH);
    gameBoard.addTileFlag({6,6}, TILE_WALL_SOUTH);

    RobotType selectedRobot = ROBOT_BLUE;

    while (!WindowShouldClose()) {
        if (state == MENU) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                if (CheckCollisionPointRec(mouse, playButton)) {
                    state = PLAYING;
                }
            }
        } else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                if (CheckCollisionPointRec(mouse, hintButton)) {
                    // Cell current = (selectedPlayer == 0) ? p1 : (selectedPlayer == 1) ? p2 : (selectedPlayer == 2) ? p3 : p4;
                    // std::vector<Cell> localBlockers;
                    // if (selectedPlayer != 0) localBlockers.push_back(p1);
                    // if (selectedPlayer != 1) localBlockers.push_back(p2);
                    // if (selectedPlayer != 2) localBlockers.push_back(p3);
                    // if (selectedPlayer != 3) localBlockers.push_back(p4);
                    // std::vector<Cell> path = breadth_first_search(current, coin, cols, rows, walls, localBlockers);

                    // if (path.size() >= 2) {
                    //     hintNext = path[1];
                    //     hasHint = true;
                    // } else {
                    //     hasHint = false;
                    // }
                } else {
                    // Calculate on-screen positions of robots
                    int bluex = gridX + gameState.bluePos.x * cellSize + cellSize / 2;
                    int bluey = gridY + gameState.bluePos.y * cellSize + cellSize / 2;
                    int redx = gridX + gameState.redPos.x * cellSize + cellSize / 2;
                    int redy = gridY + gameState.redPos.y * cellSize + cellSize / 2;
                    int greenx = gridX + gameState.greenPos.x * cellSize + cellSize / 2;
                    int greeny = gridY + gameState.greenPos.y * cellSize + cellSize / 2;
                    int orangex = gridX + gameState.orangePos.x * cellSize + cellSize / 2;
                    int orangey = gridY + gameState.orangePos.y * cellSize + cellSize / 2;
                    // int p1x = gridX + p1.x * cellSize + cellSize / 2;
                    // int p1y = gridY + p1.y * cellSize + cellSize / 2;
                    // int p2x = gridX + p2.x * cellSize + cellSize / 2;
                    // int p2y = gridY + p2.y * cellSize + cellSize / 2;
                    // int p3x = gridX + p3.x * cellSize + cellSize / 2;
                    // int p3y = gridY + p3.y * cellSize + cellSize / 2;
                    // int p4x = gridX + p4.x * cellSize + cellSize / 2;
                    // int p4y = gridY + p4.y * cellSize + cellSize / 2;

                    if (CheckCollisionPointCircle(mouse, {(float)bluex, (float)bluey}, 16.0f)) {
                        selectedRobot = ROBOT_BLUE;
                    } else if (CheckCollisionPointCircle(mouse, {(float)redx, (float)redy}, 16.0f)) {
                        selectedRobot = ROBOT_RED;
                    } else if (CheckCollisionPointCircle(mouse, {(float)greenx, (float)greeny}, 16.0f)) {
                        selectedRobot = ROBOT_GREEN;
                    } else if (CheckCollisionPointCircle(mouse, {(float)orangex, (float)orangey}, 16.0f)) {
                        selectedRobot = ROBOT_ORANGE;
                    } else {
                        bool insideGrid = mouse.x >= gridX && mouse.x < gridX + cols * cellSize &&
                                          mouse.y >= gridY && mouse.y < gridY + rows * cellSize;
                        if (insideGrid) {
                            int clickedX = (int)((mouse.x - gridX) / cellSize);
                            int clickedY = (int)((mouse.y - gridY) / cellSize);

                            // Cell *currentRef = (selectedPlayer == 0) ? &p1 : (selectedPlayer == 1) ? &p2 : (selectedPlayer == 2) ? &p3 : &p4;
                            iVector2 selectedPos;
                            switch (selectedRobot) {
                                case ROBOT_BLUE:
                                selectedPos = gameState.bluePos;
                                break;
                                case ROBOT_RED:
                                selectedPos = gameState.redPos;
                                break;
                                case ROBOT_GREEN:
                                selectedPos = gameState.greenPos;
                                break;
                                case ROBOT_ORANGE:
                                selectedPos = gameState.orangePos;
                                break;
                            }
                            // int dx = 0;
                            // int dy = 0;
                            Direction dir = DIR_INVALID;
                            if (clickedX == selectedPos.x && clickedY < selectedPos.y) {
                                dir = DIR_NORTH;
                            } else if (clickedX == selectedPos.x && clickedY > selectedPos.y) {
                                dir = DIR_SOUTH;
                            } else if (clickedY == selectedPos.y && clickedX < selectedPos.x) {
                                dir = DIR_WEST;
                            } else if (clickedY == selectedPos.y && clickedX > selectedPos.x) {
                                dir = DIR_EAST;
                            }
                            if (dir != DIR_INVALID) {
                                GameState newState = slideMove(selectedRobot, dir, gameState, gameBoard);
                                if (newState != gameState) {
                                    gameState = newState;
                                    score++;
                                }
                            }
                            // if (dx != 0 || dy != 0) {
                            //     std::vector<Cell> localBlockers;
                            //     if (selectedPlayer != 0) localBlockers.push_back(p1);
                            //     if (selectedPlayer != 1) localBlockers.push_back(p2);
                            //     if (selectedPlayer != 2) localBlockers.push_back(p3);
                            //     if (selectedPlayer != 3) localBlockers.push_back(p4);

                            //     Cell next = SlideMove(*currentRef, dx, dy, cols, rows, walls, localBlockers);
                            //     if (!SameCell(next, *currentRef)) {
                            //         *currentRef = next;
                            //         score++;
                            //     }
                            // }
                        }
                    }
                }
            }

            // if (SameCell(p1, coin) || SameCell(p2, coin) || SameCell(p3, coin) || SameCell(p4, coin)) {
            //     score = 0;
            //     respawnCoin();
            // }

            // if (IsKeyPressed(KEY_R)) {
            //     p1 = {0, 0};
            //     coin = {cols-1, 0};
            //     score = 0;
            //     selectedPlayer = 0;
            //     hintNext = {-1, -1};
            //     hasHint = false;
            // }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (state == MENU) {
            DrawText("Ricochet", screenWidth / 2 - 110, 180, 56, BLACK);
            DrawRectangleRec(playButton, RED);
            DrawText("Play", screenWidth / 2 - 30, 357, 28, WHITE);
            DrawText("Click piece, then click direction on grid", screenWidth / 2 - 190, 450, 22, BLACK);
        } else {
            DrawText("Ricochet", gridX, 20, 36, BLACK);
            std::string scoreText = "Score: "+std::to_string(score);
            DrawText(scoreText.c_str(), gridX + 260, 28, 28, BLACK);
            DrawText("R to reset", gridX + 500, 34, 20, DARKGRAY);

            for (int y = 0; y < rows; y++) {
                for (int x = 0; x < cols; x++) {
                    int px = gridX + x * cellSize;
                    int py = gridY + y * cellSize;
                    DrawRectangleLines(px, py, cellSize, cellSize, LIGHTGRAY);
                }
            }

            // for (const Wall &wall : walls) {
            //     int x1 = gridX + wall.x * cellSize;
            //     int y1 = gridY + wall.y * cellSize;
            //     if (wall.horizontal) {
            //         DrawRectangle(x1, y1 - 2, cellSize, 4, DARKGRAY);
            //     } else {
            //         DrawRectangle(x1 - 2, y1, 4, cellSize, DARKGRAY);
            //     }
            // }
            for (int y = 0; y < gameBoard.height; y++) {
                for (int x = 0; x < gameBoard.width; x++) {
                    // Coordinates of top-left position of the cell (in pixels)
                    iVector2 cellTopLeft = {gridX + x*cellSize, gridY + y*cellSize};
                    char flags = gameBoard.getTile({x, y});
                    if (flags & TILE_WALL_NORTH) {
                        DrawRectangle(cellTopLeft.x, cellTopLeft.y, cellSize, 4, DARKGRAY);
                    }
                    if (flags & TILE_WALL_EAST) {
                        DrawRectangle(cellTopLeft.x + cellSize - 4, cellTopLeft.y, 4, cellSize, DARKGRAY);
                    }
                    if (flags & TILE_WALL_SOUTH) {
                        DrawRectangle(cellTopLeft.x, cellTopLeft.y + cellSize - 4, cellSize, 4, DARKGRAY);
                    }
                    if (flags & TILE_WALL_WEST) {
                        DrawRectangle(cellTopLeft.x + 4, cellTopLeft.y, 4, cellSize, DARKGRAY);
                    }
                    if (flags & TILE_GOAL) {
                        DrawCircle(cellTopLeft.x + cellSize / 2, cellTopLeft.y + cellSize / 2, cellSize / 4, MAGENTA);
                    }
                }
            }
            // int coinX = gridX + coin.x * cellSize + cellSize / 2;
            // int coinY = gridY + coin.y * cellSize + cellSize / 2;
            // DrawCircle(coinX, coinY, 12, ORANGE);

            int bluex = gridX + gameState.bluePos.x * cellSize + cellSize / 2;
            int bluey = gridY + gameState.bluePos.y * cellSize + cellSize / 2;
            int redx = gridX + gameState.redPos.x * cellSize + cellSize / 2;
            int redy = gridY + gameState.redPos.y * cellSize + cellSize / 2;
            int greenx = gridX + gameState.greenPos.x * cellSize + cellSize / 2;
            int greeny = gridY + gameState.greenPos.y * cellSize + cellSize / 2;
            int orangex = gridX + gameState.orangePos.x * cellSize + cellSize / 2;
            int orangey = gridY + gameState.orangePos.y * cellSize + cellSize / 2;
            // int p1x = gridX + p1.x * cellSize + cellSize / 2;
            // int p1y = gridY + p1.y * cellSize + cellSize / 2;
            DrawCircle(bluex, bluey, 16, BLUE);
            
            // int p2x = gridX + p2.x * cellSize + cellSize / 2;
            // int p2y = gridY + p2.y * cellSize + cellSize / 2;
            DrawCircle(redx, redy, 16, RED);

            // int p3x = gridX + p3.x * cellSize + cellSize / 2;
            // int p3y = gridY + p3.y * cellSize + cellSize / 2;
            DrawCircle(greenx, greeny, 16, GREEN);
            
            // int p4x = gridX + p4.x * cellSize + cellSize / 2;
            // int p4y = gridY + p4.y * cellSize + cellSize / 2;
            DrawCircle(orangex, orangey, 16, ORANGE);

            // if (selectedPlayer == 1) DrawCircleLines(p2x, p2y, 20, DARKBLUE);
            // if (selectedPlayer == 2) DrawCircleLines(p3x, p3y, 20, DARKGREEN);
            // if (selectedPlayer == 3) DrawCircleLines(p4x, p4y, 20, GOLD);
            switch (selectedRobot) {
                case ROBOT_BLUE:
                DrawCircleLines(bluex, bluey, 20, BLUE);
                break;
                case ROBOT_RED:
                DrawCircleLines(redx, redy, 20, RED);
                break;
                case ROBOT_GREEN:
                DrawCircleLines(greenx, greeny, 20, GREEN);
                break;
                case ROBOT_ORANGE:
                DrawCircleLines(orangex, orangey, 20, ORANGE);
                break;
            }

            DrawRectangleRec(hintButton, LIGHTGRAY);
            DrawRectangleLinesEx(hintButton, 2, DARKGRAY);
            DrawText("Hint", (int)hintButton.x + 36, (int)hintButton.y + 10, 20, BLACK);

            // if (hasHint) {
            //     DrawText(TextFormat("Next: (%d,%d)", hintNext.x, hintNext.y), (int)hintButton.x - 8, (int)hintButton.y + 55, 20, DARKGRAY);
            // }

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}