//
// Created by mbienert on 6/12/23.
//

#ifndef KINGOFTHEHILL_KI_GENERATORS_H
#define KINGOFTHEHILL_KI_GENERATORS_H


#include <cstdlib>
#include "util.h"


field generateXray(Position origin, Position target) {
    if (origin == target) {
        return (field) 0;
    }

    int xDiff = (int) target.x - (int) origin.x;
    int yDiff = (int) target.y - (int) origin.y;

    field xrayMap = 0;

    if (abs(xDiff) != abs(yDiff) && (xDiff != 0 && yDiff != 0)) {
        return (field) 0;
    }

    int xSign = xDiff != 0 ? xDiff / abs(xDiff) : 0;
    int ySign = yDiff != 0 ? yDiff / abs(yDiff) : 0;

    Position targetPosition;
    if (true) {
        int x, y;
        for (x = origin.x, y = origin.y;
             x != target.x || y != target.y;
             x += xSign, y += ySign) {
            targetPosition = Position(x, y);
            xrayMap |= (field) 1 << shift_from_position(targetPosition);
        }
    }

    return xrayMap;
}


field generateRookLookup(int originShift, uint16_t obstructionIdentifier) {
    Position originPosition = position_from_shift(originShift);

    int upLength = 7 - originPosition.y;  // 2
    int leftLength = originPosition.x;  // 1
    int rightLength = 7 - originPosition.x;  // 6
    int downLength = originPosition.y;  // 5

    int upOffset = 0;  // 0
    int leftOffset = upOffset + upLength;  // 13
    int rightOffset = leftOffset + leftLength;  // 2
    int downOffset = rightOffset + rightLength;  // 8


    field result = 0;
    // Up direction
    for (int up = upOffset + upLength - 1; up >= upOffset; --up) {
        Offset targetPositionOffset = Offset(0, (upOffset + upLength - up));
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (field) 1 << shift_from_position(targetPosition);

        // Break loop if target field is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << up);
        if (obstructionVal != 0) {
            break;
        }
    }

    // Left direction
    for (int left = leftOffset + leftLength - 1; left >= leftOffset; --left) {
        Offset targetPositionOffset = Offset(-(leftOffset + leftLength - left), 0);
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (field) 1 << shift_from_position(targetPosition);

        // Break loop if target field is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << left);
        if (obstructionVal != 0) {
            break;
        }
    }

    // Right direction
    for (int right = rightOffset; right < rightOffset + rightLength; ++right) {
        Offset targetPositionOffset = Offset(right - (rightOffset - 1), 0);
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (field) 1 << shift_from_position(targetPosition);

        // Break loop if target field is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << right);
        if (obstructionVal != 0) {
            break;
        }
    }

    // Down direction
    for (int down = downOffset; down < downOffset + downLength; ++down) {
        Offset targetPositionOffset = Offset(0, -(down - (downOffset - 1)));
        Position targetPosition = (targetPositionOffset + originPosition).toPosition();

        result |= (field) 1 << shift_from_position(targetPosition);

        // Break loop if target field is occupied
        int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << down);
        if (obstructionVal != 0) {
            break;
        }
    }

    return result;
}


field generateBishopLookup(int originShift, uint16_t obstructionIdentifier) {
    // When ignoring the borders the required band can be reduced from 14 to 9 bits
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

    field result = 0;

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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upLeft1 - occOffsetUp + tmp_offset));
            if (obstructionVal != 0) {
                upLeftStopped = true;
                break;
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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upLeft2 - occOffsetUpLeft));
            if (obstructionVal != 0) {
                break;
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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upRight1 - occOffsetUp));
            if (obstructionVal != 0) {
                upRightStopped = true;
                break;
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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (upRight2 - occOffsetUpRight));
            if (obstructionVal != 0) {
                break;
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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downLeft1 - occOffsetDown));
            if (obstructionVal != 0) {
                downLeftStopped = true;
                break;
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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downLeft2 - occOffsetDownLeft));
            if (obstructionVal != 0) {
                break;
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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downRight1 - occOffsetDown - tmp_offset));
            if (obstructionVal != 0) {
                downRightStopped = true;
                break;
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

            result |= (field) 1 << shift_from_position(targetPosition);

            int obstructionVal = obstructionIdentifier & ((uint16_t) 1 << (downRight2 - occOffsetDownRight));
            if (obstructionVal != 0) {
                break;
            }
        }
    }

    return result;
}

#endif //KINGOFTHEHILL_KI_GENERATORS_H
