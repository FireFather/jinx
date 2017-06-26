#ifndef UCI_H
#define UCI_H

#include "define.h"
#include "board.h"
#include "console.h"
#include "command.h"
#include "config.h"

class UCI :
    public Console
    {
    public:

    UCI(void);
    void init(void);
    void run(void);
    public:
    // engine to GUI
    bool checkInput(bool & stop, bool & ponderhit);
    void sendInfo(const string & info);
    void sendBestMove(const Move & best, const Move & ponder = Move());
    void sendCurrMove(const Move & move, uint32 number, uint32 depth);
    void sendBestLine(uint32 depth, uint32 seldepth, int32 eval, int32 alpha, int32 beta, const MoveList & line,
        int32 multipv = 0);
    void sendProgress(void);
    public:
    Config config;
    private:
    // UCI interface
    void uciStart(void);
    void uciNewGame(void);
    void uciReady(void);
    void uciSetOption(const Command & cmd);
    void uciPosition(const Command & cmd);
    void uciGo(const Command & cmd);
    void perft(const Command & cmd);
    void divide(const Command & cmd);
    void printBoard(void);
    private:
    Board & board(void);
    Time m_perf;
    uint32 m_movenum;
    bool m_ext_cast_fen;

    void newGame(void);
    void goEngine(SearchParams params);
    void doMove(const string & movestr);
    void undoMove(void);
    void applyConfig(void);
    string moveToStr(const uint32 & move);
    uint32 strToMove(const string & str);
    string getMoveLine(const MoveList & line);
    uint32 getMoveCount(void);
    string getFen(void);
    void setFen(const string & fenstr);
    bool isArenaUCI960(void); // there are some exceptions
    private:
    list<string> m_cmdstack;
    bool nextCommand(string & command);
    };

extern UCI uci;

#endif