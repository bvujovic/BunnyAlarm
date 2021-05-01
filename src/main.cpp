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

const int NecCodeRed = 114; // crveni taster na daljinskom

ulong msPirEnd = 0;       // Vreme poslednjeg registrovanog HIGH signala na PIR-u
bool deviceOn = false;    // Da li aparat reaguje na PIR HIGH paljenjem
ulong deviceOffUntil = 0; // Aparat nece reagovati na PIR HIGH do ovog vremena

// Da li aparat reaguje na PIR HIGH imajuci u vidu promenljive deviceOn i deviceOffUntil
bool isDeviceOn() { return deviceOn && deviceOffUntil == 0; }

// Ugradjeni LED svetli ako aparat reaguje na PIR HIGH, u suprotnom ne svetli
void setLedOn() { led.ledOn(isDeviceOn()); }

// Suspendovanje aparata na dati broj minuta
void deviceOffTemp(ulong minutes)
{
  deviceOffUntil = millis() + minutes * 60 * 1000;
  deviceOn = false;
  setLedOn();
}

// Promena deviceOn vrednosti tj. stanja aparata
void toggleDeviceOn()
{
  deviceOn = !deviceOn;
  deviceOffUntil = 0;
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
  Serial.begin(9600);
  Serial.println();

  pinMode(buzz.getPin(), OUTPUT);
  pinMode(led.getPin(), OUTPUT);
  setLedOn();

  irrecv.enableIRIn();
}

void loop()
{
  // tumacenje IR signala, ako ga ima
  if (irrecv.decode())
  {
    translateIR();
    delay(100);
    irrecv.resume();
  }
  // tumacenje klikova na taster
  btn.Update();
  if (btn.clicks == 1)
    toggleDeviceOn();
  if (btn.clicks == 2) // dvoklik: nema buzzera na 10min
  {
    led.ledOn("01.11.10", 250);
    deviceOffTemp(10);
  }
  if (btn.clicks == 3) // troklik: nema buzzera na 1h
  {
    led.ledOn("01.11.10", 750);
    deviceOffTemp(60);
  }
  if (btn.clicks == -1) // dugi klik: test zvuka
  {
    buzz.ledOn("11.10"); //todo ovo se ponavlja 2x bezveze
  }

  // on/off buzzer-a u zavisnosti od PIR-a, deviceOn, deviceOffUntil promenljivih
  ulong ms = millis();
  if (digitalRead(pinPir))
    msPirEnd = ms;
  if (ms < msPirEnd + 1000 && isDeviceOn())
    buzz.blinkAsync(ms);
  else
    buzz.off();
  if (deviceOffUntil != 0 && ms > deviceOffUntil)
    toggleDeviceOn();

  delay(10);
}
