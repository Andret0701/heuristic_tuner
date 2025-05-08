#pragma once

#include "algorithm/heuristic/heuristic.h"

HeuristicWeights add_heuristic_weights(HeuristicWeights a, HeuristicWeights b);
HeuristicWeights subtract_heuristic_weights(HeuristicWeights a, HeuristicWeights b);
HeuristicWeights multiply_heuristic_weights(HeuristicWeights a, double b);
HeuristicWeights divide_heuristic_weights(HeuristicWeights a, double b);
HeuristicWeights mask_out_unused_heuristic_weights(HeuristicWeights params, HeuristicWeights weights);