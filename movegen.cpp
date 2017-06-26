#include "movegen.h"

MoveGen::MoveGen (const Board & board) :
    m_board(& board)
    {
    }

const Board & MoveGen::board (void) const
    {
    return * m_board;
    }

void MoveGen::getAll (MoveList & moves) const
    {
    genMoves(moves, true, true, FullBoard, FullBoard);
    }

void MoveGen::getCaptures (MoveList & moves) const
    {
    genMoves(moves, true, false, FullBoard, FullBoard);
    }

void MoveGen::getQuiets (MoveList & moves) const
    {
    genMoves(moves, false, true, FullBoard, FullBoard);
    }

void MoveGen::getEvasions (MoveList & moves) const
    {
    uint64 mask_evasion = EmptyBoard;
    Color side = board().getSide();

    uint64 x = board().getPieceBits(King, side);

    while(x)
        {
        Square kingpos = popFirstBit(x);
        uint64 attacks = board().getAttackBy(kingpos, toggleColor(side));

        mask_evasion |= attacks;

        while(attacks)
            {
            Square from = popFirstBit(attacks);
            mask_evasion |= gl_bitboards.getGap(from, kingpos);
            }
        }
    genMoves(moves, true, true, FullBoard, mask_evasion);
    }

void MoveGen::genMoves (MoveList & moves, bool captures, bool quiets, uint64 from, uint64 to) const
    {
    Color side = board().getSide();
    uint64 occup = board().getOccupBits();
    uint64 empty = board().getEmptyBits();

    uint64 mask_to_pawn_c = board().getColorBits(toggleColor(side)) & to;
    uint64 mask_to_pawn_q = empty & to;
    uint64 mask_to_pieces = EmptyBoard;

    if(captures)
        mask_to_pieces |= board().getColorBits(toggleColor(side));

    if(quiets)
        mask_to_pieces |= empty;
    mask_to_pieces &= to;
    uint64 x, y;

    uint64 pawns = board().getPieceBits(Pawn, side) & from;

    if(White == side)
        {
        if(captures)
            {
            // caps NoWe
            x = shift_SE(shift_NW(pawns & ~ Rank_7) & mask_to_pawn_c);

            while(x)
                moves.addDir(popFirstBit(x), NW);

            // caps NoEa
            x = shift_SW(shift_NE(pawns & ~ Rank_7) & mask_to_pawn_c);

            while(x)
                moves.addDir(popFirstBit(x), NE);

            // prom NoWe
            x = shift_SE(shift_NW(pawns & Rank_7) & mask_to_pawn_c);

            while(x)
                addPromotions(moves, popFirstBit(x), NW);

            // prom NoEa
            x = shift_SW(shift_NE(pawns & Rank_7) & mask_to_pawn_c);

            while(x)
                addPromotions(moves, popFirstBit(x), NE);

            // prom North
            x = shift_S(shift_N(pawns & Rank_7) & mask_to_pawn_q);

            while(x)
                addPromotions(moves, popFirstBit(x), N);

            // en-passant
            Square ep = board().getEp();

            if(noSquare != ep)
                {
                if(test_bit(shift_NW(pawns), ep))
                    moves.add(Square(ep - NW), ep, EnPassant);

                if(test_bit(shift_NE(pawns), ep))
                    moves.add(Square(ep - NE), ep, EnPassant);
                }
            }

        if(quiets)
            {
            // single step
            x = shift_S(shift_N(pawns & ~ Rank_7) & mask_to_pawn_q);

            while(x)
                moves.addDir(popFirstBit(x), N);

            // double step
            x = shift_N(shift_N(pawns & Rank_2) & empty) & mask_to_pawn_q;
            x = shift_S(shift_S(x));

            while(x)
                moves.addDir(popFirstBit(x), Dir(N + N));
            }
        }
    else
        {
        if(captures)
            {
            // caps SoWe
            x = shift_NE(shift_SW(pawns & ~ Rank_2) & mask_to_pawn_c);

            while(x)
                moves.addDir(popFirstBit(x), SW);

            // caps SoEa
            x = shift_NW(shift_SE(pawns & ~ Rank_2) & mask_to_pawn_c);

            while(x)
                moves.addDir(popFirstBit(x), SE);

            // prom SoWe
            x = shift_NE(shift_SW(pawns & Rank_2) & mask_to_pawn_c);

            while(x)
                addPromotions(moves, popFirstBit(x), SW);

            // prom SoEa
            x = shift_NW(shift_SE(pawns & Rank_2) & mask_to_pawn_c);

            while(x)
                addPromotions(moves, popFirstBit(x), SE);

            // prom South
            x = shift_N(shift_S(pawns & Rank_2) & mask_to_pawn_q);

            while(x)
                addPromotions(moves, popFirstBit(x), S);

            Square ep = board().getEp();

            if(noSquare != ep)
                {
                if(test_bit(shift_SW(pawns), ep))
                    moves.add(Square(ep - SW), ep, EnPassant);

                if(test_bit(shift_SE(pawns), ep))
                    moves.add(Square(ep - SE), ep, EnPassant);
                }
            }

        if(quiets)
            {
            // single step
            x = shift_N(shift_S(pawns & ~ Rank_2) & mask_to_pawn_q);

            while(x)
                moves.addDir(popFirstBit(x), S);

            // double step
            x = shift_S(shift_S(pawns & Rank_7) & empty) & mask_to_pawn_q;
            x = shift_N(shift_N(x));

            while(x)
                moves.addDir(popFirstBit(x), Dir(S + S));
            }
        }

    x = board().getPieceBits(Knight, side) & from;

    while(x)
        {
        Square from = popFirstBit(x);

        y = gl_bitboards.getAttacksN(from) & mask_to_pieces;

        while(y)
            moves.add(from, popFirstBit(y));
        }

    x = board().getPieceBits(Bishop, side) & from;

    while(x)
        {
        Square from = popFirstBit(x);

        y = gl_bitboards.getAttacksB(from, occup) & mask_to_pieces;

        while(y)
            moves.add(from, popFirstBit(y));
        }

    x = board().getPieceBits(Rook, side) & from;

    while(x)
        {
        Square from = popFirstBit(x);

        y = gl_bitboards.getAttacksR(from, occup) & mask_to_pieces;

        while(y)
            moves.add(from, popFirstBit(y));
        }

    x = board().getPieceBits(Queen, side) & from;

    while(x)
        {
        Square from = popFirstBit(x);

        y = gl_bitboards.getAttacksQ(from, occup) & mask_to_pieces;

        while(y)
            moves.add(from, popFirstBit(y));
        }

    x = board().getPieceBits(King, side) & from;

    while(x)
        {
        Square from = popFirstBit(x);

        y = gl_bitboards.getAttacksK(from) & mask_to_pieces;

        while(y)
            moves.add(from, popFirstBit(y));

        if(!board().isCheck() && quiets)
            {
            Square king_to = noSquare;

            if(White == side)
                {
                if(board().checkCastling(king_to, from, CastleWOO))
                    moves.add(from, king_to, Castle, CastleWOO);

                if(board().checkCastling(king_to, from, CastleWOOO))
                    moves.add(from, king_to, Castle, CastleWOOO);
                }

            if(Black == side)
                {
                if(board().checkCastling(king_to, from, CastleBOO))
                    moves.add(from, king_to, Castle, CastleBOO);

                if(board().checkCastling(king_to, from, CastleBOOO))
                    moves.add(from, king_to, Castle, CastleBOOO);
                }
            }
        }
    }

bool MoveGen::isLegal (const Move & move) const
    {
    Square from = move.getFrom();
    Piece pcFrom = board().getPiece(from);

    if((noPiece == pcFrom) || (getColor(pcFrom) != board().getSide()))
        return (false);

    Square to = move.getTo();
    Piece pcTo = board().getPiece(to);

    if((noPiece != pcTo) && (getColor(pcTo) == board().getSide()))
        return (false);

    MoveList moves;

    switch(move.getType())
        {
        case Normal:
            genMoves(moves, true, true, create_bit(from), create_bit(to));
            break;

        case Castle:
            genMoves(moves, false, true, create_bit(from), create_bit(to));
            break;

        case EnPassant:
            genMoves(moves, true, false, create_bit(from), create_bit(to));
            break;

        case Promotion:
            genMoves(moves, true, false, create_bit(from), create_bit(to));
            break;
        }

    for ( int32 i = 0; i < moves.size(); ++ i )
        if(move.getMoveData() == moves[i].getMoveData())
            return true;

    return (false);
    }

void MoveGen::addPromotions (MoveList & moves, Square from, Dir dir) const
    {
    moves.add(from, Square(from + dir), Promotion, PromQueen);
    moves.add(from, Square(from + dir), Promotion, PromKnight);
    moves.add(from, Square(from + dir), Promotion, PromBishop);
    moves.add(from, Square(from + dir), Promotion, PromRook);
    }