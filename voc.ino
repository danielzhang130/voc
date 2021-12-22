#include <LowPower.h>
#include <Wire.h>
#include "SensirionI2CScd4x.h"
#include <Arduino.h>

SensirionI2CScd4x scd4x;

#define INPUT_PIN_INT   2

volatile byte count = 255;
byte digits[] = {12, 13};
byte segments[] = {11, 10, 5, 8, 3, 16, 15};
byte num[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

void showNumber(byte v, bool showLeading = false);

bool should_display = true;
long prev_isr;

void isr(void)
{
  if (millis() - prev_isr < 300) return;
  
  should_display = true;
  prev_isr = millis();
}

void setup(void)
{
  Serial.begin(115200);

  Wire.begin();

  scd4x.begin(Wire);
  scd4x.stopPeriodicMeasurement();

  pinMode(INPUT_PIN_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT), isr, RISING);
  
  for (byte i = 0; i < 7; ++i)
  {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(6, OUTPUT);

  // set ADC reference to 1.1v
  analogReference(INTERNAL);
}

void print7(byte c, bool showLeading = false)
{
  byte a, b;

  if (c > 9)
  {
    a = c / 10;
    b = c % 10;
  
    for (byte i = 0; i < 7; ++i)
    {
      digitalWrite(segments[i], num[a][i]);
    }
    digitalWrite(digits[1], 0);
    delay(5);
    digitalWrite(digits[1], 1);
  
    for (byte i = 0; i < 7; ++i)
    {
      digitalWrite(segments[i], num[b][i]);
    }
    digitalWrite(digits[0], 0);
    delay(5);
    digitalWrite(digits[0], 1);
  }
  else
  {
    if (showLeading)
    {
      for (byte i = 0; i < 7; ++i)
      {
        digitalWrite(segments[i], num[0][i]);
      }
      digitalWrite(digits[1], 0);
      delay(5);
      digitalWrite(digits[1], 1);
    }
    else
    {
      for (byte i = 0; i < 7; ++i)
      {
        digitalWrite(segments[i], 0);
      }
      digitalWrite(digits[1], 0);
      delay(5);
      digitalWrite(digits[1], 1);
    }

    for (byte i = 0; i < 7; ++i)
    {
      digitalWrite(segments[i], num[c][i]);
    }
    digitalWrite(digits[0], 0);
    delay(5);
    digitalWrite(digits[0], 1);
  }
}

void clear(void)
{
  for (byte i = 0; i < 7; ++i)
  {
    digitalWrite(segments[i], 0);
  }
  digitalWrite(13, 0);
}

void showNumber(byte v, bool showLeading)
{
  unsigned long time = millis();
  
  while (1)
  {
      print7(v, showLeading);
      if (millis() - time > 1000)
      {
        break;
      }
      delay(10);
  }
}

uint16_t co2;
float temperature;
float humidity;
uint16_t ready;

void display(uint16_t raw)
{
  if (should_display) {
    should_display = false;
   
    if (raw > 99)
    {
     showNumber(raw/100);
     showNumber(raw%100, true); 
    }
    else
    {
      showNumber(raw);
    }

    display((uint16_t) temperature);
  
    clear();
  }
}

void sleep(void)
{
  for(int i = 0; i < 22; ++i)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    display(co2);
  }
}

void loop(void)
{
  scd4x.wakeUp();
  
  scd4x.measureSingleShot();

  scd4x.getDataReadyStatus(ready);
  while((ready & 0x07FF) == 0) {
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    display(co2);
    scd4x.getDataReadyStatus(ready);
  }

  scd4x.readMeasurement(co2, temperature, humidity);

  scd4x.powerDown();

  display(co2);
  
  sleep();
}
