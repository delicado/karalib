#ifndef KAR_OUT_H
#define KAR_OUT_H
#include <stdint.h>

namespace kar
{

class OutBase
{
public:
    virtual bool Send(uint8_t event, uint8_t param1, uint8_t param2) = 0;
};

}
#endif
