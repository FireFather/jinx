#ifndef NOTATION_H
#define NOTATION_H

#include "piece.h"
#include "board.h"

bool isMove (const string & str);
string moveToStr (const uint32 & move, const Board & board, bool castling_san = false);
uint32 strToMove (const string & str, const Board & board);

string fieldToStr (Square field);
Square strToField (const string & str);

string pieceToStr (Piece piece);
Piece chrToPiece (const string::value_type & chr);

uint64 strToBB (const string & str);

#endif