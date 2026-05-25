#include "search.h"
#include "move.h"
#include <cstdint>

#include "log.h" // DEBUG LOGGING

uint64_t nodes = 0;
uint64_t qnodes = 0;
uint64_t tt_hits = 0;

constexpr int _mvv_lva[6][6] = {
    // Attacker: Pawn, Knight, Bishop, Rook, Queen, King
    {105, 205, 305, 405, 505, 605}, // Victim: Pawn
    {104, 204, 304, 404, 504, 604}, // Victim: Knight
    {103, 203, 303, 403, 503, 603}, // Victim: Bishop
    {102, 202, 302, 402, 502, 602}, // Victim: Rook
    {101, 201, 301, 401, 501, 601}, // Victim: Queen
    {100, 200, 300, 400, 500, 600}  // Victim: King
};


struct ScoredMove {
    uint32_t move;
    int score;
};


/*
 * Most Valuable Victim : Least Valuable Attacker - move ordering to get higher value moves.
 *
 * Example: pxQ should be chosen over qxP.
 */
inline int mvv_lva(uint32_t m) {
    if (get_flags(m) & MoveFlags::CAPTURE) return _mvv_lva[get_captured(m)][get_piece(m)];
    return 0;
}


/*
 * Constructor for the MovePicker class.
 * tt_mb is the size of the transposition table in MB.
 */
MovePicker::MovePicker(int tt_mb) :
    tt(tt_mb) {}


/*
 * Destructor for the MovePicker class.
 */
MovePicker::~MovePicker() {
    // TODO: implement?
}

/*
 * Public facing interface for the MovePicker. Returns the 32-bit encoded move which represents
 * the optimal move to make in the given position.
 *
 * Runs minimax search with alpha-beta pruning up to a fixed depth.
 *
 * Internally relies on search() and q_search() in order to search the game space.
 */
uint32_t MovePicker::find_best_move_fixed_depth(Position *p, int depth) {

    nodes = 0;
    qnodes = 0;
    tt_hits = 0;

    int best_score = INT_MIN;
    uint32_t best_move;

    std::array<uint32_t, 256> moves;
    int n = generate_legal_moves(p, moves.data(), false);

    for (int i = 0; i < n; i++) {
        uint32_t m = moves[i];
        p->move(m);
        int score = -(this->search(p, depth - 1, 0, -MATE_SCORE, MATE_SCORE));
        p->unmove(m);

        if (score > best_score) {
            best_score = score;
            best_move = m;
        }
    }

    LOG("nodes=" << nodes);
    LOG("qnodes=" << qnodes);
    LOG("tt_hits=" << tt_hits);

    return best_move;
}


/*
 * Public facing interface for the MovePicker. Returns the 32-bit encoded move which represents
 * the optimal move to make in the given position.
 *
 * Runs an iterative deepening search based on ms (the total time remaining in milliseconds).
 *
 * Internally relies on search() and q_search() in order to search the game space.
 */
uint32_t MovePicker::find_best_move(Position *p, int ms) {

    nodes = 0;
    qnodes = 0;
    tt_hits = 0;

    start_timer(ms);

    int best_score;
    uint32_t best_move;

    std::array<uint32_t, 256> moves;
    int n = generate_legal_moves(p, moves.data(), false);

    for (int depth = 1; ; depth++) {

        int alpha = -MATE_SCORE;
        int beta = MATE_SCORE;

        uint32_t iter_best_move = 0;
        int iter_best_score = INT_MIN;

        for (int i = 0; i < n; i++) {
            uint32_t m = moves[i];

            if (time_up()) break;

            p->move(m);
            int score = -(this->search(p, depth - 1, 0, -beta, -alpha));
            p->unmove(m);

            if (score > iter_best_score) {
                iter_best_score = score;
                iter_best_move = m;
            }

            if (score > alpha)
                alpha = score;
        }

        if (!time_up()) {
            best_move = iter_best_move;
            best_score = iter_best_score;
        } else {
            break;
        }
    }

    LOG("nodes=" << nodes);
    LOG("qnodes=" << qnodes);
    LOG("tt_hits=" << tt_hits);

    (void) best_score;

    return best_move;
}


/*
 * Alpha-beta minimax search which recursively searches positions and returns the score of the
 * best move according to the evaluation function at the given depth.
 */
int MovePicker::search(Position *p, int depth, int ply, int alpha, int beta) {
    nodes++;
    
    // Probe the transposition table first
    uint64_t z = p->zobrist();
    int probe_score;
    if (this->tt.probe(z, depth, &probe_score, alpha, beta)) {
        tt_hits++;
        return probe_score;
    }

    std::array<uint32_t, 256> moves;
    int n = generate_legal_moves(p, moves.data(), false);

    if (depth == 0) // return (p->player_to_move == Player::WHITE) ? evaluate(p) : -evaluate(p);
        return q_search(p, ply, 0, alpha, beta);

    if (n == 0) {
        if (is_in_check(p, p->player_to_move)) return -MATE_SCORE + ply; // Checkmate
        else return 0; // Stalemate
    }

    int alpha_orig = alpha;
    int best_score = INT_MIN;

    // Build a list of moves paired with a heuristic score for ordering
    std::array<ScoredMove, 256> scored_list;

    for (int i = 0; i < n; i++) {
        int score = 0;

        uint32_t m = moves[i];

        // Prioritize captures using MVV-LVA
        if (get_flags(m) & MoveFlags::CAPTURE)
            score = mvv_lva(m);

        scored_list[i] = {m, score};
    }

    // Sort moves so that higher-scoring (more promising) ones are searched first
    std::sort(scored_list.begin(), scored_list.begin() + n,
            [](const ScoredMove& a, const ScoredMove& b) {
                return a.score > b.score;
            });

    // Search moves in order with alpha-beta pruning
    for (int i = 0; i < n; i++) {
        const auto& sm = scored_list[i];
        p->move(sm.move);
        int score = -(this->search(p, depth - 1, ply + 1, -beta, -alpha));
        p->unmove(sm.move);

        if (score > alpha) alpha = score;
        if (score > best_score) best_score = score;

        if (alpha >= beta) break;
    }

    // Store new entry in the transposition table
    this->tt.store(z, depth, best_score, alpha_orig, beta);

    return best_score;
}


/*
 * Quiescence search which only returns evaluations of "quiet" positions, otherwise, continually
 * searches "noisy" positions until they become quiet.
 *
 * A "quiet" position is one where there is no immediate tactical move,
 * such as a capture/check/promotion.
 */
int MovePicker::q_search(Position *p, int ply, int qply, int alpha, int beta) {
    qnodes++;
    if (qply == 0) {
        LOG("QSEARCH ROOT enter alpha=" << alpha << " beta=" << beta);
    }

    int static_eval_white = evaluate(p);

    // Upper limit to how far q-search can check
    if (qply >= 4) return static_eval_white;
    
    int best_score = (p->player_to_move == Player::WHITE) ? static_eval_white : -static_eval_white;

    if (best_score >= beta) return best_score;
    if (best_score > alpha) alpha = best_score;

    std::array<uint32_t, 256> noisy_moves;
    int n = generate_legal_moves(p, noisy_moves.data(), true);

    // Terminal node: no moves
    if (n == 0) {
        // If in check, it's checkmate
        if (is_in_check(p, p->player_to_move)) return -MATE_SCORE + ply;
        return 0; // Otherwise it's stalemate
    }

    std::sort(noisy_moves.begin(), noisy_moves.begin() + n,
              [&](uint32_t x, uint32_t y) {return mvv_lva(x) > mvv_lva(y);});

    for (int i = 0; i < n; i++) {
        uint32_t m = noisy_moves[i];

        p->move(m);
        int score = -q_search(p, ply + 1, qply + 1, -beta, -alpha);
        p->unmove(m);

        if (qply == 0) {
            LOG("QSEARCH ROOT move=" << move_to_string(m)
                << " score=" << score);
        }

        if (score > beta) return score;

        if (score > alpha) alpha = score;
    }

    return alpha;
}
