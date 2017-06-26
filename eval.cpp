#include "define.h"
#include "bitops.h"
#include "eval.h"
#include "pst.h"
#include "uci.h"

EvalPst gl_pst;
EvalPvt gl_pvt;
EvalOutpost gl_outpost;
EvalKPK gl_kpk;
CEvalEgHits gl_egHits;

Square getEvalField (Color color, int32 idx)
    {
    static const Square flip_w[64] =
        {
        A8, B8, C8, D8, E8, F8, G8, H8,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A1, B1, C1, D1, E1, F1, G1, H1
        };

    Square field = flip_w[idx];

    if(Black == color)
        field = flipVertical(field); // flip for black
    return field;
    }

EvalPst::EvalPst (void)
    {
    init();
    }

void EvalPst::init (void)
    {
    for ( int32 i = 0; i < 12; ++ i )
        {
        for ( int32 j = 0; j < 64; ++ j )
            {
            m_pstMG[i][j] = drawValue;
            m_pstEG[i][j] = drawValue;
            }
        }

    init(WP, pstPawnMg, pstPawnEg);
    init(WN, pstKnightMg, pstKnightEg);
    init(WB, pstBishopMg, pstBishopEg);
    init(WR, pstRookMg, pstRookEg);
    init(WQ, pstQueenMg, pstQueenEg);
    init(WK, pstKingMg, pstKingEg);

    init(BP, pstPawnMg, pstPawnEg);
    init(BN, pstKnightMg, pstKnightEg);
    init(BB, pstBishopMg, pstBishopEg);
    init(BR, pstRookMg, pstRookEg);
    init(BQ, pstQueenMg, pstQueenEg);
    init(BK, pstKingMg, pstKingEg);
    }

void EvalPst::init (Piece pc, const int32 * pTable_mg, const int32 * pTable_eg)
    {
    for ( int32 i = 0; i < 64; ++ i )
        {
        int32 iField = getEvalField(getColor(pc), i);
        m_pstMG[pc][iField] = pTable_mg[i];
        m_pstEG[pc][iField] = pTable_eg[i];
        }
    }

EvalOutpost::EvalOutpost (void)
    {
    init();
    }

void EvalOutpost::init (void)
    {
    m_outpost_w = EmptyBoard;
    m_outpost_b = EmptyBoard;

    for ( int32 i = 0; i < 64; ++ i )
        {
        if(Outpost[i])
            {
            set_bit(m_outpost_w, getEvalField(White, i));
            set_bit(m_outpost_b, getEvalField(Black, i));
            }
        }
    }

EvalPvt::EvalPvt ()
    {
    setMatValue(Pawn, pawnValue);
    setMatValue(Knight, knightValue);
    setMatValue(Bishop, bishopValue);
    setMatValue(Rook, rookValue);
    setMatValue(Queen, queenValue);
    setMatValue(King, kingValue);
    }

EvalKPK::EvalKPK (void)
    {
    init();
    }

bool EvalKPK::probe (Square wk, Square bk, Square p, Piece pp, Color stm)
    {
    if(getColor(pp) == Black)
        {
        wk = flipVertical(wk);
        bk = flipVertical(bk);
        p = flipVertical(p);
        stm = toggleColor(stm);
        }

    if(getFile(p) > FileD)
        {
        wk = flipHorizontal(wk);
        bk = flipHorizontal(bk);
        p = flipHorizontal(p);
        }

    const index_t idx = encode(wk, bk, p, stm);
    return test_bit(m_bitbase[idx / 64], idx % 64);
    }

void EvalKPK::init ()
    {
    EResult res[MAX_INDEX];

    // first pass: apply static rules
    for ( int32 idx = 0; idx < MAX_INDEX; ++ idx )
        res[idx] = rules(idx);

    // iterate until all positions are computed
    bool repeat(true);

    while(repeat)
        {
        repeat = false;

        for ( int32 idx = 0; idx < MAX_INDEX; ++ idx )
            repeat |= (res[idx] == Unknown && classify(res, idx) != Unknown);
        }

    //bool ok = kpk_ok(res);

    // Pack into 64-bit entries
    for ( int32 idx = 0; idx < MAX_INDEX; ++ idx )
        if(res[idx] == Win)
            set_bit(m_bitbase[idx / 64], idx % 64);
    }

EvalKPK::index_t EvalKPK::encode (Square wk, Square bk, Square p, Color stm)
    {
    const int32 wp24 = 4 * (Rank7 - getRank(p)) + getFile(p);
    return wk ^ (bk << 6) ^ (stm << 12) ^ (wp24 << 13);
    }

void EvalKPK::decode (EvalKPK::index_t idx, Square & wk, Square & bk, Square & p, Color & stm)
    {
    wk = Square(idx & 63);
    idx >>= 6;
    bk = Square(idx & 63);
    idx >>= 6;
    stm = Color(idx & 1);
    idx >>= 1;
    p = getField(File(idx & 3), Rank(Rank7 - idx / 4));
    }

EvalKPK::EResult EvalKPK::rules (EvalKPK::index_t idx)
    {
    Square wk, bk, p;
    Color stm;
    decode(idx, wk, bk, p, stm);

    uint64 atks_wk = gl_bitboards.getAttacksK(wk);
    uint64 atks_bk = gl_bitboards.getAttacksK(bk);
    uint64 atks_p = gl_bitboards.getAttacksP(p, White);
    Square step_p = Square(p + SinglePawnStep);

    // pieces overlapping or kings checking each other
    if(test_bit(atks_wk, bk) || wk == bk || p == wk || p == bk)
        return Illegal;

    // cannot be white's turn if black is in check
    if(stm == White && test_bit(atks_p, bk))
        return Illegal;

    // win if pawn can be promoted without getting captured
    if(stm == White)
        {
        if((getRank(p) == Rank7) && (bk != step_p) && (wk != step_p) && !test_bit(atks_bk & ~ atks_wk, step_p))
            return Win;
        }

    else if(!(atks_bk & ~(atks_wk | atks_p)) || test_bit(atks_bk & ~ atks_wk, p))
        return Draw;

    return Unknown;
    }

EvalKPK::EResult EvalKPK::classify (EvalKPK::EResult res [], EvalKPK::index_t idx)
    {
    Square wk, bk, p;
    Color stm;
    decode(idx, wk, bk, p, stm);

    uint8 r = Illegal;

    // king moves
    uint64 b = gl_bitboards.getAttacksK(stm == Black ? bk : wk);

    while(b)
        {
        const Square sq = popFirstBit(b);
        r |= res[stm == Black ? encode(wk, sq, p, White) : encode(sq, bk, p, Black)];
        }

    // pawn moves
    if(stm == White && getRank(p) < Rank7)
        {
        // single push
        const Square sq = Square(p + SinglePawnStep);
        r |= res[encode(wk, bk, sq, Black)];

        // double push
        if(getRank(p) == Rank2 && sq != wk && sq != bk)
            r |= res[encode(wk, bk, Square(sq + SinglePawnStep), Black)];
        }

    if(stm == White)
        return res[idx] = r & Win ? Win : (r & Unknown ? Unknown : Draw);
    else
        return res[idx] = r & Draw ? Draw : (r & Unknown ? Unknown : Win);
    }

bool EvalKPK::kpk_ok (EvalKPK::EResult res [])
    {
    int32 illegal(0);
    int32 win(0);

    for ( int32 idx = 0; idx < MAX_INDEX; ++ idx )
        {
        if(res[idx] == Illegal)
            ++ illegal;

        else if(res[idx] == Win)
            ++ win;
        }
    return (illegal == 30932) && (win == 111282);
    }

CEvalEgHits::CEvalEgHits (void)
    {
    clear();
    }

uint64 CEvalEgHits::hits (void) const
    {
    return m_hits;
    }

void CEvalEgHits::hit (void)
    {
    ++ m_hits;
    }

void CEvalEgHits::clear (void)
    {
    m_hits = 0;
    }

Eval::Eval (const Board & board) :
    m_board(& board)
    {
    }

const Board & Eval::board (void) const
    {
    return * m_board;
    }

int32 Eval::getEval (int32 alpha, int32 beta)
    {
    int32 eval_eg = drawValue;

    if(evalEndGame(eval_eg))
        {
        gl_egHits.hit();
        return (eval_eg);
        }


    // lazy-eval
    if(0 < lazyEvalMargin)
        {
        int32 mat_eval = getEvalMat();

        if(mat_eval >= (beta + lazyEvalMargin))
            return (beta);

        if(mat_eval <= (alpha - lazyEvalMargin))
            return (alpha);
        }

    uint64 pieces = EmptyBoard;
    uint64 occup = board().getOccupBits();
    uint64 pawn_atks_w = gl_bitboards.getAttacksP(board().getPieceBits(WP), White);
    uint64 pawn_atks_b = gl_bitboards.getAttacksP(board().getPieceBits(BP), Black);
    uint64 mob_w = ~(board().getColorBits(White) | pawn_atks_b);
    uint64 mob_b = ~(board().getColorBits(Black) | pawn_atks_w);

    // pawns

    PawnHashItem * pPawnHash = getPawnHash();
    int32 eval_mg_w = pPawnHash->eval_mg[White];
    int32 eval_eg_w = pPawnHash->eval_eg[White];
    int32 eval_mg_b = pPawnHash->eval_mg[Black];
    int32 eval_eg_b = pPawnHash->eval_eg[Black];

    pieces = pPawnHash->passers[White];

    while(pieces)
        {
        Square field = popFirstBit(pieces);
        int32 rank = getRank(field);

        if(shift_N(create_bit(field)) & occup)
            {
            eval_mg_w += passedPawnBlockedMg[White][rank];
            eval_eg_w += passedPawnBlockedEg[White][rank];
            }
        else
            {
            eval_mg_w += passedPawnFreeMg[White][rank];
            eval_eg_w += passedPawnFreeEg[White][rank];
            }

        if(onlyPawnsLeft(Black))
            {
            Square f = field;

            if(board().getSide() == Black)
                f = Square(f - SinglePawnStep);

            if((gl_bitboards.getQuadratics(f, White) & board().getPieceBits(BK)) == 0)
                {
                eval_mg_w += passedPawnUnstoppableMg[White][rank];
                eval_eg_w += passedPawnUnstoppableEg[White][rank];
                }
            }
        }

    pieces = pPawnHash->passers[Black];

    while(pieces)
        {
        Square field = popFirstBit(pieces);
        int32 rank = getRank(field);

        if(shift_S(create_bit(field)) & occup)
            {
            eval_mg_b += passedPawnBlockedMg[Black][rank];
            eval_eg_b += passedPawnBlockedEg[Black][rank];
            }
        else
            {
            eval_mg_b += passedPawnFreeMg[Black][rank];
            eval_eg_b += passedPawnFreeEg[Black][rank];
            }

        if(onlyPawnsLeft(White))
            {
            Square f = field;

            if(board().getSide() == White)
                f = Square(f + SinglePawnStep);

            if((gl_bitboards.getQuadratics(f, Black) & board().getPieceBits(WK)) == 0)
                {
                eval_mg_b += passedPawnUnstoppableMg[Black][rank];
                eval_eg_b += passedPawnUnstoppableEg[Black][rank];
                }
            }
        }

    // knights

    pieces = board().getPieceBits(WN);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_w += gl_pst.getMG(WN, field);
        eval_eg_w += gl_pst.getEG(WN, field);

        uint32 atks = countBits(gl_bitboards.getAttacksN(field));
        eval_mg_w += mobilityMgKnight[atks];
        eval_eg_w += mobilityEgKnight[atks];

        if(gl_outpost.isWhite(field) && (gl_bitboards.getAttacksP(field, Black) & board().getPieceBits(WP)))
            {
            eval_mg_w += outpostMg;
            eval_eg_w += outpostEg;
            }
        }

    pieces = board().getPieceBits(BN);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_b += gl_pst.getMG(BN, field);
        eval_eg_b += gl_pst.getEG(BN, field);

        uint32 atks = countBits(gl_bitboards.getAttacksN(field));
        eval_mg_b += mobilityMgKnight[atks];
        eval_eg_b += mobilityEgKnight[atks];

        if(gl_outpost.isBlack(field) && (gl_bitboards.getAttacksP(field, White) & board().getPieceBits(BP)))
            {
            eval_mg_b += outpostMg;
            eval_eg_b += outpostEg;
            }
        }

    // bishops

    pieces = board().getPieceBits(WB);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_w += gl_pst.getMG(WB, field);
        eval_eg_w += gl_pst.getEG(WB, field);

        uint32 atks = countBits(gl_bitboards.getAttacksB(field, occup) & mob_w);
        eval_mg_w += mobilityMgBishop[atks];
        eval_eg_w += mobilityEgBishop[atks];
        }

    pieces = board().getPieceBits(BB);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_b += gl_pst.getMG(BB, field);
        eval_eg_b += gl_pst.getEG(BB, field);

        uint32 atks = countBits(gl_bitboards.getAttacksB(field, occup) & mob_b);
        eval_mg_b += mobilityMgBishop[atks];
        eval_eg_b += mobilityEgBishop[atks];
        }

    // rooks

    pieces = board().getPieceBits(WR);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_w += gl_pst.getMG(WR, field);
        eval_eg_w += gl_pst.getEG(WR, field);

        uint32 atks = countBits(gl_bitboards.getAttacksR(field, occup) & mob_w);
        eval_mg_w += mobilityMgRook[atks];
        eval_eg_w += mobilityEgRook[atks];

        File file = ::getFile(field);

        if(0 == (board().getPieceBits(WP) & gl_bitboards.getFile(file)))
            {
            if(0 == (board().getPieceBits(BP) & gl_bitboards.getFile(file)))
                {
                eval_mg_w += rookOpenFile;
                eval_eg_w += rookOpenFile;
                }
            else
                {
                eval_mg_w += rookHalfOpenFile;
                eval_eg_w += rookHalfOpenFile;
                }
            }
        }

    pieces = board().getPieceBits(BR);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_b += gl_pst.getMG(BR, field);
        eval_eg_b += gl_pst.getEG(BR, field);

        uint32 atks = countBits(gl_bitboards.getAttacksR(field, occup) & mob_b);
        eval_mg_b += mobilityMgRook[atks];
        eval_eg_b += mobilityEgRook[atks];

        File file = ::getFile(field);

        if(0 == (board().getPieceBits(BP) & gl_bitboards.getFile(file)))
            {
            if(0 == (board().getPieceBits(WP) & gl_bitboards.getFile(file)))
                {
                eval_mg_b += rookOpenFile;
                eval_eg_b += rookOpenFile;
                }
            else
                {
                eval_mg_b += rookHalfOpenFile;
                eval_eg_b += rookHalfOpenFile;
                }
            }
        }

    // queens

    pieces = board().getPieceBits(WQ);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_w += gl_pst.getMG(WQ, field);
        eval_eg_w += gl_pst.getEG(WQ, field);

        uint64 kp = board().getPieceBits(BK);

        while(kp)
            eval_mg_b += queenKingTropism * (7 - getDistSquares(field, popFirstBit(kp)));
        }

    pieces = board().getPieceBits(BQ);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_b += gl_pst.getMG(BQ, field);
        eval_eg_b += gl_pst.getEG(BQ, field);

        uint64 kp = board().getPieceBits(WK);

        while(kp)
            eval_mg_w += queenKingTropism * (7 - getDistSquares(field, popFirstBit(kp)));
        }

    // kings

    pieces = board().getPieceBits(WK);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_w += gl_pst.getMG(WK, field);
        eval_eg_w += gl_pst.getEG(WK, field);

        eval_mg_w += pPawnHash->king_safety[White][getFile(field)];
        }

    pieces = board().getPieceBits(BK);

    while(pieces)
        {
        Square field = popFirstBit(pieces);

        eval_mg_b += gl_pst.getMG(BK, field);
        eval_eg_b += gl_pst.getEG(BK, field);

        eval_mg_b += pPawnHash->king_safety[Black][getFile(field)];
        }

    int32 eval_pos = getPhaseEval(eval_mg_w - eval_mg_b, eval_eg_w - eval_eg_b);

    if(Black == board().getSide())
        eval_pos = - eval_pos;

    return getEvalMat() + eval_pos + evalContemptFactor();
    }

int32 Eval::getPhaseEval (const int32 & mg, const int32 & eg)
    {
    float64 phase = maxMatPhase - board().getMatPhase(White) - board().getMatPhase(Black);

    if(0 > phase)
        phase = 0;
    phase = (phase * 256 + (maxMatPhase / 2)) / maxMatPhase;

    return int32(((mg *(256 - phase)) + (eg * phase)) / 256);
    }

int32 Eval::getEvalMat (void) const
    {
    int32 eval = evalMaterial(White) - evalMaterial(Black);

    if(Black == board().getSide())
        eval = - eval;
    return eval;
    }

bool Eval::isDrawByMat (void) const
    {
    if(EmptyBoard == board().getPTypeBits(Pawn))
        {
        int32 mat_white = board().getMaterial(White);
        int32 mat_black = board().getMaterial(Black);

        if((kingValue == mat_white) && (kingValue == mat_black))
            return true;

        if(kingValue == mat_white)
            if(mat_black < (kingValue + gl_pvt.getMatValue(BR)))
                return true;

        if(kingValue == mat_black)
            if(mat_white < (kingValue + gl_pvt.getMatValue(WR)))
                return true;
        }
    return false;
    }

PawnHashItem * Eval::getPawnHash (void)
    {
    uint64 pawnHash = board().getHashPawn();
    PawnHashItem * item = gl_hashtable.findPawn(pawnHash);
	int backward, doubled, isolated, open;

    if(0 == item)
        item = & m_pawnHash;

    else if(item->key == pawnHash)
        return (item);

    if(item)
        {
        item->clear();
        item->key = pawnHash;

        int8 ranks[2][10];

        for ( int32 file = 0; file < 10; ++ file )
            {
            ranks[White][file] = 7;
            ranks[Black][file] = 0;
            }

        uint64 pawns_w = board().getPieceBits(WP);
        uint64 pawns_b = board().getPieceBits(BP);
        uint64 pieces = EmptyBoard;

        // white pawns

        pieces = pawns_w;

        while(pieces)
            {
            Square field = popFirstBit(pieces);

            int8 file = getFile(field) + 1;
            int8 rank = getRank(field);

            if(rank < ranks[White][file])
                ranks[White][file] = rank;
            }

        pieces = pawns_w;

        while(pieces)
            {
            Square field = popFirstBit(pieces);

            int32 file = getFile(field) + 1;
            int32 rank = getRank(field);

            item->eval_mg[White] += gl_pst.getMG(WP, field);
            item->eval_eg[White] += gl_pst.getEG(WP, field);

            if((gl_bitboards.getFrontSpans(field, White) & pawns_b) == 0)
                set_bit(item->passers[White], field);

			backward = ((ranks[White][file - 1] > rank) && (ranks[White][file + 1] > rank)
				&& (gl_bitboards.getAttacksP(field, White) & pawns_w) == 0) ? 1 : 0;
			doubled = (ranks[White][file] != rank) ? 1 : 0;
			isolated = (ranks[White][file - 1] == 7) && (ranks[White][file + 1] == 7) ? 1 : 0;
			open = ((board().getPieceBits(BP) & file) == 0) ? 1 : 0;

			if (backward)
				{
				item->eval_mg[White] += backwardPawn_mg[open];
				item->eval_eg[White] += backwardPawn_eg[open];
				}
			if (doubled)
                {
                item->eval_mg[White] += doubledPawn_mg[open];
                item->eval_eg[White] += doubledPawn_eg[open];
                }
            else if (isolated)
                {
                item->eval_mg[White] += isolatedPawn_mg[open];
                item->eval_eg[White] += isolatedPawn_eg[open];
                }
            }

        // black pawns

        pieces = pawns_b;

        while(pieces)
            {
            Square field = popFirstBit(pieces);

            int8 file = getFile(field) + 1;
            int8 rank = getRank(field);

            if(rank > ranks[Black][file])
                ranks[Black][file] = rank;
            }

        pieces = pawns_b;

        while(pieces)
            {
            Square field = popFirstBit(pieces);

            int32 file = getFile(field) + 1;
            int32 rank = getRank(field);

            item->eval_mg[Black] += gl_pst.getMG(BP, field);
            item->eval_eg[Black] += gl_pst.getEG(BP, field);

            if((gl_bitboards.getFrontSpans(field, Black) & pawns_w) == 0)
                set_bit(item->passers[Black], field);

			backward = ((ranks[Black][file - 1] < rank) && (ranks[Black][file + 1] < rank)
				&& (gl_bitboards.getAttacksP(field, Black) & pawns_b) == 0) ? 1 : 0;
			doubled = (ranks[Black][file] != rank) ? 1 : 0;
			isolated = (ranks[Black][file - 1] == 0) && (ranks[Black][file + 1] == 0) ? 1 : 0;
			open = ((board().getPieceBits(WP) & file) == 0) ? 1 : 0;

			if (backward)
				{
				item->eval_mg[Black] += backwardPawn_mg[open];
				item->eval_eg[Black] += backwardPawn_eg[open];
				}
            if(doubled)
                {
                item->eval_mg[Black] += doubledPawn_mg[open];
                item->eval_eg[Black] += doubledPawn_eg[open];
                }
            else if(isolated)
                {
                item->eval_mg[Black] += isolatedPawn_mg[open];
                item->eval_eg[Black] += isolatedPawn_eg[open];
                }
            }

#define KING_SAFETY(color, file)                                                 \
      {                                                                          \
          int32 ks = drawValue;                                                  \
          int32 cf = file + 1;                                                   \
          ks += evalPawnFile(ranks[White][cf - 1], ranks[Black][cf - 1], color); \
          ks += evalPawnFile(ranks[White][cf],     ranks[Black][cf],     color); \
          ks += evalPawnFile(ranks[White][cf + 1], ranks[Black][cf + 1], color); \
          if (ks < -127) ks = -127;                                              \
          if (ks > 127) ks = 127;                                                \
          item->king_safety[color][file] = (int8)ks;                             \
      }

        KING_SAFETY(White, FileA)
        KING_SAFETY(White, FileB)
        KING_SAFETY(White, FileC)
        KING_SAFETY(White, FileD)
        KING_SAFETY(White, FileE)
        KING_SAFETY(White, FileF)
        KING_SAFETY(White, FileG)
        KING_SAFETY(White, FileH)

        KING_SAFETY(Black, FileA)
        KING_SAFETY(Black, FileB)
        KING_SAFETY(Black, FileC)
        KING_SAFETY(Black, FileD)
        KING_SAFETY(Black, FileE)
        KING_SAFETY(Black, FileF)
        KING_SAFETY(Black, FileG)
        KING_SAFETY(Black, FileH)
        }
    return (item);
    }

int32 Eval::evalMaterial (Color side) const
    {
    int32 eval = board().getMaterial(side);

    if(isMultiBit(board().getPieceBits(Bishop, side)))
        eval += bishopPair;
    return (eval);
    }

int32 Eval::evalContemptFactor () const
    {
    if(Black == board().getSide())
        return - uci.config.contempt;
    else
        return uci.config.contempt;
    }

int32 Eval::evalPawnFile (int8 rank_white, int8 rank_black, Color side)
    {
    int32 eval = drawValue;

    if(White == side)
        {
        switch(rank_white)
            {
            case Rank8:
                eval += pawnShieldMe * 3;
                break; // no pawn
            case Rank7:
                eval += pawnShieldMe * 2;
                break;
            case Rank6:
            case Rank5:
            case Rank4:
            case Rank3:
                eval += pawnShieldMe * 1;
                break;
            }

        switch(rank_black)
            {
            case Rank4:
                eval += pawnShieldOpp * 1;
                break;
            case Rank3:
                eval += pawnShieldOpp * 2;
                break;
            case Rank2:
                eval += pawnShieldOpp * 3;
                break; // no pawn
            }
        }
    else
        {
        switch(rank_black)
            {
            case Rank6:
            case Rank5:
            case Rank4:
            case Rank3:
                eval += pawnShieldMe * 1;
                break;
            case Rank2:
                eval += pawnShieldMe * 2;
                break;
            case Rank1:
                eval += pawnShieldMe * 3;
                break; // no pawn
            }

        switch(rank_white)
            {
            case Rank7:
                eval += pawnShieldOpp * 3;
                break; // no pawn
            case Rank6:
                eval += pawnShieldOpp * 2;
                break;
            case Rank5:
                eval += pawnShieldOpp * 1;
                break;
            }
        }
    return (eval);
    }

int8 Eval::getRank (Square field)
    {
    return static_cast<int8>(::getRank(field));
    }

int8 Eval::getFile (Square field)
    {
    return static_cast<int8>(::getFile(field));
    }

bool Eval::onlyPawnsLeft (Color side) const
    {
    return (board().getMatCount(side) == board().getPieceCount(Pawn, side) + board().getPieceCount(King, side));
    }

bool Eval::evalEndGame (int32 & eval)
    {
    int32 piece_cnt = board().getMatCount();

    if(egMaxMen >= piece_cnt)
        {
        switch(piece_cnt)
            {
            case 2:
                return (true); // KK

            case 3:
                {
                if(board().getPTypeBits(Queen))
                    return (KXK(eval)); // KQK

                if(board().getPTypeBits(Rook))
                    return (KXK(eval)); // KRK

                if(board().getPTypeBits(Bishop))
                    return (true);      // KBK

                if(board().getPTypeBits(Knight))
                    return (true);      // KNK

                if(board().getPTypeBits(Pawn))
                    return (KPK(eval)); // KPK
                }
                break;

            case 4:
                {
                if(board().getMatCount(White) != board().getMatCount(Black))
                    {
                    if(isMultiBit(board().getPTypeBits(Queen)))
                        return (KXK(eval));  // KQQK

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Rook))
                        return (KXK(eval));  // KQRK

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Bishop))
                        return (KXK(eval));  // KQBK

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Knight))
                        return (KXK(eval));  // KQNK

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Pawn))
                        return (KXPK(eval)); // KQPK

                    if(isMultiBit(board().getPTypeBits(Rook)))
                        return (KXK(eval));  // KRRK

                    if(board().getPTypeBits(Rook) && board().getPTypeBits(Bishop))
                        return (KXK(eval));  // KRBK

                    if(board().getPTypeBits(Rook) && board().getPTypeBits(Knight))
                        return (KXK(eval));  // KRNK

                    if(board().getPTypeBits(Rook) && board().getPTypeBits(Pawn))
                        return (KXPK(eval)); // KRPK

                    if(isMultiBit(board().getPTypeBits(Bishop)))
                        return (KBBK(eval)); // KBBK

                    if(board().getPTypeBits(Bishop) && board().getPTypeBits(Knight))
                        return (KBNK(eval)); // KBNK

                    if(board().getPTypeBits(Bishop) && board().getPTypeBits(Pawn))
                        return (KBPK(eval)); // KBPK

                    if(isMultiBit(board().getPTypeBits(Knight)))
                        return (true);       // KNNK

                    if(board().getPTypeBits(Knight) && board().getPTypeBits(Pawn))
                        return (KXPK(eval)); // KNPK

                    if(isMultiBit(board().getPTypeBits(Pawn)))
                        return (false);      // KPPK
                    }
                else
                    {
                    if(isMultiBit(board().getPTypeBits(Queen)))
                        return (true);       // KQKQ

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Rook))
                        return (KXKx(eval)); // KQKR

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Bishop))
                        return (KXKx(eval)); // KQKB

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Knight))
                        return (KXKx(eval)); // KQKN

                    if(board().getPTypeBits(Queen) && board().getPTypeBits(Pawn))
                        return (false);      // KQKP

                    if(isMultiBit(board().getPTypeBits(Rook)))
                        return (true);       // KRKR

                    if(board().getPTypeBits(Rook) && board().getPTypeBits(Bishop))
                        return (KRKB(eval)); // KRKB

                    if(board().getPTypeBits(Rook) && board().getPTypeBits(Knight))
                        return (KRKN(eval)); // KRKN

                    if(board().getPTypeBits(Rook) && board().getPTypeBits(Pawn))
                        return (KRKP(eval)); // KRKP

                    if(isMultiBit(board().getPTypeBits(Bishop)))
                        return (true);       // KBKB

                    if(board().getPTypeBits(Bishop) && board().getPTypeBits(Knight))
                        return (true);       // KBKN

                    if(board().getPTypeBits(Bishop) && board().getPTypeBits(Pawn))
                        return (KBKP(eval)); // KBKP

                    if(isMultiBit(board().getPTypeBits(Knight)))
                        return (true);       // KNKN

                    if(board().getPTypeBits(Knight) && board().getPTypeBits(Pawn))
                        return (KNKP(eval)); // KNKP

                    if(isMultiBit(board().getPTypeBits(Pawn)))
                        return (false);      // KPKP
                    }
                }
                break;

            case 5:
                {
                if(IS_5MEN_12(Bishop, Pawn))
                    return (KBKPs(eval));

                if(IS_5MEN_12(Knight, Pawn))
                    return (KNKPs(eval));
                }
                break;

            case 6:
                {
                if(board().getMatCount(White) != board().getMatCount(Black))
                    {
                    if(IS_6MEN_13(Bishop, Pawn))
                        return (KBKPs(eval));

                    if(IS_6MEN_13(Knight, Pawn))
                        return (KNKPs(eval));
                    }
                }
                break;
            }
        }
    return (false);
    }

bool Eval::adjustEval (int32 & eval)
    {
    if(getWeakSide() == board().getSide())
        eval = - eval;
    return (true);
    }

Color Eval::getLessSide (void)
    {
    return (board().getMatCount(Black) < board().getMatCount(White) ? Black : White);
    }

Color Eval::getWeakSide (void)
    {
    return (board().getMaterial(Black) < board().getMaterial(White) ? Black : White);
    }

Square Eval::getKingPos (Color side)
    {
    uint64 x = board().getPieceBits(King, side);

    if(x)
        return getFirstBit(x);

    return noSquare;
    }

Square Eval::getWeakKingSq (void)
    {
    return (getKingPos(getWeakSide()));
    }

Square Eval::getStrongKingSq (void)
    {
    return (getKingPos(toggleColor(getWeakSide())));
    }

Square Eval::getPieceSq (PieceType pt)
    {
    return (getFirstBit(board().getPTypeBits(pt)));
    }

Color Eval::getPieceColor (PieceType pt)
    {
    return (getColor(board().getPiece(getPieceSq(pt))));
    }

Color Eval::getFieldColor (Square sq)
    {
    return (test_bit(BlackSqs, sq) ? Black : White);
    }

Square Eval::getPromSq (Square sq)
    {
    return (getField(::getFile(sq), getColor(board().getPiece(sq)) == Black ? Rank1 : Rank8));
    }

int32 Eval::getDistSquares (Square sq1, Square sq2)
    {
    return (max(abs(getRank(sq2) - getRank(sq1)), abs(getFile(sq2) - getFile(sq1))));
    }

int32 Eval::getDistBorder (Square sq)
    {
    int8 f = getFile(sq);
    int8 r = getRank(sq);
    return (min(min((7 - f), f), min((7 - r), r)));
    }

int32 Eval::getDistBorder (Square sq, Color color)
    {
    int8 r = getRank(sq);
    return (color == Black ? r : 7 - r);
    }

int32 Eval::getDistCorner (Square sq)
    {
    int8 f = getFile(sq);
    int8 r = getRank(sq);

    if(f > 4)
        f = 7 - f;

    if(r > 4)
        r = 7 - r;
    return (max(f, r));
    }

int32 Eval::getDistCorner (Square sq, Color color)
    {
    if(White == color)
        return (min(getDistSquares(sq, H1), getDistSquares(sq, A8)));
    else
        return (min(getDistSquares(sq, A1), getDistSquares(sq, H8)));
    }

int32 Eval::matBalance (void)
    {
    return (abs(board().getMaterial(White) - board().getMaterial(Black)));
    }

int32 Eval::nearSquares (Square sq1, Square sq2)
    {
    return (squareBonus[getDistSquares(sq1, sq2)]);
    }

int32 Eval::awaySquares (Square sq1, Square sq2)
    {
    return (- squareBonus[getDistSquares(sq1, sq2)]);
    }

int32 Eval::nearBorder (Square sq)
    {
    return (borderBonus[getDistBorder(sq)]);
    }

int32 Eval::awayBorder (Square sq)
    {
    return (- borderBonus[getDistBorder(sq)]);
    }

int32 Eval::nearBorder (Square sq, Color color)
    {
    return (borderBonus[getDistBorder(sq, color)]);
    }

int32 Eval::awayBorder (Square sq, Color color)
    {
    return (- borderBonus[getDistBorder(sq, color)]);
    }

int32 Eval::nearCorner (Square sq)
    {
    return (cornerBonus[getDistCorner(sq)]);
    }

int32 Eval::awayCorner (Square sq)
    {
    return (- cornerBonus[getDistCorner(sq)]);
    }

int32 Eval::nearCorner (Square sq, Color color)
    {
    return (cornerBonus[getDistCorner(sq, color)]);
    }

int32 Eval::awayCorner (Square sq, Color color)
    {
    return (- cornerBonus[getDistCorner(sq, color)]);
    }

int32 Eval::nearKings (void)
    {
    return (nearSquares(getKingPos(White), getKingPos(Black)));
    }

int32 Eval::movePawns (Color side)
    {
    int32 eval = drawValue;
    uint64 pawns = board().getPieceBits(Pawn, side);

    while(pawns)
        eval += nearBorder(popFirstBit(pawns), side);
    return (eval);
    }

int32 Eval::stopPawns (Color side)
    {
    int32 eval = drawValue;
    uint64 pawns = board().getPieceBits(Pawn, side);

    while(pawns)
        eval += awayBorder(popFirstBit(pawns), side);
    return (eval);
    }

bool Eval::KXK (int32 & eval)
    {
    eval += matBalance();
    eval += nearKings();
    eval += nearBorder(getWeakKingSq());
    return (adjustEval(eval));
    }

bool Eval::KPK (int32 & eval)
    {
    Color strongSide = toggleColor(getWeakSide());
    Piece pawn = makePiece(Pawn, strongSide);

    if(gl_kpk.probe(getStrongKingSq(), getWeakKingSq(), getPieceSq(Pawn), pawn, board().getSide()))
        {
        eval += matBalance();
        eval += nearKings();
        eval += movePawns(strongSide);
        }
    return (adjustEval(eval));
    }

bool Eval::KXPK (int32 & eval)
    {
    eval += matBalance();
    eval += nearKings();
    eval += nearBorder(getWeakKingSq());
    eval += movePawns(toggleColor(getWeakSide()));
    return (adjustEval(eval));
    }

bool Eval::KXKx (int32 & eval)
    {
    eval += matBalance();
    eval += nearKings();
    eval += nearBorder(getWeakKingSq());
    return (adjustEval(eval));
    }

bool Eval::KBBK (int32 & eval)
    {
    uint64 bishops = board().getPTypeBits(Bishop);

    if(isMultiBit(WhiteSqs & bishops) || isMultiBit(BlackSqs & bishops))
        return (true); // same color -> draw

    eval += matBalance();
    eval += nearKings();
    eval += nearCorner(getWeakKingSq());
    return (adjustEval(eval));
    }

bool Eval::KBNK (int32 & eval)
    {
    eval += matBalance();
    eval += nearKings();
    eval += nearCorner(getWeakKingSq(), getFieldColor(getPieceSq(Bishop)));
    return (adjustEval(eval));
    }

bool Eval::KBPK (int32 & eval)
    {
    Square pawn_sq = getPieceSq(Pawn);
    Square prom_sq = getPromSq(pawn_sq);

    if(FileA == getFile(pawn_sq) || FileH == getFile(pawn_sq))
        if(getFieldColor(getPieceSq(Bishop)) != getFieldColor(prom_sq))
            if(test_bit(gl_bitboards.getAttacksK(prom_sq) | create_bit(prom_sq), getWeakKingSq()))
                return (true);

    eval += matBalance();
    eval += nearKings();
    eval += nearBorder(getWeakKingSq());
    eval += movePawns(toggleColor(getWeakSide()));
    return (adjustEval(eval));
    }

bool Eval::KBKP (int32 & eval)
    {
    eval += stopPawns(getWeakSide()); // capture the pawn -> game is over
    return (adjustEval(eval));
    }

bool Eval::KNKP (int32 & eval)
    {
    eval += stopPawns(getWeakSide()); // capture the pawn -> game is over
    return (adjustEval(eval));
    }

bool Eval::KRKB (int32 & eval)
    {
    eval += nearBorder(getWeakKingSq());
    eval += awayCorner(getWeakKingSq(), toggleColor(getFieldColor(getPieceSq(Bishop))));
    return (adjustEval(eval));
    }

bool Eval::KRKN (int32 & eval)
    {
    eval += nearBorder(getWeakKingSq());
    eval += awaySquares(getWeakKingSq(), getPieceSq(Knight));
    return (adjustEval(eval));
    }

bool Eval::KRKP (int32 & eval)
    {
    eval = drawValue;
    return (false);
    }

bool Eval::KBKPs (int32 & eval)
    {
    eval += stopPawns(getWeakSide()); // capture the pawns -> game is over
    return (adjustEval(eval));
    }

bool Eval::KNKPs (int32 & eval)
    {
    eval += stopPawns(getWeakSide()); // capture the pawns -> game is over
    return (adjustEval(eval));
    }