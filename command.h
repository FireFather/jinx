#ifndef COMMAND_H
#define COMMAND_H

#include "define.h"

class Command
    {
    public:

    Command(const string & line);

    string getLine(void)const;
    string getCommand(void)const;
    bool isParam(const string & param)const;

    string getSingleValue(const string & param)const;
    string getMultiValue(const string & param, const string & nextparam)const;
    string getNextValue(const string & param)const;

    int32 getValueInt(const string & param, int32 def = 0)const;
    bool getValueBool(const string & param, bool def = false)const;
    private:
    string m_line;
    };

#endif