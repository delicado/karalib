#ifndef KAR_MIDIOUT_H_
#define KAR_MIDIOUT_H_
#include "out.h"
#include <stdint.h>
#include <windows.h>
#include <mmsystem.h>

namespace kar
{

class MidiOut : public OutBase
{
    bool _ready;
    HMIDIOUT _handle;
private:
    void InternalInit();
public:
    MidiOut();
    MidiOut(unsigned long port);
    ~MidiOut();
    bool Open(unsigned long port);
    bool Send(uint8_t event, uint8_t param1, uint8_t param2);
    void Reset();
    void Close();
};


}
#endif
