#include "gtest/gtest.h"

#include "board.h"

class BoardTest : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        startBoard = initializeBoard();
    }

    virtual void TearDown()
    {
        delete startBoard;
    }

    t_board *startBoard;
    char startBoardAsFen[44] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    char wrongBoardAsFen[44] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/ZZZZZZZZ";
};


TEST_F(BoardTest, setFenStartBoard) {

    t_board *board = initializeBoard();
    setFen(board, startBoardAsFen);

    EXPECT_EQ(startBoard->black, board->black);
    //EXPECT_ANY_THROW(setFen(board, wrongBoardAsFen)); // no check of wrong input, but not so important

}

TEST_F(BoardTest, getFenStartBoard) {

    char* fen = getFen(startBoard);

    EXPECT_STREQ(fen, startBoardAsFen);
}
