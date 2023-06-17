#ifndef KINGOFTHEHILL_KI_END_H
#define KINGOFTHEHILL_KI_END_H


#include "moveMaps.h"
#include "game.h"


typedef enum {
    NOTOVER,
    WHITE,
    BLACK,
    DRAW
} winner_t;


inline static bool isCheckmate(bool color, t_gameState *state) {
    /* Function to check whether the now moving party is checkmate
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool color: the next moving color with "false" for white and "true" for black
    */

    t_board board = state->board;

    if (color) {
        bool kingThreatened = board.blackKing & getThreatenedBlack(board);
        if (kingThreatened) {
            return generate_moves<true>(*state).empty();  // King is threatened and there are no moves
        }
    } else {
        bool kingThreatened = board.whiteKing & getThreatenedWhite(board);
        if (kingThreatened) {
            return generate_moves<false>(*state).empty();  // King is threatened and there are no moves
        }
    }

    return false;
}

inline static bool isStalemate(bool color, t_gameState *state) {
    /* Function to check whether the party whose turn is now can move. If not the game is stalemate-> draw
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool color: the next moving color with "false" for white and "true" for black
    */

    if (color) {
        return generate_moves<true>(*state).empty();
    } else {
        return generate_moves<false>(*state).empty();
    }
}


inline static bool isKingOfTheHill(bool color, t_board board) {
    /* Function to check whether King of the Hill condition is fulfilled
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the gameOld
     *  bool color: the last moved color with "false" for white and "true" for black
     */

    if (color) {
        return (board.blackKing & kingOfTheHill) != 0;
    } else {
        return (board.whiteKing & kingOfTheHill) != 0;
    }
}


inline static winner_t checkEndLimited(bool color, t_gameState *state) {
    // Check for game end while ignoring possible ends that can also be detected with empty move-generator lists
    // NOTE: color = side that last moved

    if (isKingOfTheHill(color, state->board)) {
        if (color) {
            return BLACK;
        } else {
            return WHITE;
        }
    }

    return NOTOVER;
}


inline static winner_t checkEndNoMoves(bool color, t_gameState *state) {
    // Check for game end while assuming that no more moves were found
    // NOTE: color = side that last moved
    t_board board = state->board;
    if (!color) {
        if (getThreatenedWhite(board) & board.blackKing) {
            return WHITE;
        }
        return DRAW;
    } else {
        if (getThreatenedBlack(board) & board.whiteKing) {
            return BLACK;
        }
        return DRAW;
    }
}


inline static winner_t checkEnd(bool color, t_gameState *state) {
    /* Function to check whether one of the game ending conditions is fulfilled and Returns the winner or DRAW. Given is the last moved color
     * tho check if the last action finished the game.
     * Arguments:
     *  t_game *game: Pointer to the game representing the state of the game
     *  bool moved_color: the last moved color with "false" for white and "true" for black
     */


    if (isKingOfTheHill(color, state->board)) {
        return color ? BLACK : WHITE;
    } else if (isCheckmate(!color, state)) {
        return color ? BLACK : WHITE;
    } else if (isStalemate(color, state)) {
        return DRAW;
    }

    return NOTOVER;
}


#endif //KINGOFTHEHILL_KI_END_H
