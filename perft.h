#ifndef PERFT_H
#define PERFT_H

#include "movegen.h"
#include "notation.h"
#include "fen.h"

class Perft
    {
    public:

    Perft();
    uint64 perft(const string & fen, uint32 depth);
    void divide(const string & fen, uint32 depth);
    private:
    Board m_board;
    Fen m_fen;
    MoveGen m_movegen;
    MoveList m_movelist[256];
    uint64 loop(uint32 depth);
    bool validate_board(void);
    };

#endif