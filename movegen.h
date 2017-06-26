#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "move.h"

class MoveGen
    {
    public:

    MoveGen(const Board & board);

    void getAll(MoveList & moves)const;
    void getCaptures(MoveList & moves)const;
    void getQuiets(MoveList & moves)const;
    void getEvasions(MoveList & moves)const;
    bool isLegal(const Move & move)const;
    private:
    const Board * m_board;
    const Board & board(void)const;
    void genMoves(MoveList & moves, bool captures, bool quiets, uint64 from, uint64 to)const;
    void addPromotions(MoveList & moves, Square from, Dir dir)const;
    };

#endif