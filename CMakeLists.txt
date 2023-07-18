cmake_minimum_required(VERSION 3.17)
project(KingOfTheHill_KI)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(src)
include_directories(test)

add_executable(KingOfTheHill_KI
        src/main.cpp
        src/util.h
        src/util.cpp
        src/hash.cpp
        src/hash.h
        src/board.cpp
        src/board.h
        src/move.h
        src/end.h
        src/hikaru.h
        src/game.cpp
        src/game.h
        src/scoredMove.cpp
        src/scoredMove.h
        src/moveMaps.h
        src/pieceSquareTable.h
        src/transpositionTable.cpp
        src/transpositionTable.h src/monteCarloTree.cpp src/monteCarloTree.h)
