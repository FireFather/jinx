#ifndef PIECE_H
#define PIECE_H

#include "define.h"

enum Color
    {
    White = 0,
    Black = 1
    };

enum PieceType
    {
    Pawn = 0,
    Knight = 1,
    Bishop = 2,
    Rook = 3,
    Queen = 4,
    King = 5
    };

enum Piece
    {
    WP = (Pawn << 1) | White,
    WN = (Knight << 1) | White,
    WB = (Bishop << 1) | White,
    WR = (Rook << 1) | White,
    WQ = (Queen << 1) | White,
    WK = (King << 1) | White,
    BP = (Pawn << 1) | Black,
    BN = (Knight << 1) | Black,
    BB = (Bishop << 1) | Black,
    BR = (Rook << 1) | Black,
    BQ = (Queen << 1) | Black,
    BK = (King << 1) | Black,
    noPiece
    };

__forceinline Color toggleColor (Color color)
    {
    return (Color(color ^ Black));
    }

__forceinline Piece makePiece (PieceType piece, Color color)
    {
    return (Piece((piece << 1) | color));
    }

__forceinline Color getColor (Piece piece)
    {
    return (Color(piece & 1));
    }

__forceinline PieceType getPieceType (Piece piece)
    {
    return (PieceType(piece >> 1));
    }

enum Square
    {
    noSquare = 64,
    A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63,
    A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55,
    A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47,
    A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39,
    A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31,
    A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23,
    A2 =  8, B2 =  9, C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15,
    A1 =  0, B1 =  1, C1 =  2, D1 =  3, E1 =  4, F1 =  5, G1 =  6, H1 =  7
    };

enum Dir
    {
    noDir = 0,

    // normal
    N = 8,
    NE = 9,
    E = 1,
    SE = - 7,
    S = - 8,
    SW = - 9,
    W = - 1,
    NW = 7,

    // knight
    NNE = 17,
    NNW = 15,
    SSE = - 15,
    SSW = - 17,
    ENE = 10,
    ESE = - 6,
    WNW = 6,
    WSW = - 10,
    };

enum PawnStep
    {
    SinglePawnStep = 8,
    DoublePawnStep = 16
    };

enum Rank
    {
    Rank1 = 0,
    Rank2 = 1,
    Rank3 = 2,
    Rank4 = 3,
    Rank5 = 4,
    Rank6 = 5,
    Rank7 = 6,
    Rank8 = 7
    };

enum File
    {
    FileA = 0,
    FileB = 1,
    FileC = 2,
    FileD = 3,
    FileE = 4,
    FileF = 5,
    FileG = 6,
    FileH = 7
    };

__forceinline Rank getRank (Square field)
    {
    return (Rank(field >> 3));
    }

__forceinline File getFile (Square field)
    {
    return (File(field & 7));
    }

__forceinline Square getField (File file, Rank rank)
    {
    return (Square(file | (rank << 3)));
    }

__forceinline Square flipVertical (Square field)
    {
    return (Square(field ^ 56));
    }

__forceinline Square flipHorizontal (Square field)
    {
    return (Square(field ^ 7));
    }

enum MoveType
    {
    Normal = 0,
    Castle = 1,
    EnPassant = 2,
    Promotion = 3
    };

enum CastInfo
    {
    CastleWOO = 0,  // white short
    CastleWOOO = 1, // white long
    CastleBOO = 2,  // black short
    CastleBOOO = 3  // black long
    };

enum PromInfo
    {
    PromRook = 0,   // promotion to rook
    PromBishop = 1, // promotion to bishop
    PromKnight = 2, // promotion to knight
    PromQueen = 3   // promotion to queen
    };

__forceinline PieceType getPromPiece (PromInfo prom)
    {
    switch(prom)
        {
        case PromRook:
            return (Rook);
            break;

        case PromBishop:
            return (Bishop);
            break;

        case PromKnight:
            return (Knight);
            break;

        case PromQueen:
            return (Queen);
            break;
        }
    return (Queen);
    }

#endif