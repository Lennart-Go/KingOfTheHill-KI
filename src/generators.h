//
// Created by mbienert on 6/12/23.
//

#ifndef KINGOFTHEHILL_KI_GENERATORS_H
#define KINGOFTHEHILL_KI_GENERATORS_H


#include <cstdlib>
#include "util.h"
#include "game.h"
#include "move.h"


//uint64_t generateMovable(piece p, int positionOffset, bool cutEdge) {
//    // Can also be used to find all pieces of type p that are targeting the positionOffset square
//    uint64_t positionMap = (uint64_t )1 << positionOffset;
//
//    uint64_t kp = p == piece::king ? positionMap : 0;
//    uint64_t qp = p == piece::queen ? positionMap : 0;
//    uint64_t rp = p == piece::rook ? positionMap : 0;
//    uint64_t bp = p == piece::bishop ? positionMap : 0;
//    uint64_t np = p == piece::knight ? positionMap : 0;
//
//    board newBoard = board(kp, qp, rp, bp, np, 0, 0, 0, 0, 0, 0, 0);
//    t_gameOld *gameOld = startGame(10000000000000000);
//    gameOld->board = &newBoard;
//
//    std::vector<move> possibleMoves = generate_moves(gameOld, false);
//
//    uint64_t targetMap = 0;
//    for (move possibleMove: possibleMoves) {
//        targetMap |= (uint64_t )1 << possibleMove.target;
//    }
//
//    if (cutEdge) {
//        // Cut off edge squares before returning
//        uint64_t edges = 0b1111111110000001100000011000000110000001100000011000000111111111;
//        return targetMap & ~edges;
//    }
//    return targetMap;
//}


uint64_t generateHorizontal(int originShift) {
    Position originPosition = position_from_shift(originShift);

    uint64_t targetMap = 0;
    for (int i = 0; i < originPosition.y; ++i) {
        targetMap |= (uint64_t )1 << shift_from_position(Position(originPosition.x, i));
    }
    for (int i = originPosition.y + 1; i < 8; ++i) {
        targetMap |= (uint64_t )1 << shift_from_position(Position(originPosition.x, i));
    }

    return targetMap;
}

uint64_t generateVertical(int originShift) {
    Position originPosition = position_from_shift(originShift);

    uint64_t targetMap = 0;
    for (int i = 0; i < originPosition.x; ++i) {
        targetMap |= (uint64_t )1 << shift_from_position(Position(i, originPosition.y));
    }
    for (int i = originPosition.x + 1; i < 8; ++i) {
        targetMap |= (uint64_t )1 << shift_from_position(Position(i, originPosition.y));
    }

    return targetMap;
}

uint64_t generateLeftDiagonal(int originShift) {
    Position originPosition = position_from_shift(originShift);

    uint64_t targetMap = 0;
    int offset = 1;
    while (true) {
        Offset targetOffset = Offset(originPosition.x - offset, originPosition.y + offset);
        if (!targetOffset.isWithinBounds()) {
            break;
        }
        targetMap |= (uint64_t )1 << shift_from_position(targetOffset.toPosition());

        offset++;
    }

    offset = 1;
    while (true) {
        Offset targetOffset = Offset(originPosition.x + offset, originPosition.y - offset);
        if (!targetOffset.isWithinBounds()) {
            break;
        }
        targetMap |= (uint64_t )1 << shift_from_position(targetOffset.toPosition());

        offset++;
    }

    return targetMap;
}

uint64_t generateRightDiagonal(int originShift) {
    Position originPosition = position_from_shift(originShift);

    uint64_t targetMap = 0;
    int offset = 1;
    while (true) {
        Offset targetOffset = Offset(originPosition.x + offset, originPosition.y + offset);
        if (!targetOffset.isWithinBounds()) {
            break;
        }
        targetMap |= (uint64_t )1 << shift_from_position(targetOffset.toPosition());

        offset++;
    }

    offset = 1;
    while (true) {
        Offset targetOffset = Offset(originPosition.x - offset, originPosition.y - offset);
        if (!targetOffset.isWithinBounds()) {
            break;
        }
        targetMap |= (uint64_t )1 << shift_from_position(targetOffset.toPosition());

        offset++;
    }

    return targetMap;
}


uint64_t generateXray(int originShift, int targetShift) {
    Position origin = position_from_shift(originShift);
    Position target = position_from_shift(targetShift);

    if (origin == target) {
        return ~(uint64_t )0;
    }

    int xDiff = (int) target.x - (int) origin.x;
    int yDiff = (int) target.y - (int) origin.y;

    uint64_t xrayMap = 0;

    if (abs(xDiff) != abs(yDiff) && (xDiff != 0 && yDiff != 0)) {
        return ~(uint64_t )0;
    }

    int xSign = xDiff != 0 ? xDiff / abs(xDiff) : 0;
    int ySign = yDiff != 0 ? yDiff / abs(yDiff) : 0;

    int x, y;
    Position targetPosition;
    for (x = origin.x, y = origin.y;
         x != target.x || y != target.y;
         x += xSign, y += ySign) {
        targetPosition = Position(x, y);
        xrayMap |= (uint64_t) 1 << shift_from_position(targetPosition);
    }

    return xrayMap;
}


uint64_t generateRookLookup(int originShift, uint16_t obstructionIdentifier, int piecesIgnorable) {
    // When ignoring the borders the required band can be reduced from 14 to 12 bits (worst case)
    Position originPosition = position_from_shift(originShift);

    int upLength = 7 - originPosition.y;  // 2
    int leftLength = originPosition.x;  // 1
    int rightLength = 7 - originPosition.x;  // 6
    int downLength = originPosition.y;  // 5

    int upOffset = 0;  // 0
    int leftOffset = upOffset + upLength;  // 13
    int rightOffset = leftOffset + leftLength;  // 2
    int downOffset = rightOffset + rightLength;  // 8

    int upOccOffset = upLength != 0 ? 1 : 0;
    int leftOccOffset = leftLength != 0 ? upOccOffset + 1 : upOccOffset;
    int rightOccOffset = leftOccOffset;
    int downOccOffset = rightLength != 0 ? rightOccOffset + 1 : rightOccOffset;

    int upPiecesIgnorable = piecesIgnorable;
    int leftPiecesIgnorable = piecesIgnorable;
    int rightPiecesIgnorable = piecesIgnorable;
    int downPiecesIgnorable = piecesIgnorable;


    uint64_t result = 0;
    // Up direction
    for (int up = upOffset + upLength - 1; up >= upOffset; --up) {
        Offset targetPositionOffset = Offset(0, (upOffset + upLength - up));
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (uint64_t) 1 << shift_from_position(targetPosition);

        // Break loop if target uint64_t is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (up - upOccOffset));
        if (obstructionVal != 0) {
            if (upPiecesIgnorable == 0) {
                break;
            }
            upPiecesIgnorable--;
        }
    }

    // Left direction
    for (int left = leftOffset + leftLength - 1; left >= leftOffset; --left) {
        Offset targetPositionOffset = Offset(-(leftOffset + leftLength - left), 0);
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (uint64_t) 1 << shift_from_position(targetPosition);

        // Break loop if target uint64_t is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (left - leftOccOffset));
        if (obstructionVal != 0) {
            if (leftPiecesIgnorable == 0) {
                break;
            }
            leftPiecesIgnorable--;
        }
    }

    // Right direction
    for (int right = rightOffset; right < rightOffset + rightLength; ++right) {
        Offset targetPositionOffset = Offset(right - (rightOffset - 1), 0);
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (uint64_t) 1 << shift_from_position(targetPosition);

        // Break loop if target uint64_t is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (right - rightOccOffset));
        if (obstructionVal != 0) {
            if (rightPiecesIgnorable == 0) {
                break;
            }
            rightPiecesIgnorable--;
        }
    }

    // Down direction
    for (int down = downOffset; down < downOffset + downLength; ++down) {
        Offset targetPositionOffset = Offset(0, -(down - (downOffset - 1)));
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (uint64_t) 1 << shift_from_position(targetPosition);

        // Break loop if target uint64_t is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (down - downOccOffset));
        if (obstructionVal != 0) {
            if (downPiecesIgnorable == 0) {
                break;
            }
            downPiecesIgnorable--;
        }
    }

    return result;
}


uint64_t generateBishopLookup(int originShift, uint16_t obstructionIdentifier, int piecesIgnorable) {

    // When ignoring the borders the required band can be reduced from 13 to 9 bits (worst case)
    int border_shift_offset = 2;

    Position originPosition = position_from_shift(originShift);

    int upLength = 7 - originPosition.y;  // 2
    int leftLength = originPosition.x;  // 1
    int rightLength = 7 - originPosition.x;  // 6
    int downLength = originPosition.y;  // 5

    int upLeftLength = min(upLength, leftLength);  // 1
    int upRightLength = min(upLength, rightLength);  // 2
    int downLeftLength = min(downLength, leftLength);  // 1
    int downRightLength = min(downLength, rightLength);  // 6

    int upMinLength = min(upLeftLength, upRightLength);  // 1
    int upMaxLength = max(upLeftLength, upRightLength);  // 2
    int upLeftLengthRemaining = upLeftLength - upMinLength;  // 0
    int upRightLengthRemaining = upRightLength - upMinLength;  // 1

    int downMinLength = min(downLeftLength, downRightLength);
    int downMaxLength = max(downLeftLength, downRightLength);
    int downLeftLengthRemaining = downLeftLength - downMinLength;
    int downRightLengthRemaining = downRightLength - downMinLength;

    int upLeftOffset = upRightLengthRemaining;  // 0
    int upRightOffset = upLeftLengthRemaining;  // 1
    int downLeftOffset = (upLeftLength + upRightLength);  // 3
    int downRightOffset = (upLeftLength + upRightLength) + 1;  // 4

    bool upLeftStopped = false;
    bool upRightStopped = false;
    bool downLeftStopped = false;
    bool downRightStopped = false;

    int occOffsetUpLeft = upLeftLength > 0 ? 1 : 0;
    int occOffsetUpRight = upRightLength > 0 ? 1 : 0;
    int occOffsetUp = occOffsetUpLeft + occOffsetUpRight;
    int occOffsetDown = occOffsetUp;
    int occOffsetDownLeft = downRightLength != 0 ? occOffsetDown + 1 : occOffsetDown;
    int occOffsetDownRight = downLeftLength != 0 ? occOffsetDown + 1 : occOffsetDown;

    int upLeftPiecesIgnorable = piecesIgnorable;
    int upRightPiecesIgnorable = piecesIgnorable;
    int downLeftPiecesIgnorable = piecesIgnorable;
    int downRightPiecesIgnorable = piecesIgnorable;

    uint64_t result = 0;

    // Up left direction
    // Inner
    if (upMinLength > 0) {
        int tmp_offset = 0;
        for (int upLeft1 = upLeftLength + upRightLength - 2;
             upLeft1 > upLeftLength + upRightLength - 1 - 2 * upMinLength;
             upLeft1 -= 2) {
            int offset = (upMaxLength + upMinLength + 1 - upLeft1) / 2;
            Offset targetPositionOffset = Offset(-offset, offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            if (offset == upMinLength && targetPosition.x != 7) {
                // Square on the "other" side of the opening cone is hitting the wall
                tmp_offset = 1;
            }

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upLeft1 - occOffsetUp + tmp_offset));
            if (obstructionVal != 0) {
                if (upLeftPiecesIgnorable == 0) {
                    upLeftStopped = true;
                    break;
                }
                upLeftPiecesIgnorable--;
            }
        }
    }
    // Outer
    if (!upLeftStopped) {
        for (int upLeft2 = upLeftLength + upRightLength - 1 - 2 * upMinLength;
             upLeft2 >= upLeftOffset;
             --upLeft2) {
            int offset = upMaxLength - upLeft2;
            Offset targetPositionOffset = Offset(-offset, offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upLeft2 - occOffsetUpLeft));
            if (obstructionVal != 0) {
                if (upLeftPiecesIgnorable == 0) {
                    break;
                }
                upLeftPiecesIgnorable--;
            }
        }
    }

    // Up right direction
    // Inner
    if (upMinLength > 0) {
        for (int upRight1 = upLeftLength + upRightLength - 1;
             upRight1 > upLeftLength + upRightLength - 1 - 2 * upMinLength;
             upRight1 -= 2) {
            int offset = (upMaxLength + upMinLength + 1 - upRight1) / 2;
            Offset targetPositionOffset = Offset(offset, offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upRight1 - occOffsetUp));
            if (obstructionVal != 0) {
                if (upRightPiecesIgnorable == 0) {
                    upRightStopped = true;
                    break;
                }
                upRightPiecesIgnorable--;
            }
        }
    }
    // Outer
    if (!upRightStopped) {
        for (int upRight2 = upLeftLength + upRightLength - 1 - 2 * upMinLength;
             upRight2 >= upRightOffset;
             --upRight2) {
            int offset = upMaxLength - upRight2;
            Offset targetPositionOffset = Offset(offset, offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upRight2 - occOffsetUpRight));
            if (obstructionVal != 0) {
                if (upRightPiecesIgnorable == 0) {
                    break;
                }
                upRightPiecesIgnorable--;
            }
        }
    }

    // Down left direction
    // Inner
    if (downMinLength > 0) {
        int tmp_offset = 0;
        for (int downLeft1 = downLeftOffset;
             downLeft1 < downLeftOffset + 2 * downMinLength;
             downLeft1 += 2) {
            int offset = 1 + (downLeft1 - downLeftOffset) / 2;
            Offset targetPositionOffset = Offset(-offset, -offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            if (offset == downMinLength && targetPosition.x != 0) {
                // tmp_offset = 1;
            }

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downLeft1 - occOffsetDown));
            if (obstructionVal != 0) {
                if (downLeftPiecesIgnorable == 0) {
                    downLeftStopped = true;
                    break;
                }
                downLeftPiecesIgnorable--;
            }
        }
    }
    // Outer
    if (!downLeftStopped) {
        for (int downLeft2 = downLeftOffset + 2 * downMinLength;
             downLeft2 < downLeftOffset + 2 * downMinLength + downLeftLengthRemaining;
             ++downLeft2) {
            int offset = downLeft2 - (downLeftOffset + downMinLength - 1);
            Offset targetPositionOffset = Offset(-offset, -offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downLeft2 - occOffsetDownLeft));
            if (obstructionVal != 0) {
                if (downLeftPiecesIgnorable == 0) {
                    break;
                }
                downLeftPiecesIgnorable--;
            }
        }
    }

    // Down right direction
    // Inner
    if (downMinLength > 0) {
        int tmp_offset = 0;
        for (int downRight1 = downRightOffset;
             downRight1 < downRightOffset + 2 * downMinLength;
             downRight1 += 2) {
            int offset = 1 + (downRight1 - downRightOffset) / 2;
            Offset targetPositionOffset = Offset(offset, -offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            if (offset == downMinLength && targetPosition.x != 7) {
                // Square on the "other" side of the opening cone is hitting the wall
                tmp_offset = 1;
            }

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downRight1 - occOffsetDown - tmp_offset));
            if (obstructionVal != 0) {
                if (downRightPiecesIgnorable == 0) {
                    downRightStopped = true;
                    break;
                }
                downRightPiecesIgnorable--;
            }
        }
    }
    // Outer
    if (!downRightStopped) {
        for (int downRight2 = downRightOffset + 2 * downMinLength - 1;
             downRight2 < downRightOffset + 2 * downMinLength + downRightLengthRemaining - 1;
             ++downRight2) {
            int offset = downRight2 - (downLeftOffset + downMinLength - 1);
            Offset targetPositionOffset = Offset(offset, -offset);
            Position targetPosition = (targetPositionOffset + originPosition).toPosition();

            result |= (uint64_t) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downRight2 - occOffsetDownRight));
            if (obstructionVal != 0) {
                if (downRightPiecesIgnorable == 0) {
                    break;
                }
                downRightPiecesIgnorable--;
            }
        }
    }

    return result;
}


uint64_t generateWhitePawnAttackLookup(int originShift) {
    Position originPosition = position_from_shift(originShift);
    uint64_t positionMap = (uint64_t )1 << originShift;

    uint64_t targetMap = 0;
    if (originPosition.x != 0) {
        // Take to the left
        targetMap |= positionMap << 9;
    }
    if (originPosition.x != 7) {
        // Take to the right
        targetMap |= positionMap << 7;
    }

    return targetMap;
}

uint64_t generateBlackPawnAttackLookup(int originShift) {
    Position originPosition = position_from_shift(originShift);
    uint64_t positionMap = (uint64_t )1 << originShift;

    uint64_t targetMap = 0;
    if (originPosition.x != 0) {
        // Take to the left
        targetMap |= positionMap >> 7;
    }
    if (originPosition.x != 7) {
        // Take to the right
        targetMap |= positionMap >> 9;
    }

    return targetMap;
}

#endif //KINGOFTHEHILL_KI_GENERATORS_H
