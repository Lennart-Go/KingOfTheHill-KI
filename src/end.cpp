#include "end.h"
#include "move.h"
#include <math.h>
#include <map>



bool positionTracking(t_game *game) {
    /* Function to check if same position occurred for the third time
    * Arguments:
    *  t_game *game: Pointer to the game representing the state of the game
    */
    std::map<std::string, int> &map = *game->positionHistory;
    char *currentFen = getFen(game->board);
    bool positionRepetitionDraw = false;

    if (game->positionHistory == nullptr) {
        std::map<std::string, int> *mapInit = new std::map<std::string, int>();
        game->positionHistory = mapInit;
        (*mapInit).insert(std::make_pair(currentFen, 1));
    } else {
        if (map.find(currentFen) != map.end()) {
            int n = map[currentFen];
            map[currentFen] = n + 1;
            if (map[currentFen] > 2) {
                positionRepetitionDraw = true;
            }
        } else {
            map.insert(std::make_pair(currentFen, 1));
        }
    }

    free(currentFen);
    return positionRepetitionDraw;
}

void positionTrackingUndo(t_game *game) {
    std::map<std::string, int> &map = *game->positionHistory;
    char *currentFen = getFen(game->board);

    if (game->positionHistory != nullptr) {
        if (map.find(currentFen) != map.end()) {
            int n = map[currentFen];

            if (n > 0) {
                map[currentFen] = n - 1;
            }
        }
    }

    free(currentFen);
}


//bool isCheckmate(t_game *game, bool moving_color) {
//    /* Function to check whether the now moving party is checkmate
//    * Arguments:
//    *  t_board *board: Pointer to the board representing the state of the game
//    *  bool moving_color: the next moving color with "false" for white and "true" for black
//    */
//    field color = moving_color ? game->board->black : game->board->white;
//    field King = moving_color ? game->board->blackKing : game->board->whiteKing;
//    Position kingPosition = position_from_shift((King == 0) ? 0 : (int) log2((long double) King));
//    if (is_threatened(game->board, kingPosition, moving_color)) {
//        std::vector<t_move_old> possibleMoves = generate_moves(game, moving_color);
//        for (auto currentMove : possibleMoves) {
//            if (!is_move_check(game->board, currentMove)) {
//                return false;
//            }
//        }
//        return true;
//    }
//    return false;
//}

//bool isStalemate(t_game *game, bool moving_color) {
//    /* Function to check whether the party whose turn is now can move_old. If not the game is stalemate-> draw
//    * Arguments:
//    *  t_board *board: Pointer to the board representing the state of the game
//    *  bool moving_color: the next moving color with "false" for white and "true" for black
//    */
//    std::vector<t_move_old> possibleMoves = generate_moves(game, moving_color);
//    if (possibleMoves.size() < 1) {
//        return true;
//    } else {
//        return false;
//    }
//}

/*bool isInsufficientMatingMaterial(t_board *board){
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
    field color = moved_color ? board->black : board->white;
    field King = moved_color ? board->blackKing : board->whiteKing;
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
