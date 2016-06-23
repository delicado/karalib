/**
 * @file karaoke.h
 * @author Vester
 * @version 0.1
 */

#ifndef KAR_KARAOKE_H_
#define KAR_KARAOKE_H_

#include <vector>
#include <cstdint>
#include <string>

#include "../karalib_global.h"
#include "midi.h"
#include "flags.h"

namespace kar
{

/**
 * "karaoke files are midi files too."
 * and so our karaoke class is derived from midi class.
 * we add karaoke specific features to this class. like mute the voice (actually a tune) etc..
 */

class KARALIBSHARED_EXPORT Karaoke : public Midi
{
    enum
    {
        BUFFLYRICS = BIT_1,
        DISABLE_LYRICS = BIT_2
    };

    uint32_t                  _flag;            //!< flags, bits are in _FKARA
    struct
    {
        unsigned long             param1;
        unsigned long             param2;
        std::string               strbuffer;
    } _temps;                                   //!< temporaries. because callback functions needs to save their states.

    CB_KSYS                   _prev_c_ksys;     //!< previous KSYS callback. substited by this class at creation.
    void                     *_prev_u_ksys;     //!< user data for the previous KSYS callback
    CB_TEXT                   _c_text;          //!< a CB_TEXT callback, @see EVENT_CALLBACK_TEXT
    void                     *_u_text;          //!< user specified data for c_text_ callback.
    std::vector<std::string>  _string_buff;     //!< buffered lyrics is stored in here.
    unsigned long             _rindex;          //!< index for string_buff_.
protected:
    /**
     * buff strings, method_number specifies how to process it.
     * this should only be called if a song is not playing. (because a variable is shared by this function
     * and the code that involves in playing a song, to avoid unused variables.) this will be changed in the future.
     * @param method_number, 0 - lyrics, each entry in string_buff_ is a paragraph.
     */
    void BuffStrings(unsigned long track_index, long method_number);

    /**
     * meta procedure for this class.
     * this does the text processing for text and lyrics events, then passes it
     * to an CB_TEXT function pointed by c_text_.
     * @see EVENT_CALLBACK_META
     */
    static long CBMetaProc(unsigned long track_num, unsigned long meta_type, const char *data, unsigned long Size, void* user);

    /**
     * ksys procedure for this class.
     * this class needs to know if a song has started and finished a song. and so this class supplies a CB_KSYS callback.
     * the constructor for this class does the substitution. saves the previous CB_KSYS callback to prev_c_ksys_.
     * so that events can still be passed to it after this callback.
     * @see EVENT_CALLBACK_SYSM
     */
    static long CBKSys(unsigned long track_num, unsigned long type, const char* data, unsigned long Size, void* user);
public:
    Karaoke();

    /**
     * plays a single song from the queue.
     * @param ID id of the song to play.
     * @param single_play 'true' stop after the song ended, 'false' to continue.
     */
    virtual unsigned long Play(unsigned long ID, bool single_play = false);

    /**
     * returns data associated with CB_TEXT callback
     * @return data associated with CB_TEXT callback.
     */
    void* GetTextUData() const;

    /**
     *  sets a text callback
     * @param callback a CB_TEXT callback.
     * @param user user specified data.
     * @return previous CB_TEXT callback.
     */
    CB_TEXT SetTextCallback(CB_TEXT callback, void* user = NULL);
};

}

#endif
