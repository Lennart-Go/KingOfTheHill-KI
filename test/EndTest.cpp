#include "gtest/gtest.h"

#include "end.h"
#include "util.h"

class EndTest : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        game = (t_game *) calloc(1, sizeof(t_game));
        t_board *board = initializeBoard();

        game->board = board;
        game->turn = 0;  // White's turn
        game->latestMoveTime = 0;  // TODO: Set latestMoveTime to current time in ms
        game->isOver = false;
        game->positionHistory = NULL;

        game->whiteWon = false;
        game->whiteCanCastleShort = true;
        game->whiteCanCastleLong = true;
        game->whiteCastled = false;
        game->whiteMoveCounter = 0;
        game->whiteMoveTime = 0;

        game->blackWon = false;
        game->blackCanCastleShort = true;
        game->blackCanCastleLong = true;
        game->blackCastled = false;
        game->blackMoveCounter = 0;
        game->blackMoveTime = 0;

    }

    virtual void TearDown()
    {
        free(game);
    }

    t_game *game;
};


TEST_F(EndTest, checkEndStartPosition) {
    setFen(game->board,(char*)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    EXPECT_FALSE(checkEnd(game, true));
}

TEST_F(EndTest, checkEndisCheckmateWHITE) {
    setFen(game->board,(char*)"rnbqk1nr/pppppppp/8/8/8/6b1/PPPPP1P1/RNBQKBNR");
    EXPECT_TRUE(checkEnd(game, true)==BLACK);
}

TEST_F(EndTest, checkEndisCheckmateBLACK) {
    setFen(game->board,(char*)"rnbqkbnr/ppppp1p1/6B1/8/8/6P1/PPPPPP1P/RNBQK1NR");
    EXPECT_TRUE(checkEnd(game, false)==WHITE);
}

TEST_F(EndTest, checkEndStalemateWHITE) {
    setFen(game->board,(char*)"8/8/8/8/8/3k4/3p4/3K4");
    EXPECT_TRUE(checkEnd(game, true)==DRAW);
}

TEST_F(EndTest, checkEndStalemateBLACK) {
    setFen(game->board,(char*)"3k4/3P4/3K4/8/8/8/8/8");
    EXPECT_TRUE(checkEnd(game, false)==DRAW);
}

TEST_F(EndTest, checkEndKingOfTheKillWHITE) {
    setFen(game->board,(char*)"rnbqk1nr/pppppppp/8/8/4K3/6b1/PPPPP1P1/RNBQ1BNR");
    EXPECT_TRUE(checkEnd(game, false)==WHITE);
}

TEST_F(EndTest, checkEndKingOfTheKillBLACK) {
    setFen(game->board,(char*)"rnbq2nr/pppppppp/8/2b1k3/8/8/PPPPP1P1/RNBQKBNR");
    EXPECT_TRUE(checkEnd(game, true)==BLACK);
}

TEST_F(EndTest, checkEndPositionRepetition) {
    t_move fw_w;
    fw_w.origin = shift_from_position(Position(7,0));
    fw_w.target = shift_from_position(Position(7,1));
    fw_w.taken_figure = 0;
    fw_w.color = false;
    t_move bw_w;
    bw_w.origin = shift_from_position(Position(7,1));
    bw_w.target = shift_from_position(Position(7,0));
    bw_w.taken_figure = 0;
    bw_w.color = false;
    t_move fw_b;
    fw_b.origin = shift_from_position(Position(0,7));
    fw_b.target = shift_from_position(Position(0,6));
    fw_b.taken_figure = 0;
    fw_b.color = true;
    t_move bw_b;
    bw_b.origin = shift_from_position(Position(0,6));
    bw_b.target = shift_from_position(Position(0,7));
    bw_b.taken_figure = 0;
    bw_b.color = true;


    setFen(game->board,(char*)"r3k3/8/8/8/8/8/8/3K3R");
    doMove(game->board,&fw_w);
    checkEnd(game, false);
    doMove(game->board,&fw_b);
    checkEnd(game, true);
    doMove(game->board,&bw_w);
    checkEnd(game, false);
    doMove(game->board,&bw_b);
    checkEnd(game, true);
    //first Repetition
    doMove(game->board,&fw_w);
    checkEnd(game, false);
    doMove(game->board,&fw_b);
    checkEnd(game, true);
    doMove(game->board,&bw_w);
    checkEnd(game, false);
    doMove(game->board,&bw_b);
    //second repetition
    checkEnd(game, true);
    doMove(game->board,&fw_w);
    EXPECT_TRUE(checkEnd(game, false)==DRAW);
    //third repetition
}





