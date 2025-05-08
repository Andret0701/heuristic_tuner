#include "tuner.h"

#include "../engine/board_stack.h"
#include "../algorithm/bot.h"
#include "./utils/fen.h"
#include <time.h>
#include <stdio.h>
#include "./utils/move.h"
#include "../algorithm/game_history.h"
#include "../algorithm/heuristic/heuristic.h"
#include "heuristic_weights_utils.h"

#define MAX_GAME_LENGTH 100
#define NUM_GAMES_IN_EPOCH 1

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
            game_history->game_length = move_count;
            return;
        }
    }
}

void run_epoch(GameHistory *game_histories, uint8_t search_depth, HeuristicWeights middlegame_weights, HeuristicWeights endgame_weights)
{
    for (int i = 0; i < NUM_GAMES_IN_EPOCH; i++)
        play_game(&game_histories[i], search_depth, middlegame_weights, endgame_weights);
}

HeuristicWeights gradient_descent_middlegame(double learning_rate, GameHistory *game_histories, HeuristicWeights weights)
{
    HeuristicWeights gradient_sum = {0};
    int num_moves = 0;
    double mean_square_error = 0;

    for (int i = 0; i < NUM_GAMES_IN_EPOCH; i++)
    {
        for (int j = 0; j < game_histories[i].game_length - 1; j++)
        {
            HeuristicScore current_score = game_histories[i].heuristic_scores[j];
            HeuristicScore next_score = game_histories[i].heuristic_scores[j + 1];

            // Scale x (features) by "middlegame weight"
            HeuristicWeights x = multiply_heuristic_weights(current_score.weights, 1.0 - next_score.weights.game_phase);

            double y_actual = next_score.score;
            double y_predicted = current_score.score;
            double error = y_predicted - y_actual;
            mean_square_error += (error * error);

            HeuristicWeights gradient = multiply_heuristic_weights(x, error);
            gradient = mask_out_unused_heuristic_weights(x, gradient);
            gradient_sum = add_heuristic_weights(gradient_sum, gradient);
            num_moves++;
        }
    }

    if (num_moves > 0)
    {
        HeuristicWeights avg_gradient = divide_heuristic_weights(gradient_sum, num_moves);
        HeuristicWeights adjustment = multiply_heuristic_weights(avg_gradient, learning_rate);
        HeuristicWeights new_weights = subtract_heuristic_weights(weights, adjustment);
        mean_square_error /= num_moves;
        new_weights.game_phase = mean_square_error;
        return new_weights;
    }

    return weights;
}

HeuristicWeights gradient_descent_endgame(double learning_rate, GameHistory *game_histories, HeuristicWeights weights)
{
    HeuristicWeights gradient_sum = {0};
    int num_moves = 0;
    double mean_square_error = 0;

    for (int i = 0; i < NUM_GAMES_IN_EPOCH; i++)
    {
        for (int j = 0; j < game_histories[i].game_length - 1; j++)
        {
            HeuristicScore current_score = game_histories[i].heuristic_scores[j];
            HeuristicScore next_score = game_histories[i].heuristic_scores[j + 1];

            // Scale x (features) by "endgame weight"
            HeuristicWeights x = multiply_heuristic_weights(current_score.weights, next_score.weights.game_phase);

            double y_actual = next_score.score;
            double y_predicted = current_score.score;
            double error = y_predicted - y_actual;
            mean_square_error += (error * error);

            HeuristicWeights gradient = multiply_heuristic_weights(x, error);
            gradient = mask_out_unused_heuristic_weights(x, gradient);

            gradient_sum = add_heuristic_weights(gradient_sum, gradient);
            num_moves++;
        }
    }

    if (num_moves > 0)
    {
        HeuristicWeights avg_gradient = divide_heuristic_weights(gradient_sum, num_moves);
        HeuristicWeights adjustment = multiply_heuristic_weights(avg_gradient, learning_rate);
        HeuristicWeights new_weights = subtract_heuristic_weights(weights, adjustment);
        mean_square_error /= num_moves;
        new_weights.game_phase = mean_square_error;
        return new_weights;
    }

    return weights;
}

void tune(double learning_rate)
{
    GameHistory *game_histories = init_game_histories();
    HeuristicWeights middlegame_weights = get_default_middlegame_heuristic_weights();
    HeuristicWeights endgame_weights = get_default_endgame_heuristic_weights();
    set_heuristic_weights(middlegame_weights, endgame_weights);

    int epoch = 0;

    double average_mean_square_error = 0;
    double last_average_mean_square_error = 0;
    while (true)
    {
        run_epoch(game_histories, 2, middlegame_weights, endgame_weights);
        middlegame_weights = gradient_descent_middlegame(learning_rate, game_histories, middlegame_weights);
        double mean_square_error = middlegame_weights.game_phase;
        average_mean_square_error += mean_square_error;
        endgame_weights = gradient_descent_endgame(learning_rate, game_histories, endgame_weights);
        set_heuristic_weights(middlegame_weights, endgame_weights);
        if (epoch % 100 == 0 && epoch != 0)
        {
            average_mean_square_error /= 100;
            double delta = average_mean_square_error - last_average_mean_square_error;
            double delta_percent = 100.0 * delta / (last_average_mean_square_error + 1e-8); // avoid division by zero
            const char *status;

            if (delta < -1e-6)
                status = "Improved! :)";
            else if (delta > 1e-6)
                status = "Got worse :(";
            else
                status = "No significant change";

            printf("Epoch %d summary:\n", epoch);
            printf("-------------------\n");
            if (epoch % 1000 == 0)
            {
                printf("Middlegame weights:\n");
                print_heuristic_weights(middlegame_weights);
                printf("Endgame weights:\n");
                print_heuristic_weights(endgame_weights);
            }
            printf("Mean Square Error: %.6f\n", average_mean_square_error);
            if (last_average_mean_square_error != 0)
                printf("Change: %+f (%.2f%%) - %s\n", delta, delta_percent, status);
            printf("\n");

            last_average_mean_square_error = average_mean_square_error;
            average_mean_square_error = 0;
        }
        epoch++;
    }

    destroy_game_histories(game_histories);
}