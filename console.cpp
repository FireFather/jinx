#include "define.h"
#include "console.h"

Console console;

Console::Console (void)
    {
    init();
    }

void Console::init (void)
    {
    m_waiting = true;

#if defined(IS_WINDOWS)

    DWORD dw;
    m_stdin = GetStdHandle(STD_INPUT_HANDLE);
    m_pipe = !GetConsoleMode(m_stdin, & dw);

    if(!m_pipe)
        {
        SetConsoleMode(m_stdin, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
        FlushConsoleInputBuffer(m_stdin);
        }
    else
        {
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stdout, NULL, _IONBF, 0);
        }
#endif

    }

bool Console::isinput (void)
    {
    if(m_waiting)
        return (true);

#if defined(IS_WINDOWS)
    //    if (stdin->_cnt > 0)
    //       return(true);

    if(m_pipe)
        {
        DWORD dw = 0;

        if(!PeekNamedPipe(m_stdin, NULL, 0, NULL, & dw, NULL))
            return (true);

        return (dw > 0);
        }
    else
        {
        return (0 != _kbhit());
        }

#else

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(& readfds);
    FD_SET(STDIN_FILENO, & readfds);
    select(STDIN_FILENO + 1, & readfds, NULL, NULL, & tv);
    return FD_ISSET(STDIN_FILENO, & readfds);

#endif

    }

void Console::send (const string & output)
    {
    printf("%s", output.c_str());
    }

void Console::sendline (const string & output)
    {
    printf("%s\n", output.c_str());
    }

bool Console::receive (string & input)
    {
    if(isinput())
        {
        if(fgets(m_buffer, sizeof(m_buffer), stdin))
            {
            m_buffer[strlen(m_buffer) - 1] = 0;
            input = m_buffer;

            return (true);
            }
        }
    return (false);
    }

void Console::waiting (bool set)
    {
    m_waiting = set;
    }