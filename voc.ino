#include <LowPower.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_SGP40.h"

Adafruit_SGP40 sgp;

// target 2mhz clock
#define CLK_PRESCALE_FACTOR 4
#define CLK_PRESCALE_HEX 0x02

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

void showNumber(byte v, boolean showLeading = false);

void setup(void)
{
  Serial.begin(9600);
  
  while (! sgp.begin()){
    Serial.println("Sensor not found :(");
    delay(10);
  }
  
  for (byte i = 0; i < 7; ++i)
  {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(6, OUTPUT);

  // set ADC reference to 1.1v
  analogReference(INTERNAL);

  // slow down clock
  CLKPR = 0x80;
  CLKPR = CLK_PRESCALE_HEX;
}

void print7(byte c, boolean showLeading = false)
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
    delay(5 / CLK_PRESCALE_FACTOR);
    digitalWrite(digits[1], 1);
  
    for (byte i = 0; i < 7; ++i)
    {
      digitalWrite(segments[i], num[b][i]);
    }
    digitalWrite(digits[0], 0);
    delay(5 / CLK_PRESCALE_FACTOR);
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
      delay(5 / CLK_PRESCALE_FACTOR);
      digitalWrite(digits[1], 1);
    }
    else
    {
      for (byte i = 0; i < 7; ++i)
      {
        digitalWrite(segments[i], 0);
      }
      digitalWrite(digits[1], 0);
      delay(5 / CLK_PRESCALE_FACTOR);
      digitalWrite(digits[1], 1);
    }

    for (byte i = 0; i < 7; ++i)
    {
      digitalWrite(segments[i], num[c][i]);
    }
    digitalWrite(digits[0], 0);
    delay(5 / CLK_PRESCALE_FACTOR);
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

void showNumber(byte v, boolean showLeading)
{
  unsigned long time = millis();
  
  Serial.println(v);
  while (1)
  {
      print7(v, showLeading);
      if (millis() - time > 1000 / CLK_PRESCALE_FACTOR)
      {
        break;
      }
      delay(10 / CLK_PRESCALE_FACTOR);
  }
}
byte v;
void loop(void)
{
  uint16_t raw;
  
  raw = sgp.measureVocIndex();

  Serial.print("Measurement: ");
  Serial.println(raw);

  if (raw > 99)
  {
   showNumber(raw/100);
   showNumber(raw%100, true); 
  }
  else
  {
    showNumber(raw);
  }

  clear();
  
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
}
