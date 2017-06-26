#ifndef CONFIG_H
#define CONFIG_H

#include "define.h"
#include "time.h"

class Config
    {
    public:

    Config(void);
    void init(void);
    strings getUCIOptions(void);
    void setUCIOption(const string & name, const string & value);
    public:
    int32 hash;
    int32 threads;
    bool ponder;
    bool chess960;
    int32 multiPV;
    int32 contempt;
    private:
    strings m_lines;
    strings m_options_uci;

    string get(const string & key, const string & def);
    string initCombo(const string & name, const string & name_ini, const string & def, const strings & values);
    int32 initSpin(const string & name, const string & name_ini, int32 def, int32 min, int32 max);
    bool initCheck(const string & name, const string & name_ini, bool def);
    };

class SearchParams
    {
    public:
    SearchParams(void);
    void reset(void);
    public:
    bool ponder;         // start searching in pondering mode
    bool infinite;       // search until the "stop" command
    Time::type wtime;    // white has x msec left on the clock
    Time::type btime;    // black has x msec left on the clock
    Time::type winc;     // white increment per move in mseconds if x > 0
    Time::type binc;     // black increment per move in mseconds if x > 0
    uint32 movestogo;    // there are x moves to the next time control,
    int32 depth;         // search x plies only.
    uint32 nodes;        // search x nodes only,
    uint32 mate;         // search for a mate in x moves
    Time::type movetime; // search exactly x mseconds
    uint32 movenum;      // movenumber
    };

#endif