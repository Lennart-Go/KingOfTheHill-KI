#include <cstdio>
#include "move.h"
#include "util.h"


// Util tests
void testList() {
    int arr[] = {0, 1, 2, 3, 4};

    List<int> l1 = List<int>();
    List<int> l2 = List<int>(5);
    List<int> l3 = List<int>(arr, 5);

    l1.add(12);

    int res = l1.get(0);
    printf("%d\n", res);

    l1.remove(0);

    int res2 = l1.get(0);
    printf("%d\n", res2);

    int res3 = l2.pop();
    printf("%d\n", res3);

    size_t res4 = l3.size();
    printf("%zu\n", res4);

    size_t res5 = l3.length();
    printf("%zu\n", res5);

    bool res6 = l2.empty();
    printf("%d\n", res6);

    l3.remove(2);

    int res7 = l3.get(2);
    printf("%d\n", res7);
}





int main() {
    // setlocale(LC_ALL, "en_US.UTF-8");  // Necessary before printing

    t_board *board = initializeBoard();
    setFen(board, (char *) "rnbqkbnr/ppp1pppp/8/8/8/8/PPP1PPPP/RNBQKBNR");
    // printBoard(board);

    List<t_move> moves = generate_moves(board, false);
    printf("%zu\n", moves.length());

    for (int i = 0; i < moves.length(); ++i) {
        printMove(moves.get(i));
    }
}
