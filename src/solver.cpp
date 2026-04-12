#include "solver.h"


#include <queue>
#include <stack>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <chrono>

struct Node {
    Node *parent;
    GameState state;
    // Action that lead to the current state. Doesn't make sense in the root node, i guess
    Action lastAction;
    std::vector<Node *> children;
    // For IDS
    int depth;
    int moves;

    Node(const GameState &state, const Action &lastAction) {
        parent = nullptr;
        this->state = state;
        this->lastAction = lastAction;
        depth = 0;
        moves = 0;
    }
    void addChild(Node *child) {
        this->children.push_back(child);
        child->parent = this;
    }
    void deleteChildren() {
        for (Node *child : children) {
            delete child;
        }
        children.clear();
    }
    ~Node() {
        deleteChildren();
    }
    
    // Returns true if either this node or any of it's descendants has the specified state
    bool hasState(const GameState &state) {
        if (this->state == state)
            return true;
        for (Node *child : this->children) {
            if (child->hasState(state))
                return true;
        }
        return false;
    }
};

std::string actionToString(const Action &action) {
    return robotTypeToString(action.robot) + " " + directionToString(action.dir);
}

static std::vector<Action> reconstructPath(Node *final) {
    std::vector<Action> result;
    if (final == nullptr)
        return result;

    Node *current = final;
    while (current->parent != nullptr) {
        result.push_back(current->lastAction);
        current = current->parent;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

static size_t approximateMemoryUsage(size_t nodeCount, size_t frontierSize, size_t visitedCount) {
    return nodeCount * sizeof(Node)
         + frontierSize * sizeof(Node*)
         + visitedCount * sizeof(GameState);
}

static iVector2 findGoalPos(const GameBoard &board) {
    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            iVector2 pos = {x, y};
            if (board.checkFlag(pos, TILE_GOAL))
                return pos;
        }
    }
    return {-1, -1};
}

static int manhattan(const iVector2 &a, const iVector2 &b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

static int stateDistanceToGoal(const GameState &state, const iVector2 &goal, int choice) {
    if (choice == 1){
        return std::min({
        manhattan(state.bluePos, goal),
        manhattan(state.redPos, goal),
        manhattan(state.greenPos, goal),
        manhattan(state.orangePos, goal)
        });
    } else if (choice == 2){
        return manhattan(state.bluePos, goal)+ manhattan(state.redPos, goal)+ manhattan(state.greenPos, goal)+ manhattan(state.orangePos, goal);
    }   
    return 0;
}


struct GreedyCompare {
    iVector2 goal;
    int choice;

    explicit GreedyCompare(const iVector2 &goal, int choice) : goal(goal), choice(choice) {}

    
    bool operator()(const Node *a, const Node *b) const {
        return stateDistanceToGoal(a->state, goal,choice) > stateDistanceToGoal(b->state, goal,choice);
    }
};

struct ACompare {
    iVector2 goal;
    int choice;

    explicit ACompare(const iVector2 &goal, int choice) : goal(goal), choice(choice) {}

    
    bool operator()(const Node *a, const Node *b) const {
        return stateDistanceToGoal(a->state, goal,choice)+a->moves > stateDistanceToGoal(b->state, goal,choice)+b->moves;
    }
};

struct WeightedACompare {
    iVector2 goal;
    int choice;
    double weight;

    explicit WeightedACompare(const iVector2 &goal, int choice, double weight) : goal(goal), choice(choice), weight(weight) {}

    bool operator()(const Node *a, const Node *b) const {
        return weight*stateDistanceToGoal(a->state, goal,choice)+a->moves > weight*stateDistanceToGoal(b->state, goal,choice)+b->moves;
    }
};

// Returns a vector of pairs of new states and actions needed to convert the current state to the new one
std::vector<std::pair<GameState, Action>> getNextStates(const GameState &currentState, const GameBoard &board) {
    static const RobotType robots[4] = {ROBOT_BLUE, ROBOT_RED, ROBOT_GREEN, ROBOT_ORANGE};
    static const Direction dirs[4]   = {DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST};
    std::vector<std::pair<GameState, Action>> result;
    for (RobotType robot : robots) {
        for (Direction dir : dirs) {
            GameState newState = slideMove(robot, dir, currentState, board);
            if (newState != currentState) {
                result.push_back({newState, {robot, dir}});
            }
        }
    }
    return result;
}

// Returns a sequence of actions to reach the winning state from the initial
SolverResult solveBFS(const GameState &initState, const GameBoard &board) {
    auto start = std::chrono::high_resolution_clock::now();

    SolverResult result;
    result.algorithmName = "BFS";

    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    std::queue<Node*> q;
    std::set<GameState> visitedStates;

    q.push(&root);
    visitedStates.insert(initState);
    result.maxFrontierSize = 1;

    Node *final = nullptr;
    int nodeCount = 1;

    while (!q.empty() && final == nullptr) {
        Node *node = q.front();
        q.pop();

        result.expandedStates++;

        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        for (std::pair<GameState, Action> pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;

            if (visitedStates.find(state) != visitedStates.end()) {
                result.repeatedStates++;
                continue;
            }

            visitedStates.insert(state);
            result.generatedStates++;

            Node *newChild = new Node(state, action);
            node->addChild(newChild);
            q.push(newChild);
            nodeCount++;

            if ((int)q.size() > (int)result.maxFrontierSize)
                result.maxFrontierSize = q.size();

            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
    }

    result.actions = reconstructPath(final);
    result.solved = (final != nullptr);
    result.solutionCost = (int)result.actions.size();
    result.solutionDepth = (int)result.actions.size();
    result.visitedStates = visitedStates.size();
    result.approxMemoryBytes = approximateMemoryUsage(nodeCount, q.size(), visitedStates.size());

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}


SolverResult solveDFS(const GameState &initState, const GameBoard &board) {
    auto start = std::chrono::high_resolution_clock::now();

    SolverResult result;
    result.algorithmName = "DFS";

    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    std::stack<Node*> stack;
    std::set<GameState> visitedStates;

    stack.push(&root);
    visitedStates.insert(initState);
    result.maxFrontierSize = 1;

    Node *final = nullptr;
    int nodeCount = 1;

    while (!stack.empty() && final == nullptr) {
        Node *node = stack.top();
        stack.pop();

        result.expandedStates++;

        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        std::reverse(nextStates.begin(), nextStates.end());

        for (const auto &pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;

            if (visitedStates.find(state) != visitedStates.end()) {
                result.repeatedStates++;
                continue;
            }

            visitedStates.insert(state);
            result.generatedStates++;

            Node *newChild = new Node(state, action);
            node->addChild(newChild);
            stack.push(newChild);
            nodeCount++;

            if (stack.size() > result.maxFrontierSize)
                result.maxFrontierSize = stack.size();

            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
    }

    result.actions = reconstructPath(final);
    result.solved = (final != nullptr);
    result.solutionCost = (int)result.actions.size();
    result.solutionDepth = (int)result.actions.size();
    result.visitedStates = visitedStates.size();
    result.approxMemoryBytes = approximateMemoryUsage(nodeCount, stack.size(), visitedStates.size());

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}

// At depth limit of 10 and no viable solution, it takes about 5 second on my machine
const int IDS_MAX_DEPTH = 10;

SolverResult solveIDS(const GameState &initState, const GameBoard &board) {
    auto start = std::chrono::high_resolution_clock::now();

    SolverResult result;
    result.algorithmName = "IDS";

    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    root.depth = 0;

    Node *final = nullptr;
    int nodeCount = 1;
    std::set<GameState> allVisitedStates;
    allVisitedStates.insert(initState);

    for (int maxDepth = 1; maxDepth <= IDS_MAX_DEPTH && final == nullptr; maxDepth++) {
        root.deleteChildren();

        std::stack<Node*> stack;
        std::set<GameState> visitedThisIteration;

        stack.push(&root);
        visitedThisIteration.insert(initState);

        if (stack.size() > result.maxFrontierSize)
            result.maxFrontierSize = stack.size();

        while (!stack.empty() && final == nullptr) {
            Node *node = stack.top();
            stack.pop();

            result.expandedStates++;

            if (node->depth >= maxDepth)
                continue;

            std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
            std::reverse(nextStates.begin(), nextStates.end());

            for (const auto &pair : nextStates) {
                GameState state = pair.first;
                Action action = pair.second;

                if (visitedThisIteration.find(state) != visitedThisIteration.end()) {
                    result.repeatedStates++;
                    continue;
                }

                visitedThisIteration.insert(state);
                allVisitedStates.insert(state);
                result.generatedStates++;

                Node *newChild = new Node(state, action);
                newChild->depth = node->depth + 1;
                newChild->moves = node->moves + 1;
                node->addChild(newChild);
                stack.push(newChild);
                nodeCount++;

                if (stack.size() > result.maxFrontierSize)
                    result.maxFrontierSize = stack.size();

                if (isWinningState(state, board)) {
                    final = newChild;
                    break;
                }
            }
        }
    }

    result.actions = reconstructPath(final);
    result.solved = (final != nullptr);
    result.solutionCost = (int)result.actions.size();
    result.solutionDepth = (int)result.actions.size();
    result.visitedStates = allVisitedStates.size();
    result.approxMemoryBytes = approximateMemoryUsage(nodeCount, 0, allVisitedStates.size());

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}

SolverResult solveGreedy1(const GameState &initState, const GameBoard &board) {
    auto start = std::chrono::high_resolution_clock::now();

    SolverResult result;
    result.algorithmName = "Greedy v1";

    int choice = 1;
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    iVector2 goal = findGoalPos(board);

    std::priority_queue<Node*, std::vector<Node*>, GreedyCompare> p((GreedyCompare(goal, choice)));
    std::set<GameState> visitedStates;

    p.push(&root);
    visitedStates.insert(initState);
    result.maxFrontierSize = 1;

    Node *final = nullptr;
    int nodeCount = 1;

    while (!p.empty() && final == nullptr) {
        Node *node = p.top();
        p.pop();

        result.expandedStates++;

        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        for (const auto &pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;

            if (visitedStates.find(state) != visitedStates.end()) {
                result.repeatedStates++;
                continue;
            }

            visitedStates.insert(state);
            result.generatedStates++;

            Node *newChild = new Node(state, action);
            newChild->moves = node->moves + 1;
            node->addChild(newChild);
            p.push(newChild);
            nodeCount++;

            if (p.size() > result.maxFrontierSize)
                result.maxFrontierSize = p.size();

            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
    }

    result.actions = reconstructPath(final);
    result.solved = (final != nullptr);
    result.solutionCost = (int)result.actions.size();
    result.solutionDepth = (int)result.actions.size();
    result.visitedStates = visitedStates.size();
    result.approxMemoryBytes = approximateMemoryUsage(nodeCount, p.size(), visitedStates.size());

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}

SolverResult solveGreedy2(const GameState &initState, const GameBoard &board) {
    auto start = std::chrono::high_resolution_clock::now();

    SolverResult result;
    result.algorithmName = "Greedy v2";

    int choice = 2;
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    iVector2 goal = findGoalPos(board);

    std::priority_queue<Node*, std::vector<Node*>, GreedyCompare> p((GreedyCompare(goal, choice)));
    std::set<GameState> visitedStates;

    p.push(&root);
    visitedStates.insert(initState);
    result.maxFrontierSize = 1;

    Node *final = nullptr;
    int nodeCount = 1;

    while (!p.empty() && final == nullptr) {
        Node *node = p.top();
        p.pop();

        result.expandedStates++;

        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        for (const auto &pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;

            if (visitedStates.find(state) != visitedStates.end()) {
                result.repeatedStates++;
                continue;
            }

            visitedStates.insert(state);
            result.generatedStates++;

            Node *newChild = new Node(state, action);
            newChild->moves = node->moves + 1;
            node->addChild(newChild);
            p.push(newChild);
            nodeCount++;

            if (p.size() > result.maxFrontierSize)
                result.maxFrontierSize = p.size();

            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
    }

    result.actions = reconstructPath(final);
    result.solved = (final != nullptr);
    result.solutionCost = (int)result.actions.size();
    result.solutionDepth = (int)result.actions.size();
    result.visitedStates = visitedStates.size();
    result.approxMemoryBytes = approximateMemoryUsage(nodeCount, p.size(), visitedStates.size());

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}


SolverResult solveA(const GameState &initState, const GameBoard &board) {
    auto start = std::chrono::high_resolution_clock::now();

    SolverResult result;
    result.algorithmName = "A*";

    int choice = 1;
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    iVector2 goal = findGoalPos(board);

    std::priority_queue<Node*, std::vector<Node*>, ACompare> p((ACompare(goal, choice)));
    std::map<GameState, int> bestCost;

    root.moves = 0;
    bestCost[initState] = 0;
    p.push(&root);
    result.maxFrontierSize = 1;

    Node *final = nullptr;
    int nodeCount = 1;

    while (!p.empty() && final == nullptr) {
        Node *node = p.top();
        p.pop();

        if (node->moves > bestCost[node->state]) {
            result.repeatedStates++;
            continue;
        }

        result.expandedStates++;

        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);

        for (const auto &pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;
            int nextMoves = node->moves + 1;

            auto existing = bestCost.find(state);
            if (existing != bestCost.end() && existing->second <= nextMoves) {
                result.repeatedStates++;
                continue;
            }

            bestCost[state] = nextMoves;
            result.generatedStates++;

            Node *newChild = new Node(state, action);
            newChild->moves = nextMoves;
            node->addChild(newChild);
            p.push(newChild);
            nodeCount++;

            if (p.size() > result.maxFrontierSize)
                result.maxFrontierSize = p.size();

            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
    }

    result.actions = reconstructPath(final);
    result.solved = (final != nullptr);
    result.solutionCost = (int)result.actions.size();
    result.solutionDepth = (int)result.actions.size();
    result.visitedStates = bestCost.size();
    result.approxMemoryBytes = approximateMemoryUsage(nodeCount, p.size(), bestCost.size());

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}

SolverResult solveWeightedA(const GameState &initState, const GameBoard &board) {
    auto start = std::chrono::high_resolution_clock::now();

    SolverResult result;
    result.algorithmName = "Weighted A*";

    int choice = 1;
    double weight = 1.2;
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    iVector2 goal = findGoalPos(board);

    std::priority_queue<Node*, std::vector<Node*>, WeightedACompare> p((WeightedACompare(goal, choice, weight)));
    std::map<GameState, int> bestCost;

    root.moves = 0;
    bestCost[initState] = 0;
    p.push(&root);
    result.maxFrontierSize = 1;

    Node *final = nullptr;
    int nodeCount = 1;

    while (!p.empty() && final == nullptr) {
        Node *node = p.top();
        p.pop();

        if (node->moves > bestCost[node->state]) {
            result.repeatedStates++;
            continue;
        }

        result.expandedStates++;

        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);

        for (const auto &pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;
            int nextMoves = node->moves + 1;

            auto existing = bestCost.find(state);
            if (existing != bestCost.end() && existing->second <= nextMoves) {
                result.repeatedStates++;
                continue;
            }

            bestCost[state] = nextMoves;
            result.generatedStates++;

            Node *newChild = new Node(state, action);
            newChild->moves = nextMoves;
            node->addChild(newChild);
            p.push(newChild);
            nodeCount++;

            if (p.size() > result.maxFrontierSize)
                result.maxFrontierSize = p.size();

            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
    }

    result.actions = reconstructPath(final);
    result.solved = (final != nullptr);
    result.solutionCost = (int)result.actions.size();
    result.solutionDepth = (int)result.actions.size();
    result.visitedStates = bestCost.size();
    result.approxMemoryBytes = approximateMemoryUsage(nodeCount, p.size(), bestCost.size());

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return result;
}