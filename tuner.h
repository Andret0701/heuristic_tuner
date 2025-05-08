#pragma once

#include <stdint.h>

typedef struct
{
    HeuristicScore *heuristic_scores;
    uint8_t game_length;
} GameHistory;

void tune(double learning_rate);