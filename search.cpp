#include "search.h"

SearchPool gl_search;

int32 getMateDist (int32 eval)
    {
    if(eval > (mateValue - searchMaxDepth))
        return (mateValue - eval);

    if(eval < (- mateValue + searchMaxDepth))
        return (- mateValue - eval);

    return (0);
    }

MoveListParams::MoveListParams (void)
    {
    init(0);
    }

void MoveListParams::init (int32 curPly)
    {
    ply = curPly;
    phase = 0;
    index = 0;
    hash = 0;
    killer1 = 0;
    killer2 = 0;
    caps.clear();
    quiets.clear();
    quiets_pst.clear();
    bads.clear();
    }

Search::Search (Board & board, bool guiCom) :
    m_board(& board),
    m_movegen(board),
    m_eval(board),
    m_guiCom(guiCom)
    {
    }

Board & Search::board (void)
    {
    return * m_board;
    }

bool Search::think (const SearchParams & params)
    {
    m_params = params;
    m_finalize = false;
    m_extinfos = false;
    m_ponder = m_params.ponder;
    m_nodes = 0;
    m_time_start.start();
    m_time_info.start();
    m_time_hard = timeUndef;
    m_time_soft = timeUndef;
    m_pvRoot.clear();
    m_pvCurr.clear();
    clearHistory();

    // time management
    Time::type movetime(timeUndef);
    Time::type inctime(0);

    if((timeUndef != m_params.wtime) && (White == board().getSide()))
        {
        movetime = m_params.wtime;
        inctime = m_params.winc;
        }

    if((timeUndef != m_params.btime) && (Black == board().getSide()))
        {
        movetime = m_params.btime;
        inctime = m_params.binc;
        }

    if(timeUndef != movetime)
        {
        const Time::type SAFETY = 5000;
        float64 timeweight(1.0);

        /*switch (m_params.movenum)
        {
            case 1: timeweight = 2.0; break;
            case 2: timeweight = 1.5; break;
            case 3: timeweight = 1.2; break;
            default: timeweight = 1.0;
        }*/

        if(movetime <= SAFETY)
            {
            m_time_hard = movetime / (m_params.movestogo + 10);
            m_time_soft = m_time_hard;
            }
        else
            {
            m_time_soft = ((movetime - SAFETY) / (m_params.movestogo + 10)) * (Time::type)timeweight;

            if(m_time_soft + inctime < movetime)
                m_time_soft += inctime;
            m_time_hard = m_time_soft * 3;
            }
        }

    if(timeUndef != m_params.movetime)
        {
        m_time_hard = m_params.movetime;
        m_time_soft = m_time_hard;
        }

    // depth management
    int32 maxdepth = searchMaxDepth;

    if(0 < m_params.depth)
        maxdepth = m_params.depth + 1;

    if(0 < m_params.mate)
        maxdepth = m_params.mate + 1;

    int32 alpha = - mateValue;
    int32 beta = mateValue;

    for ( int32 depth = 1; depth < maxdepth; ++ depth )
        {
        m_maxqply = 0;

        int32 eval = root(depth, 0, alpha, beta);

        if(m_finalize)
            break;
        else
            {
            if(m_pv[0].empty())
                {
                getHashPV(m_pv[0], depth);
                sendInfoRootPV(depth, eval, alpha, beta);
                }
            m_pvRoot = m_pv[0];
            }

        if(!m_params.infinite && !m_ponder)
            {
            // break on mate
            int32 matedist = getMateDist(eval);

            if((0 != matedist) && (depth >= matedist))
                break;

            if(timeUndef != m_time_soft)
                if(m_time_soft < m_time_start.getDiffReal())
                    break;
            }

        sendInfoMultiPV(depth);
        }

    sendInfoProgress();

    return (true);
    }

bool Search::getBestMove (uint32 & move)
    {
    if(0 < m_pvRoot.size())
        {
        move = m_pvRoot[0];
        return (true);
        }
    return (false);
    }

bool Search::getPonderMove (uint32 & move)
    {
    if(1 < m_pvRoot.size())
        {
        move = m_pvRoot[1];
        return (true);
        }
    return (false);
    }

void Search::finalize (void)
    {
    m_finalize = true;
    }

uint64 Search::getNodes (void)
    {
    return m_nodes;
    }

int32 Search::getSEE (uint32 & move)
    {
    return (see(Move(move)));
    }

int32 Search::root (int32 depth, int32 ply, int32 alpha, int32 beta)
    {
    m_multiPV.clear();
    m_pv[ply].clear();
    ++ m_nodes;

    int32 hash_eval = drawValue;

    if(findHash(hash_eval, depth, ply, alpha, beta))
        return (hash_eval);

    int32 eval = drawValue;
    uint32 bestMove = 0;
    HashType hashtype = Alpha;

    bool inCheck = board().isCheck();

    MoveListParams & movelist = m_movelist[ply];
    movelist.init(ply);
    int32 legalMoves = 0;

    Move move;

    while(nextMove(move, movelist))
        {
        Piece piece = board().getPiece(move.getFrom());
        bool isQuiet = !isTactical(move);

        if(board().doMove(move))
            {
            ++ legalMoves;
            m_pvCurr.add(move);

            if(m_extinfos && m_guiCom)
                uci.sendCurrMove(move, legalMoves, depth);

            int32 newDepth = depth - 1;

            // extensions
            if(board().isCheck())
                ++ newDepth;

            if(isMultiPV())
                eval = - alphabeta(newDepth, ply + 1, - beta - queenValue, - alpha + queenValue);
            else
                {
                // principal variation search (PVS)
                if(1 == legalMoves)
                    eval = - alphabeta(newDepth, ply + 1, - beta, - alpha);
                else
                    {
                    eval = - alphabeta(newDepth, ply + 1, - alpha - 1, - alpha);

                    if((eval > alpha) && (eval < beta))
                        eval = - alphabeta(newDepth, ply + 1, - beta, - alpha);
                    }
                }

            m_pvCurr.del();
            board().undoMove(move);

            if(m_finalize)
                return (alpha);

            // save multi-pv line
            if(isMultiPV())
                {
                MoveList mvlst;
                mvlst.add(move);
                mvlst.add(m_pv[ply + 1]);
                m_multiPV.add(mvlst, eval);
                }

            if(eval > alpha)
                {
                bestMove = move.getMoveData();
                hashtype = Exact;

                alpha = eval;

                if(eval >= beta)
                    {
                    hashtype = Beta;
                    addKiller(ply, move);

                    if(isQuiet)
                        updateHistorySort(piece, move.getTo(), depth, true);

                    break;
                    }

                else
					if(isQuiet)
                    	updateHistorySort(piece, move.getTo(), depth);

                buildPv(ply, move);
                sendInfoRootPV(depth, eval, alpha, beta);
                }
            }
        }

    if(0 == legalMoves)
        {
        if(inCheck)
            alpha = - mateValue + ply;
        else
            alpha = drawValue;
        }

    else
		if(board().getFifty() >= 100)
        	alpha = drawValue;

    recordHash(alpha, depth, ply, hashtype, bestMove);
    return (alpha);
    }

int32 Search::alphabeta (int32 depth, int32 ply, int32 alpha, int32 beta, bool null)
    {
    if(ply >= searchMaxDepth || board().getPly() >= maxPly)
        return (alpha);

    m_pv[ply].clear();
    ++ m_nodes;

    check();

    if(m_finalize)
        return (alpha);

    if(m_eval.isDrawByMat())
        return (drawValue);

    if(!null && 0 < ply && board().isDrawByRep())
        return (drawValue);

    int32 hash_eval = drawValue;

    if(findHash(hash_eval, depth, ply, alpha, beta))
        return (hash_eval);

    if(depth <= 0 && !board().isCheck())
        {
        -- m_nodes;
        return (quiescence(ply, 0, alpha, beta));
        }

    int32 eval = drawValue;
    uint32 bestMove = 0;
    HashType hashtype = Alpha;

    bool inCheck = board().isCheck();
    bool PV = (beta - alpha) > 1;

    // futility-pruning
    if(!inCheck && !PV && 0 < depth && depth < fpMinDepth)
        {
        int32 margin = futilityMargin[depth];
        int32 staticEval = m_eval.getEval(alpha - margin, beta + margin);

        if(staticEval <= (alpha - margin))
            return (quiescence(ply, 0, alpha, beta));

        if(staticEval >= (beta + margin))
            return (beta);
        }

    // null move
    if(!inCheck && !PV && (depth >= nmMinDepth) && !null && doNullMove())
        {
        const int32 R = nmReduction;

        board().doNullMove();
        int32 eval_null = - alphabeta(depth - 1 - R, ply + 1, - beta, - beta + 1, true);
        board().undoNullMove();

        if(eval_null >= beta)
            return beta;
        }

    MoveListParams & movelist = m_movelist[ply];
    movelist.init(ply);
    int32 legalMoves = 0;
    int32 quietMoves = 0;

    Move move;

    while(nextMove(move, movelist))
        {
        Piece piece = board().getPiece(move.getFrom());
        bool isQuiet = !isTactical(move);

        if(board().doMove(move))
            {
            ++ legalMoves;
            m_pvCurr.add(move);

            if(isQuiet)
                updateHistoryFailure(piece, move.getTo());

            // extensions
            int32 extensions(0);

            if(PV || depth < nmPVminDepth)
                {
                if(board().isCheck())
                    ++ extensions;

                if((piece == WP) && (getRank(move.getTo()) == Rank7))
                    ++ extensions;

                else if((piece == BP) && (getRank(move.getTo()) == Rank2))
                    ++ extensions;
                }

            // late move reductions (LMR)
            int32 reductions(0);

            if(isQuiet && !extensions && !PV && !inCheck && !board().isCheck())
                if((lmrMinDepth <= depth) && isReductHistory(piece, move.getTo()))
                    if(lmrMinMoves <= ++ quietMoves)
                        ++ reductions;

            int32 newDepth = depth - 1 + extensions - reductions;

            // principal variation search (PVS)
            if(1 == legalMoves)
                eval = - alphabeta(newDepth, ply + 1, - beta, - alpha);
            else
                {
                eval = - alphabeta(newDepth, ply + 1, - alpha - 1, - alpha);

                if(reductions && eval > alpha)
                    eval = - alphabeta(newDepth + reductions, ply + 1, - alpha - 1, - alpha);

                if((eval > alpha) && (eval < beta))
                    eval = - alphabeta(newDepth, ply + 1, - beta, - alpha);
                }

            m_pvCurr.del();
            board().undoMove(move);

            if(m_finalize)
                return (alpha);

            if(eval > alpha)
                {
                bestMove = move.getMoveData();
                hashtype = Exact;

                alpha = eval;

                if(eval >= beta)
                    {
                    hashtype = Beta;
                    addKiller(ply, move);

                    if(isQuiet)
                        {
                        updateHistorySort(piece, move.getTo(), depth, true);
                        updateHistorySuccess(piece, move.getTo());
                        }

                    break;
                    }

                else
					if(isQuiet)
                    	updateHistorySort(piece, move.getTo(), depth);

                buildPv(ply, move);
                }
            }
        }

    if(0 == legalMoves)
        {
        if(inCheck)
            alpha = - mateValue + ply;
        else
            alpha = drawValue;
        }

    else
		if(board().getFifty() >= 100)
        	alpha = drawValue;

    recordHash(alpha, depth, ply, hashtype, bestMove);
    return (alpha);
    }

int32 Search::quiescence (int32 ply, int32 qply, int32 alpha, int32 beta)
    {
    if(ply >= searchMaxDepth || board().getPly() >= maxPly)
        return (alpha);

    ++ m_nodes;

    if(qply > m_maxqply)
        m_maxqply = qply;

    check();

    if(m_finalize)
        return (alpha);

    bool inCheck = board().isCheck();

    if(!inCheck)
        {
        int32 eval = m_eval.getEval(alpha, beta);

        if(eval > alpha)
            {
            alpha = eval;

            if(eval >= beta)
                return (beta);
            }
        }

    MoveListParams & movelist = m_movelist[ply];
    movelist.init(ply);
    int32 legalMoves = 0;

    Move move;

    while(nextMoveQ(move, movelist))
        {
        if(board().doMove(move))
            {
            ++ legalMoves;
            int32 eval = - quiescence(ply + 1, qply + 1, - beta, - alpha);
            board().undoMove(move);

            if(m_finalize)
                return (alpha);

            if(eval > alpha)
                {
                alpha = eval;

                if(eval >= beta)
                    break;
                }
            }
        }

    if(inCheck && (0 == legalMoves))
        alpha = - mateValue + ply;

    return (alpha);
    }

bool Search::nextMove (Move & move, MoveListParams & movelist)
    {
    switch(movelist.phase)
        {
        case 0:
            if(findHashMove(move) && m_movegen.isLegal(move))
                {
                movelist.hash = move;
                movelist.phase = 1;
                return (true);
                }

        case 1:
            m_movegen.getCaptures(movelist.caps);

            scoreCaptures(movelist.caps);
            movelist.phase = 2;
            movelist.index = 0;

        case 2:
            while(movelist.index < movelist.caps.size())
                {
                move = movelist.caps.getNthMax(movelist.index++);

                if(movelist.hash == move.getMoveData())
                    continue;

                if(isBadCapture(move))
                    {
                    movelist.bads.add(move);
                    continue;
                    }
                return (true);
                }
            movelist.phase = 3;

        case 3:
            if(getKiller(movelist.ply, move, 1) && (move != movelist.hash) && m_movegen.isLegal(move))
                {
                movelist.killer1 = move;
                movelist.phase = 4;
                return (true);
                }

        case 4:
            if(getKiller(movelist.ply, move, 2) && (move != movelist.hash) && m_movegen.isLegal(move))
                {
                movelist.killer2 = move;
                movelist.phase = 5;
                return (true);
                }

        case 5:
            m_movegen.getQuiets(movelist.quiets);

            scoreQuiets(movelist.quiets);
            movelist.phase = 6;
            movelist.index = 0;

        case 6:
            while(movelist.index < movelist.quiets.size())
                {
                move = movelist.quiets.getNthMax(movelist.index++);

                if(move.getMoveData() == movelist.hash || move.getMoveData() == movelist.killer1
                    || move.getMoveData() == movelist.killer2)
                    {
                    continue;
                    }

                if(historySortMin == move.getSort())
                    {
                    movelist.quiets_pst.add(move);
                    continue;
                    }
                return (true);
                }
            scoreQuietsPst(movelist.quiets_pst);
            movelist.phase = 7;
            movelist.index = 0;

        case 7:
            if(movelist.index < movelist.quiets_pst.size())
                {
                move = movelist.quiets_pst.getNthMax(movelist.index++);
                return (true);
                }
            movelist.phase = 8;
            movelist.index = 0;

        case 8:
            if(movelist.index < movelist.bads.size())
                {
                move = movelist.bads.getNthMax(movelist.index++);
                return (true);
                }
        }
    return (false);
    }

bool Search::nextMoveQ (Move & move, MoveListParams & movelist)
    {
    switch(movelist.phase)
        {
        case 0:
            if(board().isCheck())
                m_movegen.getEvasions(movelist.caps);
            else
                m_movegen.getCaptures(movelist.caps);

            scoreCaptures(movelist.caps);
            movelist.phase = 1;
            movelist.index = 0;

        case 1:
            while(movelist.index < movelist.caps.size())
                {
                move = movelist.caps.getNthMax(movelist.index++);

                if(!board().isCheck() && isBadCapture(move))
                    continue;

                return (true);
                }
        }
    return (false);
    }

void Search::scoreCaptures (MoveList & moves)
    {
    for ( int32 i = 0; i < moves.size(); ++ i )
        {
        Move & move = moves[i];

        Piece pc_to = board().getPiece(move.getTo());

        if(noPiece != pc_to)
            {
            Piece pc_from = board().getPiece(move.getFrom());
            moves.setSort(i, (gl_pvt.getMatValue(pc_to)) * 5 - gl_pvt.getMatValue(pc_from));
            }

        else
			if(Promotion == move.getType())
            	moves.setSort(i, gl_pvt.getMatValue(getPromPiece(PromInfo(move.getInfo()))) * 5 - gl_pvt.getMatValue(Pawn));

        else
			if(EnPassant == move.getType())
            	moves.setSort(i, gl_pvt.getMatValue(Pawn) * 5 - gl_pvt.getMatValue(Pawn));
        }
    }

void Search::scoreQuiets (MoveList & moves)
    {
    for ( int32 i = 0; i < moves.size(); ++ i )
        {
        Move & move = moves[i];

        Piece piece = board().getPiece(move.getFrom());
        moves.setSort(i, m_history[piece][move.getTo()].sort);
        }
    }

void Search::scoreQuietsPst (MoveList & moves)
    {
    for ( int32 i = 0; i < moves.size(); ++ i )
        {
        Move & move = moves[i];

        Piece piece = board().getPiece(move.getFrom());
        int32 mg = gl_pst.getMG(piece, move.getTo()) - gl_pst.getMG(piece, move.getFrom());
        int32 eg = gl_pst.getEG(piece, move.getTo()) - gl_pst.getEG(piece, move.getFrom());
        moves.setSort(i, m_eval.getPhaseEval(mg, eg));
        }
    }

bool Search::isBadCapture (const Move & move)
    {
    if(Promotion == move.getType())
        return (false);

    if(EnPassant == move.getType())
        return (false);
    Piece pc_to = board().getPiece(move.getTo());
    Piece pc_from = board().getPiece(move.getFrom());

    if(gl_pvt.getMatValue(pc_to) >= gl_pvt.getMatValue(pc_from))
        return (false);

    if(noPiece == pc_to)
        return (false);
    return (0 > see(move));
    }

bool Search::isTactical (const Move & move)
    {
    return ((EnPassant <= move.getType()) || (noPiece != board().getPiece(move.getTo())));
    }

bool Search::doNullMove (void)
    {
    Color side = board().getSide();
    return (board().getMatCount(side) - board().getPieceCount(Pawn, side))
        > board().getPieceCount(King, side); // kings + pieces
    }

int32 Search::see (const Move & move)
    {
    Square from = move.getFrom();
    Square to = move.getTo();
    Piece pc_from = board().getPiece(from);
    Piece pc_to = board().getPiece(to);

    uint64 occ = board().getOccupBits();
    del_bit(occ, from);

    uint64 diag = board().getPTypeBits(Bishop) | board().getPTypeBits(Queen);
    uint64 orth = board().getPTypeBits(Rook) | board().getPTypeBits(Queen);

    uint64 attackers = board().getAttackBy(to);
    attackers |= (gl_bitboards.getAttacksB(to, occ) & diag);
    attackers |= (gl_bitboards.getAttacksR(to, occ) & orth);
    attackers &= occ;

    int32 ptype = getPieceType(pc_from);
    uint64 uint64ype = EmptyBoard;
    Color color = toggleColor(getColor(pc_from));

    int32 score[32];
    score[0] = gl_pvt.getMatValue(pc_to);
    int32 ply = 1;

    while(attackers & board().getColorBits(color))
        {
        score[ply] = gl_pvt.getMatValue(PieceType(ptype)) - score[ply - 1];

        if(max(- score[ply - 1], score[ply]) < 0)
            break;

        for ( ptype = Pawn; ptype <= King; ++ ptype )
            {
            uint64ype = board().getPieceBits(PieceType(ptype), color) & attackers;

            if(uint64ype)
                break;
            }

        occ ^= (uint64ype & - int64(uint64ype));
        attackers |= (gl_bitboards.getAttacksB(to, occ) & diag);
        attackers |= (gl_bitboards.getAttacksR(to, occ) & orth);
        attackers &= occ;

        color = toggleColor(color);
        ++ ply;
        }

    while(-- ply)
        score[ply - 1] = - max(- score[ply - 1], score[ply]);
    return score[0];
    }

void Search::check (void)
    {
    // check node limits
    if(0 < m_params.nodes)
        if(m_nodes >= m_params.nodes)
            m_finalize = true;

    // check input and time limits
    if(0 == (nodeCheck & m_nodes))
        {
        // check input
        if(m_guiCom)
            {
            bool stop(false), ponderhit(false);

            if(uci.checkInput(stop, ponderhit))
                {
                if(stop)
                    m_finalize = true;

                if(ponderhit)
                    m_ponder = false;
                }
            }

        if(!m_ponder && !m_params.infinite)
            if(timeUndef != m_time_hard)
                if(m_time_hard < m_time_start.getDiffReal())
                    m_finalize = true;

        if(infoTime <= m_time_info.getDiffReal())
            {
            m_time_info.start();
            m_extinfos = true;
            sendInfoProgress();
            }
        }
    }

void Search::sendInfoRootPV (int32 depth, int32 eval, int32 alpha, int32 beta)
    {
    if(m_guiCom && depth && !isMultiPV())
        {
        // show a draw with little material and low score
        if(egMaxMen >= board().getMatCount() && abs(eval) <= egDrawEval)
            eval = alpha = beta = drawValue;

        uci.sendBestLine(depth, depth + m_maxqply, eval, alpha, beta, m_pv[0]);
        }
    }

void Search::sendInfoMultiPV (int32 depth)
    {
    if(m_guiCom && isMultiPV())
        {
        for ( int32 i = 0; i < uci.config.multiPV; ++ i )
            {
            ItemPV mpv;

            if(m_multiPV.getNthMax(mpv, i))
                uci.sendBestLine(depth, depth + m_maxqply, mpv.m_eval, - mateValue, mateValue, mpv.m_pv, i + 1);
            }
        }
    }

void Search::sendInfoProgress (void)
    {
    if(m_guiCom)
        uci.sendProgress();
    }

bool Search::isMultiPV (void)
    {
    return (1 < uci.config.multiPV);
    }

void Search::buildPv (int32 ply, Move & move)
    {
    m_pv[ply].clear();
    m_pv[ply].add(move);
    m_pv[ply].add(m_pv[ply + 1]);
    }

void Search::addKiller (int32 ply, const Move & move)
    {
    if(!isTactical(move))
        {
        uint32 mov = move.getMoveData();

        if(mov != m_killers[ply][0])
            {
            m_killers[ply][1] = m_killers[ply][0];
            m_killers[ply][0] = mov;
            }
        }
    }

bool Search::getKiller (int32 ply, Move & move, uint32 pos)
    {
    move = m_killers[ply][pos - 1];
    return (0 != move && !isTactical(move));
    }

void Search::clearHistory (void)
    {
    for ( int32 i = 0; i < 12; ++ i )
        for ( int32 j = 0; j < 64; ++ j )
            m_history[i][j].reset();

    for ( int32 k = 0; k < searchMaxDepth; ++ k )
        m_killers[k][0] = m_killers[k][1] = 0;
    }

void Search::updateHistoryFailure (Piece piece, Square to)
    {
    m_history[piece][to].failure++;
    }

void Search::updateHistorySuccess (Piece piece, Square to)
    {
    m_history[piece][to].success++;
    m_history[piece][to].failure--;
    }

void Search::updateHistorySort (Piece piece, Square to, int32 depth, bool betacut)
    {
    m_history[piece][to].sort += (betacut ? depth * depth : depth);

    if(m_history[piece][to].sort > historySortMax)
        m_history[piece][to].sort = historySortMax;
    }

bool Search::isReductHistory (Piece piece, Square to)
    {
    return (m_history[piece][to].success > 2 * m_history[piece][to].failure) ? false : true;
    }

void Search::recordHash (int32 eval, int32 depth, int32 ply, HashType type, uint32 move)
    {
    if(0 < uci.config.hash)
        {
        if((eval > mateValue - searchMaxDepth) && (eval <= mateValue))
            eval += ply;

        else
			if((eval < - mateValue + searchMaxDepth) && (eval >= - mateValue))
            	eval -= ply;

        gl_hashtable.record(board().getHash(), depth, eval, type, move);
        }
    }

bool Search::findHash (int32 & eval, int32 depth, int32 ply, int32 alpha, int32 beta)
    {
    if(0 < uci.config.hash)
        {
        HashItem * hash_item = gl_hashtable.find(board().getHash());

        if(hash_item && hash_item->data.depth >= depth)
            {
            int32 hash_eval = hash_item->data.eval;

            if((hash_eval > mateValue - searchMaxDepth) && (hash_eval <= mateValue))
                hash_eval -= ply;

            else
				if((hash_eval < - mateValue + searchMaxDepth) && (hash_eval >= - mateValue))
                	hash_eval += ply;

            if(Exact == hash_item->data.type)
                {
                eval = hash_eval;
                return (true);
                }

            if((Alpha == hash_item->data.type) && (hash_eval <= alpha))
                {
                eval = alpha;
                return (true);
                }

            if((Beta == hash_item->data.type) && (hash_eval >= beta))
                {
                eval = beta;
                return (true);
                }
            }
        }
    return (false);
    }

bool Search::findHashMove (Move & move)
    {
    if(0 < uci.config.hash)
        {
        HashItem * hash_item = gl_hashtable.find(board().getHash());

        if(hash_item && hash_item->data.move)
            {
            move = hash_item->data.move;
            return (true);
            }
        }
    return (false);
    }

void Search::getHashPV (MoveList & pv, int32 depth)
    {
    int32 i = 0;
    uint32 move = 0;

    do
        {
        move = 0;
        HashItem * pHashItem = gl_hashtable.find(board().getHash());

        if(pHashItem)
            move = pHashItem->data.move;

        if(0 != move)
            {
            pv.add(Move(move));
            board().doMove(Move(move));
            ++ i;
            }
        }
		while ((0 != move) && (i < depth));

    while(i--)
        board().undoMove(pv[i]);
    }

/////////////////////////////////////////////////
// class SearchThread
/////////////////////////////////////////////////

SearchThread::SearchThread (bool guiCommunication) :
    m_searcher(m_board, guiCommunication)
    {
    }

void SearchThread::run (void)
    {
    m_searcher.think(m_params);
    }

void SearchThread::start (SearchParams & params)
    {
    m_params = params;
    Thread::start();
    }

void SearchThread::stop (void)
    {
    m_searcher.finalize();
    }

void SearchThread::setBoard (Board & board)
    {
    m_board = board;
    }

Board & SearchThread::getBoard (void)
    {
    return m_board;
    }

/////////////////////////////////////////////////
// class SearchPool
/////////////////////////////////////////////////

SearchPool::SearchPool (void) :
    m_master(true)
    {
    }

void SearchPool::run (Board & board, SearchParams & params)
    {
    m_slaves.clear();

    for ( int32 i = 1; i < uci.config.threads; ++ i )
        {
        SearchThread * pThread = new SearchThread(false);
        pThread->setBoard(board);
        pThread->start(params);
        m_slaves.push_back(pThread);
        }

    m_master.start(params);
    m_master.join();

    vecThreads::iterator it = m_slaves.begin();

    while(it != m_slaves.end())
        {
        vecThreads::value_type pThread = * it++;

        pThread->stop();
        pThread->join();
        delete pThread;
        }
    m_slaves.clear();
    }

Board & SearchPool::getBoard (void)
    {
    return m_master.getBoard();
    }

bool SearchPool::getBestMove (uint32 & move)
    {
    return m_master.m_searcher.getBestMove(move);
    }

bool SearchPool::getPonderMove (uint32 & move)
    {
    return m_master.m_searcher.getPonderMove(move);
    }

uint64 SearchPool::getNodes (void)
    {
    uint64 nodes = m_master.m_searcher.getNodes();

    vecThreads::iterator it = m_slaves.begin();

    while(it != m_slaves.end())
        {
        vecThreads::value_type pThread = * it++;
        nodes += pThread->m_searcher.getNodes();
        }

    return nodes;
    }