#ifndef KAR_EXAMPLES_H_
#define KAR_EXAMPLES_H_
#include <iostream>
#include <stdint.h>

using namespace std;

long DefaultMetaCallback(unsigned long track_num, unsigned long meta_type, const char *data, unsigned long size, void *user);
long DefaultMidiCallback(unsigned long track_num, uint8_t event, uint8_t param1, uint8_t param2, void* user);
long DefaultMidiCheck(unsigned long track_num, uint8_t event, uint8_t param1, uint8_t param2, void* user);
#endif
