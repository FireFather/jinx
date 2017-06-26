#ifndef DEFINE_H
#define DEFINE_H

#if defined(_MSC_VER)
#define IS_WINDOWS
#else
#define IS_LINUX
#endif

#if defined(IS_WINDOWS)
#include <windows.h>
#include <conio.h>
#include <io.h>
#include <time.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdint.h>
#endif

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <nmmintrin.h>

using namespace std;

#if (defined(_WIN64) || defined(__64BIT__))
#define IS_64BIT
const string platform = "x64";
#else
#define IS_32BIT
const string platform = "x86";
#endif

#ifdef IS_64BIT
const bool is64Bit = true;
#else
const bool is64Bit = false;
#endif

#define HW_POPCNT

#ifdef HW_POPCNT
const bool hasPopCnt = true;
#else
const bool hasPopCnt = false;
#endif

const string engine = "Jinx";
const string version = "1.0";
const string author = "Kranium";
const string name = engine + " " + version + " " + platform;

#define maxThreads 32
#define timeUndef -1

#if defined(IS_WINDOWS)
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;
typedef signed __int64 int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned __int64 uint64;
#else
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif

typedef float float32;
typedef double float64;
typedef long double float80;

typedef vector<string> strings;

// Prototypes
string int32ToStr (const int32 & val);
string int64ToStr (const int64 & val);
string floatToStr (const float64 & val);
string boolToStr (bool val);
string boolToNum (bool val);
int32 strToInt32 (const string & str, int32 def = 0);
int64 strToInt64 (const string & str, int64 def = 0);
bool strToBool (const string & str, bool def = false);

bool isNumber (const string & str);
void split (strings & result, const string & source, const string & separator = " ", bool skipEmpty = true);
string join (const strings & values, const string & separator = " ");
string trim (const string & str);
string::value_type toggleChar (const string::value_type & chr);
#endif