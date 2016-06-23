/**
 * @file midi.h
 * a header for midi.cpp
 * @author Vester
 * @version 0.1
 * @bug no bug is known yet.
 */

#ifndef KAR_MIDI_H_
#define KAR_MIDI_H_

#include <vector>
#include <stdint.h>

#include "../karalib_global.h"
#include "scheduler.h"
#include "ksong.h"
#include "midiout.h"
#include "flags.h"


namespace kar
{

class KARALIBSHARED_EXPORT Midi
{
    enum
    {
        DESTROY_SELF = BIT_1
    };

    enum
    {
        PLAYING = BIT_1,
        STOPPED = BIT_2,
        SINGLEPLAY = BIT_3
    };

    struct _HSong
    {
        kar::Song* ptr;
        uint32_t   flag;
        _HSong() : ptr(NULL), flag(0) { }
        _HSong(kar::Song* ptr, uint32_t flag) : ptr(ptr), flag(flag) { }
        _HSong(const _HSong& s) : ptr(s.ptr), flag(s.flag) { }
    };

    uint32_t                    _flag;
    std::vector<_HSong>         _songs;
    kar::MidiOut                _out;
    Scheduler                   _timer;

    unsigned long               _current_playing;
    unsigned long               _song_index;
    CB_MIDI                     _c_midi;
    void*                       _u_midi;
    CB_META                     _c_meta;
    void*                       _u_meta;
    CB_KSYS                     _c_ksys;
    void*                       _u_ksys;
protected:
    static long MidiKSys(unsigned long track_num,
                          unsigned long type,
                          const char* data,
                          unsigned long Size,
                          void* user);

    CB_KSYS SetKSysCallback(CB_KSYS ksys, void* user);
    void* GetKSysCallback() const;
    CB_META SetMetaCallback(CB_META meta, void* user);
    void* GetMetaUData() const;
public:

    /**
     * default constructor.
     */
    Midi();

    /**
     *constructor
     */
    Midi(long port, CB_MIDI c_midi, void* u_midi, CB_META c_meta, void* u_meta, CB_KSYS c_ksys, void* u_ksys);

    /**
     * destructor.
     */
    ~Midi();

    /**
     * queue's a song for streaming.
     * @param s a kar::song object.
     * @return track ID/number.
     */
    unsigned long Queue(kar::Song& s);

    /**
    * queue's a song for streaming.
    * @param s a pointer to a kar::song object.
    * @return track ID/number.
    */
    unsigned long Queue(kar::Song* s);

    /**
     * queue's a song for streaming.
     * input is a filename, but it does not mean it is recommended. what this function do
     * is create a kar::song object by 'new' than labels it as 'not owned'. so that this class
     * can delete it.
     * @param filename filename of the file to be opened.
     * @return track ID/number.
     */
     unsigned long Queue(const char* filename);

    /**
     * queue's a song for streaming.
     * input is a filename, but it does not mean it is recommended. what this function do
     * is create a kar::song object by 'new' than labels it as 'not owned'. so that this class
     * can delete it.
     * @param filename filename of the file to be opened.
     * @return track ID/number.
     */
     unsigned long Queue(const std::string& filename);

    // lets allow only this member function to compile if the compiler supports C++11 variadic templates.
    #if  (__cplusplus == 201103)
    template <typename T, typename ... U>
    unsigned long queue(T& t, U& ... u)
    {
        queue(t);
        return queue(u...);
    }
    #endif
    /**
     * poll function.
     * this be should called in intervals of 10 milliseconds or less for
     * a proper streaming of MIDI data.
     * @return 'true' if finished streaming a song, 'false' if not.
     */
    bool Poll();

    /**
     * plays a single song from the queue.
     * @param ID id of the song to play.
     */
    virtual unsigned long Play(unsigned long ID, bool single_play = false);

    /**
     * step to next song.
     * @return the ID of the song.
     */
    unsigned long Next();

    /**
     * step to previous song.
     * @return the ID of the song.
     */
    unsigned long Prev();

    /**
     * the number of 'songs' in queue.
     * @return the number of 'songs' in queue.
     */
    unsigned long Size() const;

    /**
     * stops the current playing song.
     * @return the ID of the song.
     */
     unsigned long Stop();

    /**
     * returns the index of the current playing song.
     * @return current playing song index.
     */
     unsigned long CurrentPlaying() const;

    /**
     * returns a song from the queue.
     * @param i the song ID
     * @return a kar::song object;
     */
    kar::Song& GetSong(unsigned long i);

    /**
     * clears the song queue.
     * @return returns false for success (means there was a song), or 'true' if queue was empty.
     */
     bool Clear();

     /**
     * synchronization
     * @param elapsed microseconds since the last call of this function
     * @param ppqn pointer to the current Pulse Per Quarter Note value.
     * @return '0.0' if finised streaming, '1.0' if not.
     */
    double Update(double elapsed, const double* ppqn);
};

}


#endif
