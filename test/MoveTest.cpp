#include "gtest/gtest.h"

#include "move.h"
#include "board.h"
#include "util.h"

class MoveTest : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        startBoard = initializeBoard();

        e2_e4.origin = 52;
        e2_e4.target = 36;
        e2_e4.color = 0;
    }

    virtual void TearDown()
    {
        delete startBoard;
    }

    t_board *startBoard;
    t_move e2_e4;

    char startBoardAsFen[44] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    char e4BoardAsFen[46] = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR";

};


TEST_F(MoveTest, generateMovesStartBoard) {

    List<t_move> moves = generate_moves(startBoard, false);

    EXPECT_EQ(20, moves.length());
}



TEST_F(MoveTest, doMove) {

    t_board *board = initializeBoard();

    // play e4
    doMove(board, &e2_e4);

    EXPECT_STREQ(getFen(board), e4BoardAsFen);

}

TEST_F(MoveTest, undoMove) {

    // e4 back to e2

    t_board *board = initializeBoard();

    setFen(board, e4BoardAsFen);

    // undo e4
    undoMove(board, &e2_e4);

    EXPECT_STREQ(getFen(board), startBoardAsFen);
}

