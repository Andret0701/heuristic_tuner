#include "position_score.h"
#include "piece_square_tables.h"
#include "../../utils/bitboard.h"

void print_square_weights(SquareWeights square_weights)
{
    double *fields = (double *)&square_weights;
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            int index = rank * 8 + file;
            printf("%.2f ", fields[index]);
        }
        printf("\n");
    }
}

SquareWeights get_piece_count(uint64_t white_pieces, uint64_t black_pieces)
{
    SquareWeights square_weights = {0};
    double *fields = (double *)&square_weights;

    while (white_pieces)
    {
        int square = __builtin_ctzll(white_pieces);
        white_pieces &= white_pieces - 1;
        fields[square]++;
    }

    black_pieces = flip_bitboard(black_pieces);
    while (black_pieces)
    {
        int square = __builtin_ctzll(black_pieces);
        black_pieces &= black_pieces - 1;
        fields[square]--;
    }

    return square_weights;
}

double get_piece_score(uint64_t white_pieces, uint64_t black_pieces, SquareWeights middlegame_weights,
                       SquareWeights endgame_weights, double game_phase)
{
    double score = 0;
    double *middlegame_fields = (double *)&middlegame_weights;
    double *endgame_fields = (double *)&endgame_weights;
    while (white_pieces)
    {
        int square = __builtin_ctzll(white_pieces);
        white_pieces &= white_pieces - 1;
        score += middlegame_fields[square] * (1 - game_phase) + endgame_fields[square] * game_phase;
    }

    black_pieces = flip_bitboard(black_pieces);
    while (black_pieces)
    {
        int square = __builtin_ctzll(black_pieces);
        black_pieces &= black_pieces - 1;
        score -= middlegame_fields[square] * (1 - game_phase) + endgame_fields[square] * game_phase;
    }

    return score;
}

PieceSquareWeights get_piece_square_weights(Board *board)
{
    PieceSquareWeights piece_square_weights = {0};

    piece_square_weights.pawn = get_piece_count(board->white_pieces.pawns, board->black_pieces.pawns);
    piece_square_weights.knight = get_piece_count(board->white_pieces.knights, board->black_pieces.knights);
    piece_square_weights.bishop = get_piece_count(board->white_pieces.bishops, board->black_pieces.bishops);
    piece_square_weights.rook = get_piece_count(board->white_pieces.rooks, board->black_pieces.rooks);
    piece_square_weights.queen = get_piece_count(board->white_pieces.queens, board->black_pieces.queens);
    piece_square_weights.king = get_piece_count(board->white_pieces.king, board->black_pieces.king);

    // printf("Board: \n");
    // print_board(board);
    // printf("Pawn square weights:\n");
    // print_square_weights(piece_square_weights.pawn);
    // printf("Pawn middlegame weights:\n");
    // print_square_weights(DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS.pawn);
    // printf("Knight square weights:\n");
    // print_square_weights(piece_square_weights.knight);
    // printf("Knight middlegame weights:\n");
    // print_square_weights(DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS.knight);
    // printf("Bishop square weights:\n");
    // print_square_weights(piece_square_weights.bishop);
    // printf("Bishop middlegame weights:\n");
    // print_square_weights(DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS.bishop);
    // printf("Rook square weights:\n");
    // print_square_weights(piece_square_weights.rook);
    // printf("Rook middlegame weights:\n");
    // print_square_weights(DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS.rook);
    // printf("Queen square weights:\n");
    // print_square_weights(piece_square_weights.queen);
    // printf("Queen middlegame weights:\n");
    // print_square_weights(DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS.queen);
    // printf("King square weights:\n");
    // print_square_weights(piece_square_weights.king);
    // printf("King middlegame weights:\n");
    // print_square_weights(DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS.king);

    return piece_square_weights;
}

double calculate_square_score(SquareWeights params, SquareWeights middlegame_weights,
                              SquareWeights endgame_weights, double game_phase)
{
    double score = 0;

    score += params.a1 * (middlegame_weights.a1 * (1 - game_phase) + endgame_weights.a1 * game_phase);
    score += params.a2 * (middlegame_weights.a2 * (1 - game_phase) + endgame_weights.a2 * game_phase);
    score += params.a3 * (middlegame_weights.a3 * (1 - game_phase) + endgame_weights.a3 * game_phase);
    score += params.a4 * (middlegame_weights.a4 * (1 - game_phase) + endgame_weights.a4 * game_phase);
    score += params.a5 * (middlegame_weights.a5 * (1 - game_phase) + endgame_weights.a5 * game_phase);
    score += params.a6 * (middlegame_weights.a6 * (1 - game_phase) + endgame_weights.a6 * game_phase);
    score += params.a7 * (middlegame_weights.a7 * (1 - game_phase) + endgame_weights.a7 * game_phase);
    score += params.a8 * (middlegame_weights.a8 * (1 - game_phase) + endgame_weights.a8 * game_phase);
    score += params.b1 * (middlegame_weights.b1 * (1 - game_phase) + endgame_weights.b1 * game_phase);
    score += params.b2 * (middlegame_weights.b2 * (1 - game_phase) + endgame_weights.b2 * game_phase);
    score += params.b3 * (middlegame_weights.b3 * (1 - game_phase) + endgame_weights.b3 * game_phase);
    score += params.b4 * (middlegame_weights.b4 * (1 - game_phase) + endgame_weights.b4 * game_phase);
    score += params.b5 * (middlegame_weights.b5 * (1 - game_phase) + endgame_weights.b5 * game_phase);
    score += params.b6 * (middlegame_weights.b6 * (1 - game_phase) + endgame_weights.b6 * game_phase);
    score += params.b7 * (middlegame_weights.b7 * (1 - game_phase) + endgame_weights.b7 * game_phase);
    score += params.b8 * (middlegame_weights.b8 * (1 - game_phase) + endgame_weights.b8 * game_phase);
    score += params.c1 * (middlegame_weights.c1 * (1 - game_phase) + endgame_weights.c1 * game_phase);
    score += params.c2 * (middlegame_weights.c2 * (1 - game_phase) + endgame_weights.c2 * game_phase);
    score += params.c3 * (middlegame_weights.c3 * (1 - game_phase) + endgame_weights.c3 * game_phase);
    score += params.c4 * (middlegame_weights.c4 * (1 - game_phase) + endgame_weights.c4 * game_phase);
    score += params.c5 * (middlegame_weights.c5 * (1 - game_phase) + endgame_weights.c5 * game_phase);
    score += params.c6 * (middlegame_weights.c6 * (1 - game_phase) + endgame_weights.c6 * game_phase);
    score += params.c7 * (middlegame_weights.c7 * (1 - game_phase) + endgame_weights.c7 * game_phase);
    score += params.c8 * (middlegame_weights.c8 * (1 - game_phase) + endgame_weights.c8 * game_phase);
    score += params.d1 * (middlegame_weights.d1 * (1 - game_phase) + endgame_weights.d1 * game_phase);
    score += params.d2 * (middlegame_weights.d2 * (1 - game_phase) + endgame_weights.d2 * game_phase);
    score += params.d3 * (middlegame_weights.d3 * (1 - game_phase) + endgame_weights.d3 * game_phase);
    score += params.d4 * (middlegame_weights.d4 * (1 - game_phase) + endgame_weights.d4 * game_phase);
    score += params.d5 * (middlegame_weights.d5 * (1 - game_phase) + endgame_weights.d5 * game_phase);
    score += params.d6 * (middlegame_weights.d6 * (1 - game_phase) + endgame_weights.d6 * game_phase);
    score += params.d7 * (middlegame_weights.d7 * (1 - game_phase) + endgame_weights.d7 * game_phase);
    score += params.d8 * (middlegame_weights.d8 * (1 - game_phase) + endgame_weights.d8 * game_phase);
    score += params.e1 * (middlegame_weights.e1 * (1 - game_phase) + endgame_weights.e1 * game_phase);
    score += params.e2 * (middlegame_weights.e2 * (1 - game_phase) + endgame_weights.e2 * game_phase);
    score += params.e3 * (middlegame_weights.e3 * (1 - game_phase) + endgame_weights.e3 * game_phase);
    score += params.e4 * (middlegame_weights.e4 * (1 - game_phase) + endgame_weights.e4 * game_phase);
    score += params.e5 * (middlegame_weights.e5 * (1 - game_phase) + endgame_weights.e5 * game_phase);
    score += params.e6 * (middlegame_weights.e6 * (1 - game_phase) + endgame_weights.e6 * game_phase);
    score += params.e7 * (middlegame_weights.e7 * (1 - game_phase) + endgame_weights.e7 * game_phase);
    score += params.e8 * (middlegame_weights.e8 * (1 - game_phase) + endgame_weights.e8 * game_phase);
    score += params.f1 * (middlegame_weights.f1 * (1 - game_phase) + endgame_weights.f1 * game_phase);
    score += params.f2 * (middlegame_weights.f2 * (1 - game_phase) + endgame_weights.f2 * game_phase);
    score += params.f3 * (middlegame_weights.f3 * (1 - game_phase) + endgame_weights.f3 * game_phase);
    score += params.f4 * (middlegame_weights.f4 * (1 - game_phase) + endgame_weights.f4 * game_phase);
    score += params.f5 * (middlegame_weights.f5 * (1 - game_phase) + endgame_weights.f5 * game_phase);
    score += params.f6 * (middlegame_weights.f6 * (1 - game_phase) + endgame_weights.f6 * game_phase);
    score += params.f7 * (middlegame_weights.f7 * (1 - game_phase) + endgame_weights.f7 * game_phase);
    score += params.f8 * (middlegame_weights.f8 * (1 - game_phase) + endgame_weights.f8 * game_phase);
    score += params.g1 * (middlegame_weights.g1 * (1 - game_phase) + endgame_weights.g1 * game_phase);
    score += params.g2 * (middlegame_weights.g2 * (1 - game_phase) + endgame_weights.g2 * game_phase);
    score += params.g3 * (middlegame_weights.g3 * (1 - game_phase) + endgame_weights.g3 * game_phase);
    score += params.g4 * (middlegame_weights.g4 * (1 - game_phase) + endgame_weights.g4 * game_phase);
    score += params.g5 * (middlegame_weights.g5 * (1 - game_phase) + endgame_weights.g5 * game_phase);
    score += params.g6 * (middlegame_weights.g6 * (1 - game_phase) + endgame_weights.g6 * game_phase);
    score += params.g7 * (middlegame_weights.g7 * (1 - game_phase) + endgame_weights.g7 * game_phase);
    score += params.g8 * (middlegame_weights.g8 * (1 - game_phase) + endgame_weights.g8 * game_phase);
    score += params.h1 * (middlegame_weights.h1 * (1 - game_phase) + endgame_weights.h1 * game_phase);
    score += params.h2 * (middlegame_weights.h2 * (1 - game_phase) + endgame_weights.h2 * game_phase);
    score += params.h3 * (middlegame_weights.h3 * (1 - game_phase) + endgame_weights.h3 * game_phase);
    score += params.h4 * (middlegame_weights.h4 * (1 - game_phase) + endgame_weights.h4 * game_phase);
    score += params.h5 * (middlegame_weights.h5 * (1 - game_phase) + endgame_weights.h5 * game_phase);
    score += params.h6 * (middlegame_weights.h6 * (1 - game_phase) + endgame_weights.h6 * game_phase);
    score += params.h7 * (middlegame_weights.h7 * (1 - game_phase) + endgame_weights.h7 * game_phase);
    score += params.h8 * (middlegame_weights.h8 * (1 - game_phase) + endgame_weights.h8 * game_phase);

    return score;
}

double calculate_piece_square_score(PieceSquareWeights params, PieceSquareWeights middlegame_weights,
                                    PieceSquareWeights endgame_weights, double game_phase)
{
    double score = 0;

    score += calculate_square_score(params.pawn, middlegame_weights.pawn, endgame_weights.pawn, game_phase);
    score += calculate_square_score(params.knight, middlegame_weights.knight, endgame_weights.knight, game_phase);
    score += calculate_square_score(params.bishop, middlegame_weights.bishop, endgame_weights.bishop, game_phase);
    score += calculate_square_score(params.rook, middlegame_weights.rook, endgame_weights.rook, game_phase);
    score += calculate_square_score(params.queen, middlegame_weights.queen, endgame_weights.queen, game_phase);
    score += calculate_square_score(params.king, middlegame_weights.king, endgame_weights.king, game_phase);

    return score;
}

double calculate_piece_square_score_fast(const Board *board, PieceSquareWeights middlegame_weights,
                                         PieceSquareWeights endgame_weights, double game_phase)
{
    double score = 0;
    score += get_piece_score(board->white_pieces.pawns, board->black_pieces.pawns, middlegame_weights.pawn,
                             endgame_weights.pawn, game_phase);
    score += get_piece_score(board->white_pieces.knights, board->black_pieces.knights, middlegame_weights.knight,
                             endgame_weights.knight, game_phase);
    score += get_piece_score(board->white_pieces.bishops, board->black_pieces.bishops, middlegame_weights.bishop,
                             endgame_weights.bishop, game_phase);
    score += get_piece_score(board->white_pieces.rooks, board->black_pieces.rooks, middlegame_weights.rook,
                             endgame_weights.rook, game_phase);
    score += get_piece_score(board->white_pieces.queens, board->black_pieces.queens, middlegame_weights.queen,
                             endgame_weights.queen, game_phase);
    score += get_piece_score(board->white_pieces.king, board->black_pieces.king, middlegame_weights.king,
                             endgame_weights.king, game_phase);
    return score;
}

SquareWeights table_to_square_weights(const int16_t *table)
{
    SquareWeights square_weights = {0};
    double *fields = (double *)&square_weights;

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int index = (7 - rank) * 8 + file;
            fields[rank * 8 + file] = table[index];
        }
    }

    return square_weights;
}

PieceSquareWeights
get_default_middlegame_piece_square_weights()
{
    PieceSquareWeights piece_square_weights = {0};

    piece_square_weights.pawn = table_to_square_weights(PAWN_TABLE);
    piece_square_weights.knight = table_to_square_weights(KNIGHT_TABLE);
    piece_square_weights.bishop = table_to_square_weights(BISHOP_TABLE);
    piece_square_weights.rook = table_to_square_weights(ROOK_TABLE);
    piece_square_weights.queen = table_to_square_weights(QUEEN_TABLE);
    piece_square_weights.king = table_to_square_weights(KING_TABLE);

    return piece_square_weights;
}

PieceSquareWeights get_default_endgame_piece_square_weights()
{
    PieceSquareWeights piece_square_weights = {0};

    piece_square_weights.pawn = table_to_square_weights(PAWN_TABLE_END);
    piece_square_weights.knight = table_to_square_weights(KNIGHT_TABLE);
    piece_square_weights.bishop = table_to_square_weights(BISHOP_TABLE);
    piece_square_weights.rook = table_to_square_weights(ROOK_TABLE);
    piece_square_weights.queen = table_to_square_weights(QUEEN_TABLE);
    piece_square_weights.king = table_to_square_weights(KING_TABLE_END);

    return piece_square_weights;
}

void init_default_piece_square_weights()
{
    DEFAULT_MIDDLEGAME_PIECE_SQUARE_WEIGHTS = get_default_middlegame_piece_square_weights();
    DEFAULT_ENDGAME_PIECE_SQUARE_WEIGHTS = get_default_endgame_piece_square_weights();
}
