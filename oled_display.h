// this file provides an object for easy handling of oled display

#ifndef OLED_DISPLAY_HEADER
#define OLED_DISPLAY_HEADER

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include"timer.h"

#define I2C_ADDRESS_OLED 0X3C

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64

// display and ui class

class OLED_DISPLAY
{
    Adafruit_SH1106G display;

    uint8_t x, y;

    char str[6];

    public:
    
    OLED_DISPLAY() : display(SCREEN_WIDTH, SCREEN_HEIGHT)
    {}

    // call the following function from setup()

    void setup()
    {
        // initialize and set up our display

        delay(250); // wait for our display to power up

        display.begin(I2C_ADDRESS_OLED);

        display.setContrast(10);  // dim the display

        display.clearDisplay();

        // calculating display position

        display.setTextSize(3);

        display.setTextColor(SH110X_WHITE);

        int16_t _x, _y;

        uint16_t w, h;

        display.getTextBounds("00:00", 0, 0, &_x, &_y, &w, &h);

        x = SCREEN_WIDTH / 2 - w / 2;
        
        y = SCREEN_HEIGHT / 2 - h / 2;
    }

    // print given integer in the middle of the screen

    void print_num(const TIMER& timer)
    {
        display.clearDisplay();

        // display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);

        // place the text in the middle

        display.setCursor(x, y);

        timer.timer_string(str);

        display.print(str);

        display.display();
    }

    void clear()
    {
        display.clearDisplay();

        display.display();
    }
} OLED; // display and ui object

#endif // OLED_DISPLAY_HEADER