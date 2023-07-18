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
#include "pieceSquareTable.h"

#define EXPLORATION_CONSTANT 2

#define KING_VALUE 200000
#define QUEEN_VALUE 9
#define ROOK_VALUE 5
#define BISHOP_VALUE 3
#define KNIGHT_VALUE 3
#define PAWN_VALUE 1

inline float evaluateMonteCarlo(t_game *game) {
    // Simple approach to evaluating positions by taking a look at the available material
    if (game->isOver) {
        if (game->whiteWon) {
            // White won -> Return max value minus move counter to prioritize faster wins
            return std::numeric_limits<float>::max() - (float) game->moveCounter;
        } else if (game->blackWon) {
            // Black won -> Return min value plus move counter to prioritize faster wins
            return -std::numeric_limits<float>::max() + (float) game->moveCounter;
        } else {
            // Draw -> Return neutral score, as neither side should get any scores out of it
            int sign = (game->turn * 2) - 1;
            return (float) sign / (float) (std::pow(game->moveCounter / 2, 2) + 1);
        }
    }

    float score = 0;

    score += (float) countFigure(game->board().whiteKing) * KING_VALUE;
    score += (float) countFigure(game->board().whiteQueen) * QUEEN_VALUE;
    score += (float) countFigure(game->board().whiteRook) * ROOK_VALUE;
    score += (float) countFigure(game->board().whiteBishop) * BISHOP_VALUE;
    score += (float) countFigure(game->board().whiteKnight) * KNIGHT_VALUE;
    score += (float) countFigure(game->board().whitePawn) * PAWN_VALUE;

    score -= (float) countFigure(game->board().blackKing) * KING_VALUE;
    score -= (float) countFigure(game->board().blackQueen) * QUEEN_VALUE;
    score -= (float) countFigure(game->board().blackRook) * ROOK_VALUE;
    score -= (float) countFigure(game->board().blackBishop) * BISHOP_VALUE;
    score -= (float) countFigure(game->board().blackKnight) * KNIGHT_VALUE;
    score -= (float) countFigure(game->board().blackPawn) * PAWN_VALUE;

    score *= 50;

    t_board board = game->board();

    // ------------ //
    // White pieces //
    // ------------ //

    // King
    short wKingShift = findFirst(board.whiteKing);
    score += (float) pst_king_white[wKingShift];

    // Queens
    field wQueens = board.whiteQueen;
    while (wQueens != 0) {
        short shift = findFirst(wQueens);
        score += pst_queen_white[shift];
        wQueens &= (wQueens - 1);
    }

    // Rooks
    field wRooks = board.whiteRook;
    while (wRooks != 0) {
        short shift = findFirst(wRooks);
        score += pst_rook_white[shift];
        wRooks &= (wRooks - 1);
    }

    // Bishops
    field wBishops = board.whiteBishop;
    while (wBishops != 0) {
        short shift = findFirst(wBishops);
        score += pst_bishop_white[shift];
        wBishops &= (wBishops - 1);
    }

    // Knight
    field wKnights = board.whiteKnight;
    while (wKnights != 0) {
        short shift = findFirst(wKnights);
        score += pst_knight_white[shift];
        wKnights &= (wKnights - 1);
    }

    // Pawns
    field wPawns = board.whitePawn;
    while (wPawns != 0) {
        short shift = findFirst(wPawns);
        score += pst_pawn_white[shift];
        wPawns &= (wPawns - 1);
    }


    // ------------ //
    // Black pieces //
    // ------------ //

    // King
    short bKingShift = findFirst(board.blackKing);
    score -= (float) pst_king_black[bKingShift];

    // Queens
    field bQueens = board.blackQueen;
    while (bQueens != 0) {
        short shift = findFirst(bQueens);
        score -= pst_queen_black[shift];
        bQueens &= (bQueens - 1);
    }

    // Rooks
    field bRooks = board.blackRook;
    while (bRooks != 0) {
        short shift = findFirst(bRooks);
        score -= pst_rook_black[shift];
        bRooks &= (bRooks - 1);
    }

    // Bishops
    field bBishops = board.blackBishop;
    while (bBishops != 0) {
        short shift = findFirst(bBishops);
        score -= pst_bishop_black[shift];
        bBishops &= (bBishops - 1);
    }

    // Knight
    field bKnights = board.blackKnight;
    while (bKnights != 0) {
        short shift = findFirst(bKnights);
        score -= pst_knight_black[shift];
        bKnights &= (bKnights - 1);
    }

    // Pawns
    field bPawns = board.blackPawn;
    while (bPawns != 0) {
        short shift = findFirst(bPawns);
        score -= pst_pawn_black[shift];
        bPawns &= (bPawns - 1);
    }

    // doubled pawns
    // white Pawns
    field wdPawns = board.whitePawn;
    while (wdPawns != 0) {
        short shift = findFirst(wdPawns);
        uint64_t mask = 0;
        int column = shift;

        for (int i = 0; i < 8; i++) {
            mask |= (1ULL << column);
            column += 8;
        }
        if (countFigure(board.whitePawn & mask) > 1) score -= 1;
        wdPawns &= (wdPawns - 1);
    }

    // doubled pawns
    // black Pawns
    field bdPawns = board.blackPawn;
    while (bdPawns != 0) {
        short shift = findFirst(bdPawns);
        uint64_t mask = 0;
        int column = shift;

        for (int i = 0; i < 8; i++) {
            mask |= (1ULL << column);
            column += 8;
        }
        if (countFigure(board.blackPawn & mask) > 1) score += 1;
        bdPawns &= (bdPawns - 1);
    }

    // isolated pawns
    // white Pawns
    field wiPawns = board.whitePawn;
    while (wiPawns != 0) {
        short shift = findFirst(wiPawns);
        uint64_t mask = 0;
        int column = shift-1;

        for (int i = 0; i < 8; i++) {
            // left column
            mask |= (1ULL << column);
            column += 2;
            // right column
            mask |= (1ULL << column);
            column += 6;
        }
        if (countFigure(board.whitePawn & mask) > 0) score -= 1;
        wiPawns &= (wiPawns - 1);
    }

    // isolated pawns
    // black Pawns
    field biPawns = board.blackPawn;
    while (biPawns != 0) {
        short shift = findFirst(biPawns);
        uint64_t mask = 0;
        int column = shift;

        for (int i = 0; i < 8; i++) {
            // left column
            mask |= (1ULL << column);
            column += 2;
            // right column
            mask |= (1ULL << column);
            column += 6;
        }
        if (countFigure(board.blackPawn & mask) > 0) score += 1;
        biPawns &= (biPawns - 1);
    }

//    printMoveStack(game, 4);
//    printf("=> %6.4f\n", score);

    return score;
}


class Node {
    /// NOTE: Template parameter 'color' describes that MADE THE LAST MOVE (reverse of the next player)
private:
    Node *_parent;
    std::vector<Node *> _children;

    bool _leaf = true;

    t_game _game;
public:
    int timesVisited = 0;
    float score = 0;

    explicit Node(const t_game& game) : _game(game) {
        _parent = nullptr;
        _children = std::vector<Node *>();
    }

    Node(Node *parent, const t_game& game) : _game(game) {
        _parent = parent;
        _children = std::vector<Node *>();
    }

    ~Node() {
        if (_parent != nullptr) {
            // Unlink from parent node
            _parent->removeChild(this);
        }

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

    void addChild(Node *child) {
        _children.push_back(child);

        _leaf = false;
    }

    void removeChild(Node *child) {
        /// NOTE: This method only removes the node from the _children vector, but does not deallocate it
        auto childNodeIndex = std::find(_children.begin(), _children.end(), child);
        _children.erase(childNodeIndex);

        if (_children.empty()) {
            _leaf = true;
        }
    }

    bool isLeaf() const {
        return _leaf;
    }

    float ucb(bool color) const {
        if (!_parent->timesVisited || !timesVisited) {
            return (float )(1 - (2 * color)) / 0.0f;
        }
        return (float )score / ((float )timesVisited) + EXPLORATION_CONSTANT * sqrt(log(_parent->timesVisited) / (timesVisited)) * (1 - (2 * color));
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
            Node *bestNode = nullptr;

            for (auto currentNode : node->children()) {
                current_ucb = currentNode->ucb(_color);
                if (current_ucb >= best_ucb) {
                    bestNode = currentNode;
                    best_ucb = current_ucb;
                }
            }

            return bestNode;
        } else {
            // "Optimize" for black
            float best_ucb = 1.0f / 0.0f;
            float current_ucb;
            Node *bestNode = nullptr;

            for (auto currentNode : node->children()) {
                current_ucb = currentNode->ucb(_color);
                if (current_ucb <= best_ucb) {
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
            node->addChild(newNode);
        }
    }

    static void addRandom(Node *node) {
        std::vector<t_gameState> possibleMoves;
        if (node->game()->turn) {
            possibleMoves = generate_moves<true>(*node->game()->state);
        } else {
            possibleMoves = generate_moves<false>(*node->game()->state);
        }

        if (possibleMoves.empty()) {
            return;
        }

        int randomMoveIndex = randn(0, (int )possibleMoves.size());
        t_gameState randomMove = possibleMoves.at(randomMoveIndex);

        // Create child node with copy of current game, then commit the selected move on the new node
        Node *newNode = new Node(node, *node->game());
        newNode->game()->commitMove(randomMove);

        // Append new node to the current leafNode as child
        node->addChild(newNode);
    }

    float rollout(Node *leafNode, int depth) {
        if (depth == 0) {
            return evaluateMonteCarlo(leafNode->game());
        }

        /// NOTE: Parameter leafNode must be a leaf. The program will not exit if not, but may cause unexpected behaviour
        // Check for winner for move-independent cases
        winner_t winner = checkEndLimited(leafNode->game()->turn, leafNode->game()->state);
//        if (!winner) {}
//        else if (winner == winner_t::DRAW) {
//            return {leafNode, 0};
//        } else if (winner == winner_t::WHITE) {
//            return {leafNode, 1};
//        } else if (winner == winner_t::BLACK) {
//            return {leafNode, -1};
//        }
        if (winner) {
            return evaluateMonteCarlo(leafNode->game());
        }

        MonteCarloTree::addRandom(leafNode);

        // Check for winner if the node is still a leaf (no moves are available)
        if (leafNode->isLeaf()) {
            winner = checkEndNoMoves(leafNode->game()->turn, leafNode->game()->state);

//            if (winner == winner_t::DRAW) {
//                return {leafNode, 0};  // TODO: Return something other than 0?
//            } else if (winner == winner_t::WHITE) {
//                return {leafNode, 1};
//            } else if (winner == winner_t::BLACK) {
//                return {leafNode, -1};
//            }
            if (winner) {
                return evaluateMonteCarlo(leafNode->game());
            }
        }

        int nextNodeIndex = randn(0, (int )leafNode->children().size());
        Node *nextNode = leafNode->children().at(nextNodeIndex);

        return rollout(nextNode, depth - 1);
    }

    static void propagate(Node *finalNode, float result) {
        /// Propagate the move sequence from the final (leaf) node up to the root and update the UCBs
        Node *currentNode = finalNode;
        while (currentNode->parent() != nullptr) {
            currentNode->score += result;
            currentNode->timesVisited++;

            currentNode = currentNode->parent();
        }
    }

    // TODO: Write save function and load constructor
};

#endif //KINGOFTHEHILL_KI_MONTECARLOTREE_H
