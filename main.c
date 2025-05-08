#include <stdio.h>
#include "utils/board.h"
#include "engine/board_stack.h"
#include "engine/piece_moves.h"
#include "utils/fen.h"

#include "engine/tests/count_test.h"
#include "engine/benchmark/count_benchmark.h"
#include "algorithm/board_score.h"

#include "algorithm/bot.h"
#include <string.h>
#include <stdlib.h>
#include "algorithm/game_history.h"
#include "utils/move.h"
#include "engine/tests/can_move_test.h"
#include "engine/tests/capture_move_test.h"
#include "engine/tests/encoded_move_test.h"

#include "utils/bitboard.h"
#include "algorithm/transposition_table.h"
#include "algorithm/zobrist_hash.h"

#include "algorithm/heuristic/heuristic.h"
#include "algorithm/heuristic/pawn_structure_score.h"
#include "engine/encoded_move.h"
#include "algorithm/heuristic/position_score.h"

#include "tuner.h"
int main()
{
    init_zobrist_keys();
    init_default_piece_square_weights();

    play_game(2);

    return 0;
}
