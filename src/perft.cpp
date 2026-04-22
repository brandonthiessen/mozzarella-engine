#include "perft.h"

uint64_t perft(Position *p, int depth) {
    if (depth == 0) return 1;
    std::vector<uint32_t> moves = generate_legal_moves(p);
    if (depth == 1) return moves.size();

    long total = 0;
    for (uint32_t m: moves) {
        p->move(m);
        long count = perft(p, depth - 1);
        total += count;
        p->unmove(m);
    }

    return total;
}
