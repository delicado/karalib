/**
 * @file tracks.cpp
 * a source file for tracks.h
 * @author Vester
 * @version 0.1
 */

#include "tracks.h"
#include "flags.h"
#include "utils.h"
#include <memory.h>

namespace kar
{

void Track::InternalInit()
{
    _data = NULL;
    _data_index = 0;
    _delta_time = 0.0;
    _running_status = 0;
    _tmp.d = 0.0;
}

Track::Track()
    : _queue(0), _flag(0)
{
    this->InternalInit();
}

void Track::ReadTrack(char* in_buffer, unsigned long size, unsigned long trn)
{
    this->InternalInit();
    unsigned long index = 4;
    memcpy(ID_string, in_buffer, 5);
    _size = ReadBigEndianDWord(in_buffer, index);
    _data = in_buffer + 8;
    track_number = trn;
    _data_index = 0;
    _queue.ReadEvent(_data, _size, _data_index);
}

void Track::ReadTrack(std::ifstream& file_, const kar::Chunk& header_, unsigned long track_number_)
{
    this->InternalInit();
    if(track_number < header_.track_size)
    {
        SeekToTrack(file_, header_, track_number_);
        file_.read(ID_string, 4);
        _size = ReadBigEndianDWord(file_);
        track_number = track_number_;



#ifdef   KAR_DEF_NO_EXCEPT
        data_ = new char [ aligned_alloc_size( size_ ) ];
#else
        try
        {
            _data = new char [_size];

        }catch(std::bad_alloc)
        {
            throw kar::OutOfMemory("track::read_track.");
        }
#endif
        if(_data == NULL) throw std::bad_alloc();

        file_.read(_data, _size);
        _data_index = 0;
    }
}

Track::PosType Track::GetPos() const
{
    return Track::PosType(_data_index, _delta_time, _queue, _running_status);
}

void Track::SetPos(const kar::Track::PosType& pos)
{
    _data_index         = pos._data_index;
    _delta_time         = pos._delta_time;
    _queue              = pos._queue;
    _running_status     = pos._running_status;
}

void Track::Rewind()
{
    _data_index = 0;
    _queue.SetRunningStatus(_running_status);
    _queue.ReadEvent(_data, _size, _data_index);
    _running_status = _queue.GetRunningStatus();
}

double Track::GetCurrentDelta() const
{
    return _delta_time;
}

CB_META Track::SetMetaCallback(CB_META callback_, void* user)
{
    CB_META t;
    t = _c_meta;
    _c_meta = callback_;
    _u_meta = user;
    return t;
}


CB_MIDI Track::SetMidiCallback(CB_MIDI callback_, void* user)
{
    CB_MIDI t = _c_midi;
    _c_midi = callback_;
    _u_midi = user;
    return t;
}


CB_KSYS Track::SetKSysCallback(CB_KSYS ksys, void* user)
{
    CB_KSYS t = _c_ksys;
    _c_ksys = ksys;
    _u_ksys = user;
    return t;
}


void* Track::GetMetaUData() const
{
    return _u_meta;
}


void* Track::GetMidiUData() const
{
    return _u_midi;
}


void* Track::GetKarSysUData() const
{
    return _u_ksys;
}


void Track::Mute(bool t)
{
    if(t == true)
        _flag |= FTRACK::mute;
    else
        _flag &= ~FTRACK::mute;
}


void Track::Destroy()
{
    if(_data != NULL)
    {
        delete [] _data;
        _data = NULL;
    }
}

Track::~Track()
{
    this->Destroy();
}

double Track::Update(double elapsed, double ppqn)
{
    double temp = elapsed;

    if(elapsed > 0.0)
    {
        bool status = false;
        double dt = ppqn * _delta_time;

        if(temp >= dt)
        {
            temp -= dt;
            do
            {
                switch(_queue.callback_type)
                {
                case MIDI:
                    if(!(_flag & FTRACK::mute) && !(_flag & FTRACK::no_midi))
                    {
                        if(_c_midi != NULL)
                            _c_midi(track_number, _queue.ev.midi.type, _queue.ev.midi.param1, _queue.ev.midi.param2, _u_midi);
                    }
                    break;
                case META:
                    if(_c_ksys != NULL)
                    {
                        if(_queue.ev.meta.type == 0x51)
                        {
                            _c_ksys(track_number, EKSYS::tempo_change, _queue.ev.meta.data, _queue.ev.meta.data_size, _u_ksys);
                        }
                    }
                    if(!(_flag & FTRACK::mute) && !(_flag & FTRACK::no_meta))
                    {
                        if(_c_meta != NULL)
                            _c_meta(track_number, _queue.ev.meta.type, _queue.ev.meta.data, _queue.ev.meta.data_size, _u_meta);
                    }
                    break;
                case SYSTEM:
                    break;
                }
                _queue.SetRunningStatus(_running_status);
                status = _queue.ReadEvent(_data, _size, _data_index);
                _running_status = _queue.GetRunningStatus();

            } while((_queue.delta_time == 0) && (status == true));
            _delta_time = double(_queue.delta_time);
            if(status == false) temp = 0.0;
        } else
        {
            _delta_time -= temp / ppqn;
            temp = 0;
        }
    }

    return temp;
}

bool Track::Ended() const
{
    return (_data_index >= _size);
}

bool Track::EnableMidi(bool t)
{
    bool prv = !(_flag & FTRACK::no_midi);
    if(t) _flag &= ~FTRACK::no_midi;
    else  _flag |= FTRACK::no_midi;
    return prv;
}

bool Track::EnableMeta(bool t)
{
    bool prv = !(_flag & FTRACK::no_meta);
    if(t) _flag &= ~FTRACK::no_meta;
    else  _flag |= FTRACK::no_meta;
    return prv;
}

unsigned long Track::Size() const
{
    return _size;
}

}
