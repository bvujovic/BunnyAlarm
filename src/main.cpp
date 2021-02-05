#include <Arduino.h>
#include <IRremote.h> //* Disable (set to 0) all the protocols you do not need/want!
const int pinIR = 11;
IRrecv irrecv(pinIR);

const int NecCodeRed = 114; // crveni taster na daljinskom
const int NecCodeBlue = 97; // plavi taster na daljinskom

const int itvMain = 100;
const int pinPir = 12;
const int pinLed = LED_BUILTIN;
const int pinBuzz = 10;

bool deviceOn = false; // da li je aparat ukljucen ili ne

void setLedOn() { digitalWrite(pinLed, deviceOn); }

void translateIR()
{
  // Serial.println(irrecv.decodedIRData.command);
  // Serial.println(irrecv.decodedIRData.address);
  if (irrecv.decodedIRData.command == NecCodeRed)
    deviceOn = !deviceOn;
  setLedOn();
}

void setup()
{
  // Serial.begin(9600);
  // Serial.println();

  pinMode(pinLed, OUTPUT);
  setLedOn();

  irrecv.enableIRIn();
}

void loop()
{
  if (irrecv.decode())
  {
    translateIR();
    delay(itvMain);
    irrecv.resume();
  }
  digitalWrite(pinBuzz, deviceOn && digitalRead(pinPir));
}
