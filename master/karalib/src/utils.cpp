/**
 * @file utils.cpp
 * a source file for utils.h
 * @author Vester
 * @version 0.1
 */


#include "utils.h"
#include "chunk.h"
#include "tracks.h"
#include "error.h"

namespace kar
{

/// HELPER FUNCTIONS
uint16_t ReadBigEndianWord(std::istream& file)
{
    uint16_t buf;
    file.get((((char*)&buf)[1]));
    file.get((((char*)&buf)[0]));
    return buf;
}

uint32_t ReadBigEndianDWord(std::istream& file)
{
    uint32_t buf;
    file.get((((char*)&buf)[3]));
    file.get((((char*)&buf)[2]));
    file.get((((char*)&buf)[1]));
    file.get((((char*)&buf)[0]));
    return buf;
}

unsigned long ReadVLQ(std::istream& file)
{
    union
    {
        uint8_t c[4];
        uint32_t l;
    } U;
    U.l = 0;
    for(int i = 3; i >= 0; i--)
    {
        char temp;
        file.get(temp);
        U.c[i] = temp;
        if(!(U.c[i] & 0x80))
        {
            int bshift = 0;
            for(int j = i; j < 4; j++)
            {
                U.c[j] &= 0x7F;
                if(bshift > 8) bshift = 0;
                if(bshift > 0)
                {
                    uint8_t btemp = 0xFF << bshift;
                    btemp = ~btemp;
                    btemp = uint8_t(U.c[j]) & btemp;
                    btemp = btemp << (8 - bshift);
                    U.c[j-1] |= btemp;
                    U.c[j] >>= bshift;
                }
                bshift++;
            }
            //printf("i:%i a:%x b:%x c:%x d:%x",i, U.c[0], U.c[1], U.c[2], U.c[3]);
            U.l = U.l >> (i * 8);
            break;
        }
    }
    return U.l;
}


unsigned long ReadVLQ(const char* data, unsigned long& index)
{
    union
    {
        uint8_t c[4];
        uint32_t l;
    } U;
    U.l = 0;

    for(int i = 3; i >= 0; i--)
    {
        U.c[i] = data[index++];
        if(!(U.c[i] & 0x80))
        {
            int bshift = 0;
            for(int j = i; j < 4; j++)
            {
                U.c[j] &= 0x7F;
                if(bshift > 8) bshift = 0;
                if(bshift > 0)
                {
                    uint8_t btemp = 0xFF << bshift;
                    btemp = ~btemp;
                    btemp = uint8_t(U.c[j]) & btemp;
                    btemp = btemp << (8 - bshift);
                    U.c[j-1] |= btemp;
                    U.c[j] >>= bshift;
                }
                bshift++;
            }
            //printf("i:%i a:%x b:%x c:%x d:%x",i, U.c[0], U.c[1], U.c[2], U.c[3]);
            U.l = U.l >> (i * 8);
            break;
        }
    }
    //printf("%d, ", U.l);
    return U.l;
}

uint16_t ReadBigEndianWord(const char* data, unsigned long& index)
{
    uint16_t buf;
    ((char*)&buf)[1] = data[index++];
    ((char*)&buf)[0] = data[index++];
    return buf;
}

uint32_t ReadBigEndianDWord(const char* data, unsigned long& index)
{
    uint32_t buf;
    ((char*)&buf)[3] = data[index++];
    ((char*)&buf)[2] = data[index++];
    ((char*)&buf)[1] = data[index++];
    ((char*)&buf)[0] = data[index++];
    return buf;
}

unsigned long CheckVLQ(const char* data, unsigned long size, unsigned long index)
{
    unsigned long out = (unsigned long)-1;
    if(index < size)
    {
        out = index;
        out = ReadVLQ(data, out);
    }
    return out;
}

uint8_t CheckEvent(const char* data, unsigned long size, unsigned long& index, uint8_t running_status)
{
    uint8_t out = 0;
    if(index < size)
    {
        unsigned long temp = index;
        ReadVLQ(data, temp);
        out = data[temp++];
        if(!(out & 0x80))
        {
            out = running_status;
        }
    }
    return out;
}

bool SeekToTrack(std::istream& file, const kar::Chunk& header, unsigned long track_num)
{
    bool status = false;
    if(track_num < header.track_size)
    {
        file.seekg(14, std::ios::beg);
        char ID_string[5];
        long sz;
        for(unsigned long i = 0; i < track_num; i++)
        {
            file.read(ID_string, 4);
            if(strcmp(ID_string, "MTrK"))
            {
                sz = ReadBigEndianDWord(file);
                file.seekg(sz, std::ios::cur);
            }
            else
            {
                status = true;
                break;
            }
        }
    }
    return status;
}

uint8_t* SeekToTrack(uint8_t* mem, unsigned long size, const kar::Chunk& header, unsigned long track_num)
{
    uint8_t*  addr = NULL;
    if(track_num < header.track_size)
    {
        unsigned long temp = 14;
        char ID_string[5];
        long sz;
        for(unsigned long i = 0; i < track_num; i++)
        {
            addr = &mem[temp];
            memcpy(ID_string, addr, 4);
            temp += 4;
            if(strcmp(ID_string, "MTrk") == 0)
            {
                sz = ReadBigEndianDWord((char*)mem, temp);
                temp += sz;
            }
            else
            {
                addr = NULL;
            }
        }
    }
    return addr;
}

unsigned long AlignedAllocSize(unsigned long size)
{
    static const unsigned long alignment = 15;
    if( (size & alignment) != 0 )
        size = (size & ~alignment) + alignment + 1;
    return size;
}


bool MemoryAddressOverlap(const void* iaddr1, unsigned long size1, const void* iaddr2, unsigned long size2)
{
    uint8_t* addr1 = (uint8_t*)iaddr1;
    uint8_t* addr2 = (uint8_t*)iaddr2;

    if((addr1 >= addr2) && (addr1 <= (addr2 + size2))) return true;
    if(((addr1 + size1) >= addr2) && ((addr1 + size1) <= (addr2 + size2))) return true;
    if((addr2 >= addr1) && (addr2 <= (addr1 + size1))) return true;
    if(((addr2 + size2) >= addr1) && ((addr2 + size2) <= (addr1 + size1))) return true;
    return false;
}


std::vector<unsigned long> CheckTrackSizes(std::istream& file, const kar::Chunk& header)
{
    std::vector<unsigned long> tr;

    std::ios::pos_type pos = file.tellg();

    if(strcmp(header.ID_string, "MThd") == 0)
    {
        file.seekg(14, std::ios::beg);
        char ID_string[5];
        long sz;
        for(unsigned long i = 0; i < header.chunk_size; i++)
        {
            file.read(ID_string, 4);
            if(strcmp(ID_string, "MTrK"))
            {
                sz = ReadBigEndianDWord(file);
                tr.push_back(sz);
                file.seekg(sz, std::ios::cur);
            }
            else
            {
                throw kar::ErrorBase(" corrupted file.");
            }
        }
    }

    file.seekg(pos);
    return tr;
}

unsigned long GetFileSize(std::istream& file)
{
    std::ios::pos_type pos = file.tellg();
    file.seekg(0, std::ios::end);
    unsigned long t = file.tellg();
    file.seekg(pos);
    return t;
}

unsigned long ReadFileToMemory(std::istream& file, uint8_t* mem)
{
    std::ios::pos_type pos = file.tellg();
    unsigned long size = GetFileSize(file);
    file.seekg(0, std::ios::beg);
    file.read((char*)mem, size);
    file.seekg(pos);
    return size;
}

}
