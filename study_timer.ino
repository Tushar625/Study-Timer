// https://github.com/PaulStoffregen/MsTimer2/blob/f90127ce8289b73c2e74b8be222ab7c755621717/MsTimer2.h

#include<MsTimer2.h>  // implements a software interrupt that runs a function at specific interval of time
#include"timer.h"
#include"push_button.h"
#include"rotary_encoder.h"
#include"oled_display.h"
#include"buzzer.h"
#include"state_machine.h"


#define R_PIN 3
#define G_PIN 5
#define B_PIN 6


#define WORK_COLOR {255 / 5, 100 / 5, 0} /*dim orange*/
#define REST_COLOR {0, 255, 0} /*green*/
#define EXTRA_REST_COLOR {255, 0, 0} /*red*/

// durations

#define WORK_MINS /*0*/ 20
#define WORK_SECS /*10*/ 0
#define MAX_REST_MINS 20
#define BLINK_DURATION 500lu  // in milli seconds


struct rgb
{
    uint8_t r, g, b;
};


STATE_MACHINE sm;

unsigned long time_point;

TIMER work_timer;


// setting the colour of rgb led

void set_color(rgb color)
{
    analogWrite(R_PIN, color.r);

    analogWrite(G_PIN, color.g);

    analogWrite(B_PIN, color.b);
}


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

        work_duration = (work_timer.minute * 60 + work_timer.second) * 1000lu/*in milliseconds*/;

        time_point = millis();

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
    TIMER timer;

    void Enter()	// initialze this state
		{
        ping();

        time_point = millis();

        // adding up resting time with unused resting time

        timer.second += work_timer.second / 4;

        if(timer.second >= 60)
        {
            timer.second -= 60;

            ++timer.minute;    
        }

        timer.minute += work_timer.minute / 4;

        if(timer.minute > MAX_REST_MINS)
        {
            timer.minute = MAX_REST_MINS;
        }

        timer.no_point();

        OLED.print_num(timer);

        set_color(REST_COLOR);
    }

		void Loop()
		{
        // display remaining time

        if(millis() - time_point > 999)
        {
            time_point = millis();
            
            timer.decrement();

            OLED.print_num(timer);
        }

        // state change

        if(timer.minute == 0 && timer.second == 0)
        {
            // resting time is over

            sm.change_to(extra_rest_state);
        }
        else if(button.pressed())
        {
            // our push button is pressed
            // skip rest go to work

            sm.change_to(work_state);
        }
        else if(encoder_button.pressed())
        {
            // our push button is pressed
            // skip rest go to work

            sm.change_to(extra_rest_state);
        }
    }
} rest_state;


extern class setting_state setting;

//========================
// define extra_rest state
//========================

// for now this state uses interrupts for blinking light, revert back to time point method if needed in future

class EXTRA_REST_STATE : public BASE_STATE
{
    void Enter()	// initialze this state
		{
        ping();

        encoder.setPosition(0);

        work_timer.no_point();

        OLED.print_num(work_timer);

        MsTimer2::start();  // start the independent software interrupt system
    }

		void Loop()
		{
        // indication direction

        if((int)encoder.getDirection())
        {
            // limit the position between MAX and MIN
            
            if(encoder.getPosition() >= 1)
            {
                work_timer.point_second();

                encoder.setPosition(1);
            }
            else if(encoder.getPosition() <= -1)
            {
                work_timer.point_minute();

                encoder.setPosition(-1);
            }
            else
            {
                work_timer.no_point();
            }

            OLED.print_num(work_timer);
        }

        // selection

        if(encoder_button.pressed())
        {
            switch((int)encoder.getPosition())
            {
                case -1: work_timer.select_minute(); sm.change_to(setting, &work_timer, &(work_timer.minute), 60, 0); break;

                case 1: work_timer.select_second(); sm.change_to(setting, &work_timer, &(work_timer.second), 59, 0); break;

                case 0: 
                        MsTimer2::stop(); // stop the independent software interrupt system

                        sm.change_to(work_state);
                        
                        break;
            }

            return;
        }

        // change TO WORK state

        if(button.pressed())
        {
            // our push button is pressed
            // user wants to start working

            MsTimer2::stop(); // stop the independent software interrupt system

            sm.change_to(work_state);
        }
    }

    void Exit()
    {
        // MsTimer2::stop(); // stop the independent software interrupt system
    }

    // the function used to blink the led using software interrupt

    static void blink_led()
    {
        static volatile bool blink = true;

        blink = !blink;

        // turn the light on or off depending on blink_state

        (blink) ? set_color(EXTRA_REST_COLOR) : set_color({0, 0, 0});
    }

    public:

    EXTRA_REST_STATE()
    {
        // !!!! when I call it from Enter() the work mode light turned green for no reason
        
        MsTimer2::set(BLINK_DURATION, blink_led); // setup the independent software interrupt system

        work_timer.minute = WORK_MINS;

        work_timer.second = WORK_SECS;
    }
} extra_rest_state;



class setting_state : public BASE_STATE
{
		void Enter()	// initialze this state
		{
        ping();

        OLED.print_num(*timer);

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

            OLED.print_num(*timer);
        }

        // selection

        if(encoder_button.pressed())
        {
            sm.change_to(extra_rest_state);
        }
    }

    TIMER *timer;

    uint8_t *data;  // points to a member data of timer (minute or second)

    uint8_t max_rotation, min_rotation;

    public:

		void init(TIMER *_timer, uint8_t *_data, uint8_t _max_rotation, uint8_t _min_rotation)
		{
        timer = _timer;

        data = _data;

        max_rotation = _max_rotation;
        
        min_rotation = _min_rotation;
    }
} setting;

void setup()
{
    // put your setup code here, to run once:

    pinMode(R_PIN, OUTPUT);

    pinMode(G_PIN, OUTPUT);

    pinMode(B_PIN, OUTPUT);

    OLED.setup();

    buzzer_setup();

    button_setup();

    sm.change_to(extra_rest_state);
}

void loop()
{
    button.update();

    encoder_button.update();

    encoder.tick();

    sm.Loop();
}
