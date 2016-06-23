
#include <karaoke.h>
#include <windows.h>
#include <iostream>

#include <typeinfo>

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
class VConsoleOut
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
    VConsoleOut();
    /**
     * destructor.
     */
    ~VConsoleOut();

    /**
     * sets the position of the console cursor.
     * returns 'true' if successful, 'false' if an error occured.
     */
    bool SetCursorPos(unsigned long x, unsigned long y);

    /**
     * returns the cursor position
     * see 'ECOORD' class.
     */
    ECOORD GetCursorPos();

    /**
     * sets the cursor position from an ECOORD parameter.
     */
    ECOORD SetCursorPos(const ECOORD& coord);

    /**
     * sets the text color and background color.
     * see CCOLOR enum
     */
    void SetTextAttribute(WORD attrib);

    /**
     * clears the screen.
     */
    void Clear();

    /**
     * clears a line starting from 'start' and ends at (start + lines).
     */
    void Clear(unsigned long start, unsigned long lines);

    /**
     * hides the cursor.
     */
    void HideCursor();
};

/** a sample 'text' callback function
 * track_num, the track number of the song.
 * event_type, the type of text event. it can be a 'title', 'artist', 'paragraph' etc.
 * str, the string associated with the event.
 * user, user specified data. you should cast it to the original type passed to karaoke::set_text_callback().
 */
long Callback(unsigned long track_num, uint8_t event_type, const std::string& str, void* user);

int main(int argc, const char *argv[])
{
    const char *tmp[] =
    {
        "stuff",
        //"../media/Eraserheads - Ang Huling El Bimbo.kar",
        //"../media/Eraserheads - Kailan.kar",
        //"../media/Eraserheads - Kaliwete.kar",
        "../media/Eraserheads - Para Sa Masa.kar",
        "../media/Eraserheads - Pare ko.kar",
        "../media/Eraserheads - With A Smile.kar",
        "../media/Orient Pearl - Pagsubok.kar",
        "../media/Parokya ni Edgar - Halaga.kar",
        "../media/Parokya ni Edgar - Harana.kar",
        "../media/pink-try.kar",
        "../media/Rico J. Puno - Kapalaran.kar",
        "../media/Rivermaya - Alab Ng Puso.kar",
        "../media/Rivermaya - Himala.kar",
        "../media/Rivermaya - Hinahanap- Hanap.kar",
        "../media/Rivermaya - Kisapmata.kar",
        "../media/Rivermaya - Nerbyoso.kar",
        "../media/Roel Cortez - Sa mata makikita.kar",
        "../media/Sponge Cola - Nakapagtataka.kar",
        "../media/South Border - Kahit Kailan.kar",
        "../media/Shamrock - Alipin.kar",
        "../media/Shamrock - Hold On.kar",
        "../media/Zsa Zsa Padilla - Ikaw Lamang.kar",
        "../media/Willy Garte - Nasaan ka.kar",
        "../media/VST & Company - Awitin mo.kar"
    };

    argv = tmp;
    argc = 22;

    if(argc > 1)
    {
        // create a 'karaoke' object.
        kar::Karaoke *k = new kar::Karaoke;

        // create a the 'user' data for our 'callback' function.
        VConsoleOut *out = new VConsoleOut();
        // vconsole_out has a member function to hide the cursor. you can invoke it.
        out->HideCursor();

        // tell the 'karaoke' object that you want 'callback' as the text handler and
        // a vconsole_out as its data.
        k->SetTextCallback(Callback, out);

        // queue the songs.
        if(argc > 1)
        {
            for(unsigned long i = 1; i < argc; i++)
            {
                k->Queue(argv[i]);
            }
        }
        // play the songs, then start the polling.
        k->Play(12, false);
        // poll the timer(scheduler class). k-poll() will return 'false' if there are no more
        // 'song' objects to play.
        while(k->Poll())
        {
            // 'sleep' the thread so that CPU cycles wont be wasted.
            Sleep(1);
        }
    } else
    {
        std::cout << "USAGE: cskara file1 file2 ..." << std::endl;
    }
    return 0;
}


long Callback(unsigned long track_num, uint8_t event_type, const std::string& str, void* user)
{
    VConsoleOut* cs = (VConsoleOut*) user;
    switch(event_type)
    {
        // paragraph event.
    case ESET::paragraph:
        cs->Clear(5, 8);
        cs->SetTextAttribute( CCOLOR::FBWHITE );
        for(unsigned long i = 0; i < str.size(); i++)
        {
            if(str[i] != '/')
                std::cout << str[i];
            else
            {
                printf("\n\t");
            }
        }
        cs->SetCursorPos(0, 5);
        break;
        // a single line of text event.
    case ESET::line:
        cs->SetTextAttribute( CCOLOR::FGREEN | CCOLOR::FBRIT );
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
        ECOORD coord = cs->GetCursorPos();
        cs->SetCursorPos(0, 1);
        cs->SetTextAttribute( CCOLOR::FWHITE );
        std::cout << "Title: " << str << std::endl;
        cs->SetCursorPos(coord);
    }
    break;
    // display the artists name/names.
    case ESET::artist:
    {
        ECOORD coord = cs->GetCursorPos();
        cs->SetCursorPos(0, 2);
        cs->SetTextAttribute( CCOLOR::FWHITE );
        std::cout << "Artist: " << str << std::endl;
        cs->SetCursorPos(coord);
    }
    break;
    // clear the screen when this ESET::ended event is received.
    // also, clear the screen out when an ESET::start event is received.
    case ESET::ended:
    case ESET::started:
        cs->Clear();
        break;
    }
    return 0;
}

VConsoleOut::VConsoleOut() : cur_pos_(0, 0)
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

VConsoleOut::~VConsoleOut()
{
    delete [] clr_;
}

bool VConsoleOut::SetCursorPos(unsigned long x, unsigned long y)
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

ECOORD VConsoleOut::GetCursorPos()
{
    GetConsoleScreenBufferInfo(handle_, &scr_info_);
    return ECOORD(scr_info_.dwCursorPosition);
}

ECOORD VConsoleOut::SetCursorPos(const ECOORD& coord)
{
    ECOORD c = this->GetCursorPos();
    this->SetCursorPos(coord.X, coord.Y);
    return c;
}

void VConsoleOut::SetTextAttribute(WORD attrib)
{
    SetConsoleTextAttribute(handle_, attrib);
}

void VConsoleOut::Clear()
{
    DWORD written = 0;
    for(unsigned long y = 0; y < scr_info_.dwSize.Y; y++)
    {
        this->SetCursorPos(0, y);
        WriteConsoleA(handle_, clr_, scr_info_.dwSize.X, &written, NULL);
    }
    this->SetCursorPos(0, 0);
}

void VConsoleOut::Clear(unsigned long start, unsigned long lines)
{
    DWORD written = 0;
    for(unsigned long y = start; y < (start + lines); y++)
    {
        this->SetCursorPos(0, y);
        WriteConsoleA(handle_, clr_, scr_info_.dwSize.X, &written, NULL);
    }
    this->SetCursorPos(0, start);
}

void VConsoleOut::HideCursor()
{
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 20;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(handle_, &info);
}
