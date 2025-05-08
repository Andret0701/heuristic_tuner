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
    heuristic_score.weights.game_phase = game_phase;
    if (board_state->board.side_to_move == BLACK)
        heuristic_score.score = -heuristic_score.score;

    return heuristic_score;
}
double calculate_heuristic_score(HeuristicWeights params, HeuristicWeights middlegame_weights,
                                 HeuristicWeights endgame_weights)
{
    double game_phase = params.game_phase;
    double score = 0;

    // Material
    score += calculate_material_score(params.material_weights,
                                      middlegame_weights.material_weights,
                                      endgame_weights.material_weights, game_phase);

    // Piece square
    score += calculate_piece_square_score(params.piece_square_weights,
                                          middlegame_weights.piece_square_weights,
                                          endgame_weights.piece_square_weights, game_phase);

    // King safety
    score += calculate_king_safety_score(params.king_safety_weights,
                                         middlegame_weights.king_safety_weights,
                                         endgame_weights.king_safety_weights, game_phase);

    // Pawn structure
    score += calculate_pawn_structure_score(params.pawn_structure_weights,
                                            middlegame_weights.pawn_structure_weights,
                                            endgame_weights.pawn_structure_weights, game_phase);

    // Square control
    score += calculate_square_control_score(params.square_control_weights,
                                            middlegame_weights.square_control_weights,
                                            endgame_weights.square_control_weights, game_phase);

    return score;
}

void print_heuristic_weights(HeuristicWeights weights)
{
    printf("Material weights:\n");
    printf("Center pawn: %f\n", weights.material_weights.center_pawn);
    printf("Bishop pawn: %f\n", weights.material_weights.bishop_pawn);
    printf("Knight pawn: %f\n", weights.material_weights.knight_pawn);
    printf("Rook pawn: %f\n", weights.material_weights.rook_pawn);
    printf("Knight: %f\n", weights.material_weights.knight);
    printf("Bishop: %f\n", weights.material_weights.bishop);
    printf("First rook: %f\n", weights.material_weights.first_rook);
    printf("Additional rook: %f\n", weights.material_weights.additional_rook);
    printf("First queen: %f\n", weights.material_weights.first_queen);
    printf("Additional queen: %f\n", weights.material_weights.additional_queen);
    printf("Bishop pair: %f\n", weights.material_weights.bishop_pair);

    printf("\nPiece square weights:\n");
    printf(" Pawn:\n");
    print_square_weights(weights.piece_square_weights.pawn);
    printf(" Knight:\n");
    print_square_weights(weights.piece_square_weights.knight);
    printf(" Bishop:\n");
    print_square_weights(weights.piece_square_weights.bishop);
    printf(" Rook:\n");
    print_square_weights(weights.piece_square_weights.rook);
    printf(" Queen:\n");
    print_square_weights(weights.piece_square_weights.queen);
    printf(" King:\n");
    print_square_weights(weights.piece_square_weights.king);

    // printf("\nKing safety weights:\n");
    // print_castling_weights(weights.king_safety_weights.castling_weights);
    // print_pawn_shelter_weights(weights.king_safety_weights.pawn_shelter_weights);
    // print_attacking_king_squares_weights(weights.king_safety_weights.attacking_king_squares_weights);

    // printf("\nPawn structure weights:\n");
    // print_passed_pawn_weights(weights.pawn_structure_weights.passed_pawn_weights);
    // print_pawn_chain_weights(weights.pawn_structure_weights.pawn_chain_weights);
    // print_bishop_blocking_pawn_weight(weights.pawn_structure_weights.bishop_block_weights);

    // printf("\nSquare control weights:\n");
    // print_square_control_weight(weights.square_control_weights.bishop, "bishop");
}

HeuristicWeights get_default_middlegame_heuristic_weights()
{
    HeuristicWeights weights = {0};
    weights.material_weights = DEFAULT_MIDDLEGAME_MATERIAL_WEIGHTS;
    weights.piece_square_weights = DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS;
    weights.king_safety_weights = DEFAULT_MIDDLEGAME_KING_SAFETY_WEIGHTS;
    weights.pawn_structure_weights = DEFAULT_MIDDLEGAME_PAWN_STRUCTURE_WEIGHTS;
    weights.square_control_weights = DEFAULT_MIDDLEGAME_SQUARE_CONTROL_WEIGHTS;
    weights.game_phase = 0.5; // Default game phase for middlegame
    return weights;
}

HeuristicWeights get_default_endgame_heuristic_weights()
{
    HeuristicWeights weights = {0};
    weights.material_weights = DEFAULT_ENDGAME_MATERIAL_WEIGHTS;
    weights.piece_square_weights = DEFAULT_ENDGAME_PIECE_SQUARE_WEIGHTS;
    weights.king_safety_weights = DEFAULT_ENDGAME_KING_SAFETY_WEIGHTS;
    weights.pawn_structure_weights = DEFAULT_ENDGAME_PAWN_STRUCTURE_WEIGHTS;
    weights.square_control_weights = DEFAULT_ENDGAME_SQUARE_CONTROL_WEIGHTS;
    weights.game_phase = 1.0; // Default game phase for endgame
    return weights;
}