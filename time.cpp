#include "define.h"
#include "time.h"

Time::Time (void)
    {
    start();
    }

void Time::start (void)
    {
    m_time_start_real = getTimeReal();
    m_time_start_cpu = getTimeCPU();
    }

Time::type Time::getTimeReal (void)
    {
#if defined(IS_WINDOWS)

    return (GetTickCount());
#else

    timeval time;
    gettimeofday(& time, 0);
    return (time.tv_sec * 1000 + time.tv_usec / 1000);

#endif

    }

Time::type Time::getDiffReal (void)
    {
    return getTimeReal() - m_time_start_real;
    }

Time::type Time::getTimeCPU (void)
    {
#if defined(IS_WINDOWS)

    return (clock());

#else

    struct rusage ru[1];
    getrusage(RUSAGE_SELF, ru);
    return (ru->ru_utime.tv_sec * 1000 + ru->ru_utime.tv_usec / 1000);

#endif

    }

Time::type Time::getDiffCPU (void)
    {
    return getTimeCPU() - m_time_start_cpu;
    }

float64 Time::getSecs (void)
    {
    return getDiffReal() / 1000.0;
    }

uint64 Time::getNps (uint64 nodes)
    {
    float64 time = getSecs();
    return time ? (uint64)((float64)nodes / time) : nodes;
    }

float64 Time::getCpuUsage (void)
    {
    float64 real = getDiffReal();
    float64 cpu = getDiffCPU();

    if(real <= 0.0 || cpu <= 0.0)
        return 0.0;

    float64 usage = cpu / real;

    if(usage >= 1.0)
        usage = 1.0;

    return usage;
    }