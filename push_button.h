// this file provides objects for easy setup and handling of push button

#ifndef PUSH_BUTTON_HEADER
#define PUSH_BUTTON_HEADER

#include<Bounce2.h>

// #define BUTTON_PIN 4
#define ENCODER_BUTTON_PIN 2

// object for our push button

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

// indicates if the button was held down or not

bool held_down(const Bounce2::Button& button)
{
    while((button.update(), button.isPressed()))
    {
        if(button.currentDuration() > 2000)
        {
            // the button was pressed down for 2 second

            return true;
        }
    }

    return false;
}


#endif // PUSH_BUTTON_HEADER