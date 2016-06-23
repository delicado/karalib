/**
 * @file song.h
 * a header for song.cpp
 * @author Vester
 * @version 0.1
 * @bug no bug is known yet.
 */

#ifndef  KAR_SONG_H_
#define  KAR_SONG_H_


#include <vector>
#include <stdint.h>
#include <fstream>

#include "chunk.h"
#include "tracks.h"

/// FORWARD DECLARATIONS

namespace kar
{


/**
 * a song class, a song object can contain more than 1 track objects.
 * this class manages its own memory. only a single block of memory from 'new'
 * is needed in this class to prevent some memory allocation failure cause from
 * allocating many small objects. i avoid using any containers that allocates
 * its own blocks of memory so that memory is not fragmented.
 *
 * current implementaion : memory layout pointed to by 'idata';
 *
 *   ---------------
 *   : song_idata  :
 *   ---------------
 *   : kar::track  :
 *   : kar::track  :
 *   : kar::track..:
 *   ---------------
 *   : MIDI image  :
 *   ---------------
 */

/**************************  TO DO  **********************************
> merge the flags in 'bool' to a single 32-bit value.
> create an enum for the flags.

**********************************************************************/
class Song
{
    struct _SongIData
    {
        kar::Chunk          header;
        double              prev_microsec;
        kar::Track          *trk_array;
        _SongIData() : prev_microsec(0.0) { }
    } *_idata;

protected:
    /**
     * precomputes the size of buffer before a file is to be opened.
     * used internally by this class.
     * @param the file to be opened.
     * @return the required size of the buffer.
     */
    unsigned long CheckRequiredBufferSize(std::ifstream& file);

public:

    class PosType
    {
        std::vector<kar::Track::PosType> *_pos;
    public:
        PosType(std::vector<kar::Track::PosType> *pos) : _pos(pos) { }
       ~PosType() { delete _pos; }
    };

    /**
     * default constructor.
     */
    Song();

    /**
     * constructor ; opens a file from a file name.
     * @param s filename of the file to be opened.
     */
    Song(const char* s);

    /**
     * destructor
     */
    ~Song();

    /**
     * opens a file from a file name.
     * @param filename filename of the file to be opened.
     * @return 0 - success; 1 - file not found;
     */
    long Load(const char* filename);

    /**
     * saves the current position
     * @return kar::song::pos_type object
     */
     kar::Song::PosType GetPos();

    /**
     * returns a kar::chunk header of the file
     * @return kar::chunk header of the file.
     */
    const kar::Chunk& GetHeader() const;

    /**
     * returns a track contained on this 'song'.
     * @param i ID/index of the track in this 'song' object.
     * @return a track object.
     */
    kar::Track& GetTrack(long i);

    /**
     * sets the midi callback
     * @param callback a midi callback.
     * @param user user specified data.
     */
    void SetMidiCallback(CB_MIDI callback, void* user = NULL);

    /**
     * sets the meta callback
     * @param callback a meta callback.
     * @param user user specified data.
     */
    void SetMetaCallback(CB_META callback, void* user = NULL);

    /**
     * sets the karsys callback
     * @param karsys a karsys callback.
     * @param user user specified data.
     */
    void SetKSysCallback(CB_KSYS ksys, void* user);

    /**
     * initializes the class to the default values.
     * used internally by this class.
     */
    void InternalInit();

    /**
     * returns the number of tracks
     * @return the number of tracks currently on the object of this type.
     */
    unsigned long GetTrackSize() const;

    /**
     * check if a file was loaded into the object of this type.
     * @return 'true' if no song was loaded. 'false' if there is.
     */
    bool Empty() const;

    /**
     * mutes a track.
     * @param track_n the track offset/ID to be muted.
     * @param t 'true' to mute, 'false' to unmute.
     */
    bool MuteTrack(unsigned long track_n, bool t);

    /**
     * frees the memory used up by this class, and cleans up things.
     */
    void Unload();

    /**
     * "rewinds" the 'song'
     */
    void Rewind();

    /**
     * check if the 'song' has finished streaming.
     * @return 'true' if finished streaming. 'false' if not.
     */
    bool Ended() const;

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
     * synchronization
     * @param elapsed microseconds since the last call of this function
     * @param ppqn pointer to the current Pulse Per Quarter Note value.
     * @return '0.0' if finised streaming, '1.0' if not.
     */
    double Update(double elapsed, const double* ppqn);
};

}

#endif
