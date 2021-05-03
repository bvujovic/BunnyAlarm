#include <Arduino.h>
#include <IRremote.h> //* Disable (set to 0) all the protocols you do not need/want!
const byte pinIR = 10;
IRrecv irrecv(pinIR);

#include <ClickButton.h>
const byte pinBtn = A0;
ClickButton btn(pinBtn, LOW, CLICKBTN_PULLUP); // taster za promenu stanja/ukljucenosti aparata (deviceOn)

const byte pinPir = 11;
const byte pinBuzz = 12;
#include "Blinky.h"
Blinky led = Blinky::create(); // LED_BUILTIN statusna dioda: upaljena -> buzzer se pali na PIR HIGH
Blinky buzz(pinBuzz, true);    // buzzer

const int NecCodeRed = 114;      // Crveni taster na daljinskom
const int extraAlarmTime = 1000; // Koliko ce jos msec alarm trajati posle prestanka PIR HIGH signala

ulong msLastSound = 0;        // Vreme poslednjeg test zvuka
ulong msPirEnd = 0;           // Vreme poslednjeg registrovanog HIGH signala na PIR-u
bool deviceOn = false;        // Da li aparat reaguje na PIR HIGH paljenjem alarma
bool deviceOnPirHigh = false; // true kada je PIR HIGH pri promeni deviceOn na true
ulong deviceOffUntil = 0;     // Aparat nece reagovati na PIR HIGH do ovog vremena

// Da li aparat reaguje na PIR HIGH imajuci u vidu promenljive deviceOn i deviceOffUntil
bool isDeviceOn() { return deviceOn && deviceOffUntil == 0; }

// Ugradjeni LED svetli ako aparat reaguje na PIR HIGH, u suprotnom ne svetli
void setLedOn() { led.ledOn(isDeviceOn()); }

// Suspendovanje aparata na dati broj minuta
void deviceOffTemp(ulong minutes, ulong msLedOn)
{
  led.ledOn("01.11.10", msLedOn);
  deviceOffUntil = millis() + minutes * 60 * 1000;
  deviceOn = false;
  setLedOn();
}

// Promena deviceOn vrednosti tj. stanja aparata
void toggleDeviceOn()
{
  deviceOn = !deviceOn;
  deviceOffUntil = 0;
  deviceOnPirHigh = deviceOn && digitalRead(pinPir);
  setLedOn();
}

void setup()
{
  pinMode(buzz.getPin(), OUTPUT);
  pinMode(led.getPin(), OUTPUT);
  setLedOn();
  btn.multiclickTime = 500;

  irrecv.enableIRIn();
}

void loop()
{
  // tumacenje IR signala, ako ga ima
  if (irrecv.decode())
  {
    if (irrecv.decodedIRData.command == NecCodeRed)
      toggleDeviceOn();
    delay(100);
    irrecv.resume();
  }
  // tumacenje klikova na taster
  btn.Update();
  if (btn.clicks == 1)
    toggleDeviceOn();
  if (btn.clicks == 2)      // dvoklik: nema buzzera na 10min
    deviceOffTemp(10, 250); //
  if (btn.clicks == 3)      // troklik: nema buzzera na 1h
    deviceOffTemp(60, 750); //
  if (btn.clicks == -1      // dugi klik: test zvuka
      && millis() > msLastSound + 1000)
  { // bez dodatnog uslova sa millis() zvuk se iz nekog razloga pusta 2x
    buzz.ledOn("11.10");
    msLastSound = millis();
  }

  // on/off buzzer-a u zavisnosti od PIR-a, deviceOn, deviceOffUntil promenljivih
  ulong ms = millis();
  if (digitalRead(pinPir))
    msPirEnd = ms;
  else if (ms > msPirEnd + extraAlarmTime)
    deviceOnPirHigh = false;

  if (ms < msPirEnd + extraAlarmTime && isDeviceOn() && !deviceOnPirHigh)
    // uslov !deviceOnPirHigh sprecava alarm na neki zaostali pokret (od pre paljenja aparata)
    buzz.blinkAsync(ms);
  else
    buzz.off();
  if (deviceOffUntil != 0 && ms > deviceOffUntil)
    toggleDeviceOn();

  delay(10);
}
