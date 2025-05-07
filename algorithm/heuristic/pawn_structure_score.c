#include "pawn_structure_score.h"
#include "../../utils/bitboard.h"
#include "../../engine/attack_generation/attack_generation.h"

double get_double_pawn_weights(BoardState *board_state)
{
    double double_pawn_weights = 0;

    uint64_t white_pawns = board_state->board.white_pieces.pawns;
    uint64_t black_pawns = board_state->board.black_pieces.pawns;

    uint8_t white_pawn_a_file = __builtin_popcountll(white_pawns & FILE_A_MASK);
    uint8_t white_pawn_b_file = __builtin_popcountll(white_pawns & FILE_B_MASK);
    uint8_t white_pawn_c_file = __builtin_popcountll(white_pawns & FILE_C_MASK);
    uint8_t white_pawn_d_file = __builtin_popcountll(white_pawns & FILE_D_MASK);
    uint8_t white_pawn_e_file = __builtin_popcountll(white_pawns & FILE_E_MASK);
    uint8_t white_pawn_f_file = __builtin_popcountll(white_pawns & FILE_F_MASK);
    uint8_t white_pawn_g_file = __builtin_popcountll(white_pawns & FILE_G_MASK);
    uint8_t white_pawn_h_file = __builtin_popcountll(white_pawns & FILE_H_MASK);

    uint8_t black_pawn_a_file = __builtin_popcountll(black_pawns & FILE_A_MASK);
    uint8_t black_pawn_b_file = __builtin_popcountll(black_pawns & FILE_B_MASK);
    uint8_t black_pawn_c_file = __builtin_popcountll(black_pawns & FILE_C_MASK);
    uint8_t black_pawn_d_file = __builtin_popcountll(black_pawns & FILE_D_MASK);
    uint8_t black_pawn_e_file = __builtin_popcountll(black_pawns & FILE_E_MASK);
    uint8_t black_pawn_f_file = __builtin_popcountll(black_pawns & FILE_F_MASK);
    uint8_t black_pawn_g_file = __builtin_popcountll(black_pawns & FILE_G_MASK);
    uint8_t black_pawn_h_file = __builtin_popcountll(black_pawns & FILE_H_MASK);

    double_pawn_weights += (white_pawn_a_file > 1) ? (white_pawn_a_file - 1) : 0;
    double_pawn_weights += (white_pawn_b_file > 1) ? (white_pawn_b_file - 1) : 0;
    double_pawn_weights += (white_pawn_c_file > 1) ? (white_pawn_c_file - 1) : 0;
    double_pawn_weights += (white_pawn_d_file > 1) ? (white_pawn_d_file - 1) : 0;
    double_pawn_weights += (white_pawn_e_file > 1) ? (white_pawn_e_file - 1) : 0;
    double_pawn_weights += (white_pawn_f_file > 1) ? (white_pawn_f_file - 1) : 0;
    double_pawn_weights += (white_pawn_g_file > 1) ? (white_pawn_g_file - 1) : 0;
    double_pawn_weights += (white_pawn_h_file > 1) ? (white_pawn_h_file - 1) : 0;

    double_pawn_weights -= (black_pawn_a_file > 1) ? (black_pawn_a_file - 1) : 0;
    double_pawn_weights -= (black_pawn_b_file > 1) ? (black_pawn_b_file - 1) : 0;
    double_pawn_weights -= (black_pawn_c_file > 1) ? (black_pawn_c_file - 1) : 0;
    double_pawn_weights -= (black_pawn_d_file > 1) ? (black_pawn_d_file - 1) : 0;
    double_pawn_weights -= (black_pawn_e_file > 1) ? (black_pawn_e_file - 1) : 0;
    double_pawn_weights -= (black_pawn_f_file > 1) ? (black_pawn_f_file - 1) : 0;
    double_pawn_weights -= (black_pawn_g_file > 1) ? (black_pawn_g_file - 1) : 0;
    double_pawn_weights -= (black_pawn_h_file > 1) ? (black_pawn_h_file - 1) : 0;

    return double_pawn_weights;
}

double get_isolated_pawn_weights(BoardState *board_state)
{
    double isolated_pawn_weights = 0;

    uint64_t white_pawns = board_state->board.white_pieces.pawns;
    uint64_t black_pawns = board_state->board.black_pieces.pawns;

    bool file_a_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_A_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_B_MASK) == 0;
    bool file_b_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_B_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_A_MASK) == 0 && __builtin_popcountll(white_pawns & FILE_C_MASK) == 0;
    bool file_c_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_C_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_B_MASK) == 0 && __builtin_popcountll(white_pawns & FILE_D_MASK) == 0;
    bool file_d_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_D_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_C_MASK) == 0 && __builtin_popcountll(white_pawns & FILE_E_MASK) == 0;
    bool file_e_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_E_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_D_MASK) == 0 && __builtin_popcountll(white_pawns & FILE_F_MASK) == 0;
    bool file_f_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_F_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_E_MASK) == 0 && __builtin_popcountll(white_pawns & FILE_G_MASK) == 0;
    bool file_g_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_G_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_F_MASK) == 0 && __builtin_popcountll(white_pawns & FILE_H_MASK) == 0;
    bool file_h_has_isolated_white_pawn = __builtin_popcountll(white_pawns & FILE_H_MASK) == 1 && __builtin_popcountll(white_pawns & FILE_G_MASK) == 0;

    bool file_a_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_A_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_B_MASK) == 0;
    bool file_b_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_B_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_A_MASK) == 0 && __builtin_popcountll(black_pawns & FILE_C_MASK) == 0;
    bool file_c_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_C_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_B_MASK) == 0 && __builtin_popcountll(black_pawns & FILE_D_MASK) == 0;
    bool file_d_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_D_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_C_MASK) == 0 && __builtin_popcountll(black_pawns & FILE_E_MASK) == 0;
    bool file_e_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_E_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_D_MASK) == 0 && __builtin_popcountll(black_pawns & FILE_F_MASK) == 0;
    bool file_f_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_F_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_E_MASK) == 0 && __builtin_popcountll(black_pawns & FILE_G_MASK) == 0;
    bool file_g_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_G_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_F_MASK) == 0 && __builtin_popcountll(black_pawns & FILE_H_MASK) == 0;
    bool file_h_has_isolated_black_pawn = __builtin_popcountll(black_pawns & FILE_H_MASK) == 1 && __builtin_popcountll(black_pawns & FILE_G_MASK) == 0;

    isolated_pawn_weights += file_a_has_isolated_white_pawn ? 1 : 0;
    isolated_pawn_weights += file_b_has_isolated_white_pawn ? 1 : 0;
    isolated_pawn_weights += file_c_has_isolated_white_pawn ? 1 : 0;
    isolated_pawn_weights += file_d_has_isolated_white_pawn ? 1 : 0;
    isolated_pawn_weights += file_e_has_isolated_white_pawn ? 1 : 0;
    isolated_pawn_weights += file_f_has_isolated_white_pawn ? 1 : 0;
    isolated_pawn_weights += file_g_has_isolated_white_pawn ? 1 : 0;
    isolated_pawn_weights += file_h_has_isolated_white_pawn ? 1 : 0;

    isolated_pawn_weights -= file_a_has_isolated_black_pawn ? 1 : 0;
    isolated_pawn_weights -= file_b_has_isolated_black_pawn ? 1 : 0;
    isolated_pawn_weights -= file_c_has_isolated_black_pawn ? 1 : 0;
    isolated_pawn_weights -= file_d_has_isolated_black_pawn ? 1 : 0;
    isolated_pawn_weights -= file_e_has_isolated_black_pawn ? 1 : 0;
    isolated_pawn_weights -= file_f_has_isolated_black_pawn ? 1 : 0;
    isolated_pawn_weights -= file_g_has_isolated_black_pawn ? 1 : 0;
    isolated_pawn_weights -= file_h_has_isolated_black_pawn ? 1 : 0;

    return isolated_pawn_weights;
}

double get_backward_pawn_weights(BoardState *board_state)
{
    double backward_pawn_weights = 0;

    uint64_t white_pawns = board_state->board.white_pieces.pawns;
    uint64_t black_pawns = board_state->board.black_pieces.pawns;

    uint8_t number_of_white_backward_pawns = 0;
    while (white_pawns)
    {
        int index = __builtin_ctzll(white_pawns);
        white_pawns &= white_pawns - 1;

        uint64_t position = 1ULL << index;
        uint64_t backward_pawn_mask = get_backward_pawn_mask_white(position);

        bool is_pawn_undefendable = (backward_pawn_mask & board_state->board.white_pieces.pawns) == 0;

        uint64_t front_square = increment_rank(position);
        bool is_front_under_attack = (front_square & board_state->black_attacks.pawns) != 0;

        bool has_black_pawn_in_front = (get_white_front_file_mask(position) & board_state->board.black_pieces.pawns) != 0;

        if (is_pawn_undefendable && is_front_under_attack && !has_black_pawn_in_front)
            number_of_white_backward_pawns++;
    }

    uint8_t number_of_black_backward_pawns = 0;
    while (black_pawns)
    {
        int index = __builtin_ctzll(black_pawns);
        black_pawns &= black_pawns - 1;

        uint64_t position = 1ULL << index;
        uint64_t backward_pawn_mask = get_backward_pawn_mask_black(position);
        bool is_pawn_undefendable = (backward_pawn_mask & board_state->board.black_pieces.pawns) == 0;

        uint64_t front_square = decrement_rank(position);
        bool is_front_under_attack = (front_square & board_state->white_attacks.pawns) != 0;

        bool has_white_pawn_in_front = (get_black_front_file_mask(position) & board_state->board.white_pieces.pawns) != 0;

        if (is_pawn_undefendable && is_front_under_attack && !has_white_pawn_in_front)
            number_of_black_backward_pawns++;
    }

    backward_pawn_weights += number_of_white_backward_pawns;
    backward_pawn_weights -= number_of_black_backward_pawns;

    return backward_pawn_weights;
}

PassedPawnWeights get_passed_pawn_weights(BoardState *board_state)
{
    PassedPawnWeights passed_pawn_weights = {0};

    uint64_t white_pawns = board_state->board.white_pieces.pawns;
    uint64_t black_pawns = board_state->board.black_pieces.pawns;

    while (white_pawns)
    {
        int index = __builtin_ctzll(white_pawns);
        white_pawns &= white_pawns - 1;

        uint64_t position = 1ULL << index;
        uint64_t passed_pawn_mask = get_passed_pawn_mask_white(position);
        uint8_t number_of_squares_from_promotion = 7 - (index / 8);
        if (!(passed_pawn_mask & board_state->board.black_pieces.pawns))
        {
            if (number_of_squares_from_promotion == 1)
                passed_pawn_weights.one_square_left++;
            else if (number_of_squares_from_promotion == 2)
                passed_pawn_weights.two_squares_left++;
            else if (number_of_squares_from_promotion == 3)
                passed_pawn_weights.three_squares_left++;
            else if (number_of_squares_from_promotion == 4)
                passed_pawn_weights.four_squares_left++;
            else if (number_of_squares_from_promotion == 5)
                passed_pawn_weights.five_squares_left++;
            else if (number_of_squares_from_promotion == 6)
                passed_pawn_weights.six_squares_left++;
        }
    }

    while (black_pawns)
    {
        int index = __builtin_ctzll(black_pawns);
        black_pawns &= black_pawns - 1;

        uint64_t position = 1ULL << index;
        uint64_t passed_pawn_mask = get_passed_pawn_mask_black(position);
        uint8_t number_of_squares_from_promotion = index / 8;
        if (!(passed_pawn_mask & board_state->board.white_pieces.pawns))
        {
            if (number_of_squares_from_promotion == 1)
                passed_pawn_weights.one_square_left--;
            else if (number_of_squares_from_promotion == 2)
                passed_pawn_weights.two_squares_left--;
            else if (number_of_squares_from_promotion == 3)
                passed_pawn_weights.three_squares_left--;
            else if (number_of_squares_from_promotion == 4)
                passed_pawn_weights.four_squares_left--;
            else if (number_of_squares_from_promotion == 5)
                passed_pawn_weights.five_squares_left--;
            else if (number_of_squares_from_promotion == 6)
                passed_pawn_weights.six_squares_left--;
        }
    }

    return passed_pawn_weights;
}

PawnChainWeights get_pawn_chain_weights(BoardState *board_state)
{
    PawnChainWeights pawn_chain_weights = {0};
    uint64_t white_pawn_attacks = board_state->white_attacks.pawns;
    uint64_t black_pawn_attacks = board_state->black_attacks.pawns;

    uint64_t protected_white_pawns = board_state->board.white_pieces.pawns & white_pawn_attacks;
    uint64_t protected_black_pawns = board_state->board.black_pieces.pawns & black_pawn_attacks;

    uint64_t protected_white_pawn_attacks = generate_white_pawn_attacks(protected_white_pawns);
    uint64_t protected_black_pawn_attacks = generate_black_pawn_attacks(protected_black_pawns);

    uint64_t protected_by_protected_white_pawns = protected_white_pawn_attacks & board_state->board.white_pieces.pawns;
    uint64_t protected_by_protected_black_pawns = protected_black_pawn_attacks & board_state->board.black_pieces.pawns;

    protected_white_pawns &= ~protected_by_protected_white_pawns;
    protected_black_pawns &= ~protected_by_protected_black_pawns;
    pawn_chain_weights.protected_pawn += __builtin_popcountll(protected_white_pawns);
    pawn_chain_weights.protected_pawn -= __builtin_popcountll(protected_black_pawns);
    pawn_chain_weights.protected_by_protected_pawn += __builtin_popcountll(protected_by_protected_white_pawns);
    pawn_chain_weights.protected_by_protected_pawn -= __builtin_popcountll(protected_by_protected_black_pawns);

    return pawn_chain_weights;
}

BishopBlockWeights get_bishop_block_weights(BoardState *board_state)
{
    BishopBlockWeights bishop_block_weights = {0};

    bool white_has_light_square_bishop = (board_state->board.white_pieces.bishops & LIGHT_SQUARES_MASK) != 0;
    bool white_has_dark_square_bishop = (board_state->board.white_pieces.bishops & DARK_SQUARES_MASK) != 0;
    bool black_has_light_square_bishop = (board_state->board.black_pieces.bishops & LIGHT_SQUARES_MASK) != 0;
    bool black_has_dark_square_bishop = (board_state->board.black_pieces.bishops & DARK_SQUARES_MASK) != 0;

    uint8_t white_pawn_light_square_count = __builtin_popcountll(board_state->board.white_pieces.pawns & LIGHT_SQUARES_MASK);
    uint8_t white_pawn_dark_square_count = __builtin_popcountll(board_state->board.white_pieces.pawns & DARK_SQUARES_MASK);
    uint8_t black_pawn_light_square_count = __builtin_popcountll(board_state->board.black_pieces.pawns & LIGHT_SQUARES_MASK);
    uint8_t black_pawn_dark_square_count = __builtin_popcountll(board_state->board.black_pieces.pawns & DARK_SQUARES_MASK);

    if (white_has_light_square_bishop)
    {
        bishop_block_weights.friendly_pawn += white_pawn_light_square_count;
        bishop_block_weights.enemy_pawn += black_pawn_light_square_count;
    }

    if (white_has_dark_square_bishop)
    {
        bishop_block_weights.friendly_pawn += white_pawn_dark_square_count;
        bishop_block_weights.enemy_pawn += black_pawn_dark_square_count;
    }

    if (black_has_light_square_bishop)
    {
        bishop_block_weights.friendly_pawn -= black_pawn_light_square_count;
        bishop_block_weights.enemy_pawn -= white_pawn_light_square_count;
    }

    if (black_has_dark_square_bishop)
    {
        bishop_block_weights.friendly_pawn -= black_pawn_dark_square_count;
        bishop_block_weights.enemy_pawn -= white_pawn_dark_square_count;
    }

    return bishop_block_weights;
}

PawnStructureWeights get_pawn_structure_weights(BoardState *board_state)
{
    PawnStructureWeights pawn_structure_weights = {0};

    pawn_structure_weights.double_pawn_weights = get_double_pawn_weights(board_state);
    pawn_structure_weights.isolated_pawn_weights = get_isolated_pawn_weights(board_state);
    pawn_structure_weights.backward_pawn_weights = get_backward_pawn_weights(board_state);
    pawn_structure_weights.passed_pawn_weights = get_passed_pawn_weights(board_state);
    pawn_structure_weights.pawn_chain_weights = get_pawn_chain_weights(board_state);
    pawn_structure_weights.bishop_block_weights = get_bishop_block_weights(board_state);

    return pawn_structure_weights;
}

double calculate_pawn_structure_score(PawnStructureWeights params, PawnStructureWeights middlegame_weights,
                                      PawnStructureWeights endgame_weights, double game_phase)
{
    double score = 0;

    // Double pawns
    score += params.double_pawn_weights * (middlegame_weights.double_pawn_weights * (1 - game_phase) +
                                           endgame_weights.double_pawn_weights * game_phase);

    // Isolated pawns
    score += params.isolated_pawn_weights * (middlegame_weights.isolated_pawn_weights * (1 - game_phase) +
                                             endgame_weights.isolated_pawn_weights * game_phase);

    // Backward pawns
    score += params.backward_pawn_weights * (middlegame_weights.backward_pawn_weights * (1 - game_phase) +
                                             endgame_weights.backward_pawn_weights * game_phase);

    // Passed pawns
    score += params.passed_pawn_weights.one_square_left * (middlegame_weights.passed_pawn_weights.one_square_left * (1 - game_phase) +
                                                           endgame_weights.passed_pawn_weights.one_square_left * game_phase);
    score += params.passed_pawn_weights.two_squares_left * (middlegame_weights.passed_pawn_weights.two_squares_left * (1 - game_phase) +
                                                            endgame_weights.passed_pawn_weights.two_squares_left * game_phase);
    score += params.passed_pawn_weights.three_squares_left * (middlegame_weights.passed_pawn_weights.three_squares_left * (1 - game_phase) +
                                                              endgame_weights.passed_pawn_weights.three_squares_left * game_phase);
    score += params.passed_pawn_weights.four_squares_left * (middlegame_weights.passed_pawn_weights.four_squares_left * (1 - game_phase) +
                                                             endgame_weights.passed_pawn_weights.four_squares_left * game_phase);
    score += params.passed_pawn_weights.five_squares_left * (middlegame_weights.passed_pawn_weights.five_squares_left * (1 - game_phase) +
                                                             endgame_weights.passed_pawn_weights.five_squares_left * game_phase);
    score += params.passed_pawn_weights.six_squares_left * (middlegame_weights.passed_pawn_weights.six_squares_left * (1 - game_phase) +
                                                            endgame_weights.passed_pawn_weights.six_squares_left * game_phase);

    // Pawn chains
    score += params.pawn_chain_weights.protected_by_protected_pawn * (middlegame_weights.pawn_chain_weights.protected_by_protected_pawn * (1 - game_phase) +
                                                                      endgame_weights.pawn_chain_weights.protected_by_protected_pawn * game_phase);
    score += params.pawn_chain_weights.protected_pawn * (middlegame_weights.pawn_chain_weights.protected_pawn * (1 - game_phase) +
                                                         endgame_weights.pawn_chain_weights.protected_pawn * game_phase);
    // Bishop blocks
    score += params.bishop_block_weights.friendly_pawn * (middlegame_weights.bishop_block_weights.friendly_pawn * (1 - game_phase) +
                                                          endgame_weights.bishop_block_weights.friendly_pawn * game_phase);
    score += params.bishop_block_weights.enemy_pawn * (middlegame_weights.bishop_block_weights.enemy_pawn * (1 - game_phase) +
                                                       endgame_weights.bishop_block_weights.enemy_pawn * game_phase);
    return score;
}