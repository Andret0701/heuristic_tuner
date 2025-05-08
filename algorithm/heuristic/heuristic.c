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

HeuristicWeights MIDDLEGAME_HEURISTIC_WEIGHTS = {0};
HeuristicWeights ENDGAME_HEURISTIC_WEIGHTS = {0};
void set_heuristic_weights(HeuristicWeights middlegame_weights,
                           HeuristicWeights endgame_weights)
{
    MIDDLEGAME_HEURISTIC_WEIGHTS = middlegame_weights;
    ENDGAME_HEURISTIC_WEIGHTS = endgame_weights;
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

HeuristicScore get_heuristic_score(BoardState *board_state)
{
    double game_phase = get_game_phase(&board_state->board);

    MaterialWeights material_weights = get_material_weights(&board_state->board);
    PieceSquareWeights piece_square_weights = get_piece_square_weights(&board_state->board);
    KingSafetyWeights king_safety_weights = get_king_safety_weights(board_state);
    PawnStructureWeights pawn_structure_weights = get_pawn_structure_weights(board_state);
    SquareControlWeights square_control_weights = get_square_control_weights(board_state);

    HeuristicScore heuristic_score = {0};
    heuristic_score.weights.material_weights = material_weights;
    heuristic_score.weights.piece_square_weights = piece_square_weights;
    heuristic_score.weights.king_safety_weights = king_safety_weights;
    heuristic_score.weights.pawn_structure_weights = pawn_structure_weights;
    heuristic_score.weights.square_control_weights = square_control_weights;
    heuristic_score.weights.game_phase = game_phase;

    heuristic_score.score = calculate_heuristic_score(heuristic_score.weights,
                                                      MIDDLEGAME_HEURISTIC_WEIGHTS,
                                                      ENDGAME_HEURISTIC_WEIGHTS);

    if (board_state->board.side_to_move == BLACK)
        heuristic_score.score = -heuristic_score.score;

    return heuristic_score;
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

    printf("\nKing safety weights:\n");
    printf(" Castle:\n");
    printf("  Kingside: %f\n", weights.king_safety_weights.castling_weights.kingside);
    printf("  Queenside: %f\n", weights.king_safety_weights.castling_weights.queenside);
    printf("  Both sides: %f\n", weights.king_safety_weights.castling_weights.both_sides);
    printf("  Has castled kingside: %f\n", weights.king_safety_weights.castling_weights.has_castled_kingside);
    printf("  Has castled queenside: %f\n", weights.king_safety_weights.castling_weights.has_castled_queenside);

    printf("\n Pawn shelter weights:\n");
    printf("  Front pawn: %f\n", weights.king_safety_weights.pawn_shelter_weights.front_pawn);
    printf("  Ahead pawn: %f\n", weights.king_safety_weights.pawn_shelter_weights.ahead_pawn);
    printf("  Left pawn: %f\n", weights.king_safety_weights.pawn_shelter_weights.left_pawn);
    printf("  Right pawn: %f\n", weights.king_safety_weights.pawn_shelter_weights.right_pawn);

    printf("\n Attacking king squares weights:\n");
    printf("  One square: %f\n", weights.king_safety_weights.attacking_king_squares_weights.one_square);
    printf("  Two squares: %f\n", weights.king_safety_weights.attacking_king_squares_weights.two_squares);
    printf("  Three squares: %f\n", weights.king_safety_weights.attacking_king_squares_weights.three_squares);
    printf("  Four squares: %f\n", weights.king_safety_weights.attacking_king_squares_weights.four_squares);
    printf("  Five squares: %f\n", weights.king_safety_weights.attacking_king_squares_weights.five_squares);
    printf("  Six squares: %f\n", weights.king_safety_weights.attacking_king_squares_weights.six_squares);
    printf("  Seven squares: %f\n", weights.king_safety_weights.attacking_king_squares_weights.seven_squares);
    printf("  Eight squares: %f\n", weights.king_safety_weights.attacking_king_squares_weights.eight_squares);

    printf("\nPawn structure weights:\n");
    printf(" Passed pawn weights:\n");
    printf("  One square left: %f\n", weights.pawn_structure_weights.passed_pawn_weights.one_square_left);
    printf("  Two squares left: %f\n", weights.pawn_structure_weights.passed_pawn_weights.two_squares_left);
    printf("  Three squares left: %f\n", weights.pawn_structure_weights.passed_pawn_weights.three_squares_left);
    printf("  Four squares left: %f\n", weights.pawn_structure_weights.passed_pawn_weights.four_squares_left);
    printf("  Five squares left: %f\n", weights.pawn_structure_weights.passed_pawn_weights.five_squares_left);
    printf("  Six squares left: %f\n", weights.pawn_structure_weights.passed_pawn_weights.six_squares_left);
    printf(" Pawn chain weights:\n");
    printf("  Protected pawn: %f\n", weights.pawn_structure_weights.pawn_chain_weights.protected_pawn);
    printf("  Protected by protected pawn: %f\n", weights.pawn_structure_weights.pawn_chain_weights.protected_by_protected_pawn);
    printf(" Bishop blocking pawn weights:\n");
    printf("  Friendly pawn: %f\n", weights.pawn_structure_weights.bishop_block_weights.friendly_pawn);
    printf("  Enemy pawn: %f\n", weights.pawn_structure_weights.bishop_block_weights.enemy_pawn);
    printf(" Double pawn weights: %f\n", weights.pawn_structure_weights.double_pawn_weights);
    printf(" Isolated pawn weights: %f\n", weights.pawn_structure_weights.isolated_pawn_weights);
    printf(" Backward pawn weights: %f\n", weights.pawn_structure_weights.backward_pawn_weights);

    printf("\nSquare control weights:\n");
    printf(" Bishop square control weight: %f\n", weights.square_control_weights.bishop);
    printf(" Rook square control weight: %f\n", weights.square_control_weights.rook);
    printf(" Queen square control weight: %f\n", weights.square_control_weights.queen);
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