#ifndef EVAL_H
#define EVAL_H

#include "bitboard.h"
#include "hash.h"

class Board;

// states
const int32 mateValue = 32000;
const int32 drawValue = 0;

// values for pieces
const int32 pawnValue = 100;
const int32 knightValue = 400;
const int32 bishopValue = 400;
const int32 rookValue = 600;
const int32 queenValue = 1200;
const int32 kingValue = 10000;

// games phase
const int32 matPhase[6] =
    {
    0, 1, 1, 2, 4, 0
    };

const int32 maxMatPhase = 24;

const int32 egDrawEval = 100;
const int32 egMaxMen = 6;

// lazy eval
const int32 lazyEvalMargin = 200;

// passed pawn values
const int32 passedPawnFreeMg[2][8] =
    {
    { 0, 2, 10, 20, 40, 60, 90, 120 },
    { 120, 90, 60, 40, 20, 10, 2, 0 }
    };

const int32 passedPawnFreeEg[2][8] =
    {
    { 0, 2, 10, 20, 40, 60, 90, 120 },
    { 120, 90, 60, 40, 20, 10, 2, 0 }
    };

const int32 passedPawnBlockedMg[2][8] =
    {
    { 0, 2, 8, 20, 30, 50, 70, 100 },
    { 100, 70, 50, 30, 20, 8, 2, 0 }
    };

const int32 passedPawnBlockedEg[2][8] =
    {
    { 0, 2, 8, 20, 30, 50, 70, 100 },
    { 100, 70, 50, 30, 20, 8, 2, 0 }
    };

const int32 passedPawnUnstoppableMg[2][8] =
    {
    { 0, 2, 10, 20, 40, 60, 90, 120 },
    { 120, 90, 60, 40, 20, 10, 2, 0 }
    };

const int32 passedPawnUnstoppableEg[2][8] =
    {
    { 0, 2, 10, 20, 40, 60, 90, 120 },
    { 120, 90, 60, 40, 20, 10, 2, 0 }
    };

// positional bonus/penalty

const int32 doubledPawn_mg[2] = { -16, -16 };
const int32 doubledPawn_eg[2] = { -16, -16 };

const int32 backwardPawn_mg[2] = { -4, -4 };
const int32 backwardPawn_eg[2] = { -4, -4 };

const int32 isolatedPawn_mg[2] = { -8, -8 };
const int32 isolatedPawn_eg[2] = { -8, -8 };

const int32 pawnShieldMe = - 12;
const int32 pawnShieldOpp = - 6;

const int32 bishopPair = 50;
const int32 rookOpenFile = 8;
const int32 rookHalfOpenFile = 4;
const int32 queenKingTropism = - 1;
const int32 outpostMg = 10;
const int32 outpostEg = 10;

// bonuses for endgames
const int32 squareBonus[8] =
    {
    120, 105, 90, 70, 50, 30, 10, 0
    };

const int32 borderBonus[8] =
    {
    60, 50, 40, 30, 20, 10, 5, 0
    };

const int32 cornerBonus[8] =
    {
    60, 50, 40, 30, 20, 10, 5, 0
    };

#define CNT_LESS(pt) board().getPieceCount((pt), getLessSide())
#define CNT_MORE(pt) board().getPieceCount((pt), toggleColor(getLessSide()))
#define IS_5MEN_12(pt1, pt2) (1 == (CNT_LESS((pt1))) && (2 == (CNT_MORE(pt2))))
#define IS_6MEN_13(pt1, pt2) (1 == (CNT_LESS((pt1))) && (3 == (CNT_MORE(pt2))))

class EvalPst
    {
    public:

    EvalPst(void);
    int32 getMG(Piece pc, Square fld);
    int32 getEG(Piece pc, Square fld);
    private:
    int32 m_pstMG[12][64];
    int32 m_pstEG[12][64];
    void init(void);
    void init(Piece pc, const int32 * pTable_mg, const int32 * pTable_eg);
    };

__forceinline int32 EvalPst::getMG (Piece pc, Square fld)
    {
    return (m_pstMG[pc][fld]);
    }

__forceinline int32 EvalPst::getEG (Piece pc, Square fld)
    {
    return (m_pstEG[pc][fld]);
    }

extern EvalPst gl_pst;

const int32 Outpost[64] =
    {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
    };

class EvalOutpost
    {
    public:

    EvalOutpost(void);
    bool isWhite(Square fld);
    bool isBlack(Square fld);
    private:
    uint64 m_outpost_w;
    uint64 m_outpost_b;
    void init(void);
    };

__forceinline bool EvalOutpost::isWhite (Square fld)
    {
    return (test_bit(m_outpost_w, fld));
    }

__forceinline bool EvalOutpost::isBlack (Square fld)
    {
    return (test_bit(m_outpost_b, fld));
    }

extern EvalOutpost gl_outpost;

struct EvalPvt
    {
    EvalPvt();
    void setMatValue(PieceType pt, int32 value);

    int32 getMatValue(Piece pc);
    int32 getMatValue(PieceType pt);
    int32 getMatPhase(Piece pc);
    int32 getMatPhase(PieceType pt);
    private:
    int32 m_matvalue[6];
    };

__forceinline void EvalPvt::setMatValue (PieceType pt, int32 value)
    {
    m_matvalue[pt] = value;
    }

__forceinline int32 EvalPvt::getMatValue (Piece pc)
    {
    if(noPiece == pc)
        return 0;
    return (m_matvalue[getPieceType(pc)]);
    }

__forceinline int32 EvalPvt::getMatValue (PieceType pt)
    {
    return (m_matvalue[pt]);
    }

__forceinline int32 EvalPvt::getMatPhase (Piece pc)
    {
    if(noPiece == pc)
        return 0;
    return (matPhase[getPieceType(pc)]);
    }

__forceinline int32 EvalPvt::getMatPhase (PieceType pt)
    {
    return (matPhase[pt]);
    }

extern EvalPvt gl_pvt;

#define MAX_INDEX (64 * 64 * 24 * 2) // wk * bk * p * stm

class EvalKPK
    {
    public:
    EvalKPK(void);

    bool probe(Square wk, Square bk, Square p, Piece pp, Color stm);
    private:
    typedef int32 index_t;

    enum EResult
        {
        Illegal = 0,
        Unknown = 1,
        Draw = 2,
        Win = 4
        };
    uint64 m_bitbase[MAX_INDEX / 64];
    void init();
    index_t encode(Square wk, Square bk, Square p, Color stm);
    void decode(index_t idx, Square & wk, Square & bk, Square & p, Color & stm);
    EResult rules(index_t idx);
    EResult classify(EResult res [], index_t idx);
    bool kpk_ok(EResult res []);
    };

extern EvalKPK gl_kpk;

class CEvalEgHits
    {
    public:
    CEvalEgHits(void);

    uint64 hits(void)const;
    void hit(void);
    void clear(void);
    private:
    uint64 m_hits;
    };

extern CEvalEgHits gl_egHits;

class Eval
    {
    public:
    Eval(const Board & board);

    int32 getEval(int32 alpha = - mateValue, int32 beta = mateValue);
    bool isDrawByMat(void)const;
    int32 getPhaseEval(const int32 & mg, const int32 & eg);
    private:
    const Board * m_board;
    const Board & board(void)const;
    PawnHashItem m_pawnHash;
    PawnHashItem * getPawnHash(void);
    int32 getEvalMat(void)const;
    int32 evalMaterial(Color side)const;
    int32 evalContemptFactor()const;
    int32 evalPawnFile(int8 rank_white, int8 rank_black, Color side);
    int8 getRank(Square field);
    int8 getFile(Square field);
    bool onlyPawnsLeft(Color side)const;
    private:
    bool evalEndGame(int32 & eval);
    bool adjustEval(int32 & eval);
    Color getLessSide(void);
    Color getWeakSide(void);
    Square getKingPos(Color side);
    Square getWeakKingSq(void);
    Square getStrongKingSq(void);
    Square getPieceSq(PieceType pt);
    Color getPieceColor(PieceType pt);
    Color getFieldColor(Square sq);
    Square getPromSq(Square sq);

    int32 getDistSquares(Square sq1, Square sq2);
    int32 getDistBorder(Square sq);
    int32 getDistBorder(Square sq, Color color);
    int32 getDistCorner(Square sq);
    int32 getDistCorner(Square sq, Color color);

    int32 matBalance(void);
    int32 nearSquares(Square sq1, Square sq2);
    int32 awaySquares(Square sq1, Square sq2);
    int32 nearBorder(Square sq);
    int32 awayBorder(Square sq);
    int32 nearBorder(Square sq, Color color);
    int32 awayBorder(Square sq, Color color);
    int32 nearCorner(Square sq);
    int32 awayCorner(Square sq);
    int32 nearCorner(Square sq, Color color);
    int32 awayCorner(Square sq, Color color);
    int32 nearKings(void);
    int32 movePawns(Color side);
    int32 stopPawns(Color side);

    // X = major piece (Q or R)
    // x = minor piece (B or N)
    // Ps = multi pawns
    bool KXK(int32 & eval);
    bool KPK(int32 & eval);
    bool KXPK(int32 & eval);
    bool KXKx(int32 & eval);
    bool KBBK(int32 & eval);
    bool KBNK(int32 & eval);
    bool KBPK(int32 & eval);
    bool KBKP(int32 & eval);
    bool KNKP(int32 & eval);
    bool KRKB(int32 & eval);
    bool KRKN(int32 & eval);
    bool KRKP(int32 & eval);
    bool KBKPs(int32 & eval);
    bool KNKPs(int32 & eval);
    };

// mobility
static int32 mobilityEgKnight[9] =
    {
    - 60, - 15, - 10, - 5, - 5, - 5, - 10, - 15, - 5
    };

static int32 mobilityMgKnight[9] =
    {
    - 15, - 15, - 5, - 5, 0, 0, - 5, - 5, - 15
    };

static int32 mobilityEgBishop[14] =
    {
    - 20, - 20, - 30, - 25, - 15, - 5, 0, 0, 5, 10, 0, 5, 5, 5
    };

static int32 mobilityMgBishop[14] =
    {
    - 15, - 15, - 15, - 10, - 5, 0, 5, 5, 10, 0, 5, 5, 5, 15
    };

static int32 mobilityEgRook[15] =
    {
    - 57, - 29, - 29, - 16, - 7, - 2, 5, 6, 13, 18, 21, 25, 27, 17, 7
    };

static int32 mobilityMgRook[15] =
    {
    - 27, - 21, - 19, - 17, - 18, - 12, - 10, - 3, - 1, 2, 6, 8, 8, 21, 31
    };

static int32 mobilityEgQueen[28] =
    {
    1, -24, -19, -21, -39, -36, -26, -23, -20, -26, -16, -7, 0, 1, 2,
	4, 5, 6, 3, 1, -8, -21, -26, -39, -28, -38, -23, -45
    };

static int32 mobilityMgQueen[28] =
    {
    -1, -7, -10, -14, -10, -8, -6, -6, -3, -1, 0, 0, 0, 1,
	2, 3, 5, 5, 9, 9, 20, 30, 32, 46, 33, 35, 11, 3
    };

#endif