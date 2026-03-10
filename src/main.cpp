#include "game.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

int main() {
    const int screenWidth = 900;
    const int screenHeight = 720;
    const int cellSize = 48;
    const int cols = 8;
    const int rows = 8;
    const int gridX = (screenWidth - cols * cellSize) / 2;
    const int gridY = 90;

    InitWindow(screenWidth, screenHeight, "Ricochet");
    SetTargetFPS(60);

    enum ScreenState { MENU, PLAYING };
    ScreenState state = MENU;

    Rectangle playButton = {(float)(screenWidth / 2 - 100), 340.0f, 200.0f, 60.0f};
    Rectangle hintButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY + 20), 120.0f, 42.0f};

    int stepsTaken = 0;
    
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
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, hintButton)) {
                    
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

                    // Check if a robot was clicked
                    if (CheckCollisionPointCircle(mousePos, {(float)bluex, (float)bluey}, 16.0f)) {
                        selectedRobot = ROBOT_BLUE;
                    } else if (CheckCollisionPointCircle(mousePos, {(float)redx, (float)redy}, 16.0f)) {
                        selectedRobot = ROBOT_RED;
                    } else if (CheckCollisionPointCircle(mousePos, {(float)greenx, (float)greeny}, 16.0f)) {
                        selectedRobot = ROBOT_GREEN;
                    } else if (CheckCollisionPointCircle(mousePos, {(float)orangex, (float)orangey}, 16.0f)) {
                        selectedRobot = ROBOT_ORANGE;
                    } else {
                        bool insideGrid = mousePos.x >= gridX && mousePos.x < gridX + cols * cellSize &&
                                          mousePos.y >= gridY && mousePos.y < gridY + rows * cellSize;
                        if (insideGrid) {
                            // Grid coordinates of the tile that was clicked
                            int clickedX = (int)((mousePos.x - gridX) / cellSize);
                            int clickedY = (int)((mousePos.y - gridY) / cellSize);

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
                                    stepsTaken++;
                                }
                            }
                        }
                    }
                }
            }
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
            std::string scoreText = "Steps: "+std::to_string(stepsTaken);
            DrawText(scoreText.c_str(), gridX + 260, 28, 28, BLACK);
            DrawText("R to reset", gridX + 500, 34, 20, DARKGRAY);

            for (int y = 0; y < rows; y++) {
                for (int x = 0; x < cols; x++) {
                    int px = gridX + x * cellSize;
                    int py = gridY + y * cellSize;
                    DrawRectangleLines(px, py, cellSize, cellSize, LIGHTGRAY);
                }
            }

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

            int bluex = gridX + gameState.bluePos.x * cellSize + cellSize / 2;
            int bluey = gridY + gameState.bluePos.y * cellSize + cellSize / 2;
            int redx = gridX + gameState.redPos.x * cellSize + cellSize / 2;
            int redy = gridY + gameState.redPos.y * cellSize + cellSize / 2;
            int greenx = gridX + gameState.greenPos.x * cellSize + cellSize / 2;
            int greeny = gridY + gameState.greenPos.y * cellSize + cellSize / 2;
            int orangex = gridX + gameState.orangePos.x * cellSize + cellSize / 2;
            int orangey = gridY + gameState.orangePos.y * cellSize + cellSize / 2;

            DrawCircle(bluex, bluey, 16, BLUE);
            DrawCircle(redx, redy, 16, RED);
            DrawCircle(greenx, greeny, 16, GREEN);
            DrawCircle(orangex, orangey, 16, ORANGE);

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
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}