#include "solver.h"


#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>

struct Node {
    Node *parent;
    GameState state;
    // Action that lead to the current state. Doesn't make sense in the root node, i guess
    Action lastAction;
    std::vector<Node *> children;

    Node(const GameState &state, const Action &lastAction) {
        parent = nullptr;
        this->state = state;
        this->lastAction = lastAction;
    }
    void addChild(Node *child) {
        this->children.push_back(child);
        child->parent = this;
    }
    ~Node() {
        for (Node *child : children) {
            delete child;
        }
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

static int stateDistanceToGoal(const GameState &state, const iVector2 &goal) {
    return std::min({
        manhattan(state.bluePos, goal),
        manhattan(state.redPos, goal),
        manhattan(state.greenPos, goal),
        manhattan(state.orangePos, goal)
    });
}

struct GreedyCompare {
    iVector2 goal;

    explicit GreedyCompare(const iVector2 &goal) : goal(goal) {}

    bool operator()(const Node *a, const Node *b) const {
        return stateDistanceToGoal(a->state, goal) > stateDistanceToGoal(b->state, goal);
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
std::vector<Action> solveBFS(const GameState &initState, const GameBoard &board) {
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    std::queue<Node*> q;
    q.push(&root);
    Node *final = nullptr;
    int nodeCount = 1;
    while (!q.empty() && final == nullptr) {
        Node *node = q.front();
        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        q.pop();
        for (std::pair<GameState, Action> pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;
            if (root.hasState(state))
                continue;
            Node *newChild = new Node(state, action);
            node->addChild(newChild);
            q.push(newChild);
            nodeCount++;
            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
        // std::cout << "Node count: " << nodeCount << std::endl;
    }
    std::vector<Action> result;
    if (final != nullptr) {
        Node *current = final;
        
        // result.push_back(current->lastAction);
        while (current->parent != nullptr) {
            result.push_back(current->lastAction);
            current = current->parent;
        }
        std::reverse(result.begin(), result.end());
    }
    return result;
}


std::vector<Action> solveDFS(const GameState &initState, const GameBoard &board) {
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    std::stack<Node*> stack;
    stack.push(&root);
    Node *final = nullptr;
    int nodeCount = 1;
    while (!stack.empty() && final == nullptr) {
        Node *node = stack.top();
        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        stack.pop();
        for (std::pair<GameState, Action> pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;
            if (root.hasState(state))
                continue;
            Node *newChild = new Node(state, action);
            node->addChild(newChild);
            stack.push(newChild);
            nodeCount++;
            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
        // std::cout << "Node count: " << nodeCount << std::endl;
    }
    std::vector<Action> result;
    if (final != nullptr) {
        Node *current = final;
        
        // result.push_back(current->lastAction);
        while (current->parent != nullptr) {
            result.push_back(current->lastAction);
            current = current->parent;
        }
        std::reverse(result.begin(), result.end());
    }
    return result;
}

std::vector<Action> solveGreedy(const GameState &initState, const GameBoard &board) {
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    iVector2 goal = findGoalPos(board);
    std::priority_queue<Node*, std::vector<Node*>, GreedyCompare> p((GreedyCompare(goal)));
    p.push(&root);
    Node *final = nullptr;
    int nodeCount = 1;
    while (!p.empty() && final == nullptr) {
        Node *node = p.top();
        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        p.pop();
        for (std::pair<GameState, Action> pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;
            if (root.hasState(state))
                continue;
            Node *newChild = new Node(state, action);
            node->addChild(newChild);
            p.push(newChild);
            nodeCount++;
            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
        // std::cout << "Node count: " << nodeCount << std::endl;
    }
    std::vector<Action> result;
    if (final != nullptr) {
        Node *current = final;
        
        // result.push_back(current->lastAction);
        while (current->parent != nullptr) {
            result.push_back(current->lastAction);
            current = current->parent;
        }
        std::reverse(result.begin(), result.end());
    }
    return result;
}
