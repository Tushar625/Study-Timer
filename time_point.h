#ifndef TIME_POINT_HEADER
#define TIME_POINT_HEADER

class TIME_POINT
{
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
        point = 24; // up
    }

    void point_second()
    {
        point = 26; // right
    }

    void point_minute()
    {
        point = 27; // left
    }

    void select_second()
    {
        point = 16; // right
    }

    void select_minute()
    {
        point = 17; // left
    }

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

    void time_string(char *str) const
    {
        sprintf(str, "%.2d%c%.2d", minute, point, second);
    }
};

#endif