
#include "..\..\include\karaoke.h"
#include <windows.h>
#include <iostream>

using namespace std;

/**
 * 2D a coordinate class. subclassed from WinAPI COORD.
 * used throughout this test program.
 */

struct ECOORD : public COORD
{
    ECOORD() { }
    ECOORD(long x, long y)
    {
        X = x;
        Y = y;
    }
    ECOORD(const ECOORD& coord)
    {
        X = coord.X;
        Y = coord.Y;
    }
    ECOORD(const COORD& coord)
    {
        X = coord.X;
        Y = coord.Y;
    }
};

/**
 * parameter for vconsole_out::set_text_attrib(WORD attrib).
 * see vconsole_out
 */

namespace CCOLOR
{
enum
{
    FRED            = 0x04,
    FGREEN          = 0x02,
    FBLUE           = 0x01,
    FBRIT           = 0x08,
    FWHITE          = 0x07,
    FBWHITE         = 0x0F,

    BRED            = 0x40,
    BGREEN          = 0x20,
    BBLUE           = 0x10,
    BBRIT           = 0x80,
    BWHITE          = 0x70,
    BBWHITE         = 0xF0
};
}

/**
 * console routines class.
 */
class vconsole_out
{
    ECOORD                 cur_pos_;
    char                  *clr_;
    unsigned long          sz_;
    HANDLE handle_;
    CONSOLE_SCREEN_BUFFER_INFO scr_info_;
public:
    /**
     * default constructor.
     */
    vconsole_out();
    /**
     * destructor.
     */
    ~vconsole_out();

    /**
     * sets the position of the console cursor.
     * returns 'true' if successful, 'false' if an error occured.
     */
    bool set_cursor_pos(unsigned long x, unsigned long y);

    /**
     * returns the cursor position
     * see 'ECOORD' class.
     */
    ECOORD get_cursor_pos();

    /**
     * sets the cursor position from an ECOORD parameter.
     */
    ECOORD set_cursor_pos(const ECOORD& coord);

    /**
     * sets the text color and background color.
     * see CCOLOR enum
     */
    void set_text_attrib(WORD attrib);

    /**
     * clears the screen.
     */
    void clear();

    /**
     * clears a line starting from 'start' and ends at (start + lines).
     */
    void clear(unsigned long start, unsigned long lines);

    /**
     * hides the cursor.
     */
    void hide_cursor();
};

/** a sample 'text' callback function
 * track_num, the track number of the song.
 * event_type, the type of text event. it can be a 'title', 'artist', 'paragraph' etc.
 * str, the string associated with the event.
 * user, user specified data. you should cast it to the original type passed to karaoke::set_text_callback().
 */
long callback(unsigned long track_num, uint8_t event_type, const std::string& str, void* user);

int main(int argc, const char *argv[])
{
    if(argc > 1)
    {
        // create a 'karaoke' object.
        kar::karaoke *k = new kar::karaoke;

        // create a the 'user' data for our 'callback' function.
        vconsole_out *out = new vconsole_out();
        // vconsole_out has a member function to hide the cursor. you can invoke it.
        out->hide_cursor();

        // tell the 'karaoke' object that you want 'callback' as the text handler and
        // a vconsole_out as its data.
        k->set_text_callback(callback, out);

        // queue the songs.
        if(argc > 1)
        {
            for(unsigned long i = 1; i < argc; i++)
            {
                k->queue(argv[i]);
            }
        }
        // play the songs, then start the polling.
        k->play(0, false);
        // poll the timer(scheduler class). k-poll() will return 'false' if there are no more
        // 'song' objects to play.
        while(k->poll())
        {
            // 'sleep' the thread so that CPU cycles wont be wasted.
            Sleep(3);
        }
    } else
    {
        std::cout << "USAGE: cskara file1 file2 ..." << std::endl;
    }
    return 0;
}


long callback(unsigned long track_num, uint8_t event_type, const std::string& str, void* user)
{
    vconsole_out* cs = (vconsole_out*) user;
    switch(event_type)
    {
        // paragraph event.
    case ESET::paragraph:
        cs->clear(5, 8);
        cs->set_text_attrib( CCOLOR::FBWHITE );
        for(unsigned long i = 0; i < str.size(); i++)
        {
            if(str[i] != '/')
                std::cout << str[i];
            else
            {
                printf("\n\t");
            }
        }
        cs->set_cursor_pos(0, 5);
        break;
        // a single line of text event.
    case ESET::line:
        cs->set_text_attrib( CCOLOR::FGREEN | CCOLOR::FBRIT );
        for(unsigned long i = 0; i < str.size(); i++)
        {
            if(str[i] != '/')
                std::cout << str[i];
            else
            {
                printf("\n\t");
            }
        }
        break;
        // display the title.
    case ESET::title:
    {
        ECOORD coord = cs->get_cursor_pos();
        cs->set_cursor_pos(0, 1);
        cs->set_text_attrib( CCOLOR::FWHITE );
        std::cout << "Title: " << str << std::endl;
        cs->set_cursor_pos(coord);
    }
    break;
    // display the artists name/names.
    case ESET::artist:
    {
        ECOORD coord = cs->get_cursor_pos();
        cs->set_cursor_pos(0, 2);
        cs->set_text_attrib( CCOLOR::FWHITE );
        std::cout << "Artist: " << str << std::endl;
        cs->set_cursor_pos(coord);
    }
    break;
    // clear the screen when this ESET::ended event is received.
    // also, clear the screen out when an ESET::start event is received.
    case ESET::ended:
    case ESET::started:
        cs->clear();
        break;
    }

    return 0;
}

vconsole_out::vconsole_out() : cur_pos_(0, 0)
{
    handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
    if(handle_ != INVALID_HANDLE_VALUE)
    {
        GetConsoleScreenBufferInfo(handle_, &scr_info_);
        sz_ = scr_info_.dwSize.X;
        clr_ = new char [sz_];
        for(unsigned long i = 0; i < sz_; i++)
            clr_[i] = ' ';
    }
}

vconsole_out::~vconsole_out()
{
    delete [] clr_;
}

bool vconsole_out::set_cursor_pos(unsigned long x, unsigned long y)
{
    bool status = false;
    if(x < scr_info_.dwSize.X && y < scr_info_.dwSize.Y)
    {
        cur_pos_ = ECOORD(x, y);
        if(SetConsoleCursorPosition(handle_, cur_pos_) != 0)
            status = true;
    }
    return status;
}

ECOORD vconsole_out::get_cursor_pos()
{
    GetConsoleScreenBufferInfo(handle_, &scr_info_);
    return ECOORD(scr_info_.dwCursorPosition);
}

ECOORD vconsole_out::set_cursor_pos(const ECOORD& coord)
{
    ECOORD c = this->get_cursor_pos();
    this->set_cursor_pos(coord.X, coord.Y);
    return c;
}

void vconsole_out::set_text_attrib(WORD attrib)
{
    SetConsoleTextAttribute(handle_, attrib);
}

void vconsole_out::clear()
{
    DWORD written = 0;
    for(unsigned long y = 0; y < scr_info_.dwSize.Y; y++)
    {
        this->set_cursor_pos(0, y);
        WriteConsole(handle_, clr_, scr_info_.dwSize.X, &written, NULL);
    }
    this->set_cursor_pos(0, 0);
}

void vconsole_out::clear(unsigned long start, unsigned long lines)
{
    DWORD written = 0;
    for(unsigned long y = start; y < (start + lines); y++)
    {
        this->set_cursor_pos(0, y);
        WriteConsole(handle_, clr_, scr_info_.dwSize.X, &written, NULL);
    }
    this->set_cursor_pos(0, start);
}

void vconsole_out::hide_cursor()
{
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 20;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(handle_, &info);
}
