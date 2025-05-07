#include "square_control.h"
#include "../../engine/attack_generation/attack_generation.h"

SquareControlWeights get_square_control_weights(BoardState *board_state)
{

    SquareControlWeights square_control_weights = {0};

    uint64_t white_bishops = board_state->board.white_pieces.bishops;
    while (white_bishops)
    {
        int square = __builtin_ctzll(white_bishops);
        uint64_t attacks = generate_bishop_attacks(board_state->occupied, square);
        square_control_weights.bishop += __builtin_popcountll(attacks);
        white_bishops &= white_bishops - 1;
    }

    uint64_t black_bishops = board_state->board.black_pieces.bishops;
    while (black_bishops)
    {
        int square = __builtin_ctzll(black_bishops);
        uint64_t attacks = generate_bishop_attacks(board_state->occupied, square);
        square_control_weights.bishop -= __builtin_popcountll(attacks);
        black_bishops &= black_bishops - 1;
    }

    uint64_t white_rooks = board_state->board.white_pieces.rooks;
    while (white_rooks)
    {
        int square = __builtin_ctzll(white_rooks);
        uint64_t attacks = generate_rook_attacks(board_state->occupied, square);
        square_control_weights.rook += __builtin_popcountll(attacks);
        white_rooks &= white_rooks - 1;
    }

    uint64_t black_rooks = board_state->board.black_pieces.rooks;
    while (black_rooks)
    {
        int square = __builtin_ctzll(black_rooks);
        uint64_t attacks = generate_rook_attacks(board_state->occupied, square);
        square_control_weights.rook -= __builtin_popcountll(attacks);
        black_rooks &= black_rooks - 1;
    }

    uint64_t white_queens = board_state->board.white_pieces.queens;
    while (white_queens)
    {
        int square = __builtin_ctzll(white_queens);
        uint64_t attacks = generate_queen_attacks(board_state->occupied, square);
        square_control_weights.queen += __builtin_popcountll(attacks);
        white_queens &= white_queens - 1;
    }

    uint64_t black_queens = board_state->board.black_pieces.queens;
    while (black_queens)
    {
        int square = __builtin_ctzll(black_queens);
        uint64_t attacks = generate_queen_attacks(board_state->occupied, square);
        square_control_weights.queen -= __builtin_popcountll(attacks);
        black_queens &= black_queens - 1;
    }

    return square_control_weights;
}

double calculate_square_control_score(SquareControlWeights params, SquareControlWeights middlegame_weights,
                                      SquareControlWeights endgame_weights, double game_phase)
{
    double score = 0;

    score += params.bishop * (middlegame_weights.bishop * (1 - game_phase) + endgame_weights.bishop * game_phase);
    score += params.rook * (middlegame_weights.rook * (1 - game_phase) + endgame_weights.rook * game_phase);
    score += params.queen * (middlegame_weights.queen * (1 - game_phase) + endgame_weights.queen * game_phase);

    return score;
}