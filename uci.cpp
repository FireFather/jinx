#include "uci.h"
#include "perft.h"
#include "search.h"

UCI uci;

UCI::UCI (void)
    {
    m_movenum = 0;
    m_ext_cast_fen = false;
    }

void UCI::init (void)
    {
    config.init();
    applyConfig();
    }

void UCI::run (void)
    {
    string cmdline;

    bool loop(true);

    while(loop)
        {
        if(nextCommand(cmdline))
            {
            Command cmd(cmdline);

            if(cmd.getCommand() == "uci")
                {
                uciStart();
                continue;
                }
            else if(cmd.getCommand() == "ucinewgame")
                {
                uciNewGame();
                continue;
                }
            else if(cmd.getCommand() == "isready")
                {
                uciReady();
                continue;
                }
            else if(cmd.getCommand() == "setoption")
                {
                uciSetOption(cmd);
                continue;
                }
            else if(cmd.getCommand() == "position")
                {
                uciPosition(cmd);
                continue;
                }
            else if(cmd.getCommand() == "go")
                {
                uciGo(cmd);
                continue;
                }
            else if(cmd.getCommand() == "perft")
                {
                perft(cmd);
                continue;
                }
            else if(cmd.getCommand() == "divide")
                {
                divide(cmd);
                continue;
                }
            else if(cmd.getCommand() == "quit")
                {
                loop = false;
                continue;
                }
            else if(cmd.getCommand() == "exit")
                {
                loop = false;
                continue;
                }
            }
        }
    }

bool UCI::nextCommand (string & command)
    {
    if(m_cmdstack.empty())
        return receive(command);
    else
        {
        command = m_cmdstack.front();
        m_cmdstack.pop_front();

        if(!command.empty())
            {
            sendInfo(command);
            return true;
            }
        }
    return false;
    }

void UCI::uciStart (void)
    {
    sendline("id name " + name);
    sendline("id author " + author);
    strings options = config.getUCIOptions();

    for ( size_t i = 0; i < options.size(); ++ i )
        sendline(options[i]);
    sendline("uciok");
    }

void UCI::uciNewGame (void)
    {
    newGame();
    }

void UCI::uciReady (void)
    {
    applyConfig();
    sendline("readyok");
    }

void UCI::uciSetOption (const Command & cmd)
    {
    string name = cmd.getMultiValue("name", "value");
    string value = cmd.getSingleValue("value");
    config.setUCIOption(name, value);
    }

void UCI::uciPosition (const Command & cmd)
    {
    string setfen, moves;

    if(cmd.isParam("moves"))
        {
        setfen = cmd.getMultiValue("fen", "moves");
        moves = cmd.getSingleValue("moves");
        }
    else
        setfen = cmd.getSingleValue("fen");

    setFen(setfen);

    if(!moves.empty())
        {
        strings movelist;
        split(movelist, moves);

        strings::iterator it = movelist.begin();

        while(it != movelist.end())
            doMove(* it++);
        }
    }

void UCI::uciGo (const Command & cmd)
    {
    SearchParams params;
    params.ponder = cmd.getValueBool("ponder", params.ponder);
    params.infinite = cmd.getValueBool("infinite", params.infinite);
    params.wtime = cmd.getValueInt("wtime", params.wtime);
    params.btime = cmd.getValueInt("btime", params.btime);
    params.winc = cmd.getValueInt("winc", params.winc);
    params.binc = cmd.getValueInt("binc", params.binc);
    params.movestogo = cmd.getValueInt("movestogo", params.movestogo);
    params.depth = cmd.getValueInt("depth", params.depth);
    params.nodes = cmd.getValueInt("nodes", params.nodes);
    params.mate = cmd.getValueInt("mate", params.mate);
    params.movetime = cmd.getValueInt("movetime", params.movetime);

    goEngine(params);

    uint32 bestmove;

    if(gl_search.getBestMove(bestmove))
        {
        uint32 pondermove;

        if((config.ponder || isArenaUCI960()) && gl_search.getPonderMove(pondermove))
            sendBestMove(bestmove, pondermove);
        else
            sendBestMove(bestmove);
        }
    }

void UCI::goEngine (SearchParams params)
    {
    waiting(false);

    params.movenum = ++ m_movenum;

    if(params.infinite)
        gl_hashtable.reset();
    else
        gl_hashtable.aging();

    gl_egHits.clear();

    m_perf.start();

    gl_search.run(board(), params);

    waiting(true);
    }

void UCI::perft (const Command & cmd)
    {
    int32 depth = 0;

    strings result;
    split(result, cmd.getSingleValue("perft"));

    for ( size_t i = 0; i < result.size(); i++ )
        {
        if(isNumber(result[i]))
            depth = strToInt32(result[i]);
        }

    if(depth)
        {
        Fen fen(board());
        Perft test;
        test.perft(fen.get(), depth);
        }
    }

void UCI::divide (const Command & cmd)
    {
    int32 depth = 0;

    strings result;
    split(result, cmd.getSingleValue("divide"));

    for ( size_t i = 0; i < result.size(); i++ )
        {
        if(isNumber(result[i]))
            depth = strToInt32(result[i]);
        }

    if(depth)
        {
        Fen fen(board());
        Perft test;
        test.divide(fen.get(), depth);
        }
    }

void UCI::newGame (void)
    {
    setFen("");
    m_movenum = 0;
    gl_hashtable.reset();
    }

void UCI::applyConfig (void)
    {
    // system options
    gl_hashtable.resize(config.hash);
    }

bool UCI::checkInput (bool & stop, bool & ponderhit)
    {
    string cmdline;

    if(receive(cmdline))
        {
        stop = ("stop" == cmdline);
        ponderhit = ("ponderhit" == cmdline);
        return (stop || ponderhit);
        }
    return (false);
    }

void UCI::sendInfo (const string & info)
    {
    sendline("info string " + info);
    }

void UCI::sendBestMove (const Move & best, const Move & ponder)
    {
    if(ponder != Move())
        sendline("bestmove " + moveToStr(best) + " ponder " + moveToStr(ponder));
    else
        sendline("bestmove " + moveToStr(best));
    }

void UCI::sendCurrMove (const Move & move, uint32 number, uint32 depth)
    {
    sendline("info currmove " + moveToStr(move) + " currmovenumber " + int32ToStr(number));
    }

void UCI::sendBestLine (uint32 depth, uint32 seldepth, int32 eval, int32 alpha, int32 beta, const MoveList & line,
    int32 multipv)
    {
    string info = "info";
    info += " depth " + int32ToStr(depth);
    info += " seldepth " + int32ToStr(seldepth);
    info += " time " + int32ToStr(m_perf.getDiffReal());
    info += " nodes " + int64ToStr(gl_search.getNodes());
    info += " nps " + int64ToStr(m_perf.getNps(gl_search.getNodes()));
    info += " score";
    int32 matedist = getMateDist(eval);

    if(0 != matedist)
        {
        if(0 < matedist)
            info += " mate " + int32ToStr(int32(matedist / 2) + 1);
        else
            info += " mate " + int32ToStr(int32(matedist / 2));
        }
    else
        info += " cp " + int32ToStr(eval);

    if(eval > beta)
        info += " lowerbound"; // = fail-high

    if(eval < alpha)
        info += " upperbound"; // = fail-low
    info += " hashfull " + int32ToStr(gl_hashtable.usage());

    if(0 < multipv)
        info += " multipv " + int32ToStr(multipv);
    info += " pv " + getMoveLine(line);
    sendline(info);
    }

void UCI::sendProgress (void)
    {
    string info = "info";
    info += " time " + int32ToStr(m_perf.getDiffReal());
    info += " nodes " + int64ToStr(gl_search.getNodes());
    info += " nps " + int64ToStr(m_perf.getNps(gl_search.getNodes()));
    info += " cpuload " + int32ToStr((int32(m_perf.getCpuUsage() * 1000.0)));
    info += " hashfull " + int32ToStr(gl_hashtable.usage());
    sendline(info);
    }

void UCI::printBoard (void)
    {
    sendline("");

    for ( int32 y = 7; y >= 0; y-- )
        {
        string line;

        for ( int32 x = 0; x < 8; x++ )
            {
            Piece piece = board().getPiece(Square(x + 8 * y));
            string pc = pieceToStr(piece);
            line += pc.empty() ? "." : pc;
            }
        sendline(line);
        }
    sendline("");
    }

void UCI::doMove (const string & movestr)
    {
    uint32 move = strToMove(movestr);

    if(!(move && board().doMove(move)))
        sendInfo("Illegal move: " + movestr);
    }

void UCI::undoMove (void)
    {
    board().undoLastMove();
    }

string UCI::moveToStr (const uint32 & move)
    {
    return::moveToStr(move, board(), isArenaUCI960());
    }

uint32 UCI::strToMove (const string & str)
    {
    return::strToMove(str, board());
    }

string UCI::getMoveLine (const MoveList & line)
    {
    string strline;

    for ( int32 i = 0; i < line.size(); ++ i )
        {
        if(!strline.empty())
            strline += " ";
        strline += moveToStr(line[i]);
        }
    return strline;
    }

uint32 UCI::getMoveCount (void)
    {
    uint32 count(0);

    MoveGen movegen(board());
    MoveList moves;
    movegen.getAll(moves);

    for ( int32 i = 0; i < moves.size(); ++ i )
        {
        Move move = moves[i];

        if(board().doMove(move))
            {
            ++ count;
            board().undoMove(move);
            }
        }
    return count;
    }

string UCI::getFen (void)
    {
    Fen fen(board());
    return fen.get();
    }

void UCI::setFen (const string & fenstr)
    {
    Fen fen(board());
    fen.set(fenstr);
    m_ext_cast_fen = fen.isCastExt(fenstr);
    }

bool UCI::isArenaUCI960 (void)
    {
    return config.chess960 && !m_ext_cast_fen;
    }

Board & UCI::board (void)
    {
    return gl_search.getBoard();
    }