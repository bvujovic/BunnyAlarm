#include <Arduino.h>

const byte pinLed = LED_BUILTIN;

void setup()
{
    pinMode(pinLed, OUTPUT);
    digitalWrite(pinLed, false);
    delay(2000);
}

void loop()
{
    digitalWrite(pinLed, true);
    delay(200);
    digitalWrite(pinLed, false);
    delay(800);
}