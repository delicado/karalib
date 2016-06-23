#ifndef KAR_KSONG_H_
#define KAR_KSONG_H_
#include "song.h"

namespace kar
{

class KSong : public Song
{
public:
    KSong() : Song() { }
    KSong(const char* s) : Song(s) { }

    /**
     * mutes the voice(tune) of the karaoke file if one exist. karaoke midi voice is track number 4
     * if one does not exist, this will mute track 4, whatever instrument is it.
     * @param t true to mute, false to un-mute
     * @return 'false' if successful, 'true' otherwise.
     */
    bool MuteVoice(bool t);
};


}


#endif
