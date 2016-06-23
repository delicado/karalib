/**
 * @file event.h
 * a header for event.cpp
 * @author Vester
 * @version 0.1
 * @bug no bug is known yet.
 */

 /*
 * TO DO :
 * rewrite the function names to make it shorter.
 */


#ifndef KAR_EVENT_H_
#define KAR_EVENT_H_
#include "utils.h"
#include "error.h"


namespace kar
{

/**
 * callback types
 */

enum
{
    META,
    MIDI,
    SYSTEM
};

/**
 * event class
 */
struct  Event
{
private:
    uint8_t                 _running_status;
    uint8_t                *_start_addr;
protected:
    void InternalInit();
public:

    unsigned long           delta_time;
    uint8_t                 event_type;
    uint8_t                 callback_type;
    union
    {
        struct
        {
            uint8_t         type;
            unsigned long   data_size;
            char*           data;
        } meta;

        struct
        {
            uint8_t         type;
            uint8_t         param1;
            uint8_t         param2;
        } midi;

        struct
        {
            unsigned long   data_size;
            char           *data;
        } sys;
    } ev;
    unsigned long           data_size;
    char                   *data;

    /**
    * default constructor
    * does nothing.
    */
    Event();
    /**
     * constructor.
     * @param rs running status byte.
     */
    Event(uint8_t rs);

    /**
     * copy contructor.
     * @param e event to be copied.
     */
    Event(const Event& e);

    /**
     * sets the running status.
     * before this class can be used. it must know the previous status byte that was read,
     * used to implement a running status mechanism. (see MIDI specification).
     * @param running_stat running status byte.
     */
    void SetRunningStatus(uint8_t running_stat);

    /**
     * returns the current running status.
     * @return the current running status byte.
     */
    uint8_t GetRunningStatus() const;

    /**
     * assignment operator ; copy operator
     * @param t object to be copied.
     * @return the object itself.
     */
    const Event& operator= (const Event& t);

    /**
     * reads a single event from a buffer in memory.
     * the 'index' will be incremented. and will point to the next event.
     * @param data_ address of a buffer in which events will be read.
     * @param size the size of the buffer
     * @param index an index to the start of the event. after each read, 'index' will be incremented
     *        to point to the next event.
     * @return 'false' if there is no event to read (end of track). otherwise its 'true'.
     */
    bool ReadEvent(const char* data, unsigned long size, unsigned long& index);

    /**
     * returns the address where the event was read.
     * @return the address of the event, including the delta time.
     */
     uint8_t *GetAddress() const;
};

}
#endif
