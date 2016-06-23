/**
 * @file chunk.h
 * a header for chunk.cpp
 * @author Vester
 * @version 0.1
 * @bug no bug is known yet.
 */

#ifndef KAR_chunk_H_
#define KAR_chunk_H_
#include "utils.h"
#include <memory.h>
#include <stdint.h>
#include "error.h"

namespace kar
{

/**
 * SMPTE data structure
 * used to hold an SMPTE data
 */

struct SMPTE
{
    /**
     * default constructor
     * zeros all the member variables
     */
    SMPTE();

    /**
     * constructor
     * initializes the member variables
     * @param fps_ assigns the value to fps
     * @param tpf_ assigns the value to tpf
     */
    SMPTE(unsigned long fps, unsigned long tpf);

    unsigned long fps; //!< frames per second
    unsigned long tpf; //!< ticks per frame
};

struct Chunk
{
private:
    bool _ready;
public:
    char        ID_string[5];       //!< the "MThd" string
    uint32_t    chunk_size;         //!< this is always 6
    uint16_t    format_type;        //!< format type 0, 1 or 2
    uint16_t    track_size;         //!< number of tracks in this MIDI file
    uint16_t    time_division;      //!< use is_smpte(), get_smpte(), is_tpb() and get_tpb() functions

    /**
     * constructor
     * default constructor
     */
    Chunk();

    /**
     * constructor
     * @param file the file where a header will be retrieved.
     */
    Chunk(std::ifstream& file);

    /**
     * reads the chunk header from a MIDI/Karaoke file
     * @param file the file to be read
     */
    void ReadHeaderChunk(std::istream& file);

    /**
     * checks if file uses SMPTE
     * @return returns true if timing uses SMPTE, or false if Ticks Per Beat
     */

    bool IsSMPTE() const;

    /**
     * checks if files uses delta time as "ticks per beat"
     * @return returns true if timing uses Ticks Per Beat or false if SMPTE
     */
    bool IsTPB() const;

    /**
     * decodes time division member as an SMPTE data
     * @return the SMPTE data decoded from time_division member
     */
    SMPTE GetSMPTE() const;

    /**
     * gets the ticks per beat
     * @return ticks per beat
     */
    uint16_t GetTicksPerBeat() const;
};

}

#endif
