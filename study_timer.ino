// https://github.com/PaulStoffregen/MsTimer2/blob/f90127ce8289b73c2e74b8be222ab7c755621717/MsTimer2.h

#include<MsTimer2.h>  // implements a software interrupt that runs a function at specific interval of time
#include"state_machine.h"

#define R_PIN 3
#define G_PIN 5
#define B_PIN 6

#define BUZZ_PIN 19 // buzzer output

#define IN_PIN 18 // digital input

#define WORK_COLOR {255 / 5, 100 / 5, 0} /*dim orange*/
#define REST_COLOR {0, 255, 0} /*green*/
#define EXTRA_REST_COLOR {255, 0, 0} /*red*/

// durations in milli seconds

#define WORK_DURATION /*10*1000*/ 20*60*1000lu
#define REST_DURATION /*10*1000*/ 5*60*1000lu
#define BLINK_DURATION 500lu


struct rgb
{
    uint8_t r, g, b;
};


STATE_MACHINE sm;

unsigned long time_point;


// setting the colour of rgb led

void set_color(rgb color)
{
    analogWrite(R_PIN, color.r);

    analogWrite(G_PIN, color.g);

    analogWrite(B_PIN, color.b);
}

// just generate a little sound with buzzer

void ping()
{
    digitalWrite(BUZZ_PIN, HIGH);

    delay(100);

    digitalWrite(BUZZ_PIN, LOW);
}


extern class REST_STATE rest_state;

//==================
// define work state
//==================

class WORK_STATE : public BASE_STATE
{
    void Enter()	// initialze this state
		{
        ping();

        time_point = millis();

        set_color(WORK_COLOR);
    }

		void Loop()
		{
        // state change

        if(millis() - time_point > WORK_DURATION)
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
    void Enter()	// initialze this state
		{
        ping();

        time_point = millis();

        set_color(REST_COLOR);
    }

		void Loop()
		{
        // state change

        if(millis() - time_point > REST_DURATION)
        {
            // resting time is over

            sm.change_to(extra_rest_state);
        }
        else if(digitalRead(IN_PIN) == LOW)
        {
            // our push button is pressed
            // skip rest go to work

            sm.change_to(work_state);
        }
    }
} rest_state;


//========================
// define extra_rest state
//========================

// for now this state uses interrupts for blinking light, revert back to time point method if needed in future

class EXTRA_REST_STATE : public BASE_STATE
{
    void Enter()	// initialze this state
		{
        ping();

        MsTimer2::start();  // start the independent software interrupt system
    }

		void Loop()
		{
        // state change

        if(digitalRead(IN_PIN) == LOW)
        {
            // our push button is pressed
            // user wants to start working

            sm.change_to(work_state);
        }
    }

    void Exit()
    {
        MsTimer2::stop(); // stop the independent software interrupt system
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
    }
} extra_rest_state;


void setup()
{
    // put your setup code here, to run once:

    pinMode(R_PIN, OUTPUT);

    pinMode(G_PIN, OUTPUT);

    pinMode(B_PIN, OUTPUT);

    pinMode(BUZZ_PIN, OUTPUT);

    pinMode(IN_PIN, INPUT_PULLUP);

    sm.change_to(extra_rest_state);
}

void loop()
{
    sm.Loop();
}
