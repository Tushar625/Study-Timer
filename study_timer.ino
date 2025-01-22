// https://github.com/PaulStoffregen/MsTimer2/blob/f90127ce8289b73c2e74b8be222ab7c755621717/MsTimer2.h

#include<MsTimer2.h>  // implements a software interrupt that runs a function at specific interval of time
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
#define BLINK_DURATION 500lu  // in milli seconds



// global variables

STATE_MACHINE sm;

unsigned long time_point;

TIME_POINT work_time; // stores the time of work



extern class REST_STATE rest_state;

//==================
// define work state
//==================

class WORK_STATE : public BASE_STATE
{   
    unsigned long work_duration;

    void Enter()	// initialze this state
		{
        ping();

        time_point = millis();

        work_duration = (work_time.minute * 60 + work_time.second) * 1000lu/*in milliseconds*/;

        OLED.clear();

        set_color(WORK_COLOR);
    }

		void Loop()
		{
        // state change

        if(millis() - time_point > work_duration)
        {
            sm.change_to(rest_state);
        }
    }
} work_state;



extern class EXTRA_REST_STATE extra_rest_state;

//==================
// define rest state
//==================

class REST_STATE : public BASE_STATE
{
    TIME_POINT tp;

    void Enter()	// initialze this state
		{
        ping();

        time_point = millis();

        // calculating resting time and adding it up to the unused resting time

        auto total_resting_seconds = (work_time.minute * 60 + work_time.second/*total seconds of work*/) / 4;

        tp.second += total_resting_seconds % 60;

        if(tp.second >= 60)
        {
            tp.second -= 60;

            ++tp.minute;    
        }

        tp.minute += total_resting_seconds / 60;

        if(tp.minute > MAX_REST_MINS)
        {
            tp.minute = MAX_REST_MINS;
        }

        tp.no_point();

        OLED.print(tp);

        set_color(REST_COLOR);
    }

		void Loop()
		{
        // display remaining time

        auto new_time_point = millis();

        // after some testing I found 997 millisecond is the best to represent 1 second in this system

        if(new_time_point - time_point >= 997)
        {
            time_point = new_time_point;  // updating old time point
            
            tp.decrement();

            OLED.print(tp);     
        }

        // state change

        if(tp.minute == 0 && tp.second == 0)
        {
            // resting time is over

            sm.change_to(extra_rest_state);
        }
        else if(encoder_button.pressed())
        {
            // skip rest

            sm.change_to(extra_rest_state);
        }
    }
} rest_state;



//==================
// define rest state
//==================

class COUNT_STATE : public BASE_STATE
{
    TIME_POINT *tp;

    void Enter()	// initialze this state
		{
        ping();

        // after some testing I found 997 millisecond is the best to represent 1 second in this system

        timer.set_duration(997);

        timer.start();

        tp->no_point();

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



extern class SETTING_STATE setting_state;

//========================
// define extra_rest state
//========================

// for now this state uses interrupts for blinking light, revert back to time point method if needed in future

class EXTRA_REST_STATE : public BASE_STATE
{
    bool work_mode;

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
                work_mode ? work_time.no_point() : work_time.point_up();
            }

            OLED.print(work_time);
        }

        // selection and change state

        if(encoder_button.pressed())
        {
            switch((int)encoder.getPosition())
            {
                case -1: // change TO SETTING state
                
                        work_time.select_minute();
                        
                        sm.change_to(setting_state, &work_time, &(work_time.minute), 60, 0);
                        
                        break;

                case 1: // change TO SETTING state

                        work_time.select_second();
                        
                        sm.change_to(setting_state, &work_time, &(work_time.second), 59, 0);
                        
                        break;

                case 0: // change TO WORK state

                        while((encoder_button.update(), encoder_button.isPressed()))
                        {
                            if(encoder_button.currentDuration() > 2000)
                            {
                                ping();

                                work_mode = !work_mode;

                                work_mode ? work_time.no_point() : work_time.point_up();

                                OLED.print(work_time);

                                return;
                            }
                        }

                        MsTimer2::stop(); // stop the blinking LED

                        if(work_mode)
                        {
                            sm.change_to(work_state);
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

    EXTRA_REST_STATE() : work_mode(true)
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

// this state is used to set working time

class SETTING_STATE : public BASE_STATE
{
    TIME_POINT *tp;

    uint8_t *data;  // points to a member data of TIME_POINT (minute or second) object received

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
            // limit the position between MAX and MIN
            
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

        // selection

        if(encoder_button.pressed())
        {
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

    // initialize work timer

    work_time.minute = WORK_MINS;

    work_time.second = WORK_SECS;

    // set initial state

    sm.change_to(extra_rest_state);
}

void loop()
{
    encoder_button.update();

    encoder.tick();

    sm.Loop();
}
