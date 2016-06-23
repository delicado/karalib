/**
 * @file tracks.h
 * a header for tracks.cpp
 * @author Vester
 * @version 0.1
 * @bug no bug is known yet.
 */

/**************************  TO DO  **********************************
> merge the flags in 'bool' to a single 32-bit value.
> create an enum for the flags.

**********************************************************************/

#ifndef KAR_TRACK_H_
#define KAR_TRACK_H_

#include "chunk.h"
#include "event.h"
#include <stdint.h>
#include "ktypes.h"

namespace kar
{
class Track
{
    char                   *_data;
    unsigned long           _data_index;
    unsigned long           _size;        //!< the size in bytes of the events on this track.
    double                  _delta_time;
    kar::Event              _queue;
    uint32_t                _flag;
    uint8_t                 _running_status;

    CB_META                 _c_meta;
    void                   *_u_meta;
    CB_MIDI                 _c_midi;
    void                   *_u_midi;
    CB_KSYS                 _c_ksys;
    void                   *_u_ksys;
protected:
    friend class Song;
    union
    {
        double d;
    } _tmp;

    /**
     * internal initialization function
     * initializes the class to the default values.
     */
    void InternalInit();
public:

    class PosType
    {
        unsigned long       _data_index;
        double              _delta_time;
        kar::Event          _queue;
        uint8_t             _running_status;
        friend class Track;
    public:
        PosType(unsigned long      data_index,
                 double             delta_time,
                 kar::Event         queue,
                 uint8_t            running_status)
                 : _data_index(data_index), _delta_time(delta_time),
                   _queue(queue), _running_status(running_status){}
    };

    char ID_string[5];                //!< chunk id string. it should always contain "MTrk" after a read.
    unsigned long track_number;       //!< the track number.

    /**
     * default constructor.
     */
    Track();

    /**
     * destructor.
     */
    ~Track();

    /**
     * initializes the class from an track image in memory.
     * @param in_buffer pointer to the start of a 'track' data in memory.
     * @param size size of the data pointed to by in_buffer.
     * @param track_num the track number to be ID'ed to the object of this type.
     */
    void ReadTrack(char* in_buffer, unsigned long Size, unsigned long track_num);

    /**
     * reads a track data from a file.
     * @param file an opened file where data will be read.
     * @param header the header of the MIDI file to be read.
     * @param track_num the track offset that will be read to the object of this class.
     */
    void ReadTrack(std::ifstream& file, const kar::Chunk& header, unsigned long track_num_);

    /**
     * saves the current position to a kar::track::pos_type class
     * @return position
     */
    kar::Track::PosType GetPos() const;

    /**
     * sets the current position from a kar::track::pos_type class
     * @param position
     */
    void SetPos(const kar::Track::PosType& pos);

    /**
     * "rewinds" the track.
     * makes the track point to the beginning.
     */
    void Rewind();

    /**
     * check if the track has finished streaming or playing.
     * @return returns true if the track has finished streaming or playing.
     */
    bool Ended() const;

    /**
     * gets the remaining delta time.
     * @return returns the remaining delta time.
     */
    double GetCurrentDelta() const;

    /**
     * sets the meta callback and returns the replaced one.
     * @param callback a meta callback.
     * @param user user specified data.
     * @return previously replaced meta callback.
     */
    CB_META SetMetaCallback(CB_META callback, void* user = NULL);

    /**
     * sets the midi callback and returns the replaced one.
     * @param callback a midi callback.
     * @param user user specified data.
     * @return previously replaced midi callback.
     */
    CB_MIDI SetMidiCallback(CB_MIDI callback_, void* user = NULL);

    /**
     * sets the 'karsys' a callback used by the library and returns the replaced one.
     * @param callback a karsys callback.
     * @param user user specified data.
     * @return previously replaced karsys callback.
     */
    CB_KSYS SetKSysCallback(CB_KSYS ksys, void* user);

    /**
     * mutes the track.
     * @param t 'true' for mute, 'false' to unmute.
     */
    void Mute(bool t);

    /**
     * enables or disables sending midi event
     * @param t 'true' to enable, 'false' to disable.
     * @return previous status 'true' if enabled, 'false' if disabled.
     */
     bool EnableMidi(bool t);

    /**
     * enables or disables sending meta event
     * @param t 'true' to enable, 'false' to disable.
     * @return previous status 'true' if enabled, 'false' if disabled.
     */
     bool EnableMeta(bool t);

    /**
     * call when this object will not be used anymore, to free some resources.
     */
    void Destroy();

    /**
     * returns the size of the track in bytes.
     * @return size of track in bytes.
     */
     unsigned long Size() const;

    /**
     * returns the user specified data for the meta callback.
     * @return returns user specified data for the meta callback
     */
    void* GetMetaUData() const;

    /**
     * returns the user specified data for the midi callback.
     * @return returns the user specified data for the midi callback
     */
    void* GetMidiUData() const;

    /**
     * returns the user specified data for the karsys callback.
     * @return returns the user specified data for the karsys callback
     */
    void* GetKarSysUData() const;

    /**
     * synchronization
     * @param elapsed microseconds since the last call of this function
     * @param ppqn current Pulse Per Quarter Note value.
     */
    double Update(double elapsed, double ppqn);
};


}



#endif
