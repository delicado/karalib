#ifndef KAR_FILEFORMAT_H_
#define KAR_FILEFORMAT_H_
#include <stdint.h>
#include <fstream>

namespace kar
{

/**
 * midi file format
 */

struct MidiFile
{
    struct HData
    {
        struct HChuck
        {
            uint8_t         mthd_[5];
            uint32_t        sz_chunk;
            uint16_t        format;
            uint16_t        sz_track;
            uint16_t        time_division;

            void ReadFile(std::ifstream& file);

        } chuck_header;

        struct HTrack
        {
            uint8_t     mtrk[5];
            uint32_t    sz_chunk;
            uint8_t     *data;

            void ReadMemory(uint8_t *mem);

        } *trk_array;

    } *data;

    long Load(uint8_t* buffer, unsigned long size, const char* filename);
    unsigned long RequiredBufferSize(const char* filename);
};




}
#endif
