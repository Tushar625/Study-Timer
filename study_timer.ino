// https://github.com/PaulStoffregen/MsTimer2/blob/f90127ce8289b73c2e74b8be222ab7c755621717/MsTimer2.h

/*
    main project file, here I define all the states

    note:
    
    This device can operate in two distinct modes

    i.  Work Timer
    ii. Normal Timer

    Work Timer mode:

    Extra Rest
    |       ^
    V       |
    Work    |
    |       |
    V       |
    Rest----|

    Normal Timer mode:

    Extra Rest (Pause)
    |       ^
    V       |
    Count---|
*/

#include<MsTimer2.h>  // version 1.1, implements a software interrupt that runs a function at specific interval of time
#include"timer.h"
#include"time_point.h"
#include"push_button.h"
#include"rotary_encoder.h"
#include"oled_display.h"
#include"buzzer.h"
#include"rgb_led.h"
#include"state_machine.h"



// colors

#define WORK_COLOR {255 / 5, 100 / 5, 0} /*dim orange*/
#define REST_COLOR {0, 255, 0} /*green*/
#define EXTRA_REST_COLOR {255, 0, 0} /*red*/



// durations

#define WORK_MINS /*0*/ 20
#define WORK_SECS /*10*/ 0
#define MAX_REST_MINS 20

// durations in milli seconds

#define BLINK_DURATION 500lu
// after some testing I found 997 millisecond is the best to represent 1 second in this system
#define SECOND_DURATION 997lu



// global variables

STATE_MACHINE sm;



/*
    the states are actually global variables, following state objects are declared
    here because they are referenced before they are defined
*/

extern class REST_STATE rest_state;

extern class EXTRA_REST_STATE extra_rest_state;

extern class SETTING_STATE setting_state;



//==================
// define work state
//==================

class WORK_STATE : public BASE_STATE
{   
    unsigned work_sec;  // work time in seconds

    void Enter()	// initialze this state
		{
        ping();

        timer.set_duration(work_sec * 1000lu);

        timer.start();

        OLED.clear();

        set_color(WORK_COLOR);
    }

		void Loop()
		{
        // state change

        if(timer.time_out())
        {
            sm.change_to(rest_state, work_sec);

            return;
        }

        // progress bar display how much time is left

        OLED.progress_bar(timer.remaining_time());
    }

    public:

    // receiving the work time in seconds

    void init(unsigned _work_sec)
    {
        work_sec = _work_sec;
    }
} work_state;



//==================
// define rest state
//==================

class REST_STATE : public BASE_STATE
{
    TIME_POINT tp;  // used to display remaining rest time on the screen

    void Enter()	// initialze this state
		{
        ping();

        /*
            timer is used to decrement the time point 'tp' per second
        */

        timer.set_duration(SECOND_DURATION);

        timer.start();

        OLED.print(tp);

        set_color(REST_COLOR);
    }

		void Loop()
		{
        // display remaining rest time

        if(timer.time_out())
        {   
            tp.decrement();

            OLED.print(tp);
        }

        // state change

        if((tp.minute == 0 && tp.second == 0) || encoder_button.pressed())
        {
            // resting time is over or user wants to skip rest

            sm.change_to(extra_rest_state);
        }
    }

    public:

    // receiving the work time in seconds and calculating our time point

    void init(unsigned work_sec)
    {
        // calculating resting time and adding it up to the unused resting time

        auto total_resting_seconds = work_sec / 4;

        tp.second += total_resting_seconds % 60;

        // tp.second cannot be > 59 before the addition, hence following correction is sufficient

        if(tp.second >= 60)
        {
            tp.second -= 60;

            ++tp.minute;    
        }

        tp.minute += total_resting_seconds / 60;

        // one can't get too much rest, so we truncate max resting time

        if(tp.minute > MAX_REST_MINS)
        {
            tp.minute = MAX_REST_MINS;
        }
    }
} rest_state;



//==================
// define count state
//==================

class COUNT_STATE : public BASE_STATE
{
    TIME_POINT *tp; // receives the time point from extra rest state

    void Enter()	// initialze this state
		{
        ping();

        timer.set_duration(SECOND_DURATION);

        timer.start();

        tp->no_point(); // print ':' between minute and second

        OLED.print(*tp);

        set_color(WORK_COLOR);
    }

		void Loop()
		{
        // display remaining time

        if(timer.time_out())
        {   
            tp->decrement();

            OLED.print(*tp);     
        }

        // state change

        if((tp->minute == 0 && tp->second == 0) || encoder_button.pressed())
        {
            /*
                go to extra rest state when the time is over or
                user presses the button, which actually "pauses"
                the timer
            */
            
            sm.change_to(extra_rest_state);
        }
    }

    public:

    // receive the TIME_POINT object from extra rest state

    void init(TIME_POINT *_tp)
    {
        tp = _tp;
    }
} count_state;



//========================
// define extra_rest state
//========================

class EXTRA_REST_STATE : public BASE_STATE
{
    bool work_mode; // work mode or timer mode

    TIME_POINT work_time; // stores the time of work

    void Enter()	// initialze this state
		{
        ping();

        encoder.setPosition(0);

        work_mode ? work_time.no_point() : work_time.point_up();

        OLED.print(work_time);

        MsTimer2::start();  // start the blinking LED
    }

		void Loop()
		{
        // indication direction

        if((int)encoder.getDirection())
        {
            // limit the position between -1 and 1
            
            if(encoder.getPosition() >= 1)
            {
                work_time.point_second();

                encoder.setPosition(1);
            }
            else if(encoder.getPosition() <= -1)
            {
                work_time.point_minute();

                encoder.setPosition(-1);
            }
            else
            {
                // position is 0

                work_mode ? work_time.no_point() : work_time.point_up();
            }

            OLED.print(work_time);
        }

        // selection and change state

        if(encoder_button.pressed())
        {
            switch((int)encoder.getPosition())
            {
                case -1: 

                        if(held_down(encoder_button))
                        {
                            // the button was pressed down for 2 seconds

                            // set minute to its default value

                            ping();

                            work_time.minute = WORK_MINS; // setting to default

                            OLED.print(work_time);

                            break;
                        }

                        // change TO SETTING state to set minute
                
                        work_time.select_minute();
                        
                        sm.change_to(setting_state, &work_time, &(work_time.minute), 60, 0);
                        
                        break;

                case 1: 

                        if(held_down(encoder_button))
                        {
                            // the button was pressed down for 2 seconds

                            // set second to its default value

                            ping();

                            work_time.second = WORK_SECS; // setting to default

                            OLED.print(work_time);

                            break;
                        }

                        // change TO SETTING state to set second

                        work_time.select_second();
                        
                        sm.change_to(setting_state, &work_time, &(work_time.second), 59, 0);
                        
                        break;

                case 0: 

                        if(held_down(encoder_button))
                        {
                            // the button was pressed down for 2 second

                            // switch between Work Timer and Normal Timer modes

                            ping();

                            work_mode = !work_mode;

                            work_mode ? work_time.no_point() : work_time.point_up();

                            OLED.print(work_time);

                            break;
                        }

                        /*
                            change TO WORK or count state depending on the mode of operation
                            (Work Timer or Normal Timer)
                        */

                        MsTimer2::stop(); // stop the blinking LED

                        if(work_mode)
                        {
                            sm.change_to(work_state, work_time.minute * 60 + work_time.second/*work time in seconds*/);
                        }
                        else
                        {
                            sm.change_to(count_state, &work_time);
                        }
                        
                        break;
            }
        }
    }

    public:

    EXTRA_REST_STATE() : work_mode(true), work_time(WORK_MINS, WORK_SECS)
    {   
        // setting blinking led software interrupt

        MsTimer2::set(BLINK_DURATION, [](){
            static volatile bool blink = true;

            blink = !blink;

            (blink) ? set_color(EXTRA_REST_COLOR) : set_color({0, 0, 0});
        });

        // !!!! when I call it from else where the work mode light turned green for no reason
    }
} extra_rest_state;



//========================
// define setting state
//========================

class SETTING_STATE : public BASE_STATE
{
    TIME_POINT *tp; // receives the time point from extra rest state

    uint8_t *data;  // points to a member data (minute or second) of the TIME_POINT object received

    uint8_t max_rotation, min_rotation;

		void Enter()	// initialze this state
		{
        ping();

        OLED.print(*tp);

        encoder.setPosition(*data);
    }

		void Loop()	// working of this state
		{
        // update minute or second

        if((int)encoder.getDirection())
        {
            // limit the position between MAX and MIN and set *data according to the rotar position
            
            if(encoder.getPosition() > max_rotation)
            {
                encoder.setPosition(min_rotation);
            }
            else if(encoder.getPosition() < min_rotation)
            {
                encoder.setPosition(max_rotation);
            }

            *data = encoder.getPosition();

            OLED.print(*tp);
        }

        // selection and state change

        if(encoder_button.pressed())
        {
            // the value is selected for *data

            sm.change_to(extra_rest_state);
        }
    }

    public:

		void init(TIME_POINT *_tp, uint8_t *_data, uint8_t _max_rotation, uint8_t _min_rotation)
		{
        tp = _tp;

        data = _data;

        max_rotation = _max_rotation;
        
        min_rotation = _min_rotation;
    }
} setting_state;



void setup()
{
    // put your setup code here, to run once:

    rgb_led_setup();

    OLED.setup();

    OLED.clear();

    buzzer_setup();

    button_setup();

    // set initial state

    sm.change_to(extra_rest_state);
}

void loop()
{
    encoder_button.update();

    encoder.tick();

    sm.Loop();
}
