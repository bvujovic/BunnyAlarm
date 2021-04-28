#include <Arduino.h>
#include <IRremote.h> //* Disable (set to 0) all the protocols you do not need/want!
const byte pinIR = 10;
IRrecv irrecv(pinIR);

#include <ClickButton.h>
const byte pinBtn = A0;
ClickButton btn(pinBtn, LOW, CLICKBTN_PULLUP); // taster za promenu stanja/ukljucenosti aparata (deviceOn)

const int NecCodeRed = 114; // crveni taster na daljinskom
const int itvMain = 10;
const byte pinPir = 11;
const byte pinLed = LED_BUILTIN;
const byte pinBuzz = 12;

bool deviceOn = false; // da li je aparat ukljucen ili ne

// ugradjeni LED svetli ako je deviceOn, u suprotnom ne svetli
void setLedOn() { digitalWrite(pinLed, deviceOn); }

// promena deviceOn vrednosti tj. stanja aparata
void toggleDeviceOn()
{
  deviceOn = !deviceOn;
  setLedOn();
}

void translateIR()
{
  //T Serial.println(irrecv.decodedIRData.command);
  //T Serial.println(irrecv.decodedIRData.address);
  if (irrecv.decodedIRData.command == NecCodeRed)
    toggleDeviceOn();
}

void setup()
{
  //T Serial.begin(9600);
  //T Serial.println();

  pinMode(pinBuzz, OUTPUT);
  pinMode(pinLed, OUTPUT);
  setLedOn();

  irrecv.enableIRIn();
}

void loop()
{
  btn.Update();
  if (btn.clicks == 1)
    toggleDeviceOn();
  if (irrecv.decode())
  {
    translateIR();
    delay(itvMain);
    irrecv.resume();
  }
  digitalWrite(pinBuzz, deviceOn && digitalRead(pinPir));
  
  delay(20);
}
