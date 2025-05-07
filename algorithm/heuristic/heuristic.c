#include "heuristic.h"
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

double get_game_phase(Board *board)
{
    const int PHASE_KNIGHT = 1;
    const int PHASE_BISHOP = 1;
    const int PHASE_ROOK = 2;
    const int PHASE_QUEEN = 4;

    const int MAX_PHASE = 24;

    int phase = 0;

    // Count remaining white pieces
    phase += PHASE_KNIGHT * __builtin_popcountll(board->white_pieces.knights);
    phase += PHASE_BISHOP * __builtin_popcountll(board->white_pieces.bishops);
    phase += PHASE_ROOK * __builtin_popcountll(board->white_pieces.rooks);
    phase += PHASE_QUEEN * __builtin_popcountll(board->white_pieces.queens);

    // Count remaining black pieces
    phase += PHASE_KNIGHT * __builtin_popcountll(board->black_pieces.knights);
    phase += PHASE_BISHOP * __builtin_popcountll(board->black_pieces.bishops);
    phase += PHASE_ROOK * __builtin_popcountll(board->black_pieces.rooks);
    phase += PHASE_QUEEN * __builtin_popcountll(board->black_pieces.queens);

    // Flip to endgame = 1.0, opening = 0.0
    return 1.0 - ((double)phase / MAX_PHASE);
}

HeuristicScore get_heuristic_score(BoardState *board_state)
{
    double score = 0;
    double game_phase = get_game_phase(&board_state->board);

    // Material counting
    MaterialWeights material_weights = get_material_weights(&board_state->board);
    score += calculate_material_score(material_weights,
                                      DEFAULT_MIDDLEGAME_MATERIAL_WEIGHTS,
                                      DEFAULT_ENDGAME_MATERIAL_WEIGHTS, game_phase);

    // Positional scoring. This must be fixed
    PieceSquareWeights piece_square_weights = get_piece_square_weights(&board_state->board);
    score += calculate_piece_square_score_fast(&board_state->board,
                                               DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS,
                                               DEFAULT_ENDGAME_PIECE_SQUARE_WEIGHTS, game_phase);

    // King safety scoring
    KingSafetyWeights king_safety_weights = get_king_safety_weights(board_state);
    score += calculate_king_safety_score(king_safety_weights,
                                         DEFAULT_MIDDLEGAME_KING_SAFETY_WEIGHTS,
                                         DEFAULT_ENDGAME_KING_SAFETY_WEIGHTS, game_phase);

    // Pawn structure scoring
    PawnStructureWeights pawn_structure_weights = get_pawn_structure_weights(board_state);
    score += calculate_pawn_structure_score(pawn_structure_weights,
                                            DEFAULT_MIDDLEGAME_PAWN_STRUCTURE_WEIGHTS,
                                            DEFAULT_ENDGAME_PAWN_STRUCTURE_WEIGHTS, game_phase);

    // Square control scoring
    SquareControlWeights square_control_weights = get_square_control_weights(board_state);
    score += calculate_square_control_score(square_control_weights,
                                            DEFAULT_MIDDLEGAME_SQUARE_CONTROL_WEIGHTS,
                                            DEFAULT_ENDGAME_SQUARE_CONTROL_WEIGHTS, game_phase);

    HeuristicScore heuristic_score = {0};
    heuristic_score.score = score;
    heuristic_score.weights.material_weights = material_weights;
    heuristic_score.weights.piece_square_weights = piece_square_weights;
    heuristic_score.weights.king_safety_weights = king_safety_weights;
    heuristic_score.weights.pawn_structure_weights = pawn_structure_weights;
    heuristic_score.weights.square_control_weights = square_control_weights;
    return heuristic_score;
}