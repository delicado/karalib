#include "scheduler.h"

namespace kar
{


Scheduler::Scheduler() : _data(NULL), _pptr(NULL), _playing(false)
{
    LARGE_INTEGER t;
    QueryPerformanceFrequency(&t);
    _div = double(t.QuadPart) / 1000000.0;
}

Scheduler::~Scheduler()
{

}

/**
 * set the pulse per quarter note
 * @param ppq pulse per quarter note
 */

void Scheduler::SetTimeDivision(double td)
{
    _tdiv = td;
}

void Scheduler::SetPulsePerQuarterNote(double ppq)
{
    _ppqn = ppq / _tdiv;
}

bool Scheduler::Poll()
{
    bool status = false;
    if(_playing )
    {
        QueryPerformanceCounter(&_cur);
        double temp = (_cur.QuadPart - _first.QuadPart) / _div;
        temp = (_data->*_pptr)(temp, &_ppqn);
        if(temp > 0.0) status = true;

    } else
    {
        QueryPerformanceCounter(&_first);
        _playing = true;
        status = true;
    }

    if(status == false) _playing = false;
    return status;
}

bool Scheduler::Empty() const
{
    return (_data == NULL);
}

void Scheduler::Clear()
{
    _data = NULL;
    _pptr = NULL;
}

}
