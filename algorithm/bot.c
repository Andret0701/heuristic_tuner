#include "bot.h"
#include <stdio.h>
#include "../utils/board.h"
#include "../engine/board_stack.h"
#include "../utils/fen.h"
#include "negamax.h"
#include "../engine/piece_moves.h"
#include "../utils/move.h"
#include "game_history.h"
#include "transposition_table.h"

#define BOARD_STACK_SIZE 65535
#define MAX_DEPTH 150
#define MAX_MOVES 300

void print_bot_result(BotResult result)
{
    printf("Move: %s, Depth: %d (Score: %f, Depth: %d, Result: %s)\n",
           result.move,
           result.depth,
           result.score.score,
           result.score.depth,
           result.score.result == WON ? "WON" : (result.score.result == LOST ? "LOST" : (result.score.result == DRAW ? "DRAW" : "UNKNOWN")));
}

BoardScore move_scores[MAX_DEPTH][MAX_MOVES];
HeuristicWeights heuristic_weights[MAX_DEPTH][MAX_MOVES];
void print_out_search_info(BoardStack *stack, Board *board, BoardState *best_board, BoardScore best_score, uint8_t depth, uint16_t cancelled_index)
{
    FILE *file = fopen("search_info.txt", "a");
    if (file == NULL)
    {
        perror("Failed to open file");
        return;
    }

    if (cancelled_index == 0)
    {
        depth--;
        cancelled_index = stack->count + 1;
    }

    fprintf(file, "Move: %d\n", get_full_move_count());
    fprintf(file, "Fen: %s\n", board_to_fen(board));
    if (best_board->white_check)
        fprintf(file, "White is in check\n");
    if (best_board->black_check)
        fprintf(file, "Black is in check\n");
    fprintf(file, "It is %s's turn\n", board->side_to_move == WHITE ? "White" : "Black");
    fprintf(file, "The best move is %s with a score of %f, depth of %d, and result %s\n",
            board_to_move(board, &best_board->board),
            best_score.score,
            best_score.depth,
            best_score.result == WON ? "WON" : best_score.result == LOST ? "LOST"
                                           : best_score.result == DRAW   ? "DRAW"
                                                                         : "UNKNOWN");

    fprintf(file, "Move: | ");
    for (int16_t d = depth; d >= 0; d--)
    {
        fprintf(file, "Depth: %-27d", d);
        if (d != 0)
            fprintf(file, " | ");
    }
    fprintf(file, "\n");

    for (uint16_t i = 0; i < stack->count; i++)
    {
        fprintf(file, "%-5s | ", board_to_move(board, &stack->boards[i].board));
        for (int16_t d = depth; d >= 0; d--)
        {
            if (i == cancelled_index && d == depth)
                fprintf(file, "---                               ");
            else if (i >= cancelled_index && d == depth)
                fprintf(file, "                                  ");
            else
            {
                fprintf(file, "%-10.2f %-10s Depth: %-5d",
                        (double)move_scores[d][i].score,
                        move_scores[d][i].result == WON ? "WON" : move_scores[d][i].result == LOST ? "LOST"
                                                              : move_scores[d][i].result == DRAW   ? "DRAW"
                                                                                                   : "UNKNOWN",
                        move_scores[d][i].depth);
            }
            if (d != 0)
                fprintf(file, " | ");
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");

    fclose(file);
}

void updated_best_board(BoardState **best_board, BoardScore *best_score, HeuristicWeights *best_heuristic_weights, BoardState *current_board_state, BoardScore score, HeuristicWeights heuristic_weights)
{
    if (is_greater_score(score, *best_score))
    {
        *best_board = current_board_state;
        *best_score = score;
        *best_heuristic_weights = heuristic_weights;
    }
}

BotResult run_bot(uint8_t search_depth, Board board)
{
    TT_clear_generation();
    BoardState board_state = board_to_board_state(&board);

    BoardStack *stack = create_board_stack(BOARD_STACK_SIZE);
    generate_moves(&board_state, stack);

    uint8_t depth = 0;
    while (true)
    {
        BoardScore best_score = WORST_SCORE;
        BoardState *best_board = NULL;
        HeuristicWeights best_heuristic_weights = {0};
        for (uint16_t i = 0; i < stack->count; i++)
        {
            if (depth != 0 && move_scores[depth - 1][i].result == LOST)
            {
                move_scores[depth][i] = move_scores[depth - 1][i];
                heuristic_weights[depth][i] = heuristic_weights[depth - 1][i];
                updated_best_board(&best_board, &best_score, &best_heuristic_weights, &stack->boards[i], move_scores[depth][i], heuristic_weights[depth][i]);
                continue;
            }

            BoardState *current_board_state = &stack->boards[i];
            SearchResult search_result = negamax(current_board_state, stack, depth, 0, WORST_SCORE, invert_score(best_score));
            search_result.board_score = invert_score(search_result.board_score);
            BoardScore score = search_result.board_score;
            move_scores[depth][i] = score;
            heuristic_weights[depth][i] = search_result.heuristic_weights;
            updated_best_board(&best_board, &best_score, &best_heuristic_weights, current_board_state, score, search_result.heuristic_weights);

            // If the move is winning. Do not search deeper.
            if (best_score.result == WON && best_score.depth <= depth)
            {
                print_out_search_info(stack, &board, best_board, best_score, depth, i + 1);
                BotResult result = {board_to_move(&board, &best_board->board), best_score, best_heuristic_weights, depth};
                destroy_board_stack(stack);
                return result;
            }
        }
        TT_store(hash_board(&board_state.board), depth, best_score.score, best_score.result, EXACT, best_board->move);

        // Sort the stack by score
        for (uint16_t i = 0; i < stack->count; i++)
        {
            for (uint16_t j = i + 1; j < stack->count; j++)
            {
                BoardScore a = move_scores[depth][i];
                BoardScore b = move_scores[depth][j];
                if (is_greater_score(b, a))
                {
                    // Swap the boards
                    BoardState temp_board = stack->boards[i];
                    stack->boards[i] = stack->boards[j];
                    stack->boards[j] = temp_board;

                    // Swap the scores
                    for (uint8_t d = 0; d <= depth; d++)
                    {
                        BoardScore temp_score = move_scores[d][i];
                        move_scores[d][i] = move_scores[d][j];
                        move_scores[d][j] = temp_score;

                        HeuristicWeights temp_weights = heuristic_weights[d][i];
                        heuristic_weights[d][i] = heuristic_weights[d][j];
                        heuristic_weights[d][j] = temp_weights;
                    }
                }
            }
        }

        // if no move is unknown
        bool all_moves_known = true;
        for (uint16_t i = 0; i < stack->count; i++)
        {
            if (move_scores[depth][i].result == UNKNOWN)
            {
                all_moves_known = false;
                break;
            }
        }

        if (all_moves_known)
        {
            print_out_search_info(stack, &board, best_board, best_score, depth, stack->count + 1);

            BotResult result = {board_to_move(&board, &best_board->board), best_score, best_heuristic_weights, depth};
            destroy_board_stack(stack);
            return result;
        }

        depth++;
        if (depth == MAX_DEPTH || depth > search_depth)
        {
            print_out_search_info(stack, &board, best_board, best_score, depth - 1, stack->count + 1);
            BotResult result = {board_to_move(&board, &best_board->board), best_score, best_heuristic_weights, depth - 1};
            destroy_board_stack(stack);
            return result;
        }
    }
}