#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../../utils/board.h"
#include "../board_score.h"
#include "../../engine/board_state.h"

#include "position_score.h"
#include "material_score.h"
#include "king_safety_score.h"
#include "pawn_structure_score.h"
#include "square_control.h"

typedef struct
{
    MaterialWeights material_weights;
    PieceSquareWeights piece_square_weights;
    KingSafetyWeights king_safety_weights;
    PawnStructureWeights pawn_structure_weights;
    SquareControlWeights square_control_weights;
    double game_phase;
} HeuristicWeights;

typedef struct
{
    double score;
    HeuristicWeights weights;
} HeuristicScore;

bool has_insufficient_material(Board *board);
Result get_result(BoardState *board_state, bool is_finished);
HeuristicScore get_heuristic_score(BoardState *board_state);