#ifndef SEARCH_H
#define SEARCH_H

#include "define.h"
#include "movegen.h"
#include "uci.h"
#include "thread.h"
#include "multipv.h"

static int32 nmReduction = 3;
static int32 nmMinDepth = 2;
static int32 lmrMinDepth = 3;
static int32 lmrMinMoves = 4;
static int32 fpMinDepth = 4;
static int32 nmPVminDepth = 6;

const Time::type infoTime = 1000;
const int32 nodeCheck = 8191;
const int32 searchMaxDepth = 256;

const int32 historySortMin = - 32000;
const int32 historySortMax = 32000;

static int32 futilityMargin[4] =
    {
    0, 50, 350, 550
    };

int32 getMateDist (int32 eval);

class MoveListParams
    {
    public:
    MoveListParams(void);
    void init(int32 curPly);
    public:
    int32 ply;
    int32 phase;
    int32 index;
    uint32 hash;
    uint32 killer1;
    uint32 killer2;
    MoveList caps;
    MoveList quiets;
    MoveList quiets_pst;
    MoveList bads;
    };

class Search
    {
    public:
    Search(Board & board, bool guiCom);

    bool think(const SearchParams & params);
    bool getBestMove(uint32 & move);
    bool getPonderMove(uint32 & move);
    void finalize(void);
    uint64 getNodes(void);
    int32 getSEE(uint32 & move);
    private:
    Board * m_board;
    Board & board(void);
    SearchParams m_params;
    MoveGen m_movegen;
    MoveListParams m_movelist[searchMaxDepth];
    Eval m_eval;
    bool m_finalize;
    bool m_guiCom;
    bool m_extinfos;
    bool m_ponder;
    Time m_time_start;
    Time m_time_info;
    Time::type m_time_hard;
    Time::type m_time_soft;
    uint64 m_nodes;
    int32 m_maxqply;

    int32 root(int32 depth, int32 ply, int32 alpha, int32 beta);
    int32 alphabeta(int32 depth, int32 ply, int32 alpha, int32 beta, bool null = false);
    int32 quiescence(int32 ply, int32 qply, int32 alpha, int32 beta);
    bool nextMove(Move & move, MoveListParams & movelist);
    bool nextMoveQ(Move & move, MoveListParams & movelist);
    void scoreCaptures(MoveList & moves);
    void scoreQuiets(MoveList & moves);
    void scoreQuietsPst(MoveList & moves);
    bool isBadCapture(const Move & move);
    bool isTactical(const Move & move);
    bool doNullMove(void);
    int32 see(const Move & move);
    void check(void);
    void sendInfoRootPV(int32 depth = 0, int32 eval = drawValue, int32 alpha = - mateValue, int32 beta = mateValue);
    void sendInfoMultiPV(int32 depth = 0);
    void sendInfoProgress(void);
    bool isMultiPV(void);
    private:
    MoveList m_pv[searchMaxDepth + 2];
    MoveList m_pvRoot;
    MoveList m_pvCurr;
    MultiPV m_multiPV;
    void buildPv(int32 ply, Move & move);
    private:

    struct History
        {
        int32 sort;
        int32 success;
        int32 failure;

        History (void)
            {
            reset();
            }

        void reset (void)
            {
            sort = historySortMin;
            success = 0;
            failure = 0;
            }
        };
    History m_history[12][64];
    uint32 m_killers[searchMaxDepth][2];
    void addKiller(int32 ply, const Move & move);
    bool getKiller(int32 ply, Move & move, uint32 pos = 1);
    void clearHistory(void);
    void updateHistoryFailure(Piece piece, Square to);
    void updateHistorySuccess(Piece piece, Square to);
    void updateHistorySort(Piece piece, Square to, int32 depth, bool betacut = false);
    bool isReductHistory(Piece piece, Square to);
    private:
    void recordHash(int32 eval, int32 depth, int32 ply, HashType type, uint32 move);
    bool findHash(int32 & eval, int32 depth, int32 ply, int32 alpha, int32 beta);
    bool findHashMove(Move & move);
    void getHashPV(MoveList & pv, int32 depth);
    };

class SearchThread :
    public Thread
    {
    friend class SearchPool;
    public:
    SearchThread(bool guiCommunication);
    void run(void);
    void start(SearchParams & params);
    void stop(void);

    void setBoard(Board & board);
    Board & getBoard(void);
    private:
    Board m_board;
    SearchParams m_params;
    Search m_searcher;
    };

class SearchPool
    {
    public:
    SearchPool(void);

    void run(Board & board, SearchParams & params);

    Board & getBoard(void);

    bool getBestMove(uint32 & move);
    bool getPonderMove(uint32 & move);
    uint64 getNodes(void);
    private:
    typedef vector<SearchThread *> vecThreads;
    vecThreads m_slaves;
    SearchThread m_master;
    };

extern SearchPool gl_search;

#endif