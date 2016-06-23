
#include "error.h"
#include "midiout.h"

namespace kar
{


MidiOut::MidiOut()
{
    this->InternalInit();
}

MidiOut::MidiOut(unsigned long port)
{
    this->InternalInit();
    if(this->Open(port) == true) throw kar::KarBadError("midiout::midiout(unsigned long port) : failed to open a MIDI port.");
}

MidiOut::~MidiOut()
{
    this->Close();
}

void MidiOut::InternalInit()
{
    _ready = false;
}

bool MidiOut::Open(unsigned long port)
{
    bool status = true;
    UINT error = midiOutOpen(&_handle, port, 0, 0, CALLBACK_NULL);
    if(error == MMSYSERR_NOERROR)
    {
        status = false;
        _ready = true;
    }
    return status;
}

bool MidiOut::Send(uint8_t event, uint8_t param1, uint8_t param2)
{
    bool status = true;
    union
    {
        uint32_t dword;
        uint8_t  byte[4];
    } msg;

    msg.byte[0] = event;
    msg.byte[1] = param1;
    msg.byte[2] = param2;
    if(midiOutShortMsg(_handle, msg.dword) == MMSYSERR_NOERROR)
        status = false;
    return status;
}

void MidiOut::Reset()
{
    midiOutReset(_handle);
}

void MidiOut::Close()
{
    if(_ready)
    {
        midiOutClose(_handle);
        _ready = false;
    }
}

}
