/**
 * @file song.cpp
 * a source file for song.h
 * @author Vester
 * @version 0.1
 */

#include "song.h"
#include "error.h"
#include "utils.h"

#include <cstring>

namespace kar
{

void Song::SetMidiCallback(CB_MIDI callback, void* user)
{

    for(unsigned long i = 0; i < _idata->header.track_size; i++)
        _idata->trk_array[i].SetMidiCallback(callback, user);
}

void Song::SetMetaCallback(CB_META callback, void* user)
{
    for(unsigned long i = 0; i < _idata->header.track_size; i++)
        _idata->trk_array[i].SetMetaCallback(callback, user);
}

void Song::SetKSysCallback(CB_KSYS ksys, void* user)
{
    for(unsigned long i = 0; i < _idata->header.track_size; i++)
        _idata->trk_array[i].SetKSysCallback(ksys, user);
}

void Song::InternalInit()
{
    _idata = NULL;
}

Song::Song()
{
    this->InternalInit();
}

Song::Song(const char* s)
{
    this->InternalInit();
    long t = this->Load(s);
    if(t == 1)
    {
        throw kar::KarLogicError("song::song(const std::string& s)", "file does not exist.");
    }
    else if(t == 2)
    {
        throw kar::KarLogicError("song::song(const std::string& s", "not a valid MIDI/KAR file");
    }
}

Song::~Song()
{
    this->Unload();
}

unsigned long Song::CheckRequiredBufferSize(std::ifstream& file)
{
    kar::Chunk  tchunk;
    tchunk.ReadHeaderChunk(file);
    unsigned long size = sizeof(_SongIData);
    size += sizeof(kar::Track) * tchunk.track_size;
    size += GetFileSize(file);

    return size;
}

long Song::Load(const char* filename)
{
    long status = 1;

    if(_idata != NULL)
    {
        this->Unload();
    }

    std::ifstream file(filename, std::ios::binary | std::ios::in);

    if(file.is_open())
    {
        status = 0;
        unsigned long overall_size = this->CheckRequiredBufferSize(file);
        void *buff = new uint8_t [ AlignedAllocSize(overall_size) ];
        _idata = new (buff) _SongIData();
        _idata->header.ReadHeaderChunk(file);
        _idata->trk_array = (kar::Track*)((uint8_t*)buff + sizeof(_SongIData));
        uint8_t *image_start = (uint8_t*)_idata->trk_array + (sizeof(kar::Track) * _idata->header.track_size);

        unsigned long image_size = ReadFileToMemory(file, image_start);

        image_start += 14;
        for(unsigned long i = 0; i < _idata->header.track_size; i++)
        {
            new ((void*)&_idata->trk_array[i]) kar::Track();
            _idata->trk_array[i].ReadTrack((char*)image_start, image_size, i);
            image_start += _idata->trk_array[i]._size + 8;
        }
    }

    return status;
}

kar::Song::PosType Song::GetPos()
{
    std::vector<kar::Track::PosType>  *pos = new std::vector<kar::Track::PosType>;
    for(unsigned long i = 0; i < _idata->header.track_size; i++)
    {
        pos->push_back(_idata->trk_array[i].GetPos());
    }
    return kar::Song::PosType(pos);
}

const kar::Chunk& Song::GetHeader() const
{
    return _idata->header;
}

kar::Track& Song::GetTrack(long i)
{
    if(i < _idata->header.track_size)
        return _idata->trk_array[i];
    throw kar::KarOutOfRange("song::get_track(long i)");
}

unsigned long Song::GetTrackSize() const
{
    return _idata->header.track_size;
}

bool Song::Empty() const
{
    return (_idata->header.track_size == 0);
}

bool Song::MuteTrack(unsigned long track_n, bool t)
{
    bool status = true;
    if(track_n < this->GetTrackSize())
    {
        status = false;
        _idata->trk_array[track_n].Mute(t);
    }
    return status;
}

void Song::Unload()
{
    if(_idata != NULL)
    {
        for(unsigned long i = 0; i < _idata->header.track_size; i++)
            _idata->trk_array[i].~Track();
        _idata->~_SongIData();
        delete _idata;
        _idata = NULL;
    }
}

void Song::Rewind()
{
    for(unsigned long i = 0; i < _idata->header.track_size; i++)
        _idata->trk_array[i].Rewind();
}

double Song::Update(double elapsed, const double *ptr_ppqn)
{
    double t_elapsed = elapsed - _idata->prev_microsec;
    double remain = 0.0;
    for(unsigned long i = 0; i < _idata->header.track_size; i++)
    {
        double temp = _idata->trk_array[i]._tmp.d = _idata->trk_array[i].Update(t_elapsed, *ptr_ppqn);
        if(temp > remain)
            remain = temp;
    }

    while(remain > 0.0)
    {
        remain = 0.0;
        for(unsigned long i = 0; i < _idata->header.track_size; i++)
        {
            double temp = _idata->trk_array[i]._tmp.d = _idata->trk_array[i].Update( _idata->trk_array[i]._tmp.d,
                          *ptr_ppqn );
            if(temp > remain)
                remain = temp;
        }
    }

    _idata->prev_microsec = elapsed;

    if(this->Ended() == false) return 1.0;
    return 0.0;
}

bool Song::EnableMidi(bool t)
{
    bool status = false;
    for(unsigned long i = 0; i < _idata->header.track_size; i++)
        if(_idata->trk_array[i].EnableMidi(t) == true) status = true;

    return status;
}

bool Song::EnableMeta(bool t)
{
    bool status = false;
    for(unsigned long i = 0; i < _idata->header.track_size; i++)
        if(_idata->trk_array[i].EnableMeta(t) == true) status = true;

    return status;
}

bool Song::Ended() const
{
    bool status = true;

    for(unsigned long i = 0; i < _idata->header.track_size; i++)
    {
        if(_idata->trk_array[i].Ended() == false) status = false;
    }

    return status;
}

}
