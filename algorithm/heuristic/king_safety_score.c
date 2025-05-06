#include "king_safety_score.h"
#include "../../utils/bitboard.h"

int get_file(int square)
{
    return square % 8;
}

int get_rank(int square)
{
    return square / 8;
}

int get_king_square(uint64_t king)
{
    return __builtin_ctzll(king);
}

CastlingWeights get_castling_weights(BoardState *board_state)
{
    CastlingWeights castling_weights = {0};
    Board *board = &board_state->board;
    if (((board->castling_rights & WHITE_KINGSIDE_CASTLE) != 0) && ((board->castling_rights & WHITE_QUEENSIDE_CASTLE) != 0))
        castling_weights.both_sides++;
    else if ((board->castling_rights & WHITE_KINGSIDE_CASTLE) != 0)
        castling_weights.kingside++;
    else if ((board->castling_rights & WHITE_QUEENSIDE_CASTLE) != 0)
        castling_weights.queenside++;

    if (((board->castling_rights & BLACK_KINGSIDE_CASTLE) != 0) && ((board->castling_rights & BLACK_QUEENSIDE_CASTLE) != 0))
        castling_weights.both_sides--;
    else if ((board->castling_rights & BLACK_KINGSIDE_CASTLE) != 0)
        castling_weights.kingside--;
    else if ((board->castling_rights & BLACK_QUEENSIDE_CASTLE) != 0)
        castling_weights.queenside--;

    if (board_state->has_castled & WHITE_KINGSIDE_CASTLE)
        castling_weights.has_castled_kingside++;

    if (board_state->has_castled & WHITE_QUEENSIDE_CASTLE)
        castling_weights.has_castled_queenside++;

    if (board_state->has_castled & BLACK_KINGSIDE_CASTLE)
        castling_weights.has_castled_kingside--;

    if (board_state->has_castled & BLACK_QUEENSIDE_CASTLE)
        castling_weights.has_castled_queenside--;
    return castling_weights;
}

PawnShelterWeights get_pawn_shelter_weights(Board *board)
{
    PawnShelterWeights pawn_shelter_weights = {0};

    // White king shelter
    uint64_t front_of_white_king_mask = increment_rank(board->white_pieces.king);
    uint64_t ahead_of_white_king_mask = increment_rank(front_of_white_king_mask);
    uint64_t left_of_white_king_mask = decrement_file(front_of_white_king_mask);
    uint64_t right_of_white_king_mask = increment_file(front_of_white_king_mask);

    // Black king shelter
    uint64_t front_of_black_king_mask = decrement_rank(board->black_pieces.king);
    uint64_t ahead_of_black_king_mask = decrement_rank(front_of_black_king_mask);
    uint64_t left_of_black_king_mask = decrement_file(front_of_black_king_mask);
    uint64_t right_of_black_king_mask = increment_file(front_of_black_king_mask);

    // Check white pawn shelter
    bool has_white_front_pawn = (board->white_pieces.pawns & front_of_white_king_mask) != 0;
    bool has_white_ahead_pawn = (board->white_pieces.pawns & ahead_of_white_king_mask) != 0;
    bool has_white_left_pawn = (board->white_pieces.pawns & left_of_white_king_mask) != 0;
    bool has_white_right_pawn = (board->white_pieces.pawns & right_of_white_king_mask) != 0;

    // Check black pawn shelter
    bool has_black_front_pawn = (board->black_pieces.pawns & front_of_black_king_mask) != 0;
    bool has_black_ahead_pawn = (board->black_pieces.pawns & ahead_of_black_king_mask) != 0;
    bool has_black_left_pawn = (board->black_pieces.pawns & left_of_black_king_mask) != 0;
    bool has_black_right_pawn = (board->black_pieces.pawns & right_of_black_king_mask) != 0;

    // Score white pawn shelter
    if (has_white_front_pawn)
        pawn_shelter_weights.front_pawn++;

    if (has_white_ahead_pawn)
        pawn_shelter_weights.ahead_pawn++;
    if (has_white_left_pawn)
        pawn_shelter_weights.left_pawn++;
    if (has_white_right_pawn)
        pawn_shelter_weights.right_pawn++;

    // Score black pawn shelter (mirror of white scoring)
    if (has_black_front_pawn)
        pawn_shelter_weights.front_pawn--;
    if (has_black_ahead_pawn)
        pawn_shelter_weights.ahead_pawn--;
    if (has_black_left_pawn)
        pawn_shelter_weights.left_pawn--;
    if (has_black_right_pawn)
        pawn_shelter_weights.right_pawn--;

    return pawn_shelter_weights;
}

AttackingKingSquaresWeights get_attacking_king_squares_weights(BoardState *board_state)
{
    AttackingKingSquaresWeights attacking_king_squares_weights = {0};

    // Calculate squares adjacent to the kings, excluding the squares they currently occupy
    uint64_t white_king_squares = expand_bitboard(board_state->board.white_pieces.king) & ~board_state->board.white_pieces.king;
    uint64_t black_king_squares = expand_bitboard(board_state->board.black_pieces.king) & ~board_state->board.black_pieces.king;

    // Count the number of squares attacked by the opposing pieces
    uint8_t attacked_white_king_squares = __builtin_popcountll(white_king_squares & board_state->black_attack);
    uint8_t attacked_black_king_squares = __builtin_popcountll(black_king_squares & board_state->white_attack);

    // Cap the number of attacked squares to 4
    if (attacked_white_king_squares > 4)
        attacked_white_king_squares = 4;

    if (attacked_black_king_squares > 4)
        attacked_black_king_squares = 4;

    // Apply exponential scoring for attacking squares around the enemy king
    if (attacked_white_king_squares == 1)
        attacking_king_squares_weights.one_square++;
    else if (attacked_white_king_squares == 2)
        attacking_king_squares_weights.two_squares++;
    else if (attacked_white_king_squares == 3)
        attacking_king_squares_weights.three_squares++;
    else if (attacked_white_king_squares == 4)
        attacking_king_squares_weights.four_squares++;

    if (attacked_black_king_squares == 1)
        attacking_king_squares_weights.one_square--;
    else if (attacked_black_king_squares == 2)
        attacking_king_squares_weights.two_squares--;
    else if (attacked_black_king_squares == 3)
        attacking_king_squares_weights.three_squares--;
    else if (attacked_black_king_squares == 4)
        attacking_king_squares_weights.four_squares--;

    return attacking_king_squares_weights;
}

KingSafetyWeights get_king_safety_weights(BoardState *board_state)
{
    KingSafetyWeights king_safety_weights = {0};
    king_safety_weights.castling_weights = get_castling_weights(board_state);
    king_safety_weights.pawn_shelter_weights = get_pawn_shelter_weights(&board_state->board);
    king_safety_weights.attacking_king_squares_weights = get_attacking_king_squares_weights(board_state);
    return king_safety_weights;
}

double calculate_king_safety_score(KingSafetyWeights params, KingSafetyWeights middlegame_weights,
                                   KingSafetyWeights endgame_weights, double game_phase)
{
    double score = 0;

    // Castling
    score += params.castling_weights.kingside * (middlegame_weights.castling_weights.kingside * (1 - game_phase) +
                                                 endgame_weights.castling_weights.kingside * game_phase);
    score += params.castling_weights.queenside * (middlegame_weights.castling_weights.queenside * (1 - game_phase) +
                                                  endgame_weights.castling_weights.queenside * game_phase);
    score += params.castling_weights.both_sides * (middlegame_weights.castling_weights.both_sides * (1 - game_phase) +
                                                   endgame_weights.castling_weights.both_sides * game_phase);
    score += params.castling_weights.has_castled_kingside * (middlegame_weights.castling_weights.has_castled_kingside * (1 - game_phase) +
                                                             endgame_weights.castling_weights.has_castled_kingside * game_phase);
    score += params.castling_weights.has_castled_queenside * (middlegame_weights.castling_weights.has_castled_queenside * (1 - game_phase) +
                                                              endgame_weights.castling_weights.has_castled_queenside * game_phase);

    // Pawn shelter
    score += params.pawn_shelter_weights.front_pawn * (middlegame_weights.pawn_shelter_weights.front_pawn * (1 - game_phase) +
                                                       endgame_weights.pawn_shelter_weights.front_pawn * game_phase);
    score += params.pawn_shelter_weights.ahead_pawn * (middlegame_weights.pawn_shelter_weights.ahead_pawn * (1 - game_phase) +
                                                       endgame_weights.pawn_shelter_weights.ahead_pawn * game_phase);
    score += params.pawn_shelter_weights.left_pawn * (middlegame_weights.pawn_shelter_weights.left_pawn * (1 - game_phase) +
                                                      endgame_weights.pawn_shelter_weights.left_pawn * game_phase);
    score += params.pawn_shelter_weights.right_pawn * (middlegame_weights.pawn_shelter_weights.right_pawn * (1 - game_phase) +
                                                       endgame_weights.pawn_shelter_weights.right_pawn * game_phase);
    // Attacking king squares
    score += params.attacking_king_squares_weights.one_square * (middlegame_weights.attacking_king_squares_weights.one_square * (1 - game_phase) +
                                                                 endgame_weights.attacking_king_squares_weights.one_square * game_phase);
    score += params.attacking_king_squares_weights.two_squares * (middlegame_weights.attacking_king_squares_weights.two_squares * (1 - game_phase) +
                                                                  endgame_weights.attacking_king_squares_weights.two_squares * game_phase);
    score += params.attacking_king_squares_weights.three_squares * (middlegame_weights.attacking_king_squares_weights.three_squares * (1 - game_phase) +
                                                                    endgame_weights.attacking_king_squares_weights.three_squares * game_phase);
    score += params.attacking_king_squares_weights.four_squares * (middlegame_weights.attacking_king_squares_weights.four_squares * (1 - game_phase) +
                                                                   endgame_weights.attacking_king_squares_weights.four_squares * game_phase);

    return score;
}