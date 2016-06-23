#ifndef KAR_KTYPES_H_
#define KAR_KTYPES_H_

#include <vector>

namespace kar
{

/**
 * callback type for meta events.
 * @param track_num track number
 * @param meta_type type of meta event.
 * @param data pointer the data for the event.
 * @param size the size of the buffer pointed to by 'data'
 * @param user user specified data
 */
typedef long (*EVENT_CALLBACK_META)(unsigned long track_num, unsigned long meta_type, const char *data, unsigned long size, void* user);

/**
 * callback type for midi events.
 * @param track_num track number
 * @param type type of midi event.
 * @param param1 first byte.
 * @param param2 second byte.
 * @param user user specified data.
 */
typedef long (*EVENT_CALLBACK_MIDI)(unsigned long track_num, uint8_t event_type, uint8_t param1, uint8_t param2, void* user);

/**
 * callback type for system events.
 * @param track_num track number
 * @param sysex_type type of system event.
 * @param pointer to the data for the event.
 * @param size size of data pointed to by 'data'.
 * @param user user specified data.
 */
typedef long (*EVENT_CALLBACK_SYSM)(unsigned long track_num, unsigned long sysex_type, const char* data, unsigned long size, void* user);

/**
 * callback type for string events.
 * @param track_num track number
 * @param event_type type of string event.
 * @param pointer to the data for the event.
 * @param size size of data pointed to by 'data'.
 * @param user user specified data.
 */
typedef long (*EVENT_CALLBACK_TEXT)(unsigned long track_num, uint8_t event_type, const std::string& str, void* user);


/**
 * kar callback type. parameters are the same  for the system callback.
 */
typedef EVENT_CALLBACK_SYSM  EVENT_CALLBACK_KSYS;


typedef EVENT_CALLBACK_META  CB_META;
typedef EVENT_CALLBACK_MIDI  CB_MIDI;
typedef EVENT_CALLBACK_KSYS  CB_KSYS;
typedef EVENT_CALLBACK_TEXT  CB_TEXT;

class KPosType
{
    std::vector<long> _pos;
public:

};

}
#endif
