#pragma once

#include "../../utils/board.h"
#include <stdint.h>
#include "piece_square_tables.h"

typedef struct
{
    double a1;
    double a2;
    double a3;
    double a4;
    double a5;
    double a6;
    double a7;
    double a8;

    double b1;
    double b2;
    double b3;
    double b4;
    double b5;
    double b6;
    double b7;
    double b8;

    double c1;
    double c2;
    double c3;
    double c4;
    double c5;
    double c6;
    double c7;
    double c8;

    double d1;
    double d2;
    double d3;
    double d4;
    double d5;
    double d6;
    double d7;
    double d8;

    double e1;
    double e2;
    double e3;
    double e4;
    double e5;
    double e6;
    double e7;
    double e8;

    double f1;
    double f2;
    double f3;
    double f4;
    double f5;
    double f6;
    double f7;
    double f8;

    double g1;
    double g2;
    double g3;
    double g4;
    double g5;
    double g6;
    double g7;
    double g8;

    double h1;
    double h2;
    double h3;
    double h4;
    double h5;
    double h6;
    double h7;
    double h8;
} SquareWeights;

typedef struct
{
    SquareWeights pawn;
    SquareWeights knight;
    SquareWeights bishop;
    SquareWeights rook;
    SquareWeights queen;
    SquareWeights king;
} PieceSquareWeights;

PieceSquareWeights DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS;
PieceSquareWeights DEFAULT_ENDGAME_PIECE_SQUARE_WEIGHTS;

void init_default_piece_square_weights();
PieceSquareWeights get_piece_square_weights(Board *board);
double calculate_piece_square_score(PieceSquareWeights params, PieceSquareWeights middlegame_weights,
                                    PieceSquareWeights endgame_weights, double game_phase);