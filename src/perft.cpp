#include "perft.h"

uint64_t perft(Position *p, int depth) {
    if (depth == 0) return 1;
    std::array<uint32_t, 256> moves;
    int n = generate_legal_moves(p, moves.data(), false);
    if (depth == 1) return moves.size();

    long total = 0;
    for (int i = 0; i < n; i++) {
        uint32_t m = moves[i];
        p->move(m);
        long count = perft(p, depth - 1);
        total += count;
        p->unmove(m);
    }

    return total;
}
