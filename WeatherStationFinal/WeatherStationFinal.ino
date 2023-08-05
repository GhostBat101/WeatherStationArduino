#include "Adafruit_Si7021.h"
#include "Adafruit_CCS811.h"
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

int ii = 0;

Adafruit_CCS811 ccs;

Adafruit_Si7021 sensor = Adafruit_Si7021();

int measurePin = A0; //Connect dust sensor to Arduino A0 pin
int ledPower = 2;   //Connect 3 led driver pins of dust sensor to Arduino D2

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C display1(U8G2_R0, 5,4,U8X8_PIN_NONE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.begin(115200);
  pinMode(ledPower,OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  
  display1.setI2CAddress(0x78);
  display1.begin();
  display1.setFont(u8g2_font_10x20_tf); // choose a suitable font
  display1.clearDisplay();
  
  delay(100);  

}

void software_Reset()  // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile("  jmp 0");
}

void loop() {


  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);

  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 170 * calcVoltage - 0.1;

  Serial.print("\nDust particles: ");
  Serial.print(dustDensity); // unit: ug/m3
  Serial.print(" ug/m3");
  char dustValue[8]; // Buffer big enough for 7-character float
  dtostrf(dustDensity, 6, 2, dustValue); // Leave room for too large numbers! 
  //delay(10);
  // put your main code here, to run repeatedly:
  float t = sensor.readTemperature();
  Serial.print("\nTemperature: ");
  Serial.print(t, 2);
  Serial.print(" C\n");
  char temp[8]; // Buffer big enough for 7-character float
  dtostrf(t, 6, 2, temp); // Leave room for too large numbers!  
  //delay(10);
  Serial.print("Humidity: ");
  float h = sensor.readHumidity();
  Serial.print(h, 2);
  Serial.print(" %\n");
  char hum[8]; // Buffer big enough for 7-character float
  dtostrf(h, 6, 2, hum); // Leave room for too large numbers!
  //delay(10);
  float car = ccs.geteCO2();
  char co[8]; // Buffer big enough for 7-character float
  dtostrf(car, 6, 2, co); // Leave room for too large numbers!
  Serial.print("CO2: ");
  Serial.print(car);
  Serial.print(" ppm\n");
  //delay(10);
  Serial.print("TVOC: ");
  float voc = ccs.getTVOC();
  char tvoc[8]; // Buffer big enough for 7-character float
  dtostrf(voc, 6, 2, tvoc); // Leave room for too large numbers!
  Serial.print(voc);
  Serial.print(" ppm\n");
  if (t > 24.70){
    digitalWrite(8, HIGH);
  }
  else {
    digitalWrite(8, LOW);
  }
  if (h > 70.00){
    digitalWrite(9, HIGH);
  }
  else {
    digitalWrite(9, LOW);
  }
  if (dustDensity > 250.00){
    digitalWrite(10, HIGH);
  }
  else {
    digitalWrite(10, LOW);
  }

  display1.clearDisplay();
  display1.clearBuffer();          // clear the internal memory
  display1.drawStr(0,30,"DUST: "); 
  display1.drawStr(60,30,dustValue);
  display1.drawStr(0,50,"TEMP: "); 
  display1.drawStr(50,50,temp);  // write something to the internal memory
  //display1.drawStr(20,60,hum);
  display1.sendBuffer();
  delay(5000);

  display1.clearDisplay();
  display1.clearBuffer();          // clear the internal memory
  display1.drawStr(0,30,"HUM : "); 
  display1.drawStr(50,30,hum);
  display1.drawStr(0,50,"TVOC: "); 
  display1.drawStr(50,50,tvoc);  // write something to the internal memory
  //display1.drawStr(20,60,hum);
  display1.sendBuffer();
  delay(5000);

  display1.clearDisplay();
  display1.clearBuffer();          // clear the internal memory
  display1.drawStr(0,30,"CO : "); 
  display1.drawStr(50,30,co);
  display1.sendBuffer();
  delay(2000);

  
  
  ii = ii+1;
  if (ii >1000){
    software_Reset();
  }
}
