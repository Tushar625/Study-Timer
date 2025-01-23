
/*
    Here I provide some functions to setup and handle the buzzer (Active buzzer)
*/

#ifndef BUZZER_HEADER
#define BUZZER_HEADER

#define BUZZ_PIN 17 // buzzer output

// setup the buzzer system

void buzzer_setup()
{
    pinMode(BUZZ_PIN, OUTPUT);
}

// just generate a little sound with buzzer

void ping()
{
    digitalWrite(BUZZ_PIN, HIGH);

    delay(100);

    digitalWrite(BUZZ_PIN, LOW);
}

#endif