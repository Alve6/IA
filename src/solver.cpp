#include "solver.h"


#include <queue>
#include <stack>
#include <algorithm>
#include <set>

struct Node {
    Node *parent;
    GameState state;
    // Action that lead to the current state. Doesn't make sense in the root node, i guess
    Action lastAction;
    std::vector<Node *> children;
    // For IDS
    int depth;

    Node(const GameState &state, const Action &lastAction) {
        parent = nullptr;
        this->state = state;
        this->lastAction = lastAction;
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
    std::set<GameState> visitedStates;
    q.push(&root);
    visitedStates.insert(initState);
    Node *final = nullptr;
    int nodeCount = 1;
    while (!q.empty() && final == nullptr) {
        Node *node = q.front();
        std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
        q.pop();
        for (std::pair<GameState, Action> pair : nextStates) {
            GameState state = pair.first;
            Action action = pair.second;
            // if (root.hasState(state))
            //     continue;
            if (visitedStates.find(state) != visitedStates.end())
                continue;
            visitedStates.insert(state);
            Node *newChild = new Node(state, action);
            node->addChild(newChild);
            q.push(newChild);
            nodeCount++;
            if (isWinningState(state, board)) {
                final = newChild;
                break;
            }
        }
    }
    std::cout << "Solution tree node count: " << nodeCount << std::endl;
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

// At depth limit of 10 and no viable solution, it takes about 5 second on my machine
const int IDS_MAX_DEPTH = 10;
std::vector<Action> solveIDS(const GameState &initState, const GameBoard &board) {
    Node *final = nullptr;
    Node root(initState, {ROBOT_BLUE, DIR_INVALID});
    root.depth = 0;
    for (int maxDepth = 1; maxDepth < IDS_MAX_DEPTH; maxDepth++) {
        
        std::stack<Node*> stack;
        stack.push(&root);
        while (!stack.empty() && final == nullptr) {
            Node *node = stack.top();
            stack.pop();
            int depth = node->depth;
            if (depth > maxDepth)
                continue;
            std::vector<std::pair<GameState, Action>> nextStates = getNextStates(node->state, board);
            for (std::pair<GameState, Action> pair : nextStates) {
                GameState state = pair.first;
                Action action = pair.second;
                if (root.hasState(state))
                    continue;
                Node *newChild = new Node(state, action);
                newChild->depth = depth + 1;
                node->addChild(newChild);
                stack.push(newChild);
                if (isWinningState(state, board)) {
                    final = newChild;
                    break;
                }
            }
            // std::cout << "Node count: " << nodeCount << std::endl;
        }
        if (final != nullptr)
            break;
        root.deleteChildren();
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