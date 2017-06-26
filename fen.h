#ifndef FEN_H
#define FEN_H

#include "board.h"

#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

class Fen
    {
    public:

    Fen(Board & board);

    void set(const string & fen = STARTPOS);
    string get(void);
    string flip(const string & fen);
    void flip(void);
    bool isCastExt(const string & fen);
    private:
    Board * m_board;
    Board & board(void);

    void setFENPositions(const string & fen);
    void getFENPositions(string & fen);
    string getFENLine(Square from, Square to);
    private:
    bool getRegularCastInfo(CastInfo & info, const string::value_type & chr);
    Square getExtCastField(const string::value_type & chr);
    };

#endif