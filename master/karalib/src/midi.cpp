/**
 * @file midi.cpp
 * a source file for midi.h
 * @author Vester
 * @version 0.1
 */

#include "midi.h"
#include "utils.h"
#include "defprocedures.h"
#include "error.h"
#include <fstream>
#include "flags.h"

namespace kar
{

Midi::Midi(long port, CB_MIDI c_midi, void* u_midi, CB_META c_meta, void* u_meta, CB_KSYS c_ksys, void* u_ksys)
 : _out(port), _current_playing(0), _song_index(0),
   _c_midi(c_midi), _u_midi(u_midi), _c_meta(c_meta), _u_meta(u_meta),
   _c_ksys(c_ksys), _u_ksys(u_ksys)
{
}

Midi::Midi() : _flag(0), _out(0), _current_playing(0), _song_index(0)
{
    _c_midi = DefaultMidiCallback;
    _c_meta = DefaultMetaCallback;
    _c_ksys = MidiKSys;
    _u_midi = &_out;
    _u_meta = &_out;
    _u_ksys = this;
}

Midi::~Midi()
{
    _out.Close();
    this->Clear();
}

bool Midi::Clear()
{
    bool status = true;
    if(_songs.size() > 0)
    {
        // delete the 'song's allocated by this class. cause this 'song' objects cannot have an
        // owner outside of the class. it is safe to delete it.
        for(std::vector<_HSong>::iterator i = _songs.begin(); i < _songs.end(); ++i)
            if((*i).flag & DESTROY_SELF) delete (*i).ptr;
        _songs.clear();
        // we cleared something. so we return a 'false' (which means the object was not empty).
        status = false;
    }
    return status;
}

CB_KSYS Midi::SetKSysCallback(CB_KSYS ksys, void* user)
{
    CB_KSYS prev = _c_ksys;
    _c_ksys = ksys;
    _u_ksys = user;
    return prev;
}

void* Midi::GetKSysCallback() const
{
    return _u_ksys;
}

CB_META Midi::SetMetaCallback(CB_META meta, void* user)
{
    CB_META prev = _c_meta;
    _c_meta = meta;
    _u_meta = user;
    return prev;
}

void* Midi::GetMetaUData() const
{
    return _u_meta;
}

unsigned long Midi::Queue(kar::Song& s)
{
    s.SetMidiCallback(_c_midi, _u_midi);
    s.SetMetaCallback(_c_meta, _u_meta);
    s.SetKSysCallback(_c_ksys, _u_ksys);
    s.Rewind();

    _songs.push_back(_HSong(&s, 0));

    return _songs.size() - 1;
}

unsigned long Midi::Queue(kar::Song* s)
{
    return this->Queue(*s);
}

unsigned long Midi::Queue(const char* filename)
{
    _HSong s;
    s.ptr = new kar::Song(filename);
    s.ptr->SetMidiCallback(_c_midi, _u_midi);
    s.ptr->SetMetaCallback(_c_meta, _u_meta);
    s.ptr->SetKSysCallback(_c_ksys, _u_ksys);
    s.ptr->Rewind();
    s.flag = DESTROY_SELF;
    _songs.push_back(s);

    return _songs.size() - 1;
}

unsigned long Midi::Queue(const std::string& filename)
{
    return this->Queue(filename.c_str());
}

bool Midi::Poll()
{
    bool status = false;
    if(!(_flag & STOPPED))
    {
        if(!_timer.Empty())
            status = _timer.Poll();

        // if flag_ is set to play but 'false' is returned from timer_.poll()
        // which means poll() finished playing the file. unset the flag to
        // not playing, and also let the library know about it.
        if((_flag & PLAYING) && status == false)
        {
            // let the library know that a song has finished playing.
            if(_c_ksys != NULL)
                _c_ksys(0, EKSYS::song_ended, NULL, 0, _u_ksys);
            _flag &= ~PLAYING;
        }


        if(status == false && !(_flag & SINGLEPLAY))
        {
            _timer.Empty();
            if(!_songs.empty())
            {
                if(_song_index < _songs.size())
                {
                    this->Play(_song_index, false);
                    _songs[_current_playing].ptr->Rewind();
                    status = true;
                }
            }
        }
    }
    return status;
}

unsigned long Midi::Stop()
{
    _flag |= STOPPED;
    if(_flag & PLAYING)
    {
        _flag &= ~PLAYING;
        // let the library know that a song has finished playing.
        if(_c_ksys != NULL)
            _c_ksys(0, EKSYS::song_ended, NULL, 0, _u_ksys);
    }
    return this->CurrentPlaying();
}

unsigned long Midi::Play(unsigned long ID, bool single_play)
{
    unsigned long prv = _current_playing;
    if(!_songs.empty())
    {
        if(ID < _songs.size())
        {
            _current_playing = ID;
            _song_index = _current_playing + 1;
            _timer.Set(*_songs[_current_playing].ptr, &kar::Song::Update);
            _timer.SetTimeDivision( _songs[_current_playing].ptr->GetHeader().GetTicksPerBeat() );
            if(single_play == true)
                _flag |= SINGLEPLAY;
            else
                _flag &= ~SINGLEPLAY;
            _flag |= PLAYING;
            _flag &= ~STOPPED;
            // let the library know that a song has started playing.
            if(_c_ksys != NULL)
                _c_ksys(0, EKSYS::song_started, NULL, 0, _u_ksys);
       }
    }

    return prv;
}

unsigned long Midi::Next()
{
    unsigned long out = _current_playing;
    if(!_songs.empty())
    {

        if(_flag & PLAYING)
        {
            if(_c_ksys != NULL)
                _c_ksys(0, EKSYS::song_ended, NULL, 0, _u_ksys);
             _flag &= ~PLAYING;
        }

        if((_current_playing + 1) < _songs.size())
        {
            _current_playing += 1;
            this->Play(_current_playing, false);
            out = _current_playing;
            _flag |= PLAYING;
        }
        else
        {
            this->Stop();
        }
    }
    return out;
}

unsigned long Midi::Prev()
{
    unsigned long out = _current_playing;
    if(!_songs.empty())
    {
        if(_flag & PLAYING)
        {
            if(_c_ksys != NULL)
                _c_ksys(0, EKSYS::song_ended, NULL, 0, _u_ksys);
            _flag &= ~PLAYING;
        }

        if((_current_playing - 1) >= 0)
        {
            _current_playing -= 1;
            this->Play(_current_playing, false);
            out = _current_playing;
            _flag |= PLAYING;
        }
    }
    return out;
}

unsigned long Midi::Size() const
{
    return _songs.size();
}

unsigned long Midi::CurrentPlaying() const
{
    return _current_playing;
}

kar::Song& Midi::GetSong(unsigned long i)
{
    return *_songs.at(i).ptr;
}

long Midi::MidiKSys(unsigned long track_num, unsigned long type, const char* data, unsigned long size, void* user)
{
    if(type == EKSYS::tempo_change)
    {
        kar::Midi* t = (kar::Midi*) user;
        unsigned long t1 = 0;
        unsigned long t2 = ReadBigEndianDWord(data, t1) >> 8;
        t->_timer.SetPulsePerQuarterNote(double(t2));
    }

    return 0;
}

}
