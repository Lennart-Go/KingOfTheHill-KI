#include "end.h"
#include "move.h"
#include <math.h>
#include <map>

// TODO: Check for checkmate
// TODO: Check for king of the hill
// TODO: Check for draw

/*int countFigure(uint64_t singleBoard){
    int count=0;
    for(int i=0;i<=64;i++){
        if(singleBoard & (1<<i)){
            count++;
        }
    }
    return count;
}*/

std::map<std::string,int>* initMoveTracker(){
    std::map<std::string,int> map;
    return &map;
}


bool isCheckmate(t_board* board, bool moving_color){
    uint64_t color = moving_color? board->black : board->white;
    uint64_t King = board->king & color;
    Position kingPosition = position_from_shift((King==0) ? 0 : (int)log2((long double) King));
    printf("Position x:%d y:%d\n",kingPosition.x,kingPosition.y);
    if(is_threatened(board, kingPosition, moving_color)){
        List<t_move> possibleMoves = generate_moves(board,moving_color);
        for(int i=0;i<possibleMoves.length();i++){
            t_move currentMove = possibleMoves.get(i);

            if(!is_check(board, currentMove)){
                return false;
            }
        }
        return true;
    }
    return false;
}

bool isStalemate(t_board* board, bool moving_color){

    List<t_move> possibleMoves = generate_moves(board,moving_color);
    if(possibleMoves.length() < 1){
        return true;
    }else{
        return false;
    }
};

/*bool isInsufficientMatingMaterial(t_board* board){
    //lone king or king with two ore less knights
    if((board->white&(board->knight|board->king)) > 0 && (board->white&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }
    if((board->black&(board->knight|board->king)) > 0 && (board->black&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }
    //king and one bishop
    if((board->king&board->white && board->white&board->bishop) >0 && (countFigure(board->bishop&board->white))<2 && (board->white&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }

    if((board->king&board->black && board->black&board->bishop) >0 && countFigure(board->bishop&board->black)<2 && (board->black&(board->queen|board->rook|board->bishop|board->pawn))==0){
        return true;
    }
    return false;
};*/

bool KingOfTheHill(t_board* board, bool moved_color){
    uint64_t color = moved_color? board->black : board->white;
    uint64_t King = board->king & color;
    Position kingPosition = position_from_shift((King==0) ? 0 : (int)log2((long double) King));
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

    /*if(isInsufficientMatingMaterial(board)){
        return DRAW;
    }else*/ {
        return NOTOVER;
    }




}
