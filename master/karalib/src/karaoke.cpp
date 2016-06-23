#include "karaoke.h"
#include <memory.h>

#include <conio.h>

namespace kar
{
Karaoke::Karaoke() : Midi(), _flag(0),
 _c_text(NULL), _u_text(NULL)
{
    this->SetMetaCallback(CBMetaProc, this);

    _prev_u_ksys = this->GetKSysCallback();
    _prev_c_ksys = this->SetKSysCallback(CBKSys, this);
}

void Karaoke::BuffStrings(unsigned long track_index, long method_number)
{
    switch(method_number)
    {
    case 0:
        _flag |= BUFFLYRICS;
        break;
    }

    _temps.param1 = track_index;
    _temps.param2 = 0;
    _temps.strbuffer.clear();
    _string_buff.clear();

    // disable midi output so we cant hear a sound when we stream it.
    bool midi_state = this->GetSong( this->CurrentPlaying() ).GetTrack(track_index).EnableMidi(false);
    // first we need to get the user data for meta callback
    void* u_meta_temp = this->GetSong( this->CurrentPlaying() ).GetTrack(track_index).GetMetaUData();
    // the replace the meta callback, storing the previous one.
    CB_META c_meta_temp = this->GetSong( this->CurrentPlaying() ).GetTrack(track_index).SetMetaCallback(CBMetaProc, this);
    // stream...
    double temp1 = 0.0;
    do {
        temp1 = this->GetSong( this->CurrentPlaying() ).GetTrack(track_index).Update(99999999.999999, 50.0);
    } while(temp1 > 0.0);
    // push back remaining strings in temp string buffer
    _string_buff.push_back( _temps.strbuffer );
    // then clear the temp string buffer and set to zero the temps parameters
    _temps.strbuffer.clear();
    _temps.param1 = 0;
    _temps.param2 = 0;
    // set previous meta callback
    this->GetSong( this->CurrentPlaying() ).GetTrack(track_index).SetMetaCallback(c_meta_temp, u_meta_temp);
    // rewind the track that we streamed for getting the texts
    this->GetSong( this->CurrentPlaying() ).GetTrack(track_index).Rewind();
    // re-set the state of midi.
    this->GetSong( this->CurrentPlaying() ).GetTrack(track_index).EnableMidi(midi_state);
    _flag = 0;
}

unsigned long Karaoke::Play(unsigned long ID, bool single_play)
{
    unsigned long ret = (unsigned long) -1;
    if(this->Size() > 0)
    {
        ret = Midi::Play(ID, single_play);
        if( this->GetSong( this->CurrentPlaying() ).GetTrackSize() > 2 )
            this->BuffStrings( 2, 0 );
        else this->BuffStrings( 0, 0 );
        _rindex = 0;
    }
    return ret;
}

void* Karaoke::GetTextUData() const
{
    return _u_text;
}

CB_TEXT Karaoke::SetTextCallback(CB_TEXT callback, void* user)
{
    CB_TEXT prev = _c_text;
    _c_text = callback;
    _u_text = user;
    return prev;
}

long Karaoke::CBKSys(unsigned long track_num, unsigned long type, const char* data, unsigned long size, void* user)
{
    Karaoke* k = (Karaoke*) user;
    if(k->_c_text != NULL)
    {
        if(type == EKSYS::song_started)
        {
            k->_c_text(0, ESET::started, std::string(), k->_u_text);
        }
        else if(type == EKSYS::song_ended)
        {
            k->_c_text(0, ESET::ended, std::string(), k->_u_text);
        }
    }

    if(k->_prev_c_ksys != NULL)
        k->_prev_c_ksys(track_num, type, data, size, k->_prev_u_ksys);
    return 0;
}

long Karaoke::CBMetaProc(unsigned long track_num, unsigned long event_type, const char *data, unsigned long size, void* user)
{
    Karaoke *k = static_cast<Karaoke*>(user);
    if(event_type == 0x01 || event_type == 0x05)
    {
        if((k->_flag & BUFFLYRICS) && (k->_temps.param1 == track_num))
        {
            for(unsigned long i = 0; i < size; i++)
            {
                if(k->_temps.param2 == 0 && data[i] == '\\')
                {
                    k->_temps.param2 = 1;

                } else if(k->_temps.param2 == 1)
                {
                    if(data[i] != '\\')
                    {
                        k->_temps.strbuffer += data[i];
                    } else
                    {
                        k->_string_buff.push_back(k->_temps.strbuffer);
                        k->_temps.strbuffer.clear();
                    }
                }
            }
        } else if(!(k->_flag & DISABLE_LYRICS)  && (track_num == 2) && (k->_c_text != NULL))
        {
            unsigned long t = 0;
            if(data[0] == '@')
            {
                k->_temps.strbuffer.assign(data + 2, size - 2);
                switch(data[1])
                {
                    case 'K':
                        // first '@K' is usually the type of file or @KMIDI KARAOKE FILE
                        if(k->_temps.param2 == 0)
                            t = ESET::type;
                        // second is the copyright? succeeding @K maybe are copyrights too?
                        else t = ESET::copyright;
                        k->_temps.param2++;
                        break;
                    case 'T':
                        // first '@T' is usually the title.
                        if(k->_temps.param1 == 0)
                            t = ESET::title;
                        // second is usually the artist/artists
                        else if(k->_temps.param1 == 1)
                            t = ESET::artist;
                        // third is the sequencer used?
                        else if(k->_temps.param1 == 2)
                            t = ESET::sequencer;
                        // fourth the karaoke maker?
                        else if(k->_temps.param1 == 3)
                            t = ESET::maker;
                        // the songwriter?
                        else if(k->_temps.param1 == 4)
                            t = ESET::writer;
                        // dont care.
                        else
                            t = ESET::title;
                        k->_temps.param1++;
                        break;
                    case 'L':
                        t = ESET::language;
                        break;
                    case 'I':
                        t = ESET::info;
                        break;
                    case 'W':
                        t = ESET::warning;
                        break;
                    case 'V':
                        t = ESET::version;
                }
            }
            else
            {
                if(data[0] == '\\')
                {
                    if(k->_rindex < k->_string_buff.size())
                    {
                        k->_temps.strbuffer = k->_string_buff.at(k->_rindex++);
                        k->_c_text(track_num, ESET::paragraph, k->_temps.strbuffer, k->_u_text);
                    }
                    k->_temps.strbuffer.assign(data + 1, size - 1);
                    t = ESET::line;

                } else
                {
                    k->_temps.strbuffer.assign(data, size);
                    t = ESET::line;
                }
            }
            if(t !=0 )
                k->_c_text(track_num, t, k->_temps.strbuffer, k->_u_text);

        }
    }
    return 0;
}

}

