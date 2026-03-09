#include "game.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
static std::vector<Cell> breadth_first_search(Cell initial_state, Cell goal_state, int cols, int rows, const std::vector<Wall> &walls, Cell otherPlayer) {
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

    bool found = false;
    while (!q.empty()) {
        Cell current = q.front();
        q.pop();

        if (SameCell(current, goal_state)) {
            found = true;
            break;
        }

        for (const auto &dir : dirs) {
            Cell next = SlideMove(current, dir[0], dir[1], cols, rows, walls, otherPlayer);
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

    if (!found) {
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
    Cell noPlayer = {-1, -1};
    Cell coin = {cols - 1, 0};
    int score = 0;
    Cell hintNext = {-1, -1};
    bool hasHint = false;

    auto respawnCoin = [&]() {
        do {
            coin = {GetRandomValue(0, cols - 1), GetRandomValue(0, rows - 1)};
        } while (SameCell(coin, p1));
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
                    std::vector<Cell> path = breadth_first_search(p1, coin, cols, rows, walls, noPlayer);

                    if (path.size() >= 2) {
                        hintNext = path[1];
                        hasHint = true;
                    } else {
                        hasHint = false;
                    }
                }
            }

            if (IsKeyPressed(KEY_W)) {
                int originalx = p1.x;
                int originaly = p1.y;
                p1 = SlideMove(p1, 0, -1, cols, rows, walls, noPlayer);
                if (originalx != p1.x) score++;
                if (originaly != p1.y) score++;
            }

            if (IsKeyPressed(KEY_S)) {
                int originalx = p1.x;
                int originaly = p1.y;
                p1 = SlideMove(p1, 0, 1, cols, rows, walls, noPlayer);
                if (originalx != p1.x) score++;
                if (originaly != p1.y) score++;
            }
            if (IsKeyPressed(KEY_A)) {
                int originalx = p1.x;
                int originaly = p1.y;
                p1 = SlideMove(p1, -1, 0, cols, rows, walls, noPlayer);
                if (originalx != p1.x) score++;
                if (originaly != p1.y) score++;
            }
            if (IsKeyPressed(KEY_D)) {
                int originalx = p1.x;
                int originaly = p1.y;
                p1 = SlideMove(p1, 1, 0, cols, rows, walls, noPlayer);
                if (originalx != p1.x) score++;
                if (originaly != p1.y) score++;
            }

            if (SameCell(p1, coin)) {
                score = 0;
                respawnCoin();
            }

            if (IsKeyPressed(KEY_R)) {
                p1 = {0, 0};
                coin = {cols-1, 0};
                score = 0;
                hasHint = false;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (state == MENU) {
            DrawText("Ricochet", screenWidth / 2 - 110, 180, 56, BLACK);
            DrawRectangleRec(playButton, RED);
            DrawText("Play", screenWidth / 2 - 30, 357, 28, WHITE);
            DrawText("Move: WASD", screenWidth / 2 - 70, 450, 22, BLACK);
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