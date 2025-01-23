
/*
    this file provides an object for easy handling of oled display
*/

#ifndef OLED_DISPLAY_HEADER
#define OLED_DISPLAY_HEADER

#include <Wire.h> // comes with arduino ide used for I2C communication
#include <Adafruit_GFX.h> // version 1.11.11, to draw (lines, texts, etc) on the OLED display
#include <Adafruit_SH110X.h>  // version 2.1.22, 1.3 inch OLED display driver
#include"time_point.h"

#define I2C_ADDRESS_OLED 0X3C // this address seems to work for me

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64

/*
    display and ui class, this class is very specific for this project,
    not a general purpose one, just to display the time on OLED display
    and clean the display when needed
*/

class OLED_DISPLAY
{
    Adafruit_SH1106G display; // main display object

    uint8_t x, y; // position of the text representing time

    char str[6];  // to store the text representing time

    public:
    
    OLED_DISPLAY() : display(SCREEN_WIDTH, SCREEN_HEIGHT)
    {}

    /*
        call the following function from setup(), it sets up the display and
        calculates position of the text on the display
    */

    void setup()
    {
        // initialize and set up our display

        delay(250); // wait for our display to power up

        display.begin(I2C_ADDRESS_OLED);

        display.setContrast(10);  // dim the display

        // It's a monochrome display, so it's very obvious that I want white text

        display.setTextColor(SH110X_WHITE);
        
        /*
            I want to display the time in the middle of the screen, some calculation
            is needed for that, and the result will be stored in x and y for later use
        */ 

        display.setTextSize(3);

        int16_t _x, _y;

        uint16_t w, h;

        display.getTextBounds("00:00", 0, 0, &_x, &_y, &w, &h); // getting height and width of the text

        // finally the position calculation

        x = SCREEN_WIDTH / 2 - w / 2;
        
        y = SCREEN_HEIGHT / 2 - h / 2;
    }

    // print given time in the middle of the screen

    void print(const TIME_POINT& time)
    {
        display.clearDisplay();

        // display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);

        // place the text in the middle

        display.setCursor(x, y);

        time.time_string(str);  // getting the time string

        display.print(str); // prints the string in the display buffer

        display.display();  // shine the contents of display buffer on the screen
    }

    // clean the screen

    void clear()
    {
        display.clearDisplay();

        display.display();
    }

    // display a progress bar near the bottom of the screen

    void progress_bar(float fraction)
    {
        display.clearDisplay();

        display.drawRect(0, SCREEN_HEIGHT - 7, SCREEN_WIDTH, 5, SH110X_WHITE);

        display.fillRect(SCREEN_WIDTH - SCREEN_WIDTH * fraction, SCREEN_HEIGHT - 7, SCREEN_WIDTH * fraction, 5, SH110X_WHITE);

        display.display();
    }
} OLED; // display and ui object

#endif // OLED_DISPLAY_HEADER