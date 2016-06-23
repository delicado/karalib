#include "..\..\include\karaoke.h"
#include <iostream>

long my_text_callback(unsigned long track_num,
                      uint8_t event_type,
                      const std::string& str,
                      void* user)
{
    // copy str to temp. so
    // that we can manipulate its contents.
    std::string temp(str);
    switch(event_type)
    {
        case ESET::line:
            // when a line event has the character '/' as the first character,
            // we display a new-line and a tab.
            if(temp.size() > 0 && temp[0] == '/')
            {
                // erase the characer '/' from the string.
                temp.erase(0, 1);
                // display the newline and the tab.
                std::cout << std::endl << "\t";
            }
            // then display the string.
            std::cout << temp;
            break;
        case ESET::paragraph:
            // though ESET::paragraph also has a text with it (in str),
            // we dont need to display it. all we want to do
            // is move the cursor to a new line if we receive this event.
            std::cout << std::endl;
            break;
        case ESET::ended:
            // when a song has ended playing. we receive an ESET::ended event.
            system("CLS");
    }
}

int main()
{
    // kar::karaoke class's size in bytes is a bit large,
    // so lets create kar::karaoke in the heap. maybe in the future,
    // kar::karaoke will just contain a pointer to the actual contents (like kar::song).
    kar::karaoke *k = new kar::karaoke;

    // assign 'my_text_callback' as a text handler, a text handler receives the 'text' events.
    // there are 5 types of events. 'meta', 'midi', 'ksys', 'system' and 'text' events.
    // for a karaoke soft. processing the 'text' event is all you need.

    //k->set_text_callback(my_text_callback, NULL);

    // try loading a song. a song should exist or else kar::karaoke::song will
    // throw an kar::kar_logic_error(const char* reason) exception.
    // lets load an oldie song. candle in the wind!
    kar::song candle_in_the_wind("../media/candle.kar");
    // then queue it.
    k->queue( candle_in_the_wind);

    // load another one. only this time,
    // we just pass the filename of the song.
    // and queue will create the kar::song object internally for the filename.
    k->queue("../media/feel.kar");

    // these are .kar files. so while the songs are playing, karalib will extract the
    // lyrics and pass it to the text handler as an ESET::line and ESET::paragraph events.
    // karalib can play MIDI files too. lets load "ana.mid". ana's song by silverchair.
    k->queue("../media/ana.mid");

    // thats it. lets play the songs by calling kar::karaoke::play()
    // first parameter is the song index to play.
    // put 'false' in second parameter so all the songs will be played.
    // or 'true' so only the song that is indexed by parameter 1 will be played.
    // we want all the songs to play, so lets put 'false' in there.
    k->play(0, false);

    // what is these 'poll()' for?
    // these is the function that actually 'streams' the songs,
    // synchronizes 'events' to be played, determines if a song has done playing so
    // it can stop or load another one, etc.
    // it is called 'poll' because it needs to be called at intervals. recommended
    // intervals is 10 milliseconds or less. smaller for high BPM (beats per minute) songs.
    while(k->poll())
    {
        // an interval smaller than 1 ms is
        // a waste of CPU time (and power).
        // lets suspend this thread by 3 ms.

        // note: Sleep() is an WinAPI function that suspends the current thread by milliseconds.
        // and continues after it elapses.
        // C++11 has std::this_thread::sleep_for(milliseconds), but since no compiler as of
        // yet is fully compliant with the new standard, we will use Sleep() for now.
        Sleep(3);
        // note: Sleep() can destort the playback if you choose to activate another program.
        // thats because Sleep() is not reliable. it sometimes reaches above 1 second even though
        // you specified ms to it.

    }

    return 0;
}


