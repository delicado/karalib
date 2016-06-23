#include "ksong.h"
#
namespace kar
{

bool KSong::MuteVoice(bool t)
{
    return this->MuteTrack(3, t);
}
}
