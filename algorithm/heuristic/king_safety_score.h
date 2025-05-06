#pragma once

#include "../../engine/board_state.h"
#include <stdint.h>

// #define CAN_CASTLE_KINGSIDE_BONUS 20
// #define CAN_CASTLE_QUEENSIDE_BONUS 15
// #define CAN_CASTLE_BOTH_SIDES_BONUS 25

// #define HAS_CASTLED_KINGSIDE_BONUS 30
// #define HAS_CASTLED_QUEENSIDE_BONUS 25

typedef struct CastlingWeights
{
    double kingside;
    double queenside;
    double both_sides;

    double has_castled_kingside;
    double has_castled_queenside;
} CastlingWeights;

typedef struct PawnShelterWeights
{
    double front_pawn;
    double ahead_pawn;
    double left_pawn;
    double right_pawn;
} PawnShelterWeights;

typedef struct
{
    double one_square;
    double two_squares;
    double three_squares;
    double four_squares;
} AttackingKingSquaresWeights;

typedef struct
{
    CastlingWeights castling_weights;
    PawnShelterWeights pawn_shelter_weights;
    AttackingKingSquaresWeights attacking_king_squares_weights;
} KingSafetyWeights;

const KingSafetyWeights DEFAULT_MIDDLEGAME_KING_SAFETY_WEIGHTS = {
    .castling_weights = {
        .kingside = 20.0,
        .queenside = 15.0,
        .both_sides = 25.0,
        .has_castled_kingside = 30.0,
        .has_castled_queenside = 25.0},
    .pawn_shelter_weights = {.front_pawn = 25.0, .ahead_pawn = 5.0, .left_pawn = 8.0, .right_pawn = 8.0},
    .attacking_king_squares_weights = {.one_square = 1.0, .two_squares = 2.0, .three_squares = 4.0, .four_squares = 8.0}};

const KingSafetyWeights DEFAULT_ENDGAME_KING_SAFETY_WEIGHTS = {
    .castling_weights = {
        .kingside = 20.0,
        .queenside = 15.0,
        .both_sides = 25.0,
        .has_castled_kingside = 30.0,
        .has_castled_queenside = 25.0},
    .pawn_shelter_weights = {.front_pawn = 25.0, .ahead_pawn = 5.0, .left_pawn = 8.0, .right_pawn = 8.0},
    .attacking_king_squares_weights = {.one_square = 1.0, .two_squares = 2.0, .three_squares = 4.0, .four_squares = 8.0}};

KingSafetyWeights get_king_safety_weights(BoardState *board_state);
double calculate_king_safety_score(KingSafetyWeights params, KingSafetyWeights middlegame_weights,
                                   KingSafetyWeights endgame_weights, double game_phase);