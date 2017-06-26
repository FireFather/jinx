#include "perft.h"
#include "uci.h"
#include "console.h"

Perft::Perft () :
    m_fen(m_board),
    m_movegen(m_board)
    {
    }

uint64 Perft::perft (const string & fen, uint32 depth)
    {
    uint64 nodes = 0;
    m_fen.set(fen);

    for ( uint32 i = 1; i <= depth; ++ i )
        {
        Time perf;
        nodes = loop(i);

        string info;
        info += "depth " + int32ToStr(i) + ": ";
        info += int64ToStr(nodes) + " nodes" + ", ";
        info += floatToStr(perf.getSecs()) + " secs" + ", ";
        info += int64ToStr(perf.getNps(nodes)) + " nps";
        uci.sendline(info);
        }
    return (nodes);
    }

void Perft::divide (const string & fen, uint32 depth)
    {
    Time perf;
    uint64 nodes = 0;
    int32 moveNum = 0;
    m_fen.set(fen);

    MoveList & moves = m_movelist[depth];
    moves.clear();
    m_movegen.getAll(moves);

    //    uci.sendline("");
    for ( int32 i = 0; i < moves.size(); ++ i )
        {
        Move & move = moves[i];

        if(m_board.doMove(move))
            {
            uint64 delta = loop(depth - 1);
            nodes += delta;
            m_board.undoMove(move);

            string num = int32ToStr(++ moveNum);

            if(1 == num.size())
                num = " " + num;
            string out = "move " + num + ": " + moveToStr(move, m_board) + " " + int64ToStr(delta) + " nodes";
            uci.sendline(out);
            }
        }
    string info;
    info += "total: " + int64ToStr(nodes) + " nodes, ";
    info += floatToStr(perf.getSecs()) + " secs, ";
    info += int64ToStr(perf.getNps(nodes)) + " nps";
    uci.sendline(info);
    }

uint64 Perft::loop (uint32 depth)
    {
    if(depth == 0)
        return (1);

    MoveList & moves = m_movelist[depth];
    moves.clear();
    m_movegen.getAll(moves);

    uint64 count = 0;

    for ( int32 i = 0; i < moves.size(); ++ i )
        {
        Move & move = moves[i];

        if(m_board.doMove(move))
            {
            count += loop(depth - 1);
            m_board.undoMove(move);
            }
        }
    return (count);
    }

bool Perft::validate_board (void)
    {
    Board b;
    b.clearAll();

    // take over the board
    for ( int32 i = 0; i < 64; ++ i )
        {
        Square f = Square(i);
        b.setPiece(f, m_board.getPiece(f));
        }
    b.initPieceDeps();

    if(Black == m_board.getSide())
        b.m_hash ^= gl_hashkeys.getKeyColor();

    // check bitboards for pieces
    for ( int32 j = 0; j < 12; ++ j )
        if(b.m_bitboards[j] != m_board.m_bitboards[j])
            return (false);

    // check bitboards for colors
    if(b.m_colorbits[White] != m_board.m_colorbits[White])
        return (false);

    if(b.m_colorbits[Black] != m_board.m_colorbits[Black])
        return (false);

    // check squares
    for ( int32 k = 0; k < 64; ++ k )
        if(b.m_piece[k] != m_board.m_piece[k])
            return (false);

    // check material
    if(b.m_material[White] != m_board.m_material[White])
        return (false);

    if(b.m_material[Black] != m_board.m_material[Black])
        return (false);

    // check game phase
    if(b.m_matPhase[White] != m_board.m_matPhase[White])
        return (false);

    if(b.m_matPhase[Black] != m_board.m_matPhase[Black])
        return (false);

    // check counts for material
    if(b.m_matCount[White] != m_board.m_matCount[White])
        return (false);

    if(b.m_matCount[Black] != m_board.m_matCount[Black])
        return (false);

    // check counts for pieces
    for ( int32 l = 0; l < 12; ++ l )
        {
        if(b.m_pcCount[l] != m_board.m_pcCount[l])
            return (false);

        if(b.m_pcCount[l] != countBits(b.m_bitboards[l]))
            return false;

        if(m_board.m_pcCount[l] != countBits(m_board.m_bitboards[l]))
            return false;
        }

    // check hash
    if(b.m_hash != m_board.m_hash)
        return (false);

    if(b.m_hashPawn != m_board.m_hashPawn)
        return (false);

    return (true);
    }