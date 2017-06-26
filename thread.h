#ifndef THREAD_H
#define THREAD_H

struct ThreadData;

class Thread
    {
    public:

    Thread(void);
    virtual ~Thread(void);
    void publicRun(void);
    void start(void);
    void join(void);
    private:
    ThreadData * m_data;
    virtual void run(void) = 0;
    };

#endif