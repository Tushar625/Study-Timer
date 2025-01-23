
// this file provides objects for easy setup and handling of the push buttons

#ifndef PUSH_BUTTON_HEADER
#define PUSH_BUTTON_HEADER

#include<Bounce2.h> // version 2.71

/*
    note:

    I initially had 2 push buttons, later I used only one which comes with
    the rotary encoder and removed the other one.
    
    But instead of removing the code for the other button I comment it out
    so that I can get it easily in future if needed.
*/

// #define BUTTON_PIN 4
#define ENCODER_BUTTON_PIN 2

// objects for our push button

// Bounce2::Button button;

Bounce2::Button encoder_button;

// call this from the setup()

void button_setup()
{
    /*button.attach(BUTTON_PIN, INPUT_PULLUP);

    button.interval(5);

    button.setPressedState(LOW);*/

    encoder_button.attach(ENCODER_BUTTON_PIN, INPUT_PULLUP);

    encoder_button.interval(5);

    encoder_button.setPressedState(LOW);
}

/*
    I have implemented some fuctionalities that require the button to be pressed down
    for longer time, this function is meant to be called after detecting a button press
    to check if the button was pressed down for 2 seconds
*/

bool held_down(const Bounce2::Button& button)
{
    while((button.update(), button.isPressed()))
    {
        if(button.currentDuration() > 2000)
        {
            // the button was pressed down for 2 seconds

            return true;
        }
    }

    return false;
}

#endif // PUSH_BUTTON_HEADER