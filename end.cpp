#include "end.h"
#include "move.h"


// TODO: Check for checkmate
// TODO: Check for king of the hill
// TODO: Check for draw



bool isCheckmate(t_board* board, bool moving_color){
    uint64_t color = moving_color? board->black : board->white;
    uint64_t King = board->king & color;
    Position kingPosition = Position( (int)(King%8),(int)((King-(King%8))/8));
    if(is_threatened(board, kingPosition, moving_color)){
        List<t_move> possibleMoves = generate_moves(board,moving_color);
        for(int i=0;i<possibleMoves.length();i++){
            t_move currentMove = possibleMoves.get(i);
            if(currentMove.origin==King){ //0 for King
                continue;
            }else{
                if(!is_check(board, currentMove)){
                    return false;
                }
            }
        }
        return true;
    }

};

bool isStalemate(t_board* board, bool moving_color){

    List<t_move> possibleMoves = generate_moves(board,moving_color);
    if(possibleMoves.length() < 1){
        return true;
    }else{
        return false;
    }
};

bool isInsufficientMatingMaterial(t_board* board){

    return false;
};

bool KingOfTheHill(t_board* board, bool moved_color){
    uint64_t color = moved_color? board->black : board->white;
    uint64_t King = board->king & color;
    Position kingPosition = Position( (int)(King%8),(int)((King-(King%8))/8));
    if(kingPosition.x >= 3 && kingPosition.x <= 4 && kingPosition.y >= 3 && kingPosition.y <= 4){
        return true;
    }else{
        return false;
    }
};

winner_t checkEnd(t_board* board, bool moved_color) {
    /* Function to check wether one of the game ending conditions is fullfilled and Returns the winner or DRAW. Given is the last moved color
     * tho check if the last action finsished the game.
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  bool moved_color: the last moved color with "false" for white and "true" for black
     */

    if(KingOfTheHill(board,moved_color)){
        return moved_color ? BLACK : WHITE;
    }else

    if(isCheckmate(board, !moved_color)) {
        return moved_color ? BLACK : WHITE;
    }else

    if(isStalemate(board,!moved_color)){
        return DRAW;
    }else

    if(isInsufficientMatingMaterial(board)){
        return DRAW;
    }else {
        return NOTOVER;
    }



}