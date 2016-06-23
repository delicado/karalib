#include "fileformat.h"
#include "utils.h"

#include <iostream>

namespace kar
{

void MidiFile::HData::HChuck::ReadFile(std::ifstream& file)
{
    std::ios::pos_type pos = file.tellg();

    file.seekg(0, std::ios::beg);
    file.read((char*)mthd_, 4);
    sz_chunk = ReadBigEndianDWord(file);
    format = ReadBigEndianWord(file);
    sz_track = ReadBigEndianWord(file);
    time_division = ReadBigEndianWord(file);

    file.seekg(pos);
}

void MidiFile::HData::HTrack::ReadMemory(uint8_t *mem)
{
    memcpy(mtrk, mem, 5);
    mem += 4;
    unsigned long temp = 0;
    sz_chunk = ReadBigEndianDWord((char*)mem, temp);
    data = mem + 4;
}

unsigned long MidiFile::RequiredBufferSize(const char* filename)
{
    unsigned long result = 0;

    std::ifstream file(filename, std::ios::binary | std::ios::in);
    if(!file)
    {

    } else
    {
        HData::HChuck ch;
        ch.ReadFile(file);
        result = sizeof(HData) + (sizeof(HData::HTrack) * ch.sz_track);
        result += GetFileSize(file);
    }

    return result;
}

long MidiFile::Load(uint8_t* buffer, unsigned long size, const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::in);
    if(!file)
    {

    } else
    {
        data = (HData*) buffer;
        uint8_t *cptr = (uint8_t*) new ((void*)data) HData();
        data->chuck_header.ReadFile(file);
        cptr += sizeof(HData);
        data->trk_array = (HData::HTrack*) cptr;
        cptr += sizeof(HData::HTrack) * data->chuck_header.sz_track;
        ReadFileToMemory(file, cptr);
        cptr += 14;
        for(unsigned long i = 0; i < data->chuck_header.sz_track; i++)
        {
            new ((void*)&(data->trk_array[i])) HData::HTrack();
            data->trk_array[i].ReadMemory(cptr);
            cptr += 8 + data->trk_array[i].sz_chunk;
        }
    }

    return size;
}

}
