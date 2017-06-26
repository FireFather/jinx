#include "define.h"
#include "bitops.h"
#include "board.h"
#include "fen.h"

Board::Board (void)
    {
    // set initial position
    Fen fen(* this);
    fen.set();
    }

bool Board::isDrawByRep (void) const
    {
    if(0 < m_ply)
        {
        int32 count = 1;

        for ( int32 i = m_ply - 1; i >= 0; -- i )
            {
            const CStack & undo = m_stack[i];

            if(undo.m_hash == m_hash)
                ++ count;

            if(0 == undo.m_fifty)
                break;
            }

        if(2 <= count)
            return true;
        }
    return false;
    }

uint64 Board::getAttackTo (Square field) const
    {
    switch(m_piece[field])
        {
        case WP:
            return (gl_bitboards.getAttacksP(field, White));

        case WN:
            return (gl_bitboards.getAttacksN(field));

        case WB:
            return (gl_bitboards.getAttacksB(field, getOccupBits()));

        case WR:
            return (gl_bitboards.getAttacksR(field, getOccupBits()));

        case WQ:
            return (gl_bitboards.getAttacksQ(field, getOccupBits()));

        case WK:
            return (gl_bitboards.getAttacksK(field));

        case BP:
            return (gl_bitboards.getAttacksP(field, Black));

        case BN:
            return (gl_bitboards.getAttacksN(field));

        case BB:
            return (gl_bitboards.getAttacksB(field, getOccupBits()));

        case BR:
            return (gl_bitboards.getAttacksR(field, getOccupBits()));

        case BQ:
            return (gl_bitboards.getAttacksQ(field, getOccupBits()));

        case BK:
            return (gl_bitboards.getAttacksK(field));

        case noPiece:
            return (EmptyBoard);
        }
    return (EmptyBoard);
    }

uint64 Board::getAttackBy (Square field) const
    {
    uint64 att = EmptyBoard;
    att |= gl_bitboards.getAttacksP(field, White) & getPieceBits(BP);
    att |= gl_bitboards.getAttacksP(field, Black) & getPieceBits(WP);
    att |= gl_bitboards.getAttacksN(field) & getPTypeBits(Knight);
    att |= gl_bitboards.getAttacksK(field) & getPTypeBits(King);
    att |= gl_bitboards.getAttacksB(field, getOccupBits()) & (getPTypeBits(Bishop) | getPTypeBits(Queen));
    att |= gl_bitboards.getAttacksR(field, getOccupBits()) & (getPTypeBits(Rook) | getPTypeBits(Queen));
    return (att);
    }

uint64 Board::getAttackBy (Square field, Color color) const
    {
    uint64 att = EmptyBoard;

    if(Black == color)
        {
        att |= gl_bitboards.getAttacksP(field, White) & getPieceBits(BP);
        att |= gl_bitboards.getAttacksN(field) & getPieceBits(BN);
        att |= gl_bitboards.getAttacksK(field) & getPieceBits(BK);
        att |= gl_bitboards.getAttacksB(field, getOccupBits()) & (getPieceBits(BB) | getPieceBits(BQ));
        att |= gl_bitboards.getAttacksR(field, getOccupBits()) & (getPieceBits(BR) | getPieceBits(BQ));
        }
    else
        {
        att |= gl_bitboards.getAttacksP(field, Black) & getPieceBits(WP);
        att |= gl_bitboards.getAttacksN(field) & getPieceBits(WN);
        att |= gl_bitboards.getAttacksK(field) & getPieceBits(WK);
        att |= gl_bitboards.getAttacksB(field, getOccupBits()) & (getPieceBits(WB) | getPieceBits(WQ));
        att |= gl_bitboards.getAttacksR(field, getOccupBits()) & (getPieceBits(WR) | getPieceBits(WQ));
        }
    return (att);
    }

bool Board::isAttacked (Square field, Color color) const
    {
    if(Black == color)
        {
        if(gl_bitboards.getAttacksP(field, White) & getPieceBits(BP))
            return (true);

        if(gl_bitboards.getAttacksN(field) & getPieceBits(BN))
            return (true);

        if(gl_bitboards.getAttacksK(field) & getPieceBits(BK))
            return (true);

        uint64 occ = getOccupBits();

        uint64 x = gl_bitboards.getAttacksB(field) & (getPieceBits(BB) | getPieceBits(BQ));

        while(x)
            {
            Square from = popFirstBit(x);

            if(0 == (gl_bitboards.getGap(from, field) & occ))
                return true;
            }

        x = gl_bitboards.getAttacksR(field) & (getPieceBits(BR) | getPieceBits(BQ));

        while(x)
            {
            Square from = popFirstBit(x);

            if(0 == (gl_bitboards.getGap(from, field) & occ))
                return true;
            }
        }
    else
        {
        if(gl_bitboards.getAttacksP(field, Black) & getPieceBits(WP))
            return (true);

        if(gl_bitboards.getAttacksN(field) & getPieceBits(WN))
            return (true);

        if(gl_bitboards.getAttacksK(field) & getPieceBits(WK))
            return (true);

        uint64 occ = getOccupBits();

        uint64 x = gl_bitboards.getAttacksB(field) & (getPieceBits(WB) | getPieceBits(WQ));

        while(x)
            {
            Square from = popFirstBit(x);

            if(0 == (gl_bitboards.getGap(from, field) & occ))
                return true;
            }

        x = gl_bitboards.getAttacksR(field) & (getPieceBits(WR) | getPieceBits(WQ));

        while(x)
            {
            Square from = popFirstBit(x);

            if(0 == (gl_bitboards.getGap(from, field) & occ))
                return true;
            }
        }

    return (false);
    }

bool Board::isAttacking (Square from, Square to) const
    {
    return (0 != (getAttackTo(from) & create_bit(to)));
    }

bool Board::doMove (const Move & move)
    {
    Square from = move.getFrom();
    Square to = move.getTo();
    Piece piece = getPiece(from);
    Piece capture = getPiece(to);
    Color side = getColor(piece);
    bool kingOnRook = castKingOnRook(move);
    bool kingOnKing = (from == to);

    if(kingOnRook || kingOnKing)
        capture = noPiece;

    CStack & currPos = m_stack[m_ply++];
    currPos.m_fifty = m_fifty;
    currPos.m_ep = m_ep;
    currPos.m_castle = m_castle;
    currPos.m_hash = m_hash;
    currPos.m_hashPawn = m_hashPawn;
    currPos.m_piece = piece;
    currPos.m_capture = capture;
    currPos.m_check = m_check;
    currPos.m_move = move;

    ++ m_fifty;
    m_side = toggleColor(m_side);
    m_hash ^= gl_hashkeys.getKeyColor();
    m_castle &= m_cast_mask[from] & m_cast_mask[to];
    m_ep = noSquare;

    if(!kingOnKing)
        {
        m_colorbits[side] ^= create_bit(from) | create_bit(to);
        m_bitboards[piece] ^= create_bit(from) | create_bit(to);
        m_hash ^= gl_hashkeys.getKey(from, piece) ^ gl_hashkeys.getKey(to, piece);
        }

    if(Pawn == getPieceType(piece))
        {
        m_fifty = 0;
        m_hashPawn ^= gl_hashkeys.getKey(from, piece) ^ gl_hashkeys.getKey(to, piece);

        if(DoublePawnStep == abs(to - from))
            {
            m_ep = Square(from + (White == side ? N : S));

            if(0 == (gl_bitboards.getAttacksP(m_ep, side) & getPieceBits(Pawn, m_side)))
                m_ep = noSquare;
            }
        }

    if(noPiece != capture)
        {
        m_fifty = 0;
        Color color = getColor(capture);

        m_colorbits[color] ^= create_bit(to);
        m_bitboards[capture] ^= create_bit(to);
        m_hash ^= gl_hashkeys.getKey(to, capture);

        if(Pawn == getPieceType(capture))
            m_hashPawn ^= gl_hashkeys.getKey(to, capture);

        m_material[color] -= gl_pvt.getMatValue(capture);
        m_matPhase[color] -= gl_pvt.getMatPhase(capture);
        -- m_matCount[color];
        -- m_pcCount[capture];
        }

    m_piece[from] = noPiece;
    m_piece[to] = piece;

    switch(move.getType())
        {
        case Normal:
            break;

        case Castle:
            {
            // calc squares for rook
            CCastInfo & info = m_cast_info[CastInfo(move.getInfo())];
            Square f = info.m_rookFr;
            Square t = info.m_rookTo;

            if(f != t)
                {
                Piece pc = makePiece(Rook, side);
                // do move for rook
                m_colorbits[side] ^= create_bit(f) | create_bit(t);
                m_bitboards[pc] ^= create_bit(f) | create_bit(t);
                m_hash ^= gl_hashkeys.getKey(f, pc) ^ gl_hashkeys.getKey(t, pc);

                if(!kingOnRook)
                    m_piece[f] = noPiece;
                m_piece[t] = pc;
                }
            }
            break;

        case EnPassant:
            {
            // clear square for "En Passant"
            Square ep = Square(to + (White == side ? S : N));
            Piece capt = (White == side ? BP : WP);
            Color color = getColor(capt);

            m_colorbits[color] ^= create_bit(ep);
            m_bitboards[capt] ^= create_bit(ep);
            m_hash ^= gl_hashkeys.getKey(ep, capt);
            m_hashPawn ^= gl_hashkeys.getKey(ep, capt);
            m_piece[ep] = noPiece;

            m_material[color] -= gl_pvt.getMatValue(capt);
            m_matPhase[color] -= gl_pvt.getMatPhase(capt);
            -- m_matCount[color];
            -- m_pcCount[capt];
            }
            break;

        case Promotion:
            {
            Piece prom = makePiece(getPromPiece(PromInfo(move.getInfo())), side);
            Piece pawn = makePiece(Pawn, side);

            m_bitboards[piece] ^= create_bit(to);
            m_bitboards[prom] ^= create_bit(to);
            m_hash ^= gl_hashkeys.getKey(to, piece) ^ gl_hashkeys.getKey(to, prom);
            m_hashPawn ^= gl_hashkeys.getKey(to, piece);
            m_piece[to] = prom;

            m_material[side] += (gl_pvt.getMatValue(prom) - gl_pvt.getMatValue(pawn));
            m_matPhase[side] += (gl_pvt.getMatPhase(prom) - gl_pvt.getMatPhase(pawn));
            -- m_pcCount[pawn];
            ++ m_pcCount[prom];
            }
            break;
        }

    if(isCheck(side))
        {
        undoMove(move);
        return (false);
        }
    m_check = isCheck(m_side);

    return (true);
    }

void Board::undoMove (const Move & move)
    {
    if(0 < m_ply)
        {
        CStack & currPos = m_stack[-- m_ply];
        m_fifty = currPos.m_fifty;
        m_ep = currPos.m_ep;
        m_castle = currPos.m_castle;
        m_hash = currPos.m_hash;
        m_hashPawn = currPos.m_hashPawn;
        Piece piece = currPos.m_piece;
        Piece capture = currPos.m_capture;
        m_check = currPos.m_check;

        m_side = toggleColor(m_side);

        Square from = move.getFrom();
        Square to = move.getTo();
        Color side = getColor(piece);
        bool kingOnKing = (from == to);

        if(!kingOnKing)
            {
            m_colorbits[side] ^= create_bit(from) | create_bit(to);
            m_bitboards[piece] ^= create_bit(from) | create_bit(to);
            }

        m_piece[from] = piece;

        if(!kingOnKing)
            m_piece[to] = noPiece;

        if(noPiece != capture)
            {
            Color color = getColor(capture);
            m_colorbits[color] ^= create_bit(to);
            m_bitboards[capture] ^= create_bit(to);
            m_piece[to] = capture;

            m_material[color] += gl_pvt.getMatValue(capture);
            m_matPhase[color] += gl_pvt.getMatPhase(capture);
            ++ m_matCount[color];
            ++ m_pcCount[capture];
            }

        switch(move.getType())
            {
            case Normal:
                break;

            case Castle:
                {
                // calc squares for rook
                CCastInfo & info = m_cast_info[CastInfo(move.getInfo())];
                Square f = info.m_rookFr;
                Square t = info.m_rookTo;

                if(f != t)
                    {
                    Piece pc = makePiece(Rook, side);
                    // undo move for rook
                    m_colorbits[side] ^= create_bit(f) | create_bit(t);
                    m_bitboards[pc] ^= create_bit(f) | create_bit(t);
                    m_piece[f] = pc;

                    if(t != info.m_kingFr)
                        m_piece[t] = noPiece;
                    }
                }
                break;

            case EnPassant:
                {
                // set a pawn to square of "En Passant"
                Square ep = Square(to + (White == side ? S : N));
                Piece capt = (White == side ? BP : WP);
                Color color = getColor(capt);

                m_colorbits[color] ^= create_bit(ep);
                m_bitboards[capt] ^= create_bit(ep);
                m_piece[ep] = capt;

                m_material[color] += gl_pvt.getMatValue(capt);
                m_matPhase[color] += gl_pvt.getMatPhase(capt);
                ++ m_matCount[color];
                ++ m_pcCount[capt];
                }
                break;

            case Promotion:
                {
                Piece prom = makePiece(getPromPiece(PromInfo(move.getInfo())), side);
                Piece pawn = makePiece(Pawn, side);

                m_bitboards[piece] ^= create_bit(to);
                m_bitboards[prom] ^= create_bit(to);

                m_material[side] -= (gl_pvt.getMatValue(prom) - gl_pvt.getMatValue(pawn));
                m_matPhase[side] -= (gl_pvt.getMatPhase(prom) - gl_pvt.getMatPhase(pawn));
                ++ m_pcCount[pawn];
                -- m_pcCount[prom];
                }
                break;
            }
        }
    }

void Board::doNullMove (void)
    {
    CStack & currPos = m_stack[m_ply++];
    currPos.m_fifty = m_fifty;
    currPos.m_ep = m_ep;
    currPos.m_castle = m_castle;
    //currPos.m_hash = m_hash;
    currPos.m_piece = noPiece;
    currPos.m_capture = noPiece;

    m_fifty = 0;
    m_ep = noSquare;

    m_side = toggleColor(m_side);
    m_hash ^= gl_hashkeys.getKeyColor();
    }

void Board::undoNullMove (void)
    {
    if(0 < m_ply)
        {
        CStack & currPos = m_stack[-- m_ply];
        m_fifty = currPos.m_fifty;
        m_ep = currPos.m_ep;
        m_castle = currPos.m_castle;

        m_side = toggleColor(m_side);
        m_hash ^= gl_hashkeys.getKeyColor();
        }
    }

void Board::undoLastMove (void)
    {
    if(1 < m_ply)
        undoMove(m_stack[m_ply - 1].m_move);
    }

void Board::updatePieceDeps (void)
    {
    clearPieceDeps();
    initPieceDeps();
    }

void Board::setPiece (Square field, Piece piece)
    {
    m_piece[field] = piece;
    }

void Board::clearAll (void)
    {
    for ( int32 f = 0; f < 64; ++ f )
        {
        m_piece[f] = noPiece;
        m_cast_mask[f] = WhiteCan_OO | WhiteCan_OOO | BlackCan_OO | BlackCan_OOO;
        }

    for ( int32 c = CastleWOO; c <= CastleBOOO; ++ c )
        m_cast_info[c].clear();

    m_ply = 1;
    m_side = White;
    m_fifty = 0;
    m_ep = noSquare;
    m_castle = NoCastling;
    m_check = false;

    clearPieceDeps();
    }

void Board::clearPieceDeps (void)
    {
    for ( int32 b = 0; b < 12; ++ b )
        {
        m_bitboards[b] = EmptyBoard;
        m_pcCount[b] = 0;
        }

    m_colorbits[White] = m_colorbits[Black] = EmptyBoard;
    m_matCount[White] = m_matCount[Black] = 0;
    m_material[White] = m_material[Black] = 0;
    m_matPhase[White] = m_matPhase[Black] = 0;

    m_hash = 0;
    m_hashPawn = 0;
    }

void Board::initPieceDeps (void)
    {
    for ( int32 f = 0; f < 64; ++ f )
        {
        Square field = Square(f);
        Piece piece = m_piece[field];

        if(noPiece != piece)
            {
            Color color = getColor(piece);

            m_colorbits[color] ^= create_bit(field);
            m_bitboards[piece] ^= create_bit(field);

            m_hash ^= gl_hashkeys.getKey(field, piece);

            if(Pawn == getPieceType(piece))
                m_hashPawn ^= gl_hashkeys.getKey(field, piece);

            m_material[color] += gl_pvt.getMatValue(piece);
            m_matPhase[color] += gl_pvt.getMatPhase(piece);
            ++ m_matCount[color];
            ++ m_pcCount[piece];
            }
        }

    m_check = isCheck(m_side);
    }

void Board::setCastling (CastInfo type, const string & fen_name)
    {
    Color color = (CastleWOO == type || CastleWOOO == type) ? White : Black;
    int8 rank = (color == White) ? 0 : 7;
    Square king = noSquare;
    Square rook_min = noSquare;
    Square rook_max = noSquare;

    for ( int32 f = 0; f < 64; ++ f )
        {
        Square field = Square(f);
        Piece piece = m_piece[field];

        if(rank == getRank(field))
            {
            if(makePiece(King, color) == piece)
                king = field;

            if(makePiece(Rook, color) == piece)
                {
                if((noSquare == rook_min) || (getFile(field) < getFile(rook_min)))
                    rook_min = field;

                if((noSquare == rook_max) || (getFile(field) > getFile(rook_max)))
                    rook_max = field;
                }
            }
        }

    if(CastleWOO == type || CastleBOO == type)
        {
        if((noSquare != king) && (noSquare != rook_max) && (getFile(king) < getFile(rook_max)))
            setCastling(rook_max, fen_name);
        }
    else
        {
        if((noSquare != king) && (noSquare != rook_min) && (getFile(king) > getFile(rook_min)))
            setCastling(rook_min, fen_name);
        }
    }

void Board::setCastling (Square rook_from, const string & fen_name)
    {
    if(Rank1 == getRank(rook_from)) // white ?
        {
        Square king_pos = noSquare;

        for ( int32 f = 0; f < 64; ++ f )
            {
            Square field = Square(f);

            if(WK == m_piece[field])
                {
                king_pos = field;
                break;
                }
            }

        if(Rank1 == getRank(king_pos))
            {
            m_cast_mask[king_pos] = BlackCan_OO | BlackCan_OOO;

            if(getFile(rook_from) > (getFile(king_pos)))
                {
                CCastInfo & info = m_cast_info[CastleWOO];
                info.m_kingFr = king_pos;
                info.m_kingTo = G1;
                info.m_rookFr = rook_from;
                info.m_rookTo = F1;
                info.m_fenName = fen_name;
                info.init();

                m_cast_mask[rook_from] = BlackCan_OO | BlackCan_OOO | WhiteCan_OOO;
                m_castle |= WhiteCan_OO;
                }

            if(getFile(rook_from) < (getFile(king_pos)))
                {
                CCastInfo & info = m_cast_info[CastleWOOO];
                info.m_kingFr = king_pos;
                info.m_kingTo = C1;
                info.m_rookFr = rook_from;
                info.m_rookTo = D1;
                info.m_fenName = fen_name;
                info.init();

                m_cast_mask[rook_from] = BlackCan_OO | BlackCan_OOO | WhiteCan_OO;
                m_castle |= WhiteCan_OOO;
                }
            }
        }

    if(Rank8 == getRank(rook_from)) // black ?
        {
        Square king_pos = noSquare;

        for ( int32 f = 0; f < 64; ++ f )
            {
            Square field = Square(f);

            if(BK == m_piece[field])
                {
                king_pos = field;
                break;
                }
            }

        if(Rank8 == getRank(king_pos))
            {
            m_cast_mask[king_pos] = WhiteCan_OO | WhiteCan_OOO;

            if(getFile(rook_from) > (getFile(king_pos)))
                {
                CCastInfo & info = m_cast_info[CastleBOO];
                info.m_kingFr = king_pos;
                info.m_kingTo = G8;
                info.m_rookFr = rook_from;
                info.m_rookTo = F8;
                info.m_fenName = fen_name;
                info.init();

                m_cast_mask[rook_from] = WhiteCan_OO | WhiteCan_OOO | BlackCan_OOO;
                m_castle |= BlackCan_OO;
                }

            if(getFile(rook_from) < (getFile(king_pos)))
                {
                CCastInfo & info = m_cast_info[CastleBOOO];
                info.m_kingFr = king_pos;
                info.m_kingTo = C8;
                info.m_rookFr = rook_from;
                info.m_rookTo = D8;
                info.m_fenName = fen_name;
                info.init();

                m_cast_mask[rook_from] = WhiteCan_OO | WhiteCan_OOO | BlackCan_OO;
                m_castle |= BlackCan_OOO;
                }
            }
        }
    }

bool Board::canCastle (CastInfo type) const
    {
    switch(type)
        {
        case CastleWOO:
            return (0 != (m_castle & WhiteCan_OO));

        case CastleWOOO:
            return (0 != (m_castle & WhiteCan_OOO));

        case CastleBOO:
            return (0 != (m_castle & BlackCan_OO));

        case CastleBOOO:
            return (0 != (m_castle & BlackCan_OOO));
        }
    return (false);
    }

bool Board::checkCastling (Square & king_to, Square king_pos, CastInfo type) const
    {
    if((m_cast_info[type].m_kingFr == king_pos) && canCastle(type))
        {
        if(!(m_cast_info[type].m_notOccupied & getOccupBits()))
            {
            Color opp_color = toggleColor(getColor(getPiece(king_pos)));
            bool attacks = false;
            uint64 x = m_cast_info[type].m_notAttacked;

            while(x)
                {
                if(isAttacked(popFirstBit(x), opp_color))
                    {
                    attacks = true;
                    break;
                    }
                }

            if(!attacks)
                {
                king_to = m_cast_info[type].m_kingTo;
                return (true);
                }
            }
        }
    return (false);
    }

Square Board::getCastRookPos (CastInfo type) const
    {
    return (m_cast_info[type].m_rookFr);
    }

Board::CCastInfo::CCastInfo (void)
    {
    clear();
    }

void Board::CCastInfo::clear (void)
    {
    m_kingFr = noSquare;
    m_kingTo = noSquare;
    m_rookFr = noSquare;
    m_rookTo = noSquare;
    m_notOccupied = EmptyBoard;
    m_notAttacked = EmptyBoard;
    m_fenName = "";
    }

void Board::CCastInfo::init (void)
    {
    uint64 king = gl_bitboards.getGap(m_kingFr, m_kingTo) | create_bit(m_kingTo);
    uint64 rook = gl_bitboards.getGap(m_rookFr, m_rookTo) | create_bit(m_rookTo);

    uint64 free = king | rook;
    del_bits(free, create_bit(m_kingFr) | create_bit(m_rookFr));

    m_notOccupied = free;
    m_notAttacked = king;
    }

Board::CStack::CStack (void)
    {
    clear();
    }

void Board::CStack::clear (void)
    {
    m_fifty = 0;
    m_ep = noSquare;
    m_castle = NoCastling;
    m_hash = 0;
    m_hashPawn = 0;
    m_piece = noPiece;
    m_capture = noPiece;
    m_check = false;
    }

bool Board::castKingOnRook (const Move & move)
    {
    if(Castle == move.getType())
        {
        CCastInfo & info = m_cast_info[CastInfo(move.getInfo())];
        return (info.m_kingTo == info.m_rookFr);
        }
    return (false);
    }