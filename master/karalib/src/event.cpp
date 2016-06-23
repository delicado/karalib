/**
 * @file event.h
 * source file for event.h
 * @author Vester
 * @version 0.1
 */

#include "event.h"

namespace kar
{

Event::Event() { }

Event::Event(uint8_t rs)
{
    _running_status = rs;
}

Event::Event(const Event& e)
{
    _running_status  = e._running_status;
    delta_time      = e.delta_time;
    event_type      = e.event_type;
    callback_type   = e.callback_type;
    memcpy(&ev, &e.ev, sizeof(ev));
    data_size       = e.data_size;
    data            = e.data;
}

void Event::InternalInit()
{
    delta_time = 0;
    event_type = 0;
    callback_type = 0;
    data_size = 0;
    data = NULL;
    memset(&ev, 0, sizeof(ev));
}

void Event::SetRunningStatus(uint8_t running_stat)
{
    _running_status = running_stat;
}

uint8_t Event::GetRunningStatus() const
{
    uint8_t out = event_type;
    out &= 0xF0;
    switch(out)
    {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xC0:
    case 0xD0:
    case 0xE0:
        out = event_type;
        break;
    case 0xF0:
        if((event_type & 0x0F) < 8)
            out = 0;
        break;
    default:
        out = event_type;
    }
    return out;
}

bool Event::ReadEvent(const char* data, unsigned long size, unsigned long& index)
{
    bool status = false;
    if(index < size)
    {
        this->InternalInit();
        _start_addr = (uint8_t*)&data[index];
        delta_time = ReadVLQ(data, index);
        event_type = data[index++];
        if(!(event_type & 0x80) && _running_status != 0)
        {
            event_type = _running_status;
            index -= 1;
        }

        switch(event_type & 0xF0)
        {
        case 0xF0: // meta events
            if(event_type == 0xFF)
            {
                ev.meta.type = data[index++];
                ev.meta.data_size = ReadVLQ(data, index);
                ev.meta.data = const_cast<char*>(&data[index]);
                index += ev.meta.data_size;
                callback_type = META;
            }
            else if(event_type == 0xF0 || event_type == 0xF7)
            {
                ev.sys.data_size = ReadVLQ(data, index);
                ev.sys.data = const_cast<char*>(&data[index]);
                index += ev.sys.data_size;
                callback_type = SYSTEM;
            } else
            {
                throw UnrecognizedEvent(event_type);
            }
            break;
        case 0x80: // note off
        case 0x90: // note on
        case 0xA0: // note aftertouch
        case 0xB0: // controller
        case 0xE0: // pitch event
            ev.midi.type = event_type;
            ev.midi.param1 = data[index++];
            ev.midi.param2 = data[index++];
            callback_type = MIDI;
            break;
        case 0xC0: // program change
        case 0xD0: // note aftertouch
            ev.midi.type = event_type;
            ev.midi.param1 = data[index++];
            callback_type = MIDI;
            break;
#ifndef   KAR_DEF_NO_EXCEPT
        default:
            throw UnrecognizedEvent(event_type);
#endif
        }
        status = true;
    }
    return status;
}

const Event& Event::operator= (const Event& t)
{
    memcpy(this, &t, sizeof(t));
    return *this;
}

uint8_t* Event::GetAddress() const
{
    return _start_addr;
}

}
