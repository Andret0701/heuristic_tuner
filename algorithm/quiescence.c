#include "quiescence.h"
#include "heuristic/heuristic.h"

QuiescenceResult quiescence(BoardState *board_state,
                            BoardStack *stack,
                            double alpha,
                            double beta,
                            uint8_t depth)
{
    // 1) Stand-pat
    HeuristicScore stand_pat = get_heuristic_score(board_state);
    HeuristicScore best_score = stand_pat;

    // 2) β-cutoff on stand-pat
    if (stand_pat.score >= beta)
        return (QuiescenceResult){stand_pat.score, stand_pat.weights, VALID};

    if (stand_pat.score > alpha)
        alpha = stand_pat.score;

    // 4) Recurse on captures
    uint16_t base = stack->count;
    generate_captures(board_state, stack);

    for (uint16_t i = base; i < stack->count; i++)
    {
        BoardState *child = &stack->boards[i];
        QuiescenceResult quiescence_result = quiescence(child, stack, -beta, -alpha, depth + 1);
        quiescence_result.score = -quiescence_result.score;
        if (quiescence_result.valid == INVALID)
        {
            stack->count = base;
            return (QuiescenceResult){0, stand_pat.weights, INVALID};
        }

        if (quiescence_result.score >= beta)
        {
            stack->count = base;
            return (QuiescenceResult){quiescence_result.score, quiescence_result.heuristic_weights, VALID};
        }

        if (quiescence_result.score > best_score.score)
            best_score = (HeuristicScore){quiescence_result.score, quiescence_result.heuristic_weights};

        if (quiescence_result.score > alpha)
            alpha = quiescence_result.score;
    }

    stack->count = base;
    return (QuiescenceResult){best_score.score, best_score.weights, VALID};
}
