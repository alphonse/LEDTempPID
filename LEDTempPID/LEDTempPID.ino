/*
Read and Control Temperature of BBCES LEDs
------------------------------------------
Uses PWM to control the current through a thermoelectric cooler and measures
the temperature with K-type thermocouple and MAX31850 TC amplifier.  Uses a PID 
loop as a feedback to control the temperature for up to four TC amplifiers (on 
a shared data line) and TECs.

Uses PID library v1.0.1 by Brett Beauregard:
http://playground.arduino.cc/Code/PIDLibrary

Uses the Adafruit's OneWire library:
https://github.com/adafruit/MAX31850_OneWire

and their DallasTemp library:
https://github.com/adafruit/MAX31850_DallasTemp

For BBCES LED control pin 6 is connected to the red LED, pin 9 is connected to 
green, pin 10 to blue, and pin 11 to the UV LED.

2015-02-14
AF

*/


#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// define variables for PID
double Setpoint, redInput, redOutput, greenInput, greenOutput, blueInput, blueOutput, uvInput, uvOutput;
float temps[3];
int count;

// setup PID loops:
PID redPID(&redInput, &redOutput, &Setpoint, 33, 10, 6, DIRECT);
PID greenPID(&greenInput, &greenOutput, &Setpoint, 33, 10, 6, DIRECT);
PID bluePID(&blueInput, &blueOutput, &Setpoint, 33, 10, 6, DIRECT);
PID uvPID(&uvInput, &uvOutput, &Setpoint, 33, 10, 6, DIRECT);

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // Start up the library
  sensors.begin();
    
  // setup for PID:
  Setpoint = 30;
  redPID.SetMode(AUTOMATIC);
  redPID.SetControllerDirection(REVERSE);
  greenPID.SetMode(AUTOMATIC);
  greenPID.SetControllerDirection(REVERSE);
  bluePID.SetMode(AUTOMATIC);
  bluePID.SetControllerDirection(REVERSE);
  uvPID.SetMode(AUTOMATIC);
  uvPID.SetControllerDirection(REVERSE);
}

void loop(void)
{ 
  count = sensors.getDeviceCount();
  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures();  
  
  for (int i = 0; i < count; i++) {
    temps[i] = sensors.getTempCByIndex(i);
    Serial.print(temps[i]); Serial.print(", ");
  }
  
  Serial.print(millis()); Serial.print(", ");
  
  // adjust output with PID:
  if (isnan(temps[0])) redInput = redInput;
  else redInput = temps[0];
  redPID.Compute();
  analogWrite(6, redOutput);
  
  if (isnan(temps[1])) greenInput = greenInput;
  else greenInput = temps[1];
  greenPID.Compute();
  analogWrite(9, greenOutput);
  
  if (isnan(temps[2])) blueInput = blueInput;
  else blueInput = temps[2];
  bluePID.Compute();
  analogWrite(10, blueOutput);
  
  if (isnan(temps[3])) uvInput = uvInput;
  else uvInput = temps[3];
  uvPID.Compute();
  analogWrite(11, uvOutput);
  
  Serial.print(redOutput, 0); Serial.print(", ");
  Serial.print(greenOutput, 0); Serial.print(", ");
  Serial.print(blueOutput, 0); Serial.print(", ");
  Serial.println(uvOutput, 0);  
}
