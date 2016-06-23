#include "defprocedures.h"
#include "midiout.h"

long DefaultMetaCallback(unsigned long track_num, unsigned long meta_type, const char *data, unsigned long size, void *user)
{
    if(meta_type == 0x01)
    for(unsigned long i = 0; i < size; i++)
    {

        if(data[i] == '\\')
        {
            cout << endl;
            continue;
        }
        else
        if(data[i] == '/')
        {
            cout << "\n\t";
            continue;
        }

        std::cout << data[i];
    }

    return 0;
}

long DefaultMidiCallback(unsigned long track_num, uint8_t event, uint8_t param1, uint8_t param2, void* user)
{
    kar::MidiOut* out = (kar::MidiOut*) user;

    out->Send(event, param1, param2);
    return 0;
}

long DefaultMidiCheck(unsigned long track_num, uint8_t event, uint8_t param1, uint8_t param2, void* user)
{
    printf("%x %x %x %x %x\n", track_num, event, param1, param2, user);
    DefaultMidiCallback(track_num, event, param1, param2, user);
    return 0;
}
