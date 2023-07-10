//
// Created by mbienert on 7/9/23.
//

#ifndef KINGOFTHEHILL_KI_MONTECARLOTREE_H
#define KINGOFTHEHILL_KI_MONTECARLOTREE_H

#include <utility>
#include <vector>
#include <valarray>
#include "game.h"
#include "end.h"

#define EXPLORATION_CONSTANT 2


class Node {
    /// NOTE: Template parameter 'color' describes that MADE THE LAST MOVE (reverse of the next player)
private:
    Node *_parent;
    std::vector<Node *> _children;

    bool _leaf = true;

    t_game _game;
public:
    int timesVisited = 0;
    int timesWon = 0;

    explicit Node(const t_game& game) : _game(game) {
        _parent = nullptr;
        _children = std::vector<Node *>();
    }

    Node(Node *parent, const t_game& game) : _game(game) {
        _parent = parent;
        _children = std::vector<Node *>();
    }

    ~Node() {
        // Recursively delete all children
        for (auto child : _children) {
            delete child;
        }
    }

    Node *parent() {
        return _parent;
    }

    void unsetParent() {
        _parent = nullptr;
    }

    std::vector<Node *> children() {
        return _children;
    }

    bool isLeaf() const {
        return _leaf;
    }

    void addChild(Node *child) {
        _children.push_back(child);

        _leaf = false;
    }

    void addTimesWon(int value) {
        timesWon += (!_game.turn * -1) * value;
    }

    float ucb(bool color) const {
        return (float )timesWon / (float )timesVisited + EXPLORATION_CONSTANT * sqrt(log(_parent->timesVisited) / timesVisited) * (color * -1);
    }

    t_game *game() {
        return &_game;
    }
};

class MonteCarloTree {
private:
    Node *_root;
    bool _color;
public:
    explicit MonteCarloTree(const t_game& initialGame) {
        _root = new Node(initialGame);
        _color = initialGame.turn;
    }

    explicit MonteCarloTree(Node *newRoot) {
        /// Note: This constructor assumes, that the newRoot is EXACTLY one layer below the root of its former tree
        _root = newRoot;
        _color = _root->game()->turn;

        // Remove new root from former root's children index to avoid accidental deallocation
        std::vector<Node *> parent_children = newRoot->parent()->children();
        auto positionIndex = std::find(parent_children.begin(), parent_children.end(), newRoot);
        parent_children.erase(positionIndex);

        newRoot->unsetParent();
    }

    ~MonteCarloTree() {
        delete _root;
    }

    Node *root() const {
        return _root;
    }

    float ucb(Node *node) const {
        return node->ucb(_color);
    }

    Node *select(Node *node) const {
        if (node->isLeaf()) {
            return nullptr;
        }

        if (!_color) {
            // "Optimize" for white
            float best_ucb = -1.0f / 0.0f;
            float current_ucb;
            Node *bestNode;

            for (auto currentNode : node->children()) {
                current_ucb = currentNode->ucb(_color);
                if (current_ucb > best_ucb) {
                    bestNode = currentNode;
                    best_ucb = current_ucb;
                }
            }

            return bestNode;
        } else {
            // "Optimize" for black
            float best_ucb = 1.0f / 0.0f;
            float current_ucb;
            Node *bestNode;

            for (auto currentNode : node->children()) {
                current_ucb = currentNode->ucb(_color);
                if (current_ucb < best_ucb) {
                    bestNode = currentNode;
                    best_ucb = current_ucb;
                }
            }

            return bestNode;
        }
    }

    Node *traverse(Node *node) {
        /// Traverse the tree by selecting the "best" node until a leaf is reached
        if (node->isLeaf()) {
            return node;
        }

        Node *nextNode = select(node);
        return traverse(nextNode);
    }

    static void expand(Node *node) {
        // Generate all possible moves for the current player
        std::vector<t_gameState> possibleMoves;
        if (node->game()->turn) {
            possibleMoves = generate_moves<true>(*node->game()->state);
        } else {
            possibleMoves = generate_moves<false>(*node->game()->state);
        }

        // Generate child node for each move
        for (auto move : possibleMoves) {
            // Create child node with copy of current game, then commit the move on the new node
            Node *newNode = new Node(node, *node->game());
            newNode->game()->commitMove(move);

            // Append new node to the current leafNode as child
            node->children().push_back(newNode);
        }
    }

    std::pair<Node *, int> rollout(Node *leafNode) {
        /// NOTE: Parameter leafNode must be a leaf. The program will not exit if not, but may cause unexpected behaviour
        // Check for winner for move-independent cases
        winner_t winner = checkEndLimited(leafNode->game()->turn, leafNode->game()->state);
        if (!winner) {}
        else if (winner == winner_t::DRAW) {
            return {leafNode, 0};
        } else if (winner == winner_t::WHITE) {
            return {leafNode, 1};
        } else if (winner == winner_t::BLACK) {
            return {leafNode, -1};
        }

        MonteCarloTree::expand(leafNode);

        // Check for winner if no moves are available
        if (leafNode->children().empty()) {
            winner = checkEndNoMoves(leafNode->game()->turn, leafNode->game()->state);

            if (winner == winner_t::DRAW) {
                return {leafNode, 0};  // TODO: Return something other than 0?
            } else if (winner == winner_t::WHITE) {
                return {leafNode, 1};
            } else if (winner == winner_t::BLACK) {
                return {leafNode, -1};
            }
        }

        int nextNodeIndex = randn(0, (int )leafNode->children().size());
        Node *nextNode = leafNode->children().at(nextNodeIndex);

        return rollout(nextNode);
    }

    static void propagate(Node *finalNode, int result) {
        /// Propagate the move sequence from the final (leaf) node up to the root and update the UCBs
        Node *currentNode = finalNode;
        while (currentNode->parent() != nullptr) {
            currentNode->timesVisited++;
            currentNode->timesWon += result;
        }
    }

    static void propagate(std::pair<Node *, int> values) {
        propagate(values.first, values.second);
    }
};

#endif //KINGOFTHEHILL_KI_MONTECARLOTREE_H
