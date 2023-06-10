#include "end.h"
#include "move.h"
#include <math.h>
#include <map>
#include <util.h>


int countFigure(uint64_t singleBoard) {
    int count = 0;
    for (int i = 0; i < 64; i++) {
        if (singleBoard & ((uint64_t) 1 << i)) {
            count++;
        }
    }
    return count;
}

bool positionTracking(t_game *game) {
    /* Function to check if same position occurred for the third time
    * Arguments:
    *  t_game *game: Pointer to the game representing the state of the game
    */
    uint64_t boardHash = hash(game->random,game);
    std::map<uint64_t , int> &map = *game->positionHistory;
    bool positionRepetitionDraw = false;

    if (game->positionHistory == nullptr) {
        std::map<uint64_t , int> *mapInit = new std::map<uint64_t , int>();
        game->positionHistory = mapInit;
        (*mapInit).insert(std::pair<uint64_t ,int>(hash(game->random,game), 1));
    } else {
        if (map.find(boardHash) != map.end()) {
            int n = map[boardHash];
            map[boardHash] = n + 1;
            if (map[boardHash] > 2) {
                positionRepetitionDraw = true;
            }
        } else {
            map.insert(std::make_pair(boardHash, 1));
        }
    }

    return positionRepetitionDraw;
}

void positionTrackingUndo(t_game *game) {
    uint64_t comparableBoard = hash(game->random,game);
    std::map<uint64_t , int> &map = *game->positionHistory;

    if (game->positionHistory != nullptr) {
        if (map.find(comparableBoard) != map.end()) {
            int n = map[comparableBoard];

            if (n > 0) {
                map[comparableBoard] = n - 1;
            }
        }
    }

}


bool isCheckmate(t_game *game, bool moving_color) {
    /* Function to check whether the now moving party is checkmate
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool moving_color: the next moving color with "false" for white and "true" for black
    */
    uint64_t color = moving_color ? game->board->black : game->board->white;
    uint64_t King = game->board->king & color;
    Position kingPosition = position_from_shift((King == 0) ? 0 : (int) log2((long double) King));
    if (is_threatened(game->board, kingPosition, moving_color)) {
        std::vector<t_move> possibleMoves = generate_moves(game, moving_color);
        for (auto currentMove : possibleMoves) {
            if (!is_move_check(game->board, currentMove)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool isStalemate(t_game *game, bool moving_color) {
    /* Function to check whether the party whose turn is now can move. If not the game is stalemate-> draw
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool moving_color: the next moving color with "false" for white and "true" for black
    */
    std::vector<t_move> possibleMoves = generate_moves(game, moving_color);
    if (possibleMoves.size() < 1) {
        return true;
    } else {
        return false;
    }
}

/*bool isInsufficientMatingMaterial(t_board* board){
    // lone king or king with two or fewer knights
    if((board->white&(board->knight|board->king)) > 0 && (board->white&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }
    if((board->black&(board->knight|board->king)) > 0 && (board->black&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }
    // king and one bishop
    if((board->king&board->white && board->white&board->bishop) >0 && (countFigure(board->bishop&board->white))<2 && (board->white&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }

    if((board->king&board->black && board->black&board->bishop) >0 && countFigure(board->bishop&board->black)<2 && (board->black&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }
    return false;
};*/

bool KingOfTheHill(t_board *board, bool moved_color) {
    /* Function to check whether King of the Hill condition is fulfilled
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  bool moved_color: the last moved color with "false" for white and "true" for black
     */
    uint64_t color = moved_color ? board->black : board->white;
    uint64_t King = board->king & color;
    Position kingPosition = position_from_shift((King == 0) ? 0 : (int) log2((long double) King));
    if (kingPosition.x >= 3 && kingPosition.x <= 4 && kingPosition.y >= 3 && kingPosition.y <= 4) {
        return true;
    } else {
        return false;
    }
}

winner_t checkEnd(t_game *game, bool moved_color) {
    /* Function to check whether one of the game ending conditions is fulfilled and Returns the winner or DRAW. Given is the last moved color
     * tho check if the last action finished the game.
     * Arguments:
     *  t_game *game: Pointer to the game representing the state of the game
     *  bool moved_color: the last moved color with "false" for white and "true" for black
     */


    if (KingOfTheHill(game->board, moved_color)) {
        return moved_color ? BLACK : WHITE;
    } else if (isCheckmate(game, !moved_color)) {
        return moved_color ? BLACK : WHITE;
    } else if (isStalemate(game, !moved_color)) {
        return DRAW;
    } else if (positionTracking(game)) {
        return DRAW;
    }

    /*if(isInsufficientMatingMaterial(board)){
        return DRAW;
    }else*/ {
        return NOTOVER;
    }


}
