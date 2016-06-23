#ifndef KAR_ERROR_H_
#define KAR_ERROR_H_

#include <stdexcept>
#include <cstring>
#include <stdint.h>

namespace kar
{
class ErrorBase : public std::exception
{
    char _msg[256];
protected:
    void AddHex(uint8_t byte)
    {
        char t[12];
        sprintf(t, "%x", byte);
        strcat(_msg, t);
    }

    void AddString(const char* str)
    {
        strcat(_msg, str);
    }
public:
    ErrorBase(const char* t)
    {
        strcpy(_msg, t);
    }

    virtual const char* what() const throw()
    {
        return _msg;
    }

};

    /**
     * thrown if an event status byte is not recognized.
     */

class UnrecognizedEvent : public ErrorBase
{
public:
    UnrecognizedEvent(uint8_t status_byte) : ErrorBase("unrecognized event : 0x")
    {
        this->AddHex(status_byte);
    }
};
    /**
     * thrown if cannot allocate memory, the name of the class::function must be specified
     */

class OutOfMemory : public ErrorBase
{
public:
    OutOfMemory(const char* function_name) : ErrorBase("out of memory in ")
    {
        this->AddString(function_name);
    }
};

class KarLogicError : public ErrorBase
{
public:
      KarLogicError(const char* function_name, const char* cause) : ErrorBase("logic error in ")
      {
          this->AddString(function_name);
          this->AddString(" : ");
          this->AddString(cause);
      }
};

class KarBadError : public ErrorBase
{
public:
    KarBadError(const char* function_name) : ErrorBase("bad error in ")
    {
        this->AddString(function_name);
    }
};

class KarOutOfRange : public ErrorBase
{
public:
    KarOutOfRange(const char* function_name) : ErrorBase("out of range in ")
    {
        this->AddString(function_name);
    }
};

}



#endif
