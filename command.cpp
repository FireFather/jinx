#include "command.h"

Command::Command (const string & line)
    {
    m_line = line;
    }

string Command::getLine (void) const
    {
    return m_line;
    }

string Command::getCommand (void) const
    {
    strings result;
    split(result, m_line);

    if(!result.empty())
        return (result[0]);
    return (string());
    }

bool Command::isParam (const string & param) const
    {
    string _cmd = ' ' + m_line + ' ';
    string _param = ' ' + param + ' ';
    return (string::npos != _cmd.find(_param, 0));
    }

string Command::getSingleValue (const string & param) const
    {
    string::size_type pos = m_line.find(param);

    if(string::npos != pos)
        return (trim(m_line.substr(pos + param.size())));

    return (string());
    }

string Command::getMultiValue (const string & param, const string & nextparam) const
    {
    string::size_type pos1 = m_line.find(param);
    string::size_type pos2 = m_line.find(nextparam);

    if((string::npos != pos1) && (string::npos != pos2))
        {
        pos1 += param.size();
        return (trim(m_line.substr(pos1, pos2 - pos1)));
        }
    return (string());
    }

string Command::getNextValue (const string & param) const
    {
    strings result;
    split(result, getSingleValue(param));

    if(!result.empty())
        return (result[0]);
    return (string());
    }

int32 Command::getValueInt (const string & param, int32 def) const
    {
    if(isParam(param))
        {
        string sVal = getNextValue(param);
        return (strToInt32(sVal, def));
        }
    return (def);
    }

bool Command::getValueBool (const string & param, bool def) const
    {
    if(isParam(param))
        return (true);
    return (def);
    }