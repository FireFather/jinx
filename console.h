#ifndef CONSOLE_H
#define CONSOLE_H

#include "define.h"

class Console
    {
    public:

    Console(void);
    void send(const string & output);
    void sendline(const string & output);
    protected:
    void init(void);
    bool isinput(void);
    bool receive(string & input);
    void waiting(bool set);
    private:
    bool m_waiting;
    int32 m_pipe;
    void * m_stdin;
    char m_buffer[4096];
    };

extern Console console;

#endif