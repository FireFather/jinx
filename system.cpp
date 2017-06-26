#include "define.h"
#include "system.h"
#include "console.h"

void banner (void)
    {
    console.sendline(name);
    console.send(__DATE__);
    console.send(" ");
    console.send(__TIME__);
    console.sendline("");
    }

#if defined(IS_WINDOWS)
uint32 getNumCores (void)
    {
    SYSTEM_INFO s;
    ::GetSystemInfo(& s);
    return (min(s.dwNumberOfProcessors, maxThreads));
    }
#else
uint32 getNumCores (void)
    {
#if defined(_SC_NPROCESSORS_ONLN)
    return (min(sysconf(_SC_NPROCESSORS_ONLN), maxThreads));
#else
    return 1;
#endif
    }
#endif