#pragma once

#include "../../engine/board_state.h"
#include <stdint.h>

typedef struct
{
    double one_square_left;
    double two_squares_left;
    double three_squares_left;
    double four_squares_left;
    double five_squares_left;
    double six_squares_left;
} PassedPawnWeights;

typedef struct
{
    double protected_pawn;
    double protected_by_protected_pawn;
} PawnChainWeights;

typedef struct
{
    double friendly_pawn;
    double enemy_pawn;
} BishopBlockWeights;

typedef struct
{
    PassedPawnWeights passed_pawn_weights;
    PawnChainWeights pawn_chain_weights;
    BishopBlockWeights bishop_block_weights;
    double double_pawn_weights;
    double isolated_pawn_weights;
    double backward_pawn_weights;
} PawnStructureWeights;

const PawnStructureWeights DEFAULT_MIDDLEGAME_PAWN_STRUCTURE_WEIGHTS = {
    .passed_pawn_weights = {.one_square_left = 120.0,
                            .two_squares_left = 80.0,
                            .three_squares_left = 50.0,
                            .four_squares_left = 30.0,
                            .five_squares_left = 15.0,
                            .six_squares_left = 15.0},
    .double_pawn_weights = -20.0,
    .isolated_pawn_weights = -20.0,
    .backward_pawn_weights = -10.0,
    .pawn_chain_weights = {.protected_pawn = 10.0, .protected_by_protected_pawn = 10.0},
    .bishop_block_weights = {.friendly_pawn = -3.0, .enemy_pawn = -2.0}};

const PawnStructureWeights DEFAULT_ENDGAME_PAWN_STRUCTURE_WEIGHTS = {
    .passed_pawn_weights = {.one_square_left = 120.0,
                            .two_squares_left = 80.0,
                            .three_squares_left = 50.0,
                            .four_squares_left = 30.0,
                            .five_squares_left = 15.0,
                            .six_squares_left = 15.0},
    .double_pawn_weights = -20.0,
    .isolated_pawn_weights = -20.0,
    .backward_pawn_weights = -10.0,
    .pawn_chain_weights = {.protected_pawn = 10.0, .protected_by_protected_pawn = 10.0},
    .bishop_block_weights = {.friendly_pawn = -3.0, .enemy_pawn = -2.0}};

// Double pawn penalty: -20 for each extra pawn on lfile
// Isolated pawn penalty: -20 for each isolated pawn
// Backward pawn penalty: -10 for each backward pawn
// Passed pawn bonus: 10 for each passed pawn (maybe add bonus for how far it is)
// Pawn island penalty: -10 for each pawn island
// Pawn chain bonus: 10 for each pawn in a chain

PawnStructureWeights get_pawn_structure_weights(BoardState *board_state);
double calculate_pawn_structure_score(PawnStructureWeights params, PawnStructureWeights middlegame_weights,
                                      PawnStructureWeights endgame_weights, double game_phase);