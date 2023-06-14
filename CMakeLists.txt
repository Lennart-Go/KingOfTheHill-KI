cmake_minimum_required(VERSION 3.22)
project(KingOfTheHill_KI)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(src)
include_directories(test)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(KingOfTheHill_KI
        src/board.cpp
        src/board.h
        src/game.cpp
        src/game.h
        src/main.cpp
        src/move.cpp
        src/move.h
        src/util.cpp
        src/util.h
        src/moveMaps.h)
target_link_libraries(KingOfTheHill_KI ${GTEST_LIBRARIES} pthread)

add_executable(Tests
        src/board.cpp
        src/board.h
        src/end.cpp
        src/end.h
        src/game.cpp
        src/game.h
        src/gameserver.cpp
        src/gameserver.h
        src/hikaru.cpp
        src/hikaru.h
        src/move.cpp
        src/move.h
        src/util.cpp
        src/util.h
        test/BoardTest.cpp
        test/EndTest.cpp
        test/main.cpp
        test/MoveTest.cpp src/moveMaps.h)
target_link_libraries(Tests ${GTEST_LIBRARIES} pthread)
