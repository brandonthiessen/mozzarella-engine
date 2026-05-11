#ifndef __MOVE_GENERATION_H__
#define __MOVE_GENERATION_H__

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdint>

#include "position.h"
#include "piece.h"
#include "player.h"
#include "utils.h"


extern uint64_t PAWN_ADVANCE_MASKS[2][64];
extern uint64_t PAWN_ATTACK_MASKS[2][64];
extern uint64_t KNIGHT_MOVE_MASKS[64];
extern uint64_t KING_MOVE_MASKS[64];

// Masks to check and see if the squares that need to be crossed in
// order to castle are blocked
extern uint64_t W_QS_CASTLE_MASK;
extern uint64_t W_KS_CASTLE_MASK;
extern uint64_t B_QS_CASTLE_MASK;
extern uint64_t B_KS_CASTLE_MASK;

int generate_legal_moves(Position *p, uint32_t *moves, bool noisy_only);

int generate_castle_moves(Position *p, uint32_t *moves, bool noisy_only);
int generate_en_passant_moves(Position *p, uint32_t *moves, bool noisy_only);

int generate_pawn_moves(Position *p, uint32_t *moves, bool noisy_only);
int generate_knight_moves(Position *p, uint32_t *moves, bool noisy_only);

int generate_king_moves(Position *p, uint32_t *moves, bool noisy_only);

int generate_bishop_moves(Position *p, uint32_t *moves, bool noisy_only);
int generate_rook_moves(Position *p, uint32_t *moves, bool noisy_only);
int generate_queen_moves(Position *p, uint32_t *moves, bool noisy_only);

int generate_sliding_moves(Position *p, Piece piece, int deltas[], int ndeltas, uint32_t *moves, bool noisy_only);

bool move_gives_check(Position *p, uint32_t move);

bool is_direct_attack(Position *p, Piece moved_piece, int to_sq, int king_sq);
bool is_discovered_check(Position *p, int from_sq, int king_sq);

bool is_in_check(Position *p, Player player);
bool is_in_sliding_check(Position *p, Player player);

bool is_in_check2(Position *p, Player player, uint32_t move);
bool is_in_sliding_check2(Position *p, Player player, uint32_t move);

void render_bitboard(uint64_t bb, int origin_sq, char symbol);

#endif
