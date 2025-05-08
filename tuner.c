#include "tuner.h"

#include "../engine/board_stack.h"
#include "../algorithm/bot.h"
#include "./utils/fen.h"
#include <time.h>
#include <stdio.h>
#include "./utils/move.h"
#include "../algorithm/game_history.h"
#include "../algorithm/heuristic/heuristic.h"

void play_game(uint8_t search_depth)
{
    Board board = fen_to_board(STARTFEN);

    HeuristicWeights DEFAULT_MIDDLEGAME_WEIGHTS = get_default_middlegame_heuristic_weights();
    HeuristicWeights DEFAULT_ENDGAME_WEIGHTS = get_default_endgame_heuristic_weights();

    reset_game_history();
    push_game_history(board);
    BoardState board_state = board_to_board_state(&board);
    while (true)
    {
        BotResult result = run_bot(search_depth, board);
        double score2 = calculate_heuristic_score(result.heuristic_weights, DEFAULT_MIDDLEGAME_WEIGHTS, DEFAULT_ENDGAME_WEIGHTS);
        printf("Score1: %f, Score2: %f\n", result.score.score, score2);

        Color side_to_move = board.side_to_move;
        board = apply_move(&board, result.move);
        if (board.side_to_move == side_to_move)
        {
            printf("Invalid move: %s\n", result.move);
            print_board(&board);
            exit(1);
        }

        push_game_history(board);
        board_state = board_to_board_state(&board);
        print_board(&board);
        printf("Move: %s, Score: %f, Depth: %d, Result: %s\n", result.move, result.score.score, result.score.depth, result.score.result == WON ? "WON" : (result.score.result == LOST ? "LOST" : (result.score.result == DRAW ? "DRAW" : "UNKNOWN")));

        if (threefold_repetition())
        {
            printf("Draw by threefold repetition\n");
            break;
        }

        if (has_50_move_rule_occurred())
        {
            printf("Draw by 50 move rule\n");
            break;
        }

        if (has_insufficient_material(&board))
        {
            printf("Draw by insufficient material\n");
            break;
        }

        Result game_result = get_game_result(&board_state);
        if (board_state.board.side_to_move == BLACK && game_result == LOST)
        {
            printf("White checkmated black\n");
            break;
        }
        else if (board_state.board.side_to_move == WHITE && game_result == LOST)
        {
            printf("Black checkmated white\n");
            break;
        }
        else if (game_result == DRAW)
        {
            printf("Stalemate\n");
            break;
        }

        printf("\n");
    }
}