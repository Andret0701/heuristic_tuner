#include "material_score.h"
#include "../../utils/bitboard.h"

bool has_bishop_pair(Board *board, Color color)
{
    uint64_t bishops;
    if (color == WHITE)
        bishops = board->white_pieces.bishops;
    else
        bishops = board->black_pieces.bishops;

    if (__builtin_popcountll(bishops) < 2)
        return false;

    // Check if bishops are on different colors
    bool has_dark_square_bishop = bishops & DARK_SQUARES_MASK;
    bool has_light_square_bishop = bishops & LIGHT_SQUARES_MASK;

    return has_dark_square_bishop && has_light_square_bishop;
}

MaterialWeights get_material_weights(Board *board)
{
    MaterialWeights material_weights = {0};

    material_weights.center_pawn += __builtin_popcountll(board->white_pieces.pawns & CENTER_FILES_MASK);
    material_weights.center_pawn -= __builtin_popcountll(board->black_pieces.pawns & CENTER_FILES_MASK);
    material_weights.bishop_pawn += __builtin_popcountll(board->white_pieces.pawns & BISHOP_FILES_MASK);
    material_weights.bishop_pawn -= __builtin_popcountll(board->black_pieces.pawns & BISHOP_FILES_MASK);
    material_weights.knight_pawn += __builtin_popcountll(board->white_pieces.pawns & KNIGHT_FILES_MASK);
    material_weights.knight_pawn -= __builtin_popcountll(board->black_pieces.pawns & KNIGHT_FILES_MASK);
    material_weights.rook_pawn += __builtin_popcountll(board->white_pieces.pawns & ROOK_FILES_MASK);
    material_weights.rook_pawn -= __builtin_popcountll(board->black_pieces.pawns & ROOK_FILES_MASK);
    material_weights.knight += __builtin_popcountll(board->white_pieces.knights);
    material_weights.knight -= __builtin_popcountll(board->black_pieces.knights);
    material_weights.bishop += __builtin_popcountll(board->white_pieces.bishops);
    material_weights.bishop -= __builtin_popcountll(board->black_pieces.bishops);
    material_weights.first_rook += (__builtin_popcountll(board->white_pieces.rooks) > 0) ? 1 : 0;
    material_weights.first_rook -= (__builtin_popcountll(board->black_pieces.rooks) > 0) ? 1 : 0;
    material_weights.additional_rook += (__builtin_popcountll(board->white_pieces.rooks) > 1) ? (__builtin_popcountll(board->white_pieces.rooks) - 1) : 0;
    material_weights.additional_rook -= (__builtin_popcountll(board->black_pieces.rooks) > 1) ? (__builtin_popcountll(board->black_pieces.rooks) - 1) : 0;
    material_weights.first_queen += (__builtin_popcountll(board->white_pieces.queens) > 0) ? 1 : 0;
    material_weights.first_queen -= (__builtin_popcountll(board->black_pieces.queens) > 0) ? 1 : 0;
    material_weights.additional_queen += (__builtin_popcountll(board->white_pieces.queens) > 1) ? (__builtin_popcountll(board->white_pieces.queens) - 1) : 0;
    material_weights.additional_queen -= (__builtin_popcountll(board->black_pieces.queens) > 1) ? (__builtin_popcountll(board->black_pieces.queens) - 1) : 0;
    material_weights.bishop_pair += has_bishop_pair(board, WHITE);
    material_weights.bishop_pair -= has_bishop_pair(board, BLACK);

    return material_weights;
}

double calculate_material_score(MaterialWeights params, MaterialWeights middlegame_weights,
                                MaterialWeights endgame_weights, double game_phase)
{
    double score = 0;

    // Pawns
    score += params.center_pawn * (middlegame_weights.center_pawn * (1 - game_phase) +
                                   endgame_weights.center_pawn * game_phase);
    score += params.bishop_pawn * (middlegame_weights.bishop_pawn * (1 - game_phase) +
                                   endgame_weights.bishop_pawn * game_phase);
    score += params.knight_pawn * (middlegame_weights.knight_pawn * (1 - game_phase) +
                                   endgame_weights.knight_pawn * game_phase);
    score += params.rook_pawn * (middlegame_weights.rook_pawn * (1 - game_phase) +
                                 endgame_weights.rook_pawn * game_phase);

    // Pieces
    score += params.knight * (middlegame_weights.knight * (1 - game_phase) +
                              endgame_weights.knight * game_phase);
    score += params.bishop * (middlegame_weights.bishop * (1 - game_phase) +
                              endgame_weights.bishop * game_phase);
    score += params.first_rook * (middlegame_weights.first_rook * (1 - game_phase) +
                                  endgame_weights.first_rook * game_phase);
    score += params.additional_rook * (middlegame_weights.additional_rook * (1 - game_phase) +
                                       endgame_weights.additional_rook * game_phase);
    score += params.first_queen * (middlegame_weights.first_queen * (1 - game_phase) +
                                   endgame_weights.first_queen * game_phase);
    score += params.additional_queen * (middlegame_weights.additional_queen * (1 - game_phase) +
                                        endgame_weights.additional_queen * game_phase);

    // Bishop pair
    score += params.bishop_pair * (middlegame_weights.bishop_pair * (1 - game_phase) +
                                   endgame_weights.bishop_pair * game_phase);

    return score;
}