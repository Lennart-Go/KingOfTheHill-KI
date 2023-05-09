#include "gtest/gtest.h"

#include "move.h"
#include "board.h"
#include "util.h"

class MoveTest : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        board = initializeBoard();
    }

    virtual void TearDown()
    {
        delete board;
    }

    t_board *board;
};


/*
 * FUNCTION TESTS FOR empty_between
 */
TEST_F(MoveTest, emptyBetweenWhiteVerticalValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(0, 2)),
                   0, 0};

    EXPECT_TRUE(empty_between(board, move));
}

TEST_F(MoveTest, emptyBetweenWhiteVerticalInvalid) {
    setFen(board, (char *)"8/8/8/8/8/8/P7/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(0, 2)),
                   0, 0};

    EXPECT_FALSE(empty_between(board, move));
}

TEST_F(MoveTest, emptyBetweenDiagonalValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 2)),
                   0, 0};

    EXPECT_TRUE(empty_between(board, move));
}

TEST_F(MoveTest, emptyBetweenDiagonalInvalid) {
    setFen(board, (char *)"8/8/8/8/8/8/1P6/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 2)),
                   0, 0};

    EXPECT_FALSE(empty_between(board, move));
}

TEST_F(MoveTest, emptyBetweenHorizontalValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    EXPECT_TRUE(empty_between(board, move));
}

TEST_F(MoveTest, emptyBetweenHorizontalInvalid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/QP6");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    EXPECT_FALSE(empty_between(board, move));
}


/*
 * FUNCTION TESTS FOR is_move_legal_nocheck
 */
TEST_F(MoveTest, isMoveLegalNocheckTargetOccupied) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q1P5");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;
    bool checkBetweenDefault = false;

    EXPECT_FALSE(is_move_legal_nocheck(board, move, whiteColorFilter, blackColorFilter, checkBetweenDefault));
}

TEST_F(MoveTest, isMoveLegalNocheckCheckBetweenEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal_nocheck(board, move, whiteColorFilter, blackColorFilter, true));
}

TEST_F(MoveTest, isMoveLegalNocheckCheckBetweenNotEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/QP6");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_FALSE(is_move_legal_nocheck(board, move, whiteColorFilter, blackColorFilter, true));
}

TEST_F(MoveTest, isMoveLegalNocheckNoCheckBetweenEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal_nocheck(board, move, whiteColorFilter, blackColorFilter, false));
}

TEST_F(MoveTest, isMoveLegalNocheckNoCheckBetweenNotEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/QP6");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal_nocheck(board, move, whiteColorFilter, blackColorFilter, false));
}

TEST_F(MoveTest, isMoveLegalNocheckValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal_nocheck(board, move, whiteColorFilter, blackColorFilter, true));
}


/*
 * FUNCTION TESTS FOR is_threatened WITH WHITE PIECES
 */
// TODO


/*
 * FUNCTION TESTS FOR is_threatened WITH BLACK PIECES
 */

