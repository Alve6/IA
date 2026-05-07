#include "game.h"
#include "solver.h"
#include "levels.h"

#include "raylib.h"

#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <sstream>
#include <ctime>

struct LevelButton {
    Rectangle rect;
    std::string fileName; // also a label
};

const int screenWidth = 900;
const int screenHeight = 720;
const int cellSize = 48;        // size of a single game tile (in pixels)

GameState gameState;
GameBoard gameBoard;
GameState initialGameState;

int cols;                       // width and height of the current game board (in tiles)
int rows;                       // 
int gridX;                      // Position of the top-left corner of the grid on screen (in pixels)
int gridY;                      // 

enum ScreenState { MENU, PLAYING };
ScreenState state = MENU;

Rectangle hintButton;
Rectangle solveButton;
Rectangle solveToFileButton;
Rectangle compareButton;
Rectangle compareToFileButton;
std::vector<LevelButton> levelButtons;
std::string chosenLevelName = "";
const std::vector<std::string> hintModes = {"BFS", "DFS", "IDS", "Greedy v1", "Greedy v2", "A*", "Weighted A*"};
std::vector<Rectangle> hintBoxes;
bool gameWon;
int stepsTaken = 0;
RobotType selectedRobot = ROBOT_BLUE;
bool hasHint = false;
Action currentHint;
std::string hintText = "";
std::string victoryText = "";
bool showHintMenu = false;
bool selectingAutoSolve = false;
bool selectingSolveToFile = false;
std::vector<Action> hintSolution;
bool autoSolving = false;
std::vector<Action> autoSolution;
int autoStepIndex = 0;
float autoStepTimer = 0.0f;
const float autoStepDelay = 0.45f; 
std::string selectedAlgorithm = "";
SolverResult lastSolverResult;
bool hasSolverResult = false;
std::vector<SolverResult> comparisonResults;
bool hasComparisonResults = false;

static SolverResult runSelectedAlgorithm(int selection, const GameState &gameState, const GameBoard &gameBoard) {
    switch (selection) {
        case 1: return solveBFS(gameState, gameBoard);
        case 2: return solveDFS(gameState, gameBoard);
        case 3: return solveIDS(gameState, gameBoard);
        case 4: return solveGreedy1(gameState, gameBoard);
        case 5: return solveGreedy2(gameState, gameBoard);
        case 6: return solveA(gameState, gameBoard);
        case 7: return solveWeightedA(gameState, gameBoard);
        default: return {};
    }
}
static std::vector<SolverResult> runAllAlgorithms(const GameState &gameState, const GameBoard &gameBoard) {
    std::vector<SolverResult> results;
    results.push_back(solveBFS(gameState, gameBoard));
    results.push_back(solveDFS(gameState, gameBoard));
    results.push_back(solveIDS(gameState, gameBoard));
    results.push_back(solveGreedy1(gameState, gameBoard));
    results.push_back(solveGreedy2(gameState, gameBoard));
    results.push_back(solveA(gameState, gameBoard));
    results.push_back(solveWeightedA(gameState, gameBoard));
    return results;
}
static std::string buildOutputFileName(const std::string &levelName, const std::string &algorithmName) {
    std::filesystem::create_directories("./results");

    std::string cleanAlgorithm = algorithmName;
    for (char &c : cleanAlgorithm) {
        if (c == ' ' || c == '*')
            c = '_';
    }

    std::string levelStem = std::filesystem::path(levelName).stem().string();
    std::time_t now = std::time(nullptr);

    return "./results/" + levelStem + "_" + cleanAlgorithm + "_" + std::to_string(now) + ".txt";
}

static bool saveSolutionToFile(
    const std::string &filePath,
    const std::string &levelName,
    const GameState &initialState,
    const SolverResult &result
) {
    std::ofstream out(filePath);
    if (!out.is_open())
        return false;

    out << "Ricochet Robots - Solver Result\n";
    out << "Level: " << levelName << "\n";
    out << "Algorithm: " << result.algorithmName << "\n";
    out << "Solved: " << (result.solved ? "YES" : "NO") << "\n";
    out << "Time (ms): " << result.elapsedMs << "\n";
    out << "Generated states: " << result.generatedStates << "\n";
    out << "Expanded states: " << result.expandedStates << "\n";
    out << "Repeated states: " << result.repeatedStates << "\n";
    out << "Visited states: " << result.visitedStates << "\n";
    out << "Max frontier size: " << result.maxFrontierSize << "\n";
    out << "Approx memory (bytes): " << result.approxMemoryBytes << "\n";
    out << "Move count: " << result.actions.size() << "\n";
    out << "Solution cost: " << result.solutionCost << "\n\n";

    out << "Initial State\n";
    out << "Blue: " << initialState.bluePos.x << " " << initialState.bluePos.y << "\n";
    out << "Red: " << initialState.redPos.x << " " << initialState.redPos.y << "\n";
    out << "Green: " << initialState.greenPos.x << " " << initialState.greenPos.y << "\n";
    out << "Orange: " << initialState.orangePos.x << " " << initialState.orangePos.y << "\n\n";

    out << "Solution\n";
    if (result.actions.empty()) {
        out << "No solution found.\n";
    } else {
        for (size_t i = 0; i < result.actions.size(); i++) {
            out << (i + 1) << ". " << actionToString(result.actions[i]) << "\n";
        }
    }

    return true;
}

static bool saveComparisonToFile(
    const std::string &filePath,
    const std::string &levelName,
    const std::vector<SolverResult> &results
) {
    std::ofstream out(filePath);
    if (!out.is_open())
        return false;

    out << "Ricochet Robots - Algorithm Comparison\n";
    out << "Level: " << levelName << "\n\n";

    for (const SolverResult &result : results) {
        out << "Algorithm: " << result.algorithmName << "\n";
        out << "Solved: " << (result.solved ? "YES" : "NO") << "\n";
        out << "Time (ms): " << result.elapsedMs << "\n";
        out << "Generated states: " << result.generatedStates << "\n";
        out << "Expanded states: " << result.expandedStates << "\n";
        out << "Repeated states: " << result.repeatedStates << "\n";
        out << "Visited states: " << result.visitedStates << "\n";
        out << "Max frontier size: " << result.maxFrontierSize << "\n";
        out << "Approx memory (bytes): " << result.approxMemoryBytes << "\n";
        out << "Move count: " << result.actions.size() << "\n";
        out << "Solution cost: " << result.solutionCost << "\n";
        out << "----------------------------------------\n";
    }

    return true;
}

void initLevelButtons() {
    std::vector<std::string> levelNames;
    // Just assuming that lvls directory does not contain anything invalid
    for (auto &entry : std::filesystem::directory_iterator("./lvls")) {
        levelNames.push_back(entry.path().filename());
    }
    int posx = 20;
    int posy = 20;
    const int width = 100;
    const int height = 30;
    for (std::string levelName : levelNames) {
        Rectangle newRect = {posx, posy, width, height};
        LevelButton newButton = {newRect, levelName};
        levelButtons.push_back(newButton);
        posx += width + 5;
        if (posx + width >= screenWidth - 20) {
            posy += height + 5;
            posx = 20;
        }
        // No safeguard against filling the screen with levels vertically
    }
}
void initHintButtons() {
    solveButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY), 150.0f, 42.0f};
    hintButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY + 52), 150.0f, 42.0f};
    solveToFileButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY + 104), 150.0f, 42.0f};
    compareButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY + 156), 150.0f, 42.0f};
    compareToFileButton = {(float)(gridX + cols * cellSize + 20), (float)(gridY + 208), 150.0f, 42.0f};

    hintBoxes.clear();
    for (int i = 0; i < (int)hintModes.size(); i++) {
        hintBoxes.push_back({
            (float)(gridX + cols * cellSize + 20),
            (float)(gridY + 270 + i * 50),
            160.0f,
            42.0f
        });
    }
}
void initLevel() {
    LevelData level = loadLevel("./lvls/" + chosenLevelName);
    gameState = level.state;
    gameBoard = level.board;
    
    cols = gameBoard.width;
    rows = gameBoard.height;
    gridX = (screenWidth - cols * cellSize) / 2;
    gridY = 90;
    
    state = PLAYING;
    initialGameState = gameState;
    gameWon = isWinningState(gameState, gameBoard);
    initHintButtons();
}

void menuIterate() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        for (LevelButton button : levelButtons) {
            if (CheckCollisionPointRec(mouse, button.rect)) {
                chosenLevelName = button.fileName;
            }
        }
    }
    if (chosenLevelName.length() != 0)
        initLevel();
}

void menuDraw() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        for (LevelButton button : levelButtons) {
            DrawRectangleRec(button.rect, LIGHTGRAY);
            DrawRectangleLinesEx(button.rect, 2, DARKGRAY);
            DrawText(button.fileName.c_str(), button.rect.x + 5, button.rect.y + 1, 20, BLACK);
        }
        EndDrawing();
}

// Mostly checks inputs and performs appropriate actions, but also runs the autosolving
void gameIterate() {
    if (IsKeyPressed(KEY_R)) {
        gameState = initialGameState;
        selectedRobot = ROBOT_BLUE;
        stepsTaken = 0;
        gameWon = false;
        hasHint = false;
        hintText = "";
        showHintMenu = false;

        autoSolving = false;
        autoSolution.clear();
        autoStepIndex = 0;
        autoStepTimer = 0.0f;
        selectedAlgorithm = "";
        selectingAutoSolve = false;
        selectingSolveToFile = false;
        hasSolverResult = false;
        hasComparisonResults = false;
        comparisonResults.clear();
    }
    // If there's a click and input is allowed (i.e. game is not won and autosolving is not running)
    if (!gameWon && !autoSolving && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ////////////////////////////////////////////////////
        // First check if any of the buttons were clicked //
        ////////////////////////////////////////////////////
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, hintButton)) {
            showHintMenu = !showHintMenu;
            selectingAutoSolve = false;
            selectingSolveToFile = false;
        } else if (CheckCollisionPointRec(mousePos, solveButton)) {
            showHintMenu = !showHintMenu;
            selectingAutoSolve = true;
            selectingSolveToFile = false;
        } else if (CheckCollisionPointRec(mousePos, solveToFileButton)) {
            showHintMenu = !showHintMenu;
            selectingAutoSolve = false;
            selectingSolveToFile = true;
        } else if (CheckCollisionPointRec(mousePos, compareButton)) {
            comparisonResults = runAllAlgorithms(gameState, gameBoard);
            hasComparisonResults = true;
            showHintMenu = false;
            hintText = "Comparison finished";
            hasHint = false;
            autoSolving = false;
            autoSolution.clear();
            autoStepIndex = 0;
            autoStepTimer = 0.0f;
            hasSolverResult = false;
        } else if (CheckCollisionPointRec(mousePos, compareToFileButton)) {
            comparisonResults = runAllAlgorithms(gameState, gameBoard);
            hasComparisonResults = true;
            showHintMenu = false;

            std::string filePath = buildOutputFileName(chosenLevelName, "comparison_all");
            bool saved = saveComparisonToFile(filePath, chosenLevelName, comparisonResults);

            hintText = saved ? ("Comparison saved to: " + filePath) : "Failed to save comparison file";
            hasHint = false;
            autoSolving = false;
            autoSolution.clear();
            autoStepIndex = 0;
            autoStepTimer = 0.0f;
            hasSolverResult = false;
        } else if (showHintMenu) {
            int selection = -1;
            for (int i = 0; i < (int)hintBoxes.size(); i++) {
                if (CheckCollisionPointRec(mousePos, hintBoxes[i])) {
                    selection = i + 1;
                    break;
                }
            }
            
            if (selection == -1) {
                showHintMenu = false;
            } else {
                SolverResult result = runSelectedAlgorithm(selection, gameState, gameBoard);
                lastSolverResult = result;
                hasSolverResult = true;

                std::vector<Action> solution = result.actions;
                selectedAlgorithm = result.algorithmName;

                if (selectingSolveToFile) {
                    std::string filePath = buildOutputFileName(chosenLevelName, selectedAlgorithm);
                    bool saved = saveSolutionToFile(
                        filePath,
                        chosenLevelName,
                        gameState,
                        result
                    );

                    hasHint = false;
                    hintSolution.clear();
                    autoSolving = false;
                    autoSolution.clear();
                    autoStepIndex = 0;
                    autoStepTimer = 0.0f;

                    if (saved) {
                        hintText = "Solution saved to: " + filePath;
                    } else {
                        hintText = "Failed to save solution file";
                    }
                } else if (selectingAutoSolve) {
                    autoSolution = solution;
                    autoStepIndex = 0;
                    autoStepTimer = 0.0f;
                    autoSolving = !autoSolution.empty();

                    hasHint = false;
                    hintSolution.clear();
                    hintText = autoSolving
                        ? ("Solving with " + selectedAlgorithm)
                        : "No solution found";
                } else {
                    hintSolution = solution;

                    if (!hintSolution.empty()) {
                        currentHint = hintSolution[0];
                        hasHint = true;
                        selectedRobot = currentHint.robot;

                        hintText = "Hint (" + selectedAlgorithm + "): ";
                        hintText += robotTypeToString(currentHint.robot);
                        hintText += " ";
                        hintText += directionToString(currentHint.dir);
                    } else {
                        hasHint = false;
                        hintText = "No solution found";
                    }
                }

                showHintMenu = false;
            }
        } else { 
            ///////////////////////////////////////////////////////////////
            // No buttons were clicked, now check if a robot was clicked //
            ///////////////////////////////////////////////////////////////

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
                ////////////////////////////////////////////////////////////////////////////////////////////////
                // No robot was clicked, now check if a tile was clicked to move the currently selected robot //
                ////////////////////////////////////////////////////////////////////////////////////////////////
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
                        ///////////////////////////////////////////////////////////////
                        // Yes, a valid tile was clicked, now we try to move a robot //
                        ///////////////////////////////////////////////////////////////
                        GameState newState = slideMove(selectedRobot, dir, gameState, gameBoard);
                        if (newState != gameState) {
                            gameState = newState;
                            stepsTaken++;
                            gameWon = isWinningState(gameState, gameBoard);

                            hasHint = false;
                            hintSolution.clear();
                            hintText = "";

                            autoSolving = false;
                            autoSolution.clear();
                            autoStepIndex = 0;
                            autoStepTimer = 0.0f;
                        }
                    }
                }
            }
        }
    }

    if (autoSolving && !gameWon) {
        autoStepTimer += GetFrameTime();

        if (autoStepIndex < (int)autoSolution.size() && autoStepTimer >= autoStepDelay) {
            autoStepTimer = 0.0f;

            Action step = autoSolution[autoStepIndex];
            selectedRobot = step.robot;

            GameState newState = slideMove(step.robot, step.dir, gameState, gameBoard);
            if (newState != gameState) {
                gameState = newState;
                stepsTaken++;
                gameWon = isWinningState(gameState, gameBoard);
            }

            autoStepIndex++;
        }

        if (autoStepIndex >= (int)autoSolution.size() || gameWon) {
            autoSolving = false;

            if (gameWon) {
                victoryText = "Solved with " + selectedAlgorithm + " in " + std::to_string(stepsTaken) + " moves";
            } else {
                hintText = "Auto-solve finished";
            }
        }
    }
}

void gameDraw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    DrawText("Ricochet", gridX, 20, 36, BLACK);
    std::string scoreText = "Steps: "+std::to_string(stepsTaken);
    DrawText(scoreText.c_str(), gridX + 260, 28, 28, BLACK);
    if (autoSolving) {
        std::string solvingText = "Auto-solving: " + selectedAlgorithm + " (" + std::to_string(autoStepIndex) + "/" + std::to_string((int)autoSolution.size()) + ")";
        DrawText(solvingText.c_str(), gridX, gridY + rows * cellSize + 30, 22, DARKGRAY);
    }
    DrawText("Press R to reset", gridX + 410, 34, 20, DARKGRAY);

    if (gameWon) {
        DrawText("YOU WIN!", gridX + 140, gridY + rows * cellSize + 30, 32, GREEN);
        DrawText(victoryText.c_str(), gridX+50, gridY + rows * cellSize + 80, 20, DARKGRAY);
        hintText = "";
    }

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
    DrawRectangleRec(solveButton, LIGHTGRAY);
    DrawRectangleLinesEx(solveButton, 2, DARKGRAY);
    DrawText("Solve", (int)solveButton.x + 30, (int)solveButton.y + 10, 20, BLACK);
    DrawRectangleRec(solveToFileButton, LIGHTGRAY);
    DrawRectangleLinesEx(solveToFileButton, 2, DARKGRAY);
    DrawText("Solve to File", (int)solveToFileButton.x + 12, (int)solveToFileButton.y + 10, 20, BLACK);
    DrawRectangleRec(compareButton, LIGHTGRAY);
    DrawRectangleLinesEx(compareButton, 2, DARKGRAY);
    DrawText("Compare All", (int)compareButton.x + 14, (int)compareButton.y + 10, 20, BLACK);

    DrawRectangleRec(compareToFileButton, LIGHTGRAY);
    DrawRectangleLinesEx(compareToFileButton, 2, DARKGRAY);
    DrawText("Compare to File", (int)compareToFileButton.x + 4, (int)compareToFileButton.y + 10, 20, BLACK);
    if (showHintMenu) {
        for (int i = 0; i < (int)hintModes.size(); i++) {
            DrawRectangleRec(hintBoxes[i], LIGHTGRAY);
            DrawRectangleLinesEx(hintBoxes[i], 2, DARKGRAY);
            DrawText(hintModes[i].c_str(), (int)hintBoxes[i].x + 10, (int)hintBoxes[i].y + 10, 20, BLACK);
        }
    }
    if (!showHintMenu && !hintText.empty()) {
        DrawText(hintText.c_str(), gridX, gridY + rows * cellSize + 80, 20, DARKGRAY);
    }

    if (hasSolverResult) {
        int infoX = gridX + cols * cellSize + 20;
        int infoY = gridY + 430;

        DrawText(("Alg: " + lastSolverResult.algorithmName).c_str(), infoX, infoY, 18, DARKBLUE);
        DrawText(TextFormat("Solved: %s", lastSolverResult.solved ? "YES" : "NO"), infoX, infoY + 22, 18, BLACK);
        DrawText(TextFormat("Time: %.3f ms", lastSolverResult.elapsedMs), infoX, infoY + 44, 18, BLACK);
        DrawText(TextFormat("Generated: %d", lastSolverResult.generatedStates), infoX, infoY + 66, 18, BLACK);
        DrawText(TextFormat("Expanded: %d", lastSolverResult.expandedStates), infoX, infoY + 88, 18, BLACK);
        DrawText(TextFormat("Repeated: %d", lastSolverResult.repeatedStates), infoX, infoY + 110, 18, BLACK);
        DrawText(TextFormat("Moves: %d", lastSolverResult.solutionCost), infoX, infoY + 132, 18, BLACK);
    }
    
    if (hasComparisonResults) {
        int x = 20;
        int y = screenHeight - 180;

        DrawText("Algorithm Comparison", x, y, 20, MAROON);
        y += 28;

        for (const SolverResult &r : comparisonResults) {
            std::string line =
                r.algorithmName +
                " | solved=" + std::string(r.solved ? "yes" : "no") +
                " | moves=" + std::to_string(r.solutionCost) +
                " | expanded=" + std::to_string(r.expandedStates) +
                " | time=" + std::to_string((int)r.elapsedMs) + " ms";

            DrawText(line.c_str(), x, y, 18, BLACK);
            y += 22;
        }
    }

    EndDrawing();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Ricochet");
    SetTargetFPS(60);

    initLevelButtons();

    while (!WindowShouldClose()) {
        switch(state) {
            case MENU:
            menuIterate();
            menuDraw();
            break;
            case PLAYING:
            gameIterate();
            gameDraw();            
            break;
        }
    }

    CloseWindow();
    return 0;
}