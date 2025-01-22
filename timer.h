#ifndef TIMER_HEADER
#define TIMER_HEADER

class TIMER
{
    unsigned long time_point, duration;

    public:

    void set_duration(unsigned long ms) noexcept
    {
        duration = ms;
    }

    // take a time point

    void start() noexcept
    {
        time_point = millis();
    }

    // returns true wjen time is over and resets the timer

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