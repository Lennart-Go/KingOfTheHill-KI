#include "gtest/gtest.h"

#include "move.h"
#include "board.h"
#include "util.h"


void setAllowCastle(t_game *game, bool value) {
    game->whiteCanCastleLong = value;
    game->whiteCanCastleShort = value;
    game->blackCanCastleLong = value;
    game->blackCanCastleShort = value;
}

void setEnpassant(t_game *game, int x) {
    game->enpassants = 0;
    game->enpassants |= (uint8_t )1 << x;
}

class MoveTest : public ::testing::Test {

protected:
    virtual void SetUp()
    {
        game = startGame();
        game->board = initializeBoard();
        board = initializeBoard();

        setAllowCastle(game, false);

        e2_e4.origin = 52;
        e2_e4.target = 36;
        e2_e4.color = 0;
    }

    virtual void TearDown()
    {
        delete game->board;
        delete game;
        delete board;
    }

    t_game *game;
    t_board *board;
    t_move e2_e4;

    char boardAsFen[44] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    char e4BoardAsFen[46] = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR";

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
TEST_F(MoveTest, isThreatenedByWhiteKing) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/8/1K6");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByWhiteKing) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/3K4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByWhiteQueen) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/8/1Q6");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByWhiteQueen) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/3Q4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByWhiteRook) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/8/1R6");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByWhiteRook) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/3R4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByWhiteBishop) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/1B6/8");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByWhiteBishop) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/3B4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByWhiteKnight) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/2N5/8");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByWhiteKnight) {
    Position target = Position(0, 0);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/3N4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByWhitePawn) {
    Position target = Position(0, 1);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/8/1P6");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByWhitePawn) {
    Position target = Position(0, 1);
    bool color = true;

    setFen(board, (char *)"8/8/8/8/8/8/8/P7");

    EXPECT_FALSE(is_threatened(board, target, color));
}


/*
 * FUNCTION TESTS FOR is_threatened WITH BLACK PIECES
 */
TEST_F(MoveTest, isThreatenedByBlackKing) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/8/1k6");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByBlackKing) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/3k4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByBlackQueen) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/8/1q6");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByBlackQueen) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/3q4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByBlackRook) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/8/1r6");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByBlackRook) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/3r4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByBlackBishop) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/1b6/8");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByBlackBishop) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/3b4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByBlackKnight) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/2n5/8");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByBlackKnight) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/3n4/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedByBlackPawn) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/1p6/8");

    EXPECT_TRUE(is_threatened(board, target, color));
}

TEST_F(MoveTest, isThreatenedNotByBlackPawn) {
    Position target = Position(0, 0);
    bool color = false;

    setFen(board, (char *)"8/8/8/8/8/8/p7/8");

    EXPECT_FALSE(is_threatened(board, target, color));
}


/*
 * FUNCTION TESTS FOR is_move_check
 */
TEST_F(MoveTest, isMoveCheckWhiteCheck) {
    t_move move = { 0, 0, 0, false, 0, 0 };

    setFen(board, (char *)"8/8/8/8/8/8/1q6/K7");

    EXPECT_TRUE(is_move_check(board, move));
}

TEST_F(MoveTest, isMoveCheckWhiteNoCheck) {
    t_move move = { 0, 0, 0, false, 0, 0 };

    setFen(board, (char *)"8/8/8/8/8/8/2q5/K7");

    EXPECT_FALSE(is_move_check(board, move));
}

TEST_F(MoveTest, isMoveCheckWhiteNoKing) {
    t_move move = { 0, 0, 0, false, 0, 0 };

    setFen(board, (char *)"8/8/8/8/8/8/1q6/8");

    EXPECT_FALSE(is_move_check(board, move));
}

TEST_F(MoveTest, isMoveCheckBlackCheck) {
    t_move move = { 0, 0, 0, true, 0, 0 };

    setFen(board, (char *)"8/8/8/8/8/8/1Q6/k7");

    EXPECT_TRUE(is_move_check(board, move));
}

TEST_F(MoveTest, isMoveCheckBlackNoCheck) {
    t_move move = { 0, 0, 0, true, 0, 0 };

    setFen(board, (char *)"8/8/8/8/8/8/2Q5/k7");

    EXPECT_FALSE(is_move_check(board, move));
}

TEST_F(MoveTest, isMoveCheckBlackNoKing) {
    t_move move = { 0, 0, 0, true, 0, 0 };

    setFen(board, (char *)"8/8/8/8/8/8/1Q6/8");

    EXPECT_FALSE(is_move_check(board, move));
}


/*
 * FUNCTION TESTS FOR is_move_legal
 */
TEST_F(MoveTest, isMoveLegalTargetOccupied) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q1P5");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;
    bool checkBetweenDefault = false;

    EXPECT_FALSE(is_move_legal(board, move, whiteColorFilter, blackColorFilter, checkBetweenDefault));
}

TEST_F(MoveTest, isMoveLegalCheckBetweenEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal(board, move, whiteColorFilter, blackColorFilter, true));
}

TEST_F(MoveTest, isMoveLegalCheckBetweenNotEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/QP6");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_FALSE(is_move_legal(board, move, whiteColorFilter, blackColorFilter, true));
}

TEST_F(MoveTest, isMoveLegalNoCheckBetweenEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal(board, move, whiteColorFilter, blackColorFilter, false));
}

TEST_F(MoveTest, isMoveLegalNoCheckBetweenNotEmptyBetween) {
    setFen(board, (char *)"8/8/8/8/8/8/8/QP6");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal(board, move, whiteColorFilter, blackColorFilter, false));
}

TEST_F(MoveTest, isMoveLegalIsCheck) {
    setFen(board, (char *)"8/8/8/8/8/q7/8/KQ6");
    t_move move = {(unsigned )shift_from_position(Position(1, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_FALSE(is_move_legal(board, move, whiteColorFilter, blackColorFilter, false));
}

TEST_F(MoveTest, isMoveLegalNoCheck) {
    setFen(board, (char *)"8/8/8/8/8/q7/8/KQ6");
    t_move move = {(unsigned )shift_from_position(Position(1, 0)),
                   (unsigned )shift_from_position(Position(0, 1)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal(board, move, whiteColorFilter, blackColorFilter, false));
}

TEST_F(MoveTest, isMoveLegalValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    t_move move = {(unsigned )shift_from_position(Position(0, 0)),
                   (unsigned )shift_from_position(Position(2, 0)),
                   0, 0};

    uint64_t whiteColorFilter = board->white;
    uint64_t blackColorFilter = board->black;

    EXPECT_TRUE(is_move_legal_nocheck(board, move, whiteColorFilter, blackColorFilter, true));
}


/*
 * FUNCTION TESTS FOR is_castle_legal WITH WHITE PIECES
 */
TEST_F(MoveTest, isCastleLegalWhiteShortInvalidPosition) {
    setFen(board, (char *)"8/8/8/8/8/8/8/K7");
    Position kingPosition = Position(0, 0);
    bool color = false;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteLongInvalidPosition) {
    setFen(board, (char *)"8/8/8/8/8/8/8/K7");
    Position kingPosition = Position(0, 0);
    bool color = false;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteShortCheck0) {
    setFen(board, (char *)"8/8/8/8/8/8/5q2/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteShortCheck1) {
    setFen(board, (char *)"8/8/8/8/8/8/6q1/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteShortCheck2) {
    setFen(board, (char *)"8/8/8/8/8/8/7q/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteLongCheck0) {
    setFen(board, (char *)"8/8/8/8/8/8/3q4/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteLongCheck1) {
    setFen(board, (char *)"8/8/8/8/8/8/2q5/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteLongCheck2) {
    setFen(board, (char *)"8/8/8/8/8/8/1q6/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteShortValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = true;

    EXPECT_TRUE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalWhiteLongValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/4K3");
    Position kingPosition = Position(4, 0);
    bool color = false;
    bool direction = false;

    EXPECT_TRUE(is_castle_legal(board, kingPosition, color, direction));
}


/*
 * FUNCTION TESTS FOR is_castle_legal WITH BLACK PIECES
 */
TEST_F(MoveTest, isCastleLegalBlackShortInvalidPosition) {
    setFen(board, (char *)"k7/8/8/8/8/8/8/8");
    Position kingPosition = Position(0, 7);
    bool color = true;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackLongInvalidPosition) {
    setFen(board, (char *)"k7/8/8/8/8/8/8/8");
    Position kingPosition = Position(0, 7);
    bool color = true;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackShortCheck0) {
    setFen(board, (char *)"4k3/5Q2/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackShortCheck1) {
    setFen(board, (char *)"4k3/6Q1/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackShortCheck2) {
    setFen(board, (char *)"4k3/7Q/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = true;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackLongCheck0) {
    setFen(board, (char *)"4k3/3Q4/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackLongCheck1) {
    setFen(board, (char *)"4k3/2Q5/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackLongCheck2) {
    setFen(board, (char *)"4k3/1Q6/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = false;

    EXPECT_FALSE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackShortValid) {
    setFen(board, (char *)"4k3/8/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = true;

    EXPECT_TRUE(is_castle_legal(board, kingPosition, color, direction));
}

TEST_F(MoveTest, isCastleLegalBlackLongValid) {
    setFen(board, (char *)"4k3/8/8/8/8/8/8/8");
    Position kingPosition = Position(4, 7);
    bool color = true;
    bool direction = false;

    EXPECT_TRUE(is_castle_legal(board, kingPosition, color, direction));
}


/*
 * FUNCTION TESTS FOR is_enpassant
 */
TEST_F(MoveTest, isEnpassantNoPawn) {
    setFen(board, (char *)"8/8/8/3Qp3/8/8/8/8");

    t_move move;
    move.origin = shift_from_position(Position(3, 4));
    move.target = shift_from_position(Position(4, 5));
    move.color = false;

    EXPECT_FALSE(is_enpassant(board, &move));
}

TEST_F(MoveTest, isEnpassantTargetNotEmpty) {
    setFen(board, (char *)"8/8/4p3/3Pp3/8/8/8/8");

    t_move move;
    move.origin = shift_from_position(Position(3, 4));
    move.target = shift_from_position(Position(4, 5));
    move.color = false;

    EXPECT_FALSE(is_enpassant(board, &move));
}

TEST_F(MoveTest, isEnpassantMoveNotDiagonal) {
    setFen(board, (char *)"8/8/8/3Pp3/8/8/8/8");

    t_move move;
    move.origin = shift_from_position(Position(3, 4));
    move.target = shift_from_position(Position(3, 5));
    move.color = false;

    EXPECT_FALSE(is_enpassant(board, &move));
}

TEST_F(MoveTest, isEnpassantValid) {
    setFen(board, (char *)"8/8/8/3Pp3/8/8/8/8");

    t_move move;
    move.origin = shift_from_position(Position(3, 4));
    move.target = shift_from_position(Position(4, 5));
    move.color = false;

    EXPECT_TRUE(is_enpassant(board, &move));
}


/*
 * FUNCTION TESTS FOR is_double_pawn_move
 */
TEST_F(MoveTest, isDoublePawnMoveNoPawn) {
    setFen(board, (char *)"8/8/8/8/8/8/Q7/8");

    t_move move;
    move.origin = shift_from_position(Position(0, 1));
    move.target = shift_from_position(Position(0, 3));
    move.color = false;

    EXPECT_FALSE(is_double_pawn_move(board, &move));
}

TEST_F(MoveTest, isDoublePawnMoveWrongDiff) {
    setFen(board, (char *)"8/8/8/8/8/8/P7/8");

    t_move move;
    move.origin = shift_from_position(Position(0, 1));
    move.target = shift_from_position(Position(0, 2));
    move.color = false;

    EXPECT_FALSE(is_double_pawn_move(board, &move));
}

TEST_F(MoveTest, isDoublePawnMoveValid) {
    setFen(board, (char *)"8/8/8/8/8/8/P7/8");

    t_move move;
    move.origin = shift_from_position(Position(0, 1));
    move.target = shift_from_position(Position(0, 3));
    move.color = false;

    EXPECT_TRUE(is_double_pawn_move(board, &move));
}


/*
 * FUNCTION TESTS FOR is_castle
 */
TEST_F(MoveTest, isCastleNoKing) {
    setFen(board, (char *)"8/8/8/8/8/8/8/4Q3");

    t_move move;
    move.origin = shift_from_position(Position(4, 0));
    move.target = shift_from_position(Position(2, 0));
    move.color = false;

    EXPECT_FALSE(is_castle(board, &move));
}

TEST_F(MoveTest, isCastleWrongDiff) {
    setFen(board, (char *)"8/8/8/8/8/8/8/4K3");

    t_move move;
    move.origin = shift_from_position(Position(4, 0));
    move.target = shift_from_position(Position(3, 0));
    move.color = false;

    EXPECT_FALSE(is_castle(board, &move));
}

TEST_F(MoveTest, isCastleValid) {
    setFen(board, (char *)"8/8/8/8/8/8/8/4K3");

    t_move move;
    move.origin = shift_from_position(Position(4, 0));
    move.target = shift_from_position(Position(2, 0));
    move.color = false;

    EXPECT_TRUE(is_castle(board, &move));
}


/*
 * FUNCTION TESTS FOR doMove
 */
TEST_F(MoveTest, doMoveSetTakenFigure) {
    t_move move;
    move.origin = shift_from_position(Position(0, 0));
    move.target = shift_from_position(Position(0, 1));
    move.color = false;

    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    doMove(board, &move);
    EXPECT_EQ(move.taken_figure, 0);

    setFen(board, (char *)"8/8/8/8/8/8/q7/Q7");
    doMove(board, &move);
    EXPECT_EQ(move.taken_figure, 1);

    setFen(board, (char *)"8/8/8/8/8/8/r7/Q7");
    doMove(board, &move);
    EXPECT_EQ(move.taken_figure, 2);

    setFen(board, (char *)"8/8/8/8/8/8/b7/Q7");
    doMove(board, &move);
    EXPECT_EQ(move.taken_figure, 3);

    setFen(board, (char *)"8/8/8/8/8/8/n7/Q7");
    doMove(board, &move);
    EXPECT_EQ(move.taken_figure, 4);

    setFen(board, (char *)"8/8/8/8/8/8/p7/Q7");
    doMove(board, &move);
    EXPECT_EQ(move.taken_figure, 5);
}

TEST_F(MoveTest, doMoveSetColor) {
    t_move move;
    move.origin = shift_from_position(Position(0, 0));
    move.target = shift_from_position(Position(0, 1));

    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    doMove(board, &move);
    EXPECT_EQ(move.color, false);

    setFen(board, (char *)"8/8/8/8/8/8/8/q7");
    doMove(board, &move);
    EXPECT_EQ(move.color, true);
}

TEST_F(MoveTest, doMoveTargetCleared) {
    t_move move;
    move.origin = shift_from_position(Position(0, 0));
    move.target = shift_from_position(Position(0, 1));
    move.color = false;

    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");

    int targetShift = shift_from_position(Position(0, 1));
    uint64_t targetBitmask = (uint64_t )1 << targetShift;

    board->king |= targetBitmask;
    board->queen |= targetBitmask;
    board->rook |= targetBitmask;
    board->bishop |= targetBitmask;
    board->knight |= targetBitmask;
    board->pawn |= targetBitmask;

    doMove(board, &move);

    EXPECT_FALSE(board_value_from_shift(board->king, targetShift));
    EXPECT_TRUE(board_value_from_shift(board->queen, targetShift));
    EXPECT_FALSE(board_value_from_shift(board->rook, targetShift));
    EXPECT_FALSE(board_value_from_shift(board->bishop, targetShift));
    EXPECT_FALSE(board_value_from_shift(board->knight, targetShift));
    EXPECT_FALSE(board_value_from_shift(board->pawn, targetShift));
}

TEST_F(MoveTest, doMoveOriginCleared) {
    t_move move;
    move.origin = shift_from_position(Position(0, 0));
    move.target = shift_from_position(Position(0, 1));
    move.color = false;

    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    doMove(board, &move);

    int originShift = move.origin;
    EXPECT_FALSE(board_value_from_shift(board->king, originShift));
    EXPECT_FALSE(board_value_from_shift(board->queen, originShift));
    EXPECT_FALSE(board_value_from_shift(board->rook, originShift));
    EXPECT_FALSE(board_value_from_shift(board->bishop, originShift));
    EXPECT_FALSE(board_value_from_shift(board->knight, originShift));
    EXPECT_FALSE(board_value_from_shift(board->pawn, originShift));
    EXPECT_FALSE(board_value_from_shift(board->white, originShift));
    EXPECT_FALSE(board_value_from_shift(board->black, originShift));
}

TEST_F(MoveTest, doMovePieceMoved) {
    t_move move;
    move.origin = shift_from_position(Position(0, 0));
    move.target = shift_from_position(Position(0, 1));

    // White pieces
    setFen(board, (char *)"8/8/8/8/8/8/8/K7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/K7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/Q7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/Q7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/R7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/R7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/B7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/B7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/N7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/N7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/P7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/P7/8");

    // Black pieces
    setFen(board, (char *)"8/8/8/8/8/8/8/k7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/k7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/q7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/q7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/r7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/r7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/b7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/b7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/n7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/n7/8");

    setFen(board, (char *)"8/8/8/8/8/8/8/p7");
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/p7/8");
}

TEST_F(MoveTest, doMoveEnPassantWhite) {
    setFen(board, (char *)"8/8/8/Pp6/8/8/8/8");

    t_move move;
    move.origin = shift_from_position(Position(0, 4));
    move.target = shift_from_position(Position(1, 5));
    move.color = false;

    doMove(board, &move);
    ASSERT_STREQ(getFen(board), (char *)"8/8/1P6/8/8/8/8/8");
}

TEST_F(MoveTest, doMoveEnPassantBlack) {
    setFen(board, (char *)"8/8/8/8/pP6/8/8/8");

    t_move move;
    move.origin = shift_from_position(Position(0, 3));
    move.target = shift_from_position(Position(1, 2));
    move.color = true;

    doMove(board, &move);
    ASSERT_STREQ(getFen(board), (char *)"8/8/8/8/8/1p6/8/8");
}

TEST_F(MoveTest, doMoveCastleWhite) {
    // Short castle
    setFen(board, (char *)"r3k2r/8/8/8/8/8/8/R3K2R");

    t_move move;
    move.origin = shift_from_position(Position(4, 0));
    move.target = shift_from_position(Position(6, 0));
    move.color = false;

    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"r3k2r/8/8/8/8/8/8/R4RK1");


    // Long castle
    setFen(board, (char *)"r3k2r/8/8/8/8/8/8/R3K2R");

    move.origin = shift_from_position(Position(4, 0));
    move.target = shift_from_position(Position(2, 0));
    move.color = false;

    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"r3k2r/8/8/8/8/8/8/2KR3R");
}

TEST_F(MoveTest, doMoveCastleBlack) {
    // Short castle
    setFen(board, (char *)"r3k2r/8/8/8/8/8/8/R3K2R");

    t_move move;
    move.origin = shift_from_position(Position(4, 7));
    move.target = shift_from_position(Position(6, 7));
    move.color = true;

    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"r4rk1/8/8/8/8/8/8/R3K2R");


    // Long castle
    setFen(board, (char *)"r3k2r/8/8/8/8/8/8/R3K2R");

    move.origin = shift_from_position(Position(4, 7));
    move.target = shift_from_position(Position(2, 7));
    move.color = true;

    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"2kr3r/8/8/8/8/8/8/R3K2R");
}

TEST_F(MoveTest, doMovePromoteWhite) {
    t_move move;
    move.origin = shift_from_position(Position(0, 6));
    move.target = shift_from_position(Position(0, 7));
    move.color = false;
    move.promoted = true;

    setFen(board, (char *)"8/P7/8/8/8/8/8/8");
    move.promoted_to = 0;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"Q7/8/8/8/8/8/8/8");

    setFen(board, (char *)"8/P7/8/8/8/8/8/8");
    move.promoted_to = 1;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"R7/8/8/8/8/8/8/8");

    setFen(board, (char *)"8/P7/8/8/8/8/8/8");
    move.promoted_to = 2;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"B7/8/8/8/8/8/8/8");

    setFen(board, (char *)"8/P7/8/8/8/8/8/8");
    move.promoted_to = 3;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"N7/8/8/8/8/8/8/8");
}

TEST_F(MoveTest, doMovePromoteBlack) {
    t_move move;
    move.origin = shift_from_position(Position(0, 1));
    move.target = shift_from_position(Position(0, 0));
    move.color = true;
    move.promoted = true;

    setFen(board, (char *)"8/8/8/8/8/8/p7/8");
    move.promoted_to = 0;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/8/q7");

    setFen(board, (char *)"8/8/8/8/8/8/p7/8");
    move.promoted_to = 1;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/8/r7");

    setFen(board, (char *)"8/8/8/8/8/8/p7/8");
    move.promoted_to = 2;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/8/b7");

    setFen(board, (char *)"8/8/8/8/8/8/p7/8");
    move.promoted_to = 3;
    doMove(board, &move);
    EXPECT_STREQ(getFen(board), (char *)"8/8/8/8/8/8/8/n7");
}


// TODO: Tests for is_enpassant, is_double_pawn_move, is_castle, doMove, undoMove
// TODO: Specialized tests for generate_moves (?)


//TEST_F(MoveTest, doMove) {
//    // play e4
//    doMove(board, &e2_e4);
//    EXPECT_STREQ(getFen(board), e4BoardAsFen);
//}
//
//TEST_F(MoveTest, undoMove) {
//    // e4 back to e2
//    setFen(board, e4BoardAsFen);
//    // undo e4
//    undoMove(board, &e2_e4);
//    EXPECT_STREQ(getFen(board), boardAsFen);
//}
//
//// Stellung 1 Startstellung
//TEST_F(MoveTest, starting_position) {
//
//    List<t_move> moves = generate_moves(game, false);
//    EXPECT_EQ(20, moves.length());
//
//}
//
//// Stellung 1 Gruppe A
//TEST_F(MoveTest, groupA1) {
//    setFen(game->board, "2b1k3/p5rp/Pppb1p1P/n2qpPpR/1PPpP1P1/R2P1Q1B/1Br4n/1N2K1N1");
//    List<t_move> moves = generate_moves(game, false);
//    EXPECT_EQ(33, moves.length());
//}
//
///*
//// Stellung 2 Gruppe A
//TEST_F(MoveTest, groupA2) {
//    setFen(game->board, "r1bqk2r/ppp3pp/5p2/2nPp3/P1Pn2P1/3P1N2/1P2BP1P/RN1Q1RK1");
//    List<t_move> moves = generate_moves(game, true);
//    EXPECT_EQ(42, moves.length());
//} */
//
//// Stellung Giuoco Piano (Gruppe AI)
//TEST_F(MoveTest, giuoco_pianoAI) {
//    setFen(game->board, "rnbqkbnr/1pppppp1/p6p/8/2B1P3/5N2/PPPP1PPP/RNBQK2R");
//    List<t_move> moves = generate_moves(game, true);
//    EXPECT_EQ(18, moves.length());
//}
//
//// Stellung Dutch Defense (Gruppe AI)
//TEST_F(MoveTest, dutch_defenseAI) {
//    setFen(game->board, "rnbqkbnr/ppppp1pp/8/5p2/3P4/8/PPP1PPPP/RNBQKBNR");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(28, moves.length());
//}
//
//// Stellung nicht erlaubter En passant (Gruppe B), last move: pc6->c5
//TEST_F(MoveTest, en_passantB) {
//    setFen(game->board, "rnbqkbnr/pp1p1ppp/4p3/1Pp5/8/2N5/P1PPPPPP/R1BQKBNR");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(22, moves.length());
//}
//
//// Stellung Matt in 2 (Gruppe B)
//TEST_F(MoveTest, mate_in_twoB) {
//    setFen(game->board, "4kr2/4b3/8/2p1R2p/4n1p1/3q2P1/7P/4K3");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(10, moves.length());
//}
//
//// Stellung Rochade (Gruppe H)
//TEST_F(MoveTest, rochadeH) {
//    setFen(game->board, "1rbqk2r/3nBppp/p2p4/3Qp3/Pp2P3/6PB/1PP1NP1P/R3K2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(48, moves.length());
//}
//
//// Schwarz am Zug, König im Schach (Gruppe E)
//TEST_F(MoveTest, king_in_checkE) {
//    setFen(game->board, "3k4/2P5/1K4p1/8/8/8/8/8");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(4, moves.length());
//}
//
//// Weiß am Zug (Gruppe E)
//TEST_F(MoveTest, groupE) {
//    setFen(game->board, "4nb2/2r3k1/6p1/5p2/8/4P3/2R2PP1/3K4");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(19, moves.length());
//}
//
//// ten is correct!
//
//// Stelllung Rochade und Pin (Gruppe F)
//TEST_F(MoveTest, rochade_and_pinF) {
//    setFen(game->board, "r2kq3/3r4/8/8/7b/p7/P3BN2/R3K2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(10, moves.length());
//}
//
//// Stellung Kolision (Gruppe F)
//TEST_F(MoveTest, collisionF) {
//    setFen(game->board, "r7/P1P5/p7/P4k2/n7/P1P2K2/1BP2PBN/1QR2R1n");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(16, moves.length());
//}
//
//// Stellung schwarz Rochade (Gruppe G)
//TEST_F(MoveTest, rochadeG) {
//    setFen(game->board, "B3k2r/p1pqbppp/n2p3n/4p3/6b1/1PPPP1P1/P4P1P/RNBQK1NR");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(37, moves.length());
//}
//
//// Stellung weiß en passant pseudo-legal, aber nicht legal (Gruppe G)
//TEST_F(MoveTest, en_passant_pseudo_legalG) {
//    setFen(game->board, "rnb1kbnr/ppp2ppp/8/K2pP2q/5p2/3P4/PPP1B1PP/RNBQ2NR");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(29, moves.length());
//}
//
///*
//// Stellung En Passant (Gruppe H)
//TEST_F(MoveTest, en_passantH) {
//    setFen(game->board, "r1bq1r2/pp2n3/4N2k/3pPppP/1b1n2Q1/2N5/PP3PP1/R1B1K2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(47, moves.length());
//} */
//
//// Stellung Endgame 1 (Gruppe R)
//TEST_F(MoveTest, endgameR) {
//    setFen(game->board, "6k1/B5q1/KR6/8/8/8/6p1/8");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(16, moves.length());
//}
//
//
//// exchange pawn, how many moves? one per piece?
//
//// Stellung Endgame Reverse - mit Bauernumwandlung (Gruppe R)
//TEST_F(MoveTest, pawn_exchangeR) {
//    setFen(game->board, "6k1/B5q1/KR6/8/8/8/6p1/8");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(28, moves.length());
//}
//
//// Stellungen I (Gruppe O)
//TEST_F(MoveTest, groupO1) {
//    setFen(game->board, " r1b3k1/ppb2ppp/8/2B1p1P1/1P2N2P/P3P3/2P2P2/3rK2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(2, moves.length());
//}
//
//// Stellungen II (Gruppe O)
//TEST_F(MoveTest, groupO2) {
//    setFen(game->board, "8/8/6k1/5n2/5P2/7p/2KN4/8 ");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(13, moves.length());
//}
//
//// Stellung Mittelspiel Gruppe V
//TEST_F(MoveTest, rochadeV) {
//    setFen(game->board, "r3k1nr/pp3ppp/3p4/3P4/8/3P4/PP3PPP/RN2K2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(21, moves.length());
//}
//
//// Stellung Endspiel Gruppe V
//TEST_F(MoveTest, endgameV) {
//    setFen(game->board, "8/8/1k6/8/8/8/6K1/8");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(8, moves.length());
//}
//
//// Stellung King of the Hill (Gruppe L)
//TEST_F(MoveTest, king_of_the_hillL) {
//    setFen(game->board, "1r6/8/2pbk1p1/p4p2/2KPnP2/4P3/PB4PP/5R2");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(21, moves.length());
//}
//
//// Stellung (Gruppe L)
//TEST_F(MoveTest, groupL) {
//    setFen(game->board, "2R5/2r2bkp/2n4p/1p6/4p2N/2K1n3/7B/8");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(20, moves.length());
//}
//
//// Stellung I (Gruppe J)
//TEST_F(MoveTest, groupJ1) {
//    setFen(game->board, "3k4/8/8/8/8/8/7r/3K4");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(2, moves.length());
//}
//
//// Stellung II (Gruppe J)
//TEST_F(MoveTest, groupJ2) {
//    setFen(game->board, "r1bqkbnr/pppppppp/2n5/8/8/2N5/PPPPPPPP/R1BQKBNR");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(22, moves.length());
//}
//
//// Stellung I (Gruppe M)
//TEST_F(MoveTest, groupM1) {
//    setFen(game->board, "rnq1kb1r/p1ppppp1/1p6/7p/2PPb1nP/5N2/PP3P2/RNBQKBR1");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(34, moves.length());
//}
//
//// Stellung II (Gruppe M)
//TEST_F(MoveTest, groupM2) {
//    setFen(game->board, "1rbqk3/p1p3p1/np1p1n1r/B1b1pp1p/2P4P/3PP1P1/PP1QBP2/RN3KNR");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(33, moves.length());
//}
//
//// Stellung I (Gruppe A)
//TEST_F(MoveTest, groupAA1) {
//    setFen(game->board, "2bqk2r/ppp4p/3npp2/n7/2pPPbBR/1Pr2N2/PBP4P/RN1QK3");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(32, moves.length());
//}
//
//// Stellung II (Gruppe A)
//TEST_F(MoveTest, pawn_exchange2) {
//    setFen(game->board, "7k/1P4pp/4B3/3b1n2/6P1/5P2/3P2N1/3KQ3");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(31, moves.length());
//}
//
//// Queen's gambit (Gruppe C)
//TEST_F(MoveTest, queens_gambitC) {
//    setFen(game->board, "rnbkqbnr/ppp1pppp/8/3p4/2PP4/8/PP2PPPP/RNBKQBNR");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(30, moves.length());
//}
//
//// King of the Hill für weiß (Gruppe C)
//TEST_F(MoveTest, king_of_the_hill) {
//    setFen(game->board, "r3kb1r/1p1nqp1p/p2p1np1/P3p3/2PQP3/2N1K1PP/1P3P2/R1B2B1R");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(25, moves.length());
//}
//
//// Stellung II (Gruppe C)
//TEST_F(MoveTest, groupC2) {
//    setFen(game->board, "rn1qkbnr/ppp1ppp1/4b3/3pN2p/8/2N4P/PPPPPPP1/R1BQKB1R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(29, moves.length());
//}
//
//// Schwarz ist am Zug, hat aber keinen legalen Zug: also Remi (Gruppe D)
//TEST_F(MoveTest, remiD) {
//    setFen(game->board, "8/8/7k/7p/4B2P/6R1/8/4K3");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(0, moves.length());
//}
//
//// somethings wrong here
//
//// Stellung 1: Start (Gruppe S)
//TEST_F(MoveTest, openingS) {
//    setFen(game->board, "rnbqkb1r/ppp1pppp/5n2/3p4/4P3/2N5/PPPP1PPP/R1BQKBNR");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(32, moves.length());
//}
//
//// Stellung 2: Endspiel mit Turm (Gruppe S)
//TEST_F(MoveTest, endgameS) {
//    setFen(game->board, "8/8/8/8/2R5/6pk/1r6/6K1");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(16, moves.length());
//}
//
///*
//// Stellung 1 (Gruppe K)
//TEST_F(MoveTest, groupK1) {
//    setFen(game->board, "4K3/PP3PP1/8/8/1p2n1P1/7R/3k4/2R5");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(41, moves.length());
//} */
//
//// sollte nur ein move sein!
//
//// Stellung 2 (Gruppe K)
//TEST_F(MoveTest, groupK2) {
//    setFen(game->board, "R3K2R/P5PP/2P3QN/7k/6p1/2n4n/p5pp/7r");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(1, moves.length());
//}
//
//// Stellung 1 (Gruppe W)
//TEST_F(MoveTest, groupW1) {
//    setFen(game->board, "2k5/p1P4r/1p3p1p/8/P3P3/2P3b1/8/5K2 ");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(6, moves.length());
//}
//
//// Stellung2(Gruppe W)
//TEST_F(MoveTest, groupW2) {
//    setFen(game->board, "5b2/1pkP2p1/8/4P2P/2Pn3P/3B4/3P4/3QK2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(27, moves.length());
//}
//
//// Stellung1(Gruppe AI)
//TEST_F(MoveTest, groupAI1) {
//    setFen(game->board, "4K3/PP3PP1/8/8/1p2n1P1/7R/3k4/2R5");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(10, moves.length());
//}
//
//// Stellung 1 Gruppe T - London System Schach
//TEST_F(MoveTest, london_system_checkT) {
//    setFen(game->board, "r1bqk1nr/ppp2ppp/2n1p3/3p4/1b1P1B2/4PN2/PPP2PPP/RN1QKB1R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(6, moves.length());
//}
//
//
//// Stellung 2 gruppe T - Endspiel, Forciertes Unendschieden
//TEST_F(MoveTest, forced_remiT) {
//    setFen(game->board, "8/8/4k3/3pP3/3K4/8/8/8");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(4, moves.length());
//}
//
//// Stellung 1 Gruppe U - Sicilian Defense, beide Seite king's side castle
//TEST_F(MoveTest, sicilian_defense_castleU) {
//    setFen(game->board, "r1bq1rk1/ppp2ppp/2np1n2/3p4/2PP4/2NBPN2/PP3PPP/R1BQK2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(43, moves.length());
//}
//
//// Stellung 2 Gruppe U - King's Indian Defense weiß King's side castle, Schwarz Queen's side
//TEST_F(MoveTest, kings_indian_defense_castleU) {
//    setFen(game->board, "r4rk1/1pp1q1pp/2np1pn1/p3p3/2PPP3/2N1BP2/PPQ2P1P/R3K2R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(40, moves.length());
//}
//
//// Stellung 3 Gruppe U - French Defense, weiß King's side castle, Schwarz Queen's side
//TEST_F(MoveTest, french_defense_castleU) {
//    setFen(game->board, "2kr3r/1p3pp1/p1nqbn1p/3p4/3P2P1/1BN2N2/PPP1QPBP/R4RK1");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(41, moves.length());
//}
//
//// Stellung 4 Gruppe U - Nimzo Indian Defense, beide Seite king's side castle
//TEST_F(MoveTest, nimzo_indian_defense_castleU) {
//    setFen(game->board, "r4rk1/1bqn1ppp/p2bpn2/1p1p4/2pP4/1PNBPN2/PB3PPP/R2Q1RK1");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(40, moves.length());
//}
//
//// Stellung 1 Gruppe X - Mate in one
//TEST_F(MoveTest, mate_in_oneX) {
//    setFen(game->board, "8/6P1/5K1k/6N1/5N2/8/8/8");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(20, moves.length());
//}
//
//// SStellung 2 Gruppe X - Queens Gambit, Schach
//TEST_F(MoveTest, queens_gambit_checkX) {
//    setFen(game->board, "rnbqk1nr/pppp1ppp/4p3/8/1bPP4/8/PP2PPPP/RNBQKBNR");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(4, moves.length());
//}
//
//// Stellung 1 Gruppe F (SoSe23)
//TEST_F(MoveTest, castleF1) {
//    setFen(game->board, "rnbqk2r/ppppppbp/5np1/8/2PP4/2N2N2/PP2PPPP/R1BQKB1R");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(26, moves.length());
//}
//
//// Stellung 2 Gruppe F (SoSe23)
//TEST_F(MoveTest, groupF2) {
//    setFen(game->board, "3r2k1/p6p/6p1/4b3/2P5/8/P1K3PP/5R2");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(24, moves.length());
//}
//
//// Stellung 2 Gruppe AD (SoSe23)
//TEST_F(MoveTest, groupAD1) {
//    setFen(game->board, "r3rbk1/p4pp1/q1p1nnb1/B3pN1p/PpN1P1P1/1P5P/2P1QPB1/R2R2K1");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(34, moves.length());
//}
//
//// Stellung1(Gruppe AA)
//TEST_F(MoveTest, groupAAA1) {
//    setFen(game->board, "K7/8/8/pP6/8/4R3/2Pq1k2/8");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(20, moves.length());
//}
//
//// Stellung 2(Gruppe AA)
//TEST_F(MoveTest, groupAAA2) {
//    setFen(game->board, "8/5k1K/1q5P/2p5/8/5pp1/8/8");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(1, moves.length());
//}
//
//// Stellung 1 Gruppe V (SoSe23)
//TEST_F(MoveTest, groupV1) {
//    setFen(game->board, "r1bq1rk1/ppp2ppp/2nb1n2/3pp3/2PPP3/2N2N2/PP2BPPP/R1BQ1RK1");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(36, moves.length());
//}
//
//// Stellung 2 Gruppe V (SoSe23)
//TEST_F(MoveTest, groupV2) {
//    setFen(game->board, "2kr1b1r/pp2pppp/2p5/1B6/2PPn3/8/PP3PPP/R1B1R1K1");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(30, moves.length());
//}
//
//// Halloween Gambit (Gruppe I)
//TEST_F(MoveTest, halloween_gambitI) {
//    setFen(game->board, "R1BQKB1R/PPPP1PPP/2N2N2/4n3/4p3/2n5/pppp1ppp/r1bqkb1r");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(30, moves.length());
//}
//
//// Stellung 2 - Endgame (Gruppe I)
//TEST_F(MoveTest, endgameI) {
//    setFen(game->board, "R3K2R/PP3PPP/3P1NB1/3pP3/1ppQ4/2n5/p2k2pp/r2q1b1r");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(4, moves.length());
//}
//
//// Stellung 1 Gruppe G (SoSe2023)
//TEST_F(MoveTest, groupG1) {
//    setFen(game->board, "r2r2k1/ppp1p2p/6p1/4b1p1/PnP4P/2N4P/1P6/R3KB2");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(22, moves.length());
//}
//
//// Stellung 2 Gruppe G (SoSe 2023)
//TEST_F(MoveTest, groupG2) {
//    setFen(game->board, "r1bqkr2/pp1pbpQp/8/2p1P3/2B5/2N5/PPP2PPP/R1B1K2R");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(20, moves.length());
//}
//
//// need to take a closer look on those two
//
//// Stellung 1 Gruppe J (SoSe2023)
//TEST_F(MoveTest, groupJJ1) {
//    setFen(game->board, "r3k2r/pp6/2p3Pb/2N1pP2/Q2p4/4P3/PP1K4/7R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(43, moves.length());
//}
//
//// Stellung 2 Gruppe J (SoSe2023)
//TEST_F(MoveTest, groupJJ2) {
//    setFen(game->board, "r3k2r/pp2qppp/1np2n2/2bPp1B1/B2P2Q1/2N2N2/PPP2PPP/2KR3R");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(36, moves.length());
//}
//
//// Stellung 1 (Gruppe AH) SS 2023
//TEST_F(MoveTest, groupAH1) {
//    setFen(game->board, "1n1qkbnr/2pppppp/p7/p2P4/6Q1/8/PPPP1PPP/R1B1K1NR");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(16, moves.length());
//}
//
//// 2. Stellung Gruppe(AH) SS 2023
//TEST_F(MoveTest, groupAH2) {
//    setFen(game->board, "8/8/r3k3/8/8/3K4/8/8");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(18, moves.length());
//}
//
//// 1. Stellung Gruppe(L) SS 2023
//TEST_F(MoveTest, pawnexchangeL1) {
//    setFen(game->board, "r2qk2r/p1p1p1P1/1pn4b/1N1Pb3/1PB1N1nP/8/1B1PQPp1/R3K2R");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(42, moves.length());
//}
//
//// closer look on those two
//
//// 2. Stellung Gruppe(L) SS 2023
//TEST_F(MoveTest, groupL2) {
//    setFen(game->board, "r1bq4/pp1p1k1p/2p2p1p/2b5/3Nr1Q1/2N1P3/PPPK1PPP/3R1B1R");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(50, moves.length());
//}
//
//// 1. Stellung Gruppe(AB) SS 2023
//TEST_F(MoveTest, groupAB1) {
//    setFen(game->board, "rnbqk3/p6P/2n1p1P1/1r3p2/8/1PN1K3/P4P2/R1BQ1BNR");
//    List<t_move> moves = generate_moves(game, 0);
//    EXPECT_EQ(50, moves.length());
//}
//
//// 2. Stellung Gruppe(AB) SS 2023
//TEST_F(MoveTest, groupAB2) {
//    setFen(game->board, "rnb1kbnr/p6p/Bp4p1/8/Q4p2/N2qBN2/PP3PPP/R3K2R");
//    List<t_move> moves = generate_moves(game, 1);
//    EXPECT_EQ(9, moves.length());
//}
