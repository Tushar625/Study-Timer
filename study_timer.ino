#define R_PIN 3
#define G_PIN 5
#define B_PIN 6

#define BUZZ_PIN 19 // buzzer output

#define IN_PIN 18 // digital input

#define WORK_COLOR {255 / 5, 100 / 5, 0} /*dim orange*/
#define REST_COLOR {0, 255, 0} /*green*/
#define EXTRA_REST_COLOR {255, 0, 0} /*red*/

#define WORK_DURATION /*10*1000*/ 20*60*1000lu
#define REST_DURATION /*10*1000*/ 5*60*1000lu
#define BLINK_DURATION 500lu

enum STATES{WORK, REST, EXTRA_REST, STATE_COUNT};

struct rgb
{
    uint8_t r, g, b;
};

STATES current_state;

unsigned long t0;

bool blink;

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

void setup()
{
    // put your setup code here, to run once:

    current_state = EXTRA_REST;

    t0 = millis();

    blink = true;

    pinMode(R_PIN, OUTPUT);

    pinMode(G_PIN, OUTPUT);

    pinMode(B_PIN, OUTPUT);

    pinMode(BUZZ_PIN, OUTPUT);

    pinMode(IN_PIN, INPUT_PULLUP);

    ping();
}

void loop()
{
    // put your main code here, to run repeatedly:
  
    switch(current_state)
    {
        case WORK:

          // working state

          set_color(WORK_COLOR);

          if(millis() - t0 > WORK_DURATION)
          {
              // time for rest

              ping();
              
              current_state = REST;

              t0 = millis();  // initialize t0 for next state
          }

          break;
        
        case REST:

          // resting state

          set_color(REST_COLOR);

          if(millis() - t0 > REST_DURATION)
          {
              // resting time is over

              ping();

              current_state = EXTRA_REST;

              t0 = millis();  // initialize t0 for next state
          }
          else if(digitalRead(IN_PIN) == LOW)
          {
              // our push button is pressed
              // skip rest go to work

              ping();

              current_state = WORK;

              t0 = millis();  // initialize t0 for next state
          }

          break;
        
        case EXTRA_REST:

          // extra resting state

          // we want our light to blink to warn user

          if(millis() - t0 > BLINK_DURATION)
          {
              blink = !blink;

              t0 = millis();
          }

          // turn the light on or off depending on blink_state

          if(blink)
          {
              set_color(EXTRA_REST_COLOR);

              //digitalWrite(BUZZ_PIN, HIGH);
          }
          else
          {
              set_color({0, 0, 0});

              //digitalWrite(BUZZ_PIN, LOW);
          }

          if(digitalRead(IN_PIN) == LOW)
          {
              // our push button is pressed
              // user wants to start working

              ping();

              current_state = WORK;

              t0 = millis();
          }

          break;
    }
}
