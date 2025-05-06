#pragma once

#include "../../utils/board.h"
#include <stdint.h>

// score += calculate_piece_score(board->white_pieces.pawns & CENTER_FILES_MASK, 100);
// score -= calculate_piece_score(board->black_pieces.pawns & CENTER_FILES_MASK, 100);
// score += calculate_piece_score(board->white_pieces.pawns & BISHOP_FILES_MASK, 95);
// score -= calculate_piece_score(board->black_pieces.pawns & BISHOP_FILES_MASK, 95);
// score += calculate_piece_score(board->white_pieces.pawns & KNIGHT_FILES_MASK, 85);
// score -= calculate_piece_score(board->black_pieces.pawns & KNIGHT_FILES_MASK, 85);
// score += calculate_piece_score(board->white_pieces.pawns & ROOK_FILES_MASK, 70);
// score -= calculate_piece_score(board->black_pieces.pawns & ROOK_FILES_MASK, 70);
// score += calculate_piece_score(board->white_pieces.pawns, 100);
// score -= calculate_piece_score(board->black_pieces.pawns, 100);
// score += calculate_piece_score(board->white_pieces.knights, 320);
// score -= calculate_piece_score(board->black_pieces.knights, 320);
// score += calculate_piece_score(board->white_pieces.bishops, 330);
// score -= calculate_piece_score(board->black_pieces.bishops, 330);
// score += calculate_combined_piece_score(board->white_pieces.rooks, 530, 500);
// score -= calculate_combined_piece_score(board->black_pieces.rooks, 530, 500);

typedef struct
{
    double center_pawn;
    double bishop_pawn;
    double knight_pawn;
    double rook_pawn;
    double knight;
    double bishop;
    double first_rook;
    double additional_rook;
    double first_queen;
    double additional_queen;
    double bishop_pair;
} MaterialWeights;

const MaterialWeights DEFAULT_MIDDLEGAME_MATERIAL_WEIGHTS = {
    .center_pawn = 100.0,
    .bishop_pawn = 95.0,
    .knight_pawn = 85.0,
    .rook_pawn = 70.0,
    .knight = 320.0,
    .bishop = 330.0,
    .first_rook = 470.0,
    .additional_rook = 450.0,
    .first_queen = 900.0,
    .additional_queen = 900.0,
    .bishop_pair = 30.0};

const MaterialWeights DEFAULT_ENDGAME_MATERIAL_WEIGHTS = {
    .center_pawn = 100.0,
    .bishop_pawn = 100.0,
    .knight_pawn = 100.0,
    .rook_pawn = 100.0,
    .knight = 320.0,
    .bishop = 330.0,
    .first_rook = 530.0,
    .additional_rook = 500.0,
    .first_queen = 900.0,
    .additional_queen = 900.0,
    .bishop_pair = 50.0};

MaterialWeights get_material_weights(Board *board);
double calculate_material_score(MaterialWeights params, MaterialWeights middlegame_weights,
                                MaterialWeights endgame_weights, double game_phase);