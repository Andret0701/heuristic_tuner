#pragma once
#include "board_score.h"
#include "heuristic/heuristic.h"

typedef struct
{
    char *move;
    BoardScore score;
    HeuristicWeights heuristic_weights;
    uint8_t depth;
} BotResult;

void print_bot_result(BotResult result);
BotResult run_bot(uint8_t search_depth, Board board);