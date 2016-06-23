/**
 * @file chunk.cpp
 * a source file for chunk.h
 * @author Vester
 * @version 0.1
 */

#include "chunk.h"

namespace kar
{

SMPTE::SMPTE()
    :fps(0), tpf(0)
{
}


SMPTE::SMPTE(unsigned long t_fps, unsigned long t_tpf)
    :fps(t_fps), tpf(t_tpf)
{
}

Chunk::Chunk() : _ready(false) { };

Chunk::Chunk(std::ifstream& file)
{
    this->ReadHeaderChunk(file);
}

void Chunk::ReadHeaderChunk(std::istream &file)
{
    memset(this, 0, sizeof(Chunk));
    _ready = true;
    std::ios::pos_type pos = file.tellg();
    file.seekg(0, std::ios::beg);
    file.read(ID_string, 4);
    chunk_size = ReadBigEndianDWord(file);
    format_type = ReadBigEndianWord(file);
    track_size = ReadBigEndianWord(file);
    time_division = ReadBigEndianWord(file);
    file.seekg(pos);
}

bool Chunk::IsSMPTE() const
{
#ifdef KAR_DEF_NO_EXCEPT
    if(!ready_) throw kar_logic_error("chunk::is_tpb()", "object is empty");
#endif
    return (time_division & 0x8000);
}


bool Chunk::IsTPB() const
{
#ifdef KAR_DEF_NO_EXCEPT
    if(!ready_) throw kar_logic_error("chunk::is_tpb()", "object is empty");
#endif
    return !(time_division & 0x8000);
}

SMPTE Chunk::GetSMPTE() const
{
#ifdef KAR_DEF_NO_EXCEPT
    if(!ready_) throw kar_logic_error("chunk::get_smpte()", "object is empty");
#endif
    return SMPTE(((time_division & 0x7F00) >> 8), time_division & 0x00FF);
}

uint16_t Chunk::GetTicksPerBeat() const
{
    return time_division & 0x7FFF;
}
}
