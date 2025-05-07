#pragma once

#include "board_score.h"
#include "heuristic/heuristic.h"
typedef enum
{
    INVALID,
    VALID
} Validity;

typedef struct
{
    BoardScore board_score;
    HeuristicWeights heuristic_weights;
    Validity valid;
} SearchResult;

typedef struct
{
    double score;
    HeuristicWeights heuristic_weights;
    Validity valid;
} QuiescenceResult;

void print_search_result(SearchResult search_result);
void print_quiescence_result(QuiescenceResult quiescence_result);
