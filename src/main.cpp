#include "game.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <queue>


std::vector<Cell> breadth_first_search(Cell initial_state, Cell goal_state, int cols, int rows, const std::vector<Wall> &walls, const std::vector<Cell> otherPlayer) {
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::vector<std::vector<Cell>> parent(rows, std::vector<Cell>(cols, {-1, -1}));
    std::queue<Cell> q;

    visited[initial_state.y][initial_state.x] = true;
    q.push(initial_state);

    const int dirs[4][2] = {
        {0, -1},
        {-1, 0},
        {0, 1},
        {1, 0}
    };
    while (!q.empty()) {
        Cell current = q.front();
        q.pop();

        if (SameCell(current, goal_state)) {
            break;
        }

        for (int i = 0; i < 4; i++) {
            Cell next = SlideMove(current, dirs[i][0], dirs[i][1], cols, rows, walls, otherPlayer);
            if (SameCell(next, current)) {
                continue;
            }
            if (!visited[next.y][next.x]) {
                visited[next.y][next.x] = true;
                parent[next.y][next.x] = current;
                q.push(next);
            }
        }
    }

    if (!visited[goal_state.y][goal_state.x]) {
        return {};
    }

    std::vector<Cell> path;
    Cell cur = goal_state;
    path.push_back(cur);
    while (!SameCell(cur, initial_state)) {
        Cell prev = parent[cur.y][cur.x];
        if (prev.x == -1 && prev.y == -1) {
            return {};
        }
        cur = prev;
        path.push_back(cur);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

int main() {
    const int screenWidth = 900;
    const int screenHeight = 720;
    const int cellSize = 48;
    const int cols = 8;
    const int rows = 8;
    const int gridX = (screenWidth - cols * cellSize) / 2;
    const int gridY = 90;

    std::vector<Wall> walls;
    auto addWall = [&](int x, int y, bool horizontal) {
        if (!HasWall(walls, x, y, horizontal)) {
            walls.push_back({x, y, horizontal});
        }
    };

    auto addCellWall = [&](int cellNumber, char side) {
        int index = cellNumber - 1;
        int x = index % cols;
        int y = index / cols;

        if (side == 'R') addWall(x + 1, y, false);
        if (side == 'L') addWall(x, y, false);
        if (side == 'T') addWall(x, y, true);
        if (side == 'B') addWall(x, y + 1, true);
    };

    addCellWall(5, 'R');
    addCellWall(12, 'R');
    addCellWall(12, 'B');
    addCellWall(15, 'L');
    addCellWall(15, 'B');
    addCellWall(26, 'L');
    addCellWall(26, 'B');
    addCellWall(37, 'L');
    addCellWall(37, 'T');
    addCellWall(39, 'R');
    addCellWall(39, 'T');
    addCellWall(50, 'T');
    addCellWall(50, 'R');
    addCellWall(54, 'B');
    addCellWall(54, 'R');
    addCellWall(56, 'T');
    addCellWall(59, 'R');

    InitWindow(screenWidth, screenHeight, "Ricochet");
    SetTargetFPS(60);

    enum ScreenState { MENU, PLAYING };
    ScreenState state = MENU;

    Rectangle playButton = {(float)(screenWidth / 2 - 100), 340.0f, 200.0f, 60.0f};
    Rectangle hintButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY + 20), 120.0f, 42.0f};

    Cell p1 = {0, 0};
    Cell p2 = {0,7};
    Cell p3 = {7,0};
    Cell p4 = {7,7};
    Cell coin = {5, 6};
    int score = 0;
    int selectedPlayer = 0;
    Cell hintNext = {-1, -1};
    bool hasHint = false;

    auto respawnCoin = [&]() {
        do {
            coin = {GetRandomValue(0, cols - 1), GetRandomValue(0, rows - 1)};
        } while (SameCell(coin, p1) || SameCell(coin, p2) || SameCell(coin, p3) || SameCell(coin, p4));
    };

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
                    Cell current = (selectedPlayer == 0) ? p1 : (selectedPlayer == 1) ? p2 : (selectedPlayer == 2) ? p3 : p4;
                    std::vector<Cell> localBlockers;
                    if (selectedPlayer != 0) localBlockers.push_back(p1);
                    if (selectedPlayer != 1) localBlockers.push_back(p2);
                    if (selectedPlayer != 2) localBlockers.push_back(p3);
                    if (selectedPlayer != 3) localBlockers.push_back(p4);
                    std::vector<Cell> path = breadth_first_search(current, coin, cols, rows, walls, localBlockers);

                    if (path.size() >= 2) {
                        hintNext = path[1];
                        hasHint = true;
                    } else {
                        hasHint = false;
                    }
                } else {
                    int p1x = gridX + p1.x * cellSize + cellSize / 2;
                    int p1y = gridY + p1.y * cellSize + cellSize / 2;
                    int p2x = gridX + p2.x * cellSize + cellSize / 2;
                    int p2y = gridY + p2.y * cellSize + cellSize / 2;
                    int p3x = gridX + p3.x * cellSize + cellSize / 2;
                    int p3y = gridY + p3.y * cellSize + cellSize / 2;
                    int p4x = gridX + p4.x * cellSize + cellSize / 2;
                    int p4y = gridY + p4.y * cellSize + cellSize / 2;

                    if (CheckCollisionPointCircle(mouse, {(float)p1x, (float)p1y}, 16.0f)) {
                        selectedPlayer = 0;
                    } else if (CheckCollisionPointCircle(mouse, {(float)p2x, (float)p2y}, 16.0f)) {
                        selectedPlayer = 1;
                    } else if (CheckCollisionPointCircle(mouse, {(float)p3x, (float)p3y}, 16.0f)) {
                        selectedPlayer = 2;
                    } else if (CheckCollisionPointCircle(mouse, {(float)p4x, (float)p4y}, 16.0f)) {
                        selectedPlayer = 3;
                    } else {
                        bool insideGrid = mouse.x >= gridX && mouse.x < gridX + cols * cellSize &&
                                          mouse.y >= gridY && mouse.y < gridY + rows * cellSize;
                        if (insideGrid) {
                            int clickedX = (int)((mouse.x - gridX) / cellSize);
                            int clickedY = (int)((mouse.y - gridY) / cellSize);

                            Cell *currentRef = (selectedPlayer == 0) ? &p1 : (selectedPlayer == 1) ? &p2 : (selectedPlayer == 2) ? &p3 : &p4;

                            int dx = 0;
                            int dy = 0;
                            if (clickedX == currentRef->x && clickedY < currentRef->y) {
                                dy = -1;
                            } else if (clickedX == currentRef->x && clickedY > currentRef->y) {
                                dy = 1;
                            } else if (clickedY == currentRef->y && clickedX < currentRef->x) {
                                dx = -1;
                            } else if (clickedY == currentRef->y && clickedX > currentRef->x) {
                                dx = 1;
                            }

                            if (dx != 0 || dy != 0) {
                                std::vector<Cell> localBlockers;
                                if (selectedPlayer != 0) localBlockers.push_back(p1);
                                if (selectedPlayer != 1) localBlockers.push_back(p2);
                                if (selectedPlayer != 2) localBlockers.push_back(p3);
                                if (selectedPlayer != 3) localBlockers.push_back(p4);

                                Cell next = SlideMove(*currentRef, dx, dy, cols, rows, walls, localBlockers);
                                if (!SameCell(next, *currentRef)) {
                                    *currentRef = next;
                                    score++;
                                }
                            }
                        }
                    }
                }
            }

            if (SameCell(p1, coin) || SameCell(p2, coin) || SameCell(p3, coin) || SameCell(p4, coin)) {
                score = 0;
                respawnCoin();
            }

            if (IsKeyPressed(KEY_R)) {
                p1 = {0, 0};
                coin = {cols-1, 0};
                score = 0;
                selectedPlayer = 0;
                hintNext = {-1, -1};
                hasHint = false;
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

            for (const Wall &wall : walls) {
                int x1 = gridX + wall.x * cellSize;
                int y1 = gridY + wall.y * cellSize;
                if (wall.horizontal) {
                    DrawRectangle(x1, y1 - 2, cellSize, 4, DARKGRAY);
                } else {
                    DrawRectangle(x1 - 2, y1, 4, cellSize, DARKGRAY);
                }
            }

            int coinX = gridX + coin.x * cellSize + cellSize / 2;
            int coinY = gridY + coin.y * cellSize + cellSize / 2;
            DrawCircle(coinX, coinY, 12, ORANGE);

            int p1x = gridX + p1.x * cellSize + cellSize / 2;
            int p1y = gridY + p1.y * cellSize + cellSize / 2;
            DrawCircle(p1x, p1y, 16, RED);

            int p2x = gridX + p2.x * cellSize + cellSize / 2;
            int p2y = gridY + p2.y * cellSize + cellSize / 2;
            DrawCircle(p2x, p2y, 16, BLUE);

            int p3x = gridX + p3.x * cellSize + cellSize / 2;
            int p3y = gridY + p3.y * cellSize + cellSize / 2;
            DrawCircle(p3x, p3y, 16, GREEN);
            
            int p4x = gridX + p4.x * cellSize + cellSize / 2;
            int p4y = gridY + p4.y * cellSize + cellSize / 2;
            DrawCircle(p4x, p4y, 16, YELLOW);

            if (selectedPlayer == 0) DrawCircleLines(p1x, p1y, 20, MAROON);
            if (selectedPlayer == 1) DrawCircleLines(p2x, p2y, 20, DARKBLUE);
            if (selectedPlayer == 2) DrawCircleLines(p3x, p3y, 20, DARKGREEN);
            if (selectedPlayer == 3) DrawCircleLines(p4x, p4y, 20, GOLD);

            DrawRectangleRec(hintButton, LIGHTGRAY);
            DrawRectangleLinesEx(hintButton, 2, DARKGRAY);
            DrawText("Hint", (int)hintButton.x + 36, (int)hintButton.y + 10, 20, BLACK);

            if (hasHint) {
                DrawText(TextFormat("Next: (%d,%d)", hintNext.x, hintNext.y), (int)hintButton.x - 8, (int)hintButton.y + 55, 20, DARKGRAY);
            }

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}