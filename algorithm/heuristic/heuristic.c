#include "heuristic.h"
#include "position_score.h"
#include "material_score.h"
#include "king_safety_score.h"
#include "pawn_structure_score.h"
#include "square_control.h"
#include "../../utils/bitboard.h"

bool has_insufficient_material(Board *board)
{
    // Check pawns, rooks, queens - if any exist, material is sufficient
    if (__builtin_popcountll(board->white_pieces.pawns | board->black_pieces.pawns |
                             board->white_pieces.rooks | board->black_pieces.rooks |
                             board->white_pieces.queens | board->black_pieces.queens) > 0)
        return false;

    int white_knights = __builtin_popcountll(board->white_pieces.knights);
    int black_knights = __builtin_popcountll(board->black_pieces.knights);
    int white_bishops = __builtin_popcountll(board->white_pieces.bishops);
    int black_bishops = __builtin_popcountll(board->black_pieces.bishops);

    // King vs King
    if (white_knights + black_knights + white_bishops + black_bishops == 0)
        return true;

    // King + minor piece vs King
    if (white_knights + black_knights + white_bishops + black_bishops == 1)
        return true;

    // King + Bishop vs King + Bishop (need to check if bishops are same color)
    if (white_knights + black_knights == 0 && white_bishops == 1 && black_bishops == 1)
    {
        // Check if bishops are on same colored squares
        bool white_bishop_on_light = (board->white_pieces.bishops & 0b0101010101010101010101010101010101010101010101010101010101010101) > 0;
        bool black_bishop_on_light = (board->black_pieces.bishops & 0b0101010101010101010101010101010101010101010101010101010101010101) > 0;
        return white_bishop_on_light == black_bishop_on_light;
    }

    return false;
}

Result get_result(BoardState *board_state, bool is_finished)
{
    Result result = UNKNOWN;
    if (has_insufficient_material(&board_state->board))
        result = DRAW;
    else if (is_finished)
    {
        if (board_state->white_check || board_state->black_check)
            result = LOST;
        else
            result = DRAW;
    }
    return result;
}

BoardScore score_board(BoardState *board_state, uint8_t depth, bool is_finished)
{
    double score = 0;
    Result result = get_result(board_state, is_finished);
    double game_phase = 0.0;

    // Material counting
    MaterialWeights material_weights = get_material_weights(&board_state->board);
    score += calculate_material_score(material_weights,
                                      DEFAULT_MIDDLEGAME_MATERIAL_WEIGHTS,
                                      DEFAULT_ENDGAME_MATERIAL_WEIGHTS, game_phase);

    // Positional scoring
    score += get_position_score(&board_state->board);

    // King safety scoring
    KingSafetyWeights king_safety_weights = get_king_safety_weights(board_state);
    score += calculate_king_safety_score(king_safety_weights,
                                         DEFAULT_MIDDLEGAME_KING_SAFETY_WEIGHTS,
                                         DEFAULT_ENDGAME_KING_SAFETY_WEIGHTS, game_phase);

    // Pawn structure scoring
    score += get_pawn_structure_score(board_state);

    // Square control scoring
    score += get_square_control(board_state);

    if (board_state->board.side_to_move == BLACK)
        score = -score;
    return (BoardScore){score, result, depth};
}