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

bool positionTracking(t_game* game){
    /* Function to check if same position occurred for the fifth time
    * Arguments:
    *  t_game *game: Pointer to the game representing the state of the game
    */
    std::map<std::string,int> &map = *game->positionHistory;
    char* currentFen = getFen(game->board);
    bool positionRepetionDraw = false;

    if(game->positionHistory == NULL){
        std::map<std::string,int>* mapInit = new std::map<std::string,int>();
        game->positionHistory = mapInit;
        (*mapInit).insert(std::make_pair(currentFen,1));
    }else{
        if(map.find(currentFen) != map.end()){
            map[currentFen] = map[currentFen]++;
            if(map[currentFen]>4){
                positionRepetionDraw = true;
                delete game->positionHistory;
            }
        }else{
            map.insert(std::make_pair(currentFen,1));
        }
    }
    return positionRepetionDraw;
}


bool isCheckmate(t_game* game, bool moving_color){
    /* Function to check whether the now moving party is checkmate
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool moving_color: the next moving color with "false" for white and "true" for black
    */
    uint64_t color = moving_color? game->board->black : game->board->white;
    uint64_t King = game->board->king & color;
    Position kingPosition = position_from_shift((King==0) ? 0 : (int)log2((long double) King));
    printf("Position x:%d y:%d\n",kingPosition.x,kingPosition.y);
    if(is_threatened(game->board, kingPosition, moving_color)){
        List<t_move> possibleMoves = generate_moves(game,moving_color);
        for(int i=0;i<possibleMoves.length();i++){
            t_move currentMove = possibleMoves.get(i);

            if(!is_move_check(game->board, currentMove)){
                return false;
            }
        }
        return true;
    }
    return false;
}

bool isStalemate(t_game* game, bool moving_color){
    /* Function to check whether the party whichs turn is now can move. If not the game is stalemate-> draw
    * Arguments:
    *  t_board *board: Pointer to the board representing the state of the game
    *  bool moving_color: the next moving color with "false" for white and "true" for black
    */
    List<t_move> possibleMoves = generate_moves(game,moving_color);
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
    /* Function to check whether King of the Hill condition is fullfilled
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  bool moved_color: the last moved color with "false" for white and "true" for black
     */
    uint64_t color = moved_color? board->black : board->white;
    uint64_t King = board->king & color;
    Position kingPosition = position_from_shift((King==0) ? 0 : (int)log2((long double) King));
    if(kingPosition.x >= 3 && kingPosition.x <= 4 && kingPosition.y >= 3 && kingPosition.y <= 4){
        return true;
    }else{
        return false;
    }
};

winner_t checkEnd(t_game* game, bool moved_color) {
    /* Function to check wether one of the game ending conditions is fullfilled and Returns the winner or DRAW. Given is the last moved color
     * tho check if the last action finsished the game.
     * Arguments:
     *  t_game *game: Pointer to the game representing the state of the game
     *  bool moved_color: the last moved color with "false" for white and "true" for black
     */


    if(KingOfTheHill(game->board,moved_color)){
        delete game->positionHistory;
        return moved_color ? BLACK : WHITE;
    }else

    if(isCheckmate(game, !moved_color)) {
        delete game->positionHistory;
        return moved_color ? BLACK : WHITE;
    }else

    if(isStalemate(game,!moved_color)){
        delete game->positionHistory;
        return DRAW;
    }else

    if(positionTracking(game)){
        return DRAW;
    }

    /*if(isInsufficientMatingMaterial(board)){
        return DRAW;
    }else*/ {
        return NOTOVER;
    }




}
