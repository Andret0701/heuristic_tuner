#pragma once

#include "../../engine/board_state.h"
#include <stdint.h>

typedef struct
{
    double bishop;
    double rook;
    double queen;
} SquareControlWeights;

const SquareControlWeights DEFAULT_MIDDLEGAME_SQUARE_CONTROL_WEIGHTS = {
    .bishop = 5.0,
    .rook = 2.0,
    .queen = 1.0};
const SquareControlWeights DEFAULT_ENDGAME_SQUARE_CONTROL_WEIGHTS = {
    .bishop = 5.0,
    .rook = 2.0,
    .queen = 1.0};

SquareControlWeights get_square_control_weights(BoardState *board_state);
double calculate_square_control_score(SquareControlWeights params, SquareControlWeights middlegame_weights,
                                      SquareControlWeights endgame_weights, double game_phase);