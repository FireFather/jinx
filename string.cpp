#include "define.h"

string int32ToStr (const int32 & val)
    {
    stringstream s;
    s << val;
    return s.str();
    }

string int64ToStr (const int64 & val)
    {
    stringstream s;
    s << val;
    return s.str();
    }

string floatToStr (const float64 & val)
    {
    stringstream s;
    s << val;
    return s.str();
    }

string boolToStr (bool val)
    {
    return val ? "true" : "false";
    }

string boolToNum (bool val)
    {
    return val ? "1" : "0";
    }

int32 strToInt32 (const string & str, int32 def)
    {
    if(isNumber(str))
        return (atol(str.c_str()));
    return def;
    }

int64 strToInt64 (const string & str, int64 def)
    {
    if(isNumber(str))
        return (atol(str.c_str()));
    return def;
    }

bool strToBool (const string & str, bool def)
    {
    if("true" == str || "1" == str)
        return true;

    else if("false" == str || "0" == str)
        return false;

    else
        return def;
    }

bool isNumber (const string & str)
    {
    return !str.empty() && str.find_first_not_of("-.0123456789") == string::npos;
    }

void split (strings & result, const string & source, const string & separator, bool skipEmpty)
    {
    string copy = source;

    bool check(true);

    while(check)
        {
        string::size_type pos = copy.find(separator);

        if(string::npos != pos)
            {
            string substr = copy.substr(0, pos);

            if(!(skipEmpty && substr.empty()))
                result.push_back(substr);
            copy.erase(0, pos + separator.length());
            check = true;
            }
        else
            {
            result.push_back(copy);
            check = false;
            }
        }
    }

string join (const strings & values, const string & separator)
    {
    string result;

    for ( size_t i = 0; i < values.size(); i++ )
        result += (0 == i ? "" : separator) + values[i];
    return result;
    }

string trim (const string & str)
    {
    string tr = str;
    tr.erase(0, tr.find_first_not_of(' ')); //prefixing spaces
    tr.erase(tr.find_last_not_of(' ') + 1); //suffixing spaces
    return tr;
    }

string::value_type toggleChar (const string::value_type & chr)
    {
    return string::value_type(islower(chr) ? toupper(chr) : tolower(chr));
    }