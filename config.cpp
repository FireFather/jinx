#include "define.h"
#include "hash.h"
#include "uci.h"
#include "search.h"
#include "config.h"
#include "system.h"

Config::Config (void)
    {
    }

void Config::init (void)
    {
    m_lines.clear();
    m_options_uci.clear();

    if(is64Bit)
        hash = initSpin("Hash", "", 128, 0, 32768);
    else
        hash = initSpin("Hash", "", 128, 0, 1024);

    threads = initSpin("Threads", "", 1, 1, getNumCores());
    multiPV = initSpin("MultiPV", "", 1, 1, maxMPV);
    contempt = initSpin("Contempt", "", 0, - 100, 100);
    ponder = initCheck("Ponder", "", false);
    chess960 = initCheck("UCI_Chess960", "Chess960", false);
    }

void Config::setUCIOption (const string & name, const string & value)
    {
    if(name == "Hash")
        hash = strToInt32(value, hash);

    if(name == "Threads")
        threads = strToInt32(value, threads);

    if(name == "MultiPV")
        multiPV = strToInt32(value, multiPV);

    if(name == "Contempt")
        contempt = strToInt32(value, contempt);

    if(name == "Ponder")
        ponder = strToBool(value, ponder);

    if(name == "UCI_Chess960")
        chess960 = strToBool(value, chess960);
    }

strings Config::getUCIOptions (void)
    {
    return m_options_uci;
    }

string Config::get (const string & key, const string & def)
    {
    strings::iterator it = m_lines.begin();

    while(it != m_lines.end())
        {
        strings::value_type & line = * it++;

        strings valueline;
        split(valueline, line, "#");

        if(!valueline.empty() && !valueline[0].empty())
            {
            strings values;
            split(values, valueline[0], "=");

            if(2 == values.size() && key == trim(values[0]))
                return trim(values[1]);
            }
        }
    return def;
    }

string Config::initCombo (const string & name, const string & name_ini, const string & def, const strings & values)
    {
    string value = get((name_ini.empty() ? name : name_ini), def);
    m_options_uci.push_back(("option name " + name + " type combo var " + join(values, " var ") + " default " + value));
    return value;
    }

int32 Config::initSpin (const string & name, const string & name_ini, int32 def, int32 min, int32 max)
    {
    string value = get((name_ini.empty() ? name : name_ini), int32ToStr(def));
    m_options_uci.push_back(("option name " + name + " type spin min " + int32ToStr(min) + " max " + int32ToStr(max)
        + " default " + value));
    return strToInt32(value);
    }

bool Config::initCheck (const string & name, const string & name_ini, bool def)
    {
    string value = get((name_ini.empty() ? name : name_ini), boolToStr(def));
    m_options_uci.push_back(("option name " + name + " type check default " + value));
    return strToBool(value);
    }

SearchParams::SearchParams (void)
    {
    reset();
    }

void SearchParams::reset (void)
    {
    ponder = false;
    infinite = false;
    wtime = timeUndef;
    btime = timeUndef;
    winc = 0;
    binc = 0;
    movestogo = 40;
    depth = 0;
    nodes = 0;
    mate = 0;
    movetime = timeUndef;
    movenum = 0;
    }