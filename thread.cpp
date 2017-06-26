#include "define.h"
#include "thread.h"

#if defined(IS_WINDOWS)
typedef HANDLE HThread;
#define thread_create(x,f,t) (x = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)f,t,0,0))
#define thread_join(x) { WaitForSingleObject(x, INFINITE); CloseHandle(x); }

typedef HMODULE HModule;
#define lib_load(x) LoadLibraryA(x)
#define lib_proc GetProcAddress
#define lib_free(x) FreeLibrary(x)

#else

typedef pthread_t HThread;
typedef void* (*pt_start_fn)(void*);
#define thread_create(x,f,t) pthread_create(&(x),NULL,(pt_start_fn)f,t)
#define thread_join(x) pthread_join(x, NULL)

typedef void * HModule;
#define lib_load(x) dlopen(x,RTLD_LAZY)
#define lib_proc dlsym
#define lib_free(x) dlclose(x)

#endif

struct ThreadData
    {
    HThread m_handle;

    ThreadData (void)
        {
        m_handle = 0;
        }
    };

extern "C"
    {
    void * threadRunFunction (void * pvoid)
        {
        static_cast<Thread *>(pvoid)->publicRun();
        return (0);
        }
    }

Thread::Thread (void)
    {
    m_data = new ThreadData();
    }

Thread::~Thread (void)
    {
    delete m_data;
    }

void Thread::start (void)
    {
    thread_create(m_data->m_handle, threadRunFunction, this);
    }

void Thread::join (void)
    {
    thread_join(m_data->m_handle);
    }

void Thread::publicRun (void)
    {
    run();
    }