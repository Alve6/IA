#include "game.h"
#include "raylib.h"
#include <string>
#include <vector>

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

    Cell p1 = {0, 0};
    Cell p2 = {cols - 1, rows - 1};
    Cell coin = {cols / 2, rows / 2};
    bool redTurn = true;
    int score1 = 0;
    int score2 = 0;

    auto respawnCoin = [&]() {
        do {
            coin = {GetRandomValue(0, cols - 1), GetRandomValue(0, rows - 1)};
        } while (SameCell(coin, p1) || SameCell(coin, p2));
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
            if (IsKeyPressed(KEY_W)) p1 = SlideMove(p1, 0, -1, cols, rows, walls, p2);
            if (IsKeyPressed(KEY_S)) p1 = SlideMove(p1, 0, 1, cols, rows, walls, p2);
            if (IsKeyPressed(KEY_A)) p1 = SlideMove(p1, -1, 0, cols, rows, walls, p2);
            if (IsKeyPressed(KEY_D)) p1 = SlideMove(p1, 1, 0, cols, rows, walls, p2);

            if (IsKeyPressed(KEY_UP)) p2 = SlideMove(p2, 0, -1, cols, rows, walls, p1);
            if (IsKeyPressed(KEY_DOWN)) p2 = SlideMove(p2, 0, 1, cols, rows, walls, p1);
            if (IsKeyPressed(KEY_LEFT)) p2 = SlideMove(p2, -1, 0, cols, rows, walls, p1);
            if (IsKeyPressed(KEY_RIGHT)) p2 = SlideMove(p2, 1, 0, cols, rows, walls, p1);

            if (redTurn && SameCell(p1, coin)) {
                score1++;
                redTurn = false;
                respawnCoin();
            }

            if (!redTurn && SameCell(p2, coin)) {
                score2++;
                redTurn = true;
                respawnCoin();
            }

            if (IsKeyPressed(KEY_R)) {
                p1 = {0, 0};
                p2 = {cols - 1, rows - 1};
                coin = {cols / 2, rows / 2};
                redTurn = true;
                score1 = 0;
                score2 = 0;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (state == MENU) {
            DrawText("Ricochet", screenWidth / 2 - 110, 180, 56, BLACK);
            DrawRectangleRec(playButton, RED);
            DrawText("Play", screenWidth / 2 - 30, 357, 28, WHITE);
            DrawText("P1: WASD | P2: Arrows", screenWidth / 2 - 145, 450, 22, BLACK);
        } else {
            DrawText("Ricochet", gridX, 20, 36, BLACK);
            std::string scoreText = "P1: " + std::to_string(score1) + "   P2: " + std::to_string(score2);
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
            DrawCircle(coinX, coinY, 12, redTurn ? RED : BLUE);

            int p1x = gridX + p1.x * cellSize + cellSize / 2;
            int p1y = gridY + p1.y * cellSize + cellSize / 2;
            DrawCircle(p1x, p1y, 16, RED);

            int p2x = gridX + p2.x * cellSize + cellSize / 2;
            int p2y = gridY + p2.y * cellSize + cellSize / 2;
            DrawCircle(p2x, p2y, 16, BLUE);


        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}