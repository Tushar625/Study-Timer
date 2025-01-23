
// this file provides an object for easy handling of timimg

#ifndef TIMER_HEADER
#define TIMER_HEADER

/*
    this class provides a timer like functionality, you set a duration in ms, start the timer
    and check for time out. Upon time out the timer autometically resets and starts again for
    the same duration.
*/

class TIMER
{
    unsigned long time_point, duration;

    public:

    void set_duration(unsigned long ms) noexcept
    {
        duration = ms;
    }

    // take a time point and start the timer

    void start() noexcept
    {
        time_point = millis();
    }

    // returns the remaining time as a fraction

    float remaining_time() noexcept
    {
        return 1 - (millis() - time_point) / (float)duration;
    }

    /*
        returns true once after set duration has been passed and
        resets and restarts the timer
    */

    bool time_out() noexcept
    {
        auto new_time_point = millis();

        if(new_time_point - time_point >= duration)
        {
            time_point = new_time_point;  // updating old time point

            return true;
        }

        return false;
    }
} timer;

#endif