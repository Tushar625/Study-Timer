
// this file provides a class to store a particular point in time in minutes and seconds

#ifndef TIME_POINT_HEADER
#define TIME_POINT_HEADER

class TIME_POINT
{
    /*
        the "point" character is printed between the minute and second in the display

        I use it as an UI element. For example, it is normally ':' but can be changed
        into a left arrow to point towards "minute" and so on.
    */

    char point;

    public:

    uint8_t minute, second;

    // constructors

    TIME_POINT(uint8_t min, uint8_t sec) : minute(min), second(sec), point(':')
    {}

    TIME_POINT() : minute(0), second(0), point(':')
    {}

    void no_point()
    {
        point = ':';
    }

    void point_up()
    {
        point = 24; // up arrow
    }

    void point_second()
    {
        point = 26; // right arrow
    }

    void point_minute()
    {
        point = 27; // left arrow
    }

    void select_second()
    {
        point = 16; // right facing triangle
    }

    void select_minute()
    {
        point = 17; // left facing triangle
    }

    // decrement the time point

    void decrement()
    {
        if(second)
        {
            --second;
        }
        else if(minute)
        {
            --minute;

            second = 59;
        }
    }

    /*
        takes a character array of length 6 stores the time point in it as a string
        like "mm:ss"
        
        the character between minute and second is determined by "point"
    */

    void time_string(char *str) const
    {
        sprintf(str, "%.2d%c%.2d", minute, point, second);
    }
};

#endif