#include <Arduino.h>
#include <IRremote.h> //* Disable (set to 0) all the protocols you do not need/want!
const int pinIR = 4;
IRrecv irrecv(pinIR);
decode_results results;

const int NecCodeRed = 114;
const int NecCodeBlue = 97;

const int itvMain = 100;
const int pinPir = 12;
const int pinLed = LED_BUILTIN;
const int pinBuzz = 10;

bool deviceOn = false; // da li je aparat ukljucen ili ne

void translateIR()
{
  Serial.println(irrecv.decodedIRData.command);
  Serial.println(irrecv.decodedIRData.address);
  if (irrecv.decodedIRData.command == NecCodeRed)
    deviceOn = !deviceOn;
  digitalWrite(pinLed, deviceOn);
}

void setup()
{
  Serial.begin(9600);
  Serial.println();

  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, false);

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

  digitalWrite(pinBuzz, digitalRead(pinPir));
}
