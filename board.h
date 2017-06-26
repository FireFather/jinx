#ifndef BOARD_H
#define BOARD_H

#include "move.h"
#include "eval.h"

const int32 maxPly = 512;

enum Castle
    {
    NoCastling = 0x00,
    WhiteCan_OO = 0x01,
    WhiteCan_OOO = 0x02,
    BlackCan_OO = 0x04,
    BlackCan_OOO = 0x08
    };

class Board
    {
    friend class Fen;
    friend class Perft;
    public:
    Board(void);
    public:
    uint64 getColorBits(Color color)const;
    uint64 getPieceBits(Piece piece)const;
    uint64 getPieceBits(PieceType piece, Color color)const;
    uint64 getPTypeBits(PieceType piece)const;
    uint64 getOccupBits(void)const;
    uint64 getEmptyBits(void)const;
    uint64 getAttackTo(Square field)const;
    uint64 getAttackBy(Square field)const;
    uint64 getAttackBy(Square field, Color color)const;
    bool isAttacked(Square field, Color color)const;
    bool isAttacking(Square from, Square to)const;

    Piece getPiece(Square field)const;
    int32 getMaterial(Color color)const;
    int32 getMatPhase(Color color)const;
    int32 getPieceCount(Piece piece)const;
    int32 getPieceCount(PieceType piece, Color color)const;
    int32 getMatCount(Color color)const;
    int32 getMatCount(void)const;

    int32 getPly(void)const;
    Color getSide(void)const;
    int32 getFifty(void)const;
    Square getEp(void)const;
    uint64 getHash(void)const;
    uint64 getHashPawn(void)const;

    bool isCheck(void)const;
    bool isDrawByRep(void)const;

    bool canCastle(CastInfo type)const;
    bool checkCastling(Square & king_to, Square king_pos, CastInfo type)const;
    Square getCastRookPos(CastInfo type)const;

    bool doMove(const Move & move);
    void undoMove(const Move & move);
    void doNullMove(void);
    void undoNullMove(void);
    void undoLastMove(void);

    void updatePieceDeps(void);
    private:
    uint64 m_bitboards[12]; // bitboards for pieces
    uint64 m_colorbits[2];  // bitboards for color
    Piece m_piece[64];      // classic board
    int32 m_material[2];    // material
    int32 m_matPhase[2];    // material-phase
    uint32 m_matCount[2];   // material-count
    uint32 m_pcCount[12];   // piece-count

    bool isCheck(Color color)const;
    void setPiece(Square field, Piece piece);
    void clearAll(void);
    void clearPieceDeps(void); // clear piece dependencies
    void initPieceDeps(void);  // init piece dependencies
    private:

    struct CCastInfo
        {
        Square m_kingFr;
        Square m_kingTo;
        Square m_rookFr;
        Square m_rookTo;
        uint64 m_notOccupied;
        uint64 m_notAttacked;
        string m_fenName;

        CCastInfo(void);
        void clear(void);
        void init(void);
        };

    CCastInfo m_cast_info[4];
    int32 m_cast_mask[64];

    void setCastling(CastInfo type, const string & fen_name);
    void setCastling(Square rook_from, const string & fen_name);
    bool castKingOnRook(const Move & move);
    private:
    int32 m_ply;
    Color m_side;
    int32 m_fifty;
    Square m_ep;
    int32 m_castle;
    uint64 m_hash;
    uint64 m_hashPawn;
    bool m_check;

    struct CStack
        {
        int32 m_fifty;
        Square m_ep;
        int32 m_castle;
        uint64 m_hash;
        uint64 m_hashPawn;
        Piece m_piece;
        Piece m_capture;
        bool m_check;
        uint32 m_move;

        CStack(void);
        void clear(void);
        };

    CStack m_stack[maxPly];
    };

__forceinline uint64 Board::getColorBits (Color color) const
    {
    return (m_colorbits[color]);
    }

__forceinline uint64 Board::getPieceBits (Piece piece) const
    {
    return (m_bitboards[piece]);
    }

__forceinline uint64 Board::getPieceBits (PieceType piece, Color color) const
    {
    return (m_bitboards[makePiece(piece, color)]);
    }

__forceinline uint64 Board::getPTypeBits (PieceType piece) const
    {
    return (m_bitboards[makePiece(piece, White)] | m_bitboards[makePiece(piece, Black)]);
    }

__forceinline uint64 Board::getOccupBits (void) const
    {
    return (m_colorbits[White] | m_colorbits[Black]);
    }

__forceinline uint64 Board::getEmptyBits (void) const
    {
    return ( ~getOccupBits());
    }

__forceinline Piece Board::getPiece (Square field) const
    {
    return (m_piece[field]);
    }

__forceinline int32 Board::getMaterial (Color color) const
    {
    return (m_material[color]);
    }

__forceinline int32 Board::getMatPhase (Color color) const
    {
    return (m_matPhase[color]);
    }

__forceinline int32 Board::getPieceCount (Piece piece) const
    {
    return (m_pcCount[piece]);
    }

__forceinline int32 Board::getPieceCount (PieceType piece, Color color) const
    {
    return (m_pcCount[makePiece(piece, color)]);
    }

__forceinline int32 Board::getMatCount (Color color) const
    {
    return (m_matCount[color]);
    }

__forceinline int32 Board::getMatCount (void) const
    {
    return getMatCount(White) + getMatCount(Black);
    }

__forceinline int32 Board::getPly (void) const
    {
    return (m_ply);
    }

__forceinline Color Board::getSide (void) const
    {
    return (m_side);
    }

__forceinline int32 Board::getFifty (void) const
    {
    return (m_fifty);
    }

__forceinline Square Board::getEp (void) const
    {
    return (m_ep);
    }

__forceinline uint64 Board::getHash (void) const
    {
    return (m_hash ^ m_castle ^ m_ep);
    }

__forceinline uint64 Board::getHashPawn (void) const
    {
    return (m_hashPawn);
    }

__forceinline bool Board::isCheck (void) const
    {
    return (m_check);
    }

__forceinline bool Board::isCheck (Color color) const
    {
    uint64 x = getPieceBits(King, color);

    while(x)
        if(isAttacked(popFirstBit(x), toggleColor(color)))
            return true;

    return false;
    }

#endif