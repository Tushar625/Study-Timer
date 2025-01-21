#ifndef RGB_LED_HEADER
#define RGB_LED_HEADER

#define R_PIN 3
#define G_PIN 5
#define B_PIN 6

struct rgb
{
    uint8_t r, g, b;
};

void rgb_led_setup()
{
    pinMode(R_PIN, OUTPUT);

    pinMode(G_PIN, OUTPUT);

    pinMode(B_PIN, OUTPUT);
}

// setting the colour of rgb led

void set_color(rgb color)
{
    analogWrite(R_PIN, color.r);

    analogWrite(G_PIN, color.g);

    analogWrite(B_PIN, color.b);
}

#endif