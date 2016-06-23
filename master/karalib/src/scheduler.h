#ifndef KAR_SCHEDULER_H_
#define KAR_SCHEDULER_H_
#include <windows.h>
#include <stdint.h>

namespace kar
{

struct Time
{
    unsigned long hh;
    unsigned long mm;
    unsigned long ss;
    unsigned long ms;
    unsigned long us;
};


class Scheduler
{
    struct _IType { };
    typedef double (_IType::*p_ptr)(double, const double*);

    _IType* _data;
    p_ptr   _pptr;
    LARGE_INTEGER _first, _cur;
    double  _div;
    double  _microf;
    double  _ppqn;  //!< pulse per quarter note (in microseconds).
    double  _tdiv;  //!< time divisor.
    bool    _playing;
protected:

public:
    Scheduler();
    ~Scheduler();
    template <class A>
    void Set(A& dat, double (A::*t_p)(double, const double*));
    /**
     * set the pulse per quarter note
     * @param ppq pulse per quarter note
     */
    void SetTimeDivision(double td);
    void SetPulsePerQuarterNote(double ppq);
    bool Poll();
    bool Empty() const;
    void Clear();
};


template <class A>
void Scheduler::Set(A& dat, double (A::*t_p)(double, const double*))
{
    _data = (_IType*) &dat;
    _pptr = (p_ptr) t_p;
}

}

#endif
