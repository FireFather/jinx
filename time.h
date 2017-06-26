#ifndef TIME_H
#define TIME_H

#include "define.h"

class Time
    {
    public:

    typedef int32 type; // time in milli-seconds

    Time(void);
    void start(void);

    type getTimeReal(void);
    type getDiffReal(void);

    type getTimeCPU(void);
    type getDiffCPU(void);

    float64 getSecs(void);
    uint64 getNps(uint64 nodes);
    float64 getCpuUsage(void);
    private:
    type m_time_start_real;
    type m_time_start_cpu;
    };

#endif