#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "Arduino.h"

class Stopwatch
{
public:
    Stopwatch(bool autoStart = true) :
        m_startUSec(0),
        m_elapsedUSec(0)
    {
        if (autoStart)
            start();
    }

    inline uint32_t nowUSec()
    {
        // TODO handle wrapping
        return micros();
    }

    //  If stopwatch is curently running, elapsed time is since start.
    //  If stopwatch is curently stopped, elapsed time is the previous run (0 if none).
    //  Returns elapsed time, in microseconds.
    uint32_t elapsed()
    {
        if (m_startUSec)
            return nowUSec() - m_startUSec;
        else
            return m_elapsedUSec;
    }

    uint32_t lap()
    {
        if (m_startUSec)
        {
            uint32_t now = nowUSec();
            uint32_t elapsed = now - m_startUSec;
            m_startUSec = now;
            return elapsed;
        }
        else
        {
            start();
            return 0;
        }
    }

    uint32_t start()
    {
        m_startUSec = nowUSec();
        m_elapsedUSec = 0;
        return m_startUSec;
    }

    void rebase(int16_t delta)
    {
        m_startUSec += delta;
    }

    //  Stops the stopwatch, if it is running.
    //  Returns elapsed time, in microseconds, of the most recent run.
    uint32_t stop()
    {
        if (m_startUSec)
        {
            m_elapsedUSec = nowUSec() - m_startUSec;
            m_startUSec = 0;
        }
        return m_elapsedUSec;
    }

    bool running()
    {
        return m_startUSec != 0;
    }

protected:
    uint32_t m_startUSec;
    uint32_t m_elapsedUSec;
};

#endif
