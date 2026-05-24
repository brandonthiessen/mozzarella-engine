#include "move_generation.h"
#include "move.h"
#include "piece.h"
#include <cstdint>

#include "log.h"

uint64_t PAWN_ADVANCE_MASKS[2][64] = {
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x1010000, 0x2020000, 0x4040000, 0x8080000, 0x10100000, 0x20200000, 0x40400000, 0x80800000 ,
        0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 ,
        0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000 ,
        0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000, 0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000 ,
        0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000, 0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
        0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    },
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
        0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
        0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
        0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
        0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
        0x10100000000, 0x20200000000, 0x40400000000, 0x80800000000, 0x101000000000, 0x202000000000, 0x404000000000, 0x808000000000,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    }
};
uint64_t PAWN_ATTACK_MASKS[2][64] = {
    {
        0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
        0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
        0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
        0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
        0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
        0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
        0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000, 0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    },
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x2, 0x5, 0xa, 0x14, 0x28, 0x50, 0xa0, 0x40,
        0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
        0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
        0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
        0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
        0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
        0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000
    }
};
uint64_t KNIGHT_MOVE_MASKS[64] = {
    0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000,
    0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010, 0x40200020,
    0x204000402, 0x508000805, 0xa1100110a, 0x1422002214, 0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040,
    0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000,
    0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000,
    0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
    0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
    0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000
};
uint64_t KING_MOVE_MASKS[64] = {
    0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040,
    0x30203, 0x70507, 0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0,
    0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000,
    0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000,
    0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000,
    0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000,
    0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
    0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000
};
uint64_t BISHOP_MOVE_MASKS[64] = {
    0x8040201008040200, 0x80402010080500, 0x804020110a00, 0x8041221400, 0x182442800, 0x10204885000, 0x102040810a000, 0x102040810204000, 0x4020100804020002, 0x8040201008050005, 0x804020110a000a, 0x804122140014, 0x18244280028, 0x1020488500050, 0x102040810a000a0, 0x204081020400040, 0x2010080402000204, 0x4020100805000508, 0x804020110a000a11, 0x80412214001422, 0x1824428002844, 0x102048850005088, 0x2040810a000a010, 0x408102040004020, 0x1008040200020408, 0x2010080500050810, 0x4020110a000a1120, 0x8041221400142241, 0x182442800284482, 0x204885000508804, 0x40810a000a01008, 0x810204000402010, 0x804020002040810, 0x1008050005081020, 0x20110a000a112040, 0x4122140014224180, 0x8244280028448201, 0x488500050880402, 0x810a000a0100804, 0x1020400040201008, 0x402000204081020, 0x805000508102040, 0x110a000a11204080, 0x2214001422418000, 0x4428002844820100, 0x8850005088040201, 0x10a000a010080402, 0x2040004020100804, 0x200020408102040, 0x500050810204080, 0xa000a1120408000, 0x1400142241800000, 0x2800284482010000, 0x5000508804020100, 0xa000a01008040201, 0x4000402010080402, 0x2040810204080, 0x5081020408000, 0xa112040800000, 0x14224180000000, 0x28448201000000, 0x50880402010000, 0xa0100804020100, 0x40201008040201, 
};
uint64_t ROOK_MOVE_MASKS[64] = {
    0x1010101010101fe, 0x2020202020202fd, 0x4040404040404fb, 0x8080808080808f7, 0x10101010101010ef, 0x20202020202020df, 0x40404040404040bf, 0x808080808080807f, 0x10101010101fe01, 0x20202020202fd02, 0x40404040404fb04, 0x80808080808f708, 0x101010101010ef10, 0x202020202020df20, 0x404040404040bf40, 0x8080808080807f80, 0x101010101fe0101, 0x202020202fd0202, 0x404040404fb0404, 0x808080808f70808, 0x1010101010ef1010, 0x2020202020df2020, 0x4040404040bf4040, 0x80808080807f8080, 0x1010101fe010101, 0x2020202fd020202, 0x4040404fb040404, 0x8080808f7080808, 0x10101010ef101010, 0x20202020df202020, 0x40404040bf404040, 0x808080807f808080, 0x10101fe01010101, 0x20202fd02020202, 0x40404fb04040404, 0x80808f708080808, 0x101010ef10101010, 0x202020df20202020, 0x404040bf40404040, 0x8080807f80808080, 0x101fe0101010101, 0x202fd0202020202, 0x404fb0404040404, 0x808f70808080808, 0x1010ef1010101010, 0x2020df2020202020, 0x4040bf4040404040, 0x80807f8080808080, 0x1fe010101010101, 0x2fd020202020202, 0x4fb040404040404, 0x8f7080808080808, 0x10ef101010101010, 0x20df202020202020, 0x40bf404040404040, 0x807f808080808080, 0xfe01010101010101, 0xfd02020202020202, 0xfb04040404040404, 0xf708080808080808, 0xef10101010101010, 0xdf20202020202020, 0xbf40404040404040, 0x7f80808080808080, 
};

uint64_t W_QS_CASTLE_MASK = 0xE;
uint64_t W_KS_CASTLE_MASK = 0x60;
uint64_t B_QS_CASTLE_MASK = 0xE00000000000000;
uint64_t B_KS_CASTLE_MASK = 0x6000000000000000;

int generate_legal_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int n = 0;

    Player player = p->player_to_move;

    int (*generators[])(Position*, uint32_t*, bool) = {
        generate_pawn_moves,
        generate_en_passant_moves,
        generate_knight_moves,
        generate_bishop_moves,
        generate_rook_moves,
        generate_queen_moves,
        generate_king_moves
    };

    for (auto gen : generators) {
        n += gen(p, moves + n, noisy_only);
    }

    // Cannot castle while you're in check
    if (!is_in_check(p, player)) {
        n += generate_castle_moves(p, moves + n, noisy_only);
    }

    // Filter for legal moves
    int j = 0;

    for (int i = 0; i < n; i++) {
        uint32_t m = moves[i];

        p->move(m);
        if (!is_in_check(p, player)) {
            moves[j++] = m;
        }
        p->unmove(m);
    }

    return j;
}


int generate_castle_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int n = 0;

    uint64_t all_occupied = p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK);
    
    // TODO: merge these shifts since they are the same, just different directions
    if (p->castling_rights[p->player_to_move] & CastlingRights::KING_UNMOVED) {
        if (p->castling_rights[p->player_to_move] & CastlingRights::QROOK) {
            bool castle_through_check = false;
            p->bitboards[p->player_to_move][Piece::KING] >>= 1;
            if (is_in_check(p, p->player_to_move)) castle_through_check = true;
            p->bitboards[p->player_to_move][Piece::KING] <<= 1;

            if (!castle_through_check) {
                if (p->player_to_move == Player::WHITE && !(W_QS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(4, 2, Piece::KING, 0, 0, MoveFlags::QUEEN_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        p->move(move);
                        if (is_in_check(p, Player::BLACK)) {
                            moves[n++] = move;
                        }
                        p->unmove(move);
                    }
                    else {
                        moves[n++] = move;

                    }
                } else if (p->player_to_move == Player::BLACK && !(B_QS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(60, 58, Piece::KING, 0, 0, MoveFlags::QUEEN_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        p->move(move);
                        if (is_in_check(p, Player::WHITE)) {
                            moves[n++] = move;
                        }
                        p->unmove(move);
                    }
                    else {
                        moves[n++] = move;

                    }

                }
            }
        }

        if (p->castling_rights[p->player_to_move] & CastlingRights::KROOK) {
            bool castle_through_check = false;
            p->bitboards[p->player_to_move][Piece::KING] <<= 1;
            if (is_in_check(p, p->player_to_move)) castle_through_check = true;
            p->bitboards[p->player_to_move][Piece::KING] >>= 1;

            if (!castle_through_check) {
                if (p->player_to_move == Player::WHITE && !(W_KS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(4, 6, Piece::KING, 0, 0, KING_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        p->move(move);
                        if (is_in_check(p, Player::BLACK)) {
                            moves[n++] = move;
                        }
                        p->unmove(move);
                    }
                    else {
                        moves[n++] = move;
                    }

                } else if (p->player_to_move == Player::BLACK && !(B_KS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(60, 62, Piece::KING, 0, 0, KING_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        p->move(move);
                        if (is_in_check(p, Player::WHITE)) {
                            moves[n++] = move;
                        }
                        p->unmove(move);
                    }
                    else {
                        moves[n++] = move;
                    }
                }
            }
        }
    }

    return n;
}

int generate_en_passant_moves(Position *p, uint32_t *moves, bool noisy_only) {
    (void) noisy_only;
    int n = 0;

    if (!(p->move_stack.empty()) && get_flags(p->move_stack.back()) == MoveFlags::DOUBLE_PAWN_PUSH) {
        uint64_t occupied = (p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK));
        
        int push_to_sq = get_to_sq(p->move_stack.back());
        int to_sq = push_to_sq + (1 - 2 * (p->player_to_move)) * 8;
        int double_push_file = push_to_sq % 8;

        if (!((1ULL << to_sq) & occupied)) {
            if (p->bitboards[p->player_to_move][Piece::PAWN] & (1ULL << (push_to_sq - 1)) && double_push_file != 0) {
                moves[n++] = encode_move(push_to_sq - 1, to_sq, Piece::PAWN, Piece::PAWN, 0, MoveFlags::EN_PASSANT | MoveFlags::CAPTURE);
            }

            if (p->bitboards[p->player_to_move][Piece::PAWN] & (1ULL << (push_to_sq + 1)) && double_push_file != 7) {
                moves[n++] = encode_move(push_to_sq + 1, to_sq, Piece::PAWN, Piece::PAWN, 0, MoveFlags::EN_PASSANT | MoveFlags::CAPTURE);
            }
        }
    }

    return n;
}

int generate_pawn_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int n = 0;

    Player player = p->player_to_move;
    Player opponent = Player (1 - player);

    uint64_t occupied = p->get_occupied(player);
    uint64_t opponent_occupied = p->get_occupied(opponent);
    uint64_t all_occupied = occupied | opponent_occupied;
    uint64_t pawn_bb = p->bitboards[p->player_to_move][Piece::PAWN];

    while (pawn_bb) {
        int pawn_sq = pop_lsb(pawn_bb);

        uint64_t all_pawn_advances = PAWN_ADVANCE_MASKS[player][pawn_sq] & ~(all_occupied);

        // Edge case: a pawn cannot advance two steps if obstructed
        // TODO: change this to use bitboards and shifting directly (there is an (incomplete) Python implementation of this)
        if (player == Player::WHITE && 8 <= pawn_sq && pawn_sq <= 15) {
            if (all_occupied & (1ULL << (pawn_sq + 8))) {
                all_pawn_advances &= ~(1ULL << (pawn_sq + 16));
            }
        } else if (player == Player::BLACK && 48 <= pawn_sq && pawn_sq <= 55) {
            if (all_occupied & (1ULL << (pawn_sq - 8))) {
                all_pawn_advances &= ~(1ULL << (pawn_sq - 16));
            }
        }

        // This is kind of ridiculous.
        // Why am I iterating over a mask which will (83% of the time) have one item?
        while (all_pawn_advances) {
            int to_sq = pop_lsb(all_pawn_advances);

            uint32_t flags = 0;

            // If the target square is a promotion
            if ((56 * (opponent)) <= to_sq && to_sq <= (7 + 56 * (opponent))) {
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::QUEEN, MoveFlags::QUEEN_PROMO);
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::ROOK, MoveFlags::ROOK_PROMO);
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::BISHOP, MoveFlags::BISHOP_PROMO);
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::KNIGHT, MoveFlags::KNIGHT_PROMO);
                break;
            }
            if (std::abs(pawn_sq - to_sq) == 16) {
                flags = DOUBLE_PAWN_PUSH;
            }
            if (noisy_only) {
                uint32_t move = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, 0, flags);
                if (move_gives_check(p, move)) {
                    moves[n++] = move;
                }
            } else {
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, 0, flags);
            }
        }

        uint64_t all_pawn_attacks = PAWN_ATTACK_MASKS[player][pawn_sq] & opponent_occupied;

        Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        while (all_pawn_attacks) {
            int to_sq = pop_lsb(all_pawn_attacks);

            for (Piece attackable_piece: attackable_pieces) {
                if (p->bitboards[opponent][attackable_piece] & (1ULL << to_sq)) {
                    if ((56 * opponent) <= to_sq && to_sq <= (7 + 56 * opponent)) {
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::QUEEN, MoveFlags::QUEEN_PROMO | MoveFlags::CAPTURE);
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::ROOK, MoveFlags::ROOK_PROMO | MoveFlags::CAPTURE);
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::BISHOP, MoveFlags::BISHOP_PROMO | MoveFlags::CAPTURE);
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::KNIGHT, MoveFlags::KNIGHT_PROMO | MoveFlags::CAPTURE);
                        break;
                    }
                    moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, 0, MoveFlags::CAPTURE);
                }
            }
        }
    }

    return n;
}

int generate_knight_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int n = 0;

    Player player = p->player_to_move;
    Player opponent = (Player) (1 - p->player_to_move);

    uint64_t occupied = p->get_occupied(player);
    uint64_t opponent_occupied = p->get_occupied(opponent);

    uint64_t knight_bb = p->bitboards[player][Piece::KNIGHT];

    Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};

    while (knight_bb) {
        int knight_sq = pop_lsb(knight_bb);

        uint64_t all_knight_moves = KNIGHT_MOVE_MASKS[knight_sq] & ~occupied;

        while (all_knight_moves) {
            int to_sq = pop_lsb(all_knight_moves);
            uint64_t to_bb = (1ULL << to_sq); 

            if (opponent_occupied & to_bb) {
                for (Piece attackable_piece: attackable_pieces) {
                    if (p->bitboards[opponent][attackable_piece] & to_bb) {
                        // Always store captures
                        uint32_t move = encode_move(knight_sq, to_sq, Piece::KNIGHT, attackable_piece, 0, MoveFlags::CAPTURE);
                        moves[n++] = move;
                    }
                }
            } else {
                uint32_t move = encode_move(knight_sq, to_sq, Piece::KNIGHT, 0, 0, 0);
                if (noisy_only) {
                    if (!(move_gives_check(p, move))) {
                        continue;
                    }
                }
                moves[n++] = move;
            }
        }
    }

    return n;
}

int generate_king_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int n = 0;

    Player opponent = (Player) (1 - p->player_to_move);
    uint64_t king_bb = p->bitboards[p->player_to_move][Piece::KING];

    int king_sq = pop_lsb(king_bb);

    uint64_t king_move_bb = KING_MOVE_MASKS[king_sq] & ~(p->get_occupied(p->player_to_move));

    uint64_t captures = KING_MOVE_MASKS[king_sq] & p->get_occupied(opponent);
    
    if (!noisy_only) {
        uint64_t non_captures = king_move_bb & ~(p->get_occupied(opponent));

        while (non_captures) {
            int to_sq = pop_lsb(non_captures);
            uint32_t move = encode_move(king_sq, to_sq, Piece::KING, 0, 0, 0);
            moves[n++] = move;
        }
    } else {
        // Noisy only king move generation - only need to check to see if a move gives discovered check
        uint64_t non_captures = king_move_bb & ~(p->get_occupied(opponent));
        while (non_captures) {
            int to_sq = pop_lsb(non_captures);
            uint32_t move = encode_move(king_sq, to_sq, Piece::KING, 0, 0, 0);
            if (move_gives_check(p, move)) {
                moves[n++] = move;
            }
        }
    }

    Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
    while (captures) {
        int to_sq = pop_lsb(captures);
        uint64_t to_bb = (1ULL << to_sq);

        for (Piece attackable_piece: attackable_pieces) {
            if (to_bb & p->bitboards[opponent][attackable_piece]) {
                moves[n++] = encode_move(king_sq, to_sq, Piece::KING, attackable_piece, 0, MoveFlags::CAPTURE);
            }
        } 
    }

    return n;
}

int generate_bishop_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int deltas[] = {-9, -7, 7, 9};
    return generate_sliding_moves(p, Piece::BISHOP, deltas, 4, moves, noisy_only);
}

int generate_rook_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int deltas[] = {-8, -1, 1, 8};
    return generate_sliding_moves(p, Piece::ROOK, deltas, 4, moves, noisy_only);
}

int generate_queen_moves(Position *p, uint32_t *moves, bool noisy_only) {
    int deltas[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    return generate_sliding_moves(p, Piece::QUEEN, deltas, 8, moves, noisy_only);
}

int generate_sliding_moves(Position *p, Piece piece, int deltas[], int ndeltas, uint32_t *moves, bool noisy_only) {
    Player player = p->player_to_move;
    Player opponent = (Player) (1 - player);

    uint64_t occupied = p->get_occupied(player);
    uint64_t opponent_occupied = p->get_occupied(opponent);

    Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
    
    uint64_t piece_sqs = p->bitboards[player][piece];

    // Number of moves generated
    int n = 0;

    while (piece_sqs) {
        int piece_sq = pop_lsb(piece_sqs);
        
        int piece_file = piece_sq % 8;

        for (int i = 0; i < ndeltas; i++) {
            int last_file = piece_file;

            for (int step = 1; step < 8; step++) {
                int to_sq = piece_sq + deltas[i] * step;
                uint64_t to_bb = (1ULL << to_sq);
                int to_file = to_sq % 8;

                if ((to_sq < 0 || to_sq > 63)
                    || (abs(to_file - last_file) > 1)
                    || (occupied & (1ULL << to_sq))) {
                    break;
                }

                if (opponent_occupied & to_bb) {
                    for (Piece attackable_piece: attackable_pieces) {
                        if (p->bitboards[opponent][attackable_piece] & to_bb) {
                            // Always store this move - it is a capture
                            moves[n++] = encode_move(piece_sq, to_sq, piece, attackable_piece, 0, MoveFlags::CAPTURE);
                            break;
                        }
                    }
                    break;
                } else {
                    // This move is not a capture
                    uint32_t move = encode_move(piece_sq, to_sq, piece, 0, 0, 0);
                    if (noisy_only) {
                        // Check to see if it leads to a check
                        if (move_gives_check(p, move)) {
                            moves[n++] = move;
                        }
                    } else {
                        moves[n++] = move;
                    }
                }

                last_file = to_file;
            }
        }
    }

    return n;
}


// Checks to see if playing the move puts the opposing king in check.
// Only assumes that move is pseudolegal.
// Assumes that the move has not been played on position p.
// NOTE: This function (and its helpers) *do not* take pawn promotions into consideration,
// since they are only intended to be used for noisy move filtering. If a pawn is being
// promoted, the move is already noisy! As such, the function is not 100% "correct".
bool move_gives_check(Position *p, uint32_t move) {
    
    Player player = p->player_to_move;
    Player opponent = (Player) (1 - player);

    // Find the opposing king square
    uint64_t king_bb = p->bitboards[opponent][Piece::KING];
    int king_sq = pop_lsb(king_bb);

    // Get the piece type
    Piece moved_piece = get_piece(move);

    // Get the destination square
    int to_sq = get_to_sq(move);
    int from_sq = get_from_sq(move);

    // Check direct attack by moved piece
    if (is_direct_attack(p, moved_piece, to_sq, king_sq)) {
        return true;
    }

    // Check discovered attack
    if (is_discovered_check(p, from_sq, king_sq)) {
        return true;
    }

    return false;
}


bool is_direct_attack(Position *p, Piece moved_piece, int to_sq, int king_sq) {
    
    Player player = p->player_to_move;
    uint64_t king_bb = 1ULL << king_sq;

    switch (moved_piece) {
        case Piece::PAWN:
            return PAWN_ATTACK_MASKS[player][to_sq] & king_bb;
        case Piece::KNIGHT:
            return KNIGHT_MOVE_MASKS[to_sq] & king_bb;
        default:
            break;
    }

    uint64_t attack_mask = 0;

    switch (moved_piece) {
        case Piece::BISHOP:
            attack_mask = BISHOP_MOVE_MASKS[to_sq];
            break;
        case Piece::ROOK:
            attack_mask = ROOK_MOVE_MASKS[to_sq];
            break;
        case Piece::QUEEN:
            attack_mask = BISHOP_MOVE_MASKS[to_sq] | ROOK_MOVE_MASKS[to_sq];
            break;
        default:
            break;
    }

    if (!(attack_mask & king_bb)) {
        return false;
    }

    int kr = king_sq / 8, kf = king_sq % 8;
    int tr = to_sq / 8, tf = to_sq % 8;

    int dr = kr - tr, df = kf - tf;

    int sr = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
    int sf = (df == 0) ? 0 : (df > 0 ? 1 : -1);

    int step = sr * 8 + sf;

    uint64_t occupied = (p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK));
    occupied &= ~king_bb;

    for (int sq = to_sq + step; sq != king_sq; sq += step) {
        if (occupied & 1ULL << sq) {
            return false;
        }
    }

    return true;
}


bool is_discovered_check(Position *p, int from_sq, int king_sq) {
    
    uint64_t from_bb = 1ULL << from_sq;
    uint64_t king_bb = 1ULL << king_sq;
    Player player = p->player_to_move;

    // Isolate the direction
    int kr = king_sq / 8, kf = king_sq % 8;
    int fr = from_sq / 8, ff = from_sq % 8;

    int dr = kr - fr, df = kf - ff;

    // Check for geometrically impossible rays
    if (!(dr == 0 || df == 0 || abs(dr) == abs(df))) {
        return false;
    }

    // Normalize and invert the direction (we're going king square -> away from from_square)
    int sr = (dr == 0) ? 0 : (dr > 0 ? -1 : 1);
    int sf = (df == 0) ? 0 : (df > 0 ? -1 : 1);

    int step = sr * 8 + sf;

    uint64_t threats = p->bitboards[player][Piece::QUEEN];
    if (abs(sr) == abs(sf)) {
        threats |= p->bitboards[player][Piece::BISHOP];
    } else {
        threats |= p->bitboards[player][Piece::ROOK];
    }
    threats &= ~from_bb;

    uint64_t non_threats = ((p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK)) & ~king_bb) & ~threats;

    for (int sq = king_sq + step; sq >= 0 && sq < 64; sq += step) {
        // Prevent wrapping across file boundaries
        int prev_file = (sq - step) % 8;
        int curr_file = sq % 8;
        
        // If files are too far apart, we've wrapped
        if (abs(curr_file - prev_file) > 1) {
            return false;
        }

        uint64_t sq_bb = 1ULL << sq;

        if (threats & sq_bb) {
            return true;
        }

        if (non_threats & sq_bb) {
            return false;
        }
    }

    // Will this ever get reached?
    // LOG("discovered check: reached final condition")
    return false;

}


bool is_in_check(Position *p, Player player) {
    Player opponent = (Player) (1 - player);
    uint64_t king_bb = p->bitboards[player][Piece::KING];
    int king_sq = pop_lsb(king_bb);

    if (KNIGHT_MOVE_MASKS[king_sq] & p->bitboards[opponent][Piece::KNIGHT]) {
        return true;
    }
    if (PAWN_ATTACK_MASKS[player][king_sq] & p->bitboards[opponent][Piece::PAWN]) {
        return true;
    }
    if (KING_MOVE_MASKS[king_sq] & p->bitboards[opponent][Piece::KING]) {
        return true;
    }

    return is_in_sliding_check(p, player);
}


bool is_in_sliding_check(Position* p, Player player) {
    // Locate the king square
    uint64_t king_bb = p->bitboards[player][Piece::KING];
    int king_sq = pop_lsb(king_bb);
    int king_file = king_sq % 8;
    Player opponent = (Player) (1 - player);

    // Opponent's sliding piece bitboards
    uint64_t opp_bishops_and_queens = p->bitboards[opponent][Piece::BISHOP] | p->bitboards[opponent][Piece::QUEEN];
    uint64_t opp_rooks_and_queens   = p->bitboards[opponent][Piece::ROOK] | p->bitboards[opponent][Piece::QUEEN];

    uint64_t all_occupied = (p->get_occupied(Player::WHITE) | p->get_occupied(Player::BLACK));

    // Sliding directions
    uint64_t sliding_bbs[] = {opp_bishops_and_queens, opp_rooks_and_queens};
    int deltas[2][4] = {{-9, -7, 7, 9}, {-8, -1, 1, 8}};

    // Loop over bishop- and rook-like attackers
    for (int i = 0; i < 2; i++) {
        uint64_t occupied_no_sliding = (all_occupied & ~(sliding_bbs[i]));
        for (int j = 0; j < 4; j++) {
            int curr_file = king_file;

            for (int step = 1; step < 8; step++) {
                int to_sq = king_sq + deltas[i][j] * step;
                uint64_t to_bb = (1ULL << to_sq);
                int to_file = to_sq % 8;

                if (to_sq < 0 || to_sq > 63) break;
                if (abs(to_file - curr_file) > 1) break;
                if (occupied_no_sliding & to_bb) break;

                if (sliding_bbs[i] & to_bb) {
                    return true;
                }

                curr_file = to_file;
            }
        }
    }

    return false;
}

void render_bitboard(uint64_t bb, int origin_sq = -1, char symbol = 'x') {
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));

    for (int i = 0; i < 64; i++) {
        if ((bb >> i) & 1ULL) {
            int row = 7 - (i / 8);
            int col = i % 8;
            board[row][col] = symbol;
        }
    }

    if (origin_sq != -1) {
        int row = 7 - (origin_sq / 8);
        int col = origin_sq % 8;
        board[row][col] = 'P';
    }

    for (const auto& row : board) {
        for (char c : row) {
            std::cout << c << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
