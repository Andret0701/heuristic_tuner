#include "tuner.h"

#include "../engine/board_stack.h"
#include "../algorithm/bot.h"
#include "./utils/fen.h"
#include <time.h>
#include <stdio.h>
#include "./utils/move.h"
#include "../algorithm/game_history.h"
#include "../algorithm/heuristic/heuristic.h"

#define MAX_GAME_LENGTH 100
#define NUM_GAMES_IN_EPOCH 100

GameHistory *init_game_histories()
{
    GameHistory *game_histories = malloc(sizeof(GameHistory) * NUM_GAMES_IN_EPOCH);
    for (int i = 0; i < NUM_GAMES_IN_EPOCH; i++)
    {
        game_histories[i].heuristic_scores = malloc(sizeof(HeuristicScore) * MAX_GAME_LENGTH);
        game_histories[i].game_length = 0;
    }
    return game_histories;
}

void destroy_game_histories(GameHistory *game_histories)
{
    for (int i = 0; i < NUM_GAMES_IN_EPOCH; i++)
    {
        free(game_histories[i].heuristic_scores);
    }
    free(game_histories);
}

void play_game(GameHistory *game_history, uint8_t search_depth, HeuristicWeights middlegame_weights, HeuristicWeights endgame_weights)
{
    Board board = fen_to_board(STARTFEN);

    reset_game_history();
    push_game_history(board);
    BoardState board_state = board_to_board_state(&board);

    uint8_t move_count = 0;
    while (true)
    {

        BotResult result = run_bot(search_depth, board);
        double score = calculate_heuristic_score(result.heuristic_weights,
                                                 middlegame_weights,
                                                 endgame_weights);
        game_history->heuristic_scores[move_count].score = score;
        game_history->heuristic_scores[move_count].weights = result.heuristic_weights;

        board = apply_move(&board, result.move);
        push_game_history(board);
        board_state = board_to_board_state(&board);

        move_count++;

        bool game_over = false;
        game_over |= threefold_repetition();
        game_over |= has_50_move_rule_occurred();
        game_over |= has_insufficient_material(&board);
        game_over |= get_game_result(&board_state) != UNKNOWN;
        game_over |= move_count == MAX_GAME_LENGTH;
        if (game_over)
        {
            game_history->game_length = move_count + 1;
            return;
        }
    }
}

void tune()
{
    GameHistory *game_histories = init_game_histories();
    HeuristicWeights middlegame_weights = get_default_middlegame_heuristic_weights();
    HeuristicWeights endgame_weights = get_default_endgame_heuristic_weights();
    set_heuristic_weights(middlegame_weights, endgame_weights);

    for (int i = 0; i < NUM_GAMES_IN_EPOCH; i++)
    {
        play_game(&game_histories[i], 2, middlegame_weights, endgame_weights);
        printf("Finished game: %u\n", i);
    }

    destroy_game_histories(game_histories);
}