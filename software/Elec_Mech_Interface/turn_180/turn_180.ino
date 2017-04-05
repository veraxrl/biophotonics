/*
 *   turn_180.ino
 *   Master MCU code for controlling the following components
 *   + stepper motor
 *
 *   adapted from amis-30543 examples
 *   http://pololu.github.io/amis-30543-arduino/
 *   Copyright (c) 2016-2017 Daniel Hull

*/

#include <SPI.h>
#include <AMIS30543.h>
AMIS30543 stepper;

// set higher for slower, set lower for faster (msec)
#define SPEEDDELAY 1000

// current in milliamps, DO NOT GO ABOVE 1000
#define CURRENT_MAX 200

// 4, 8, 16, 32, lower value will make smoother
#define DEGREES 180
#define MICROSTEP_COUNT 1

const uint8_t amisDirPin = 2;
const uint8_t amisStepPin = 3;
const uint8_t amisSlaveSelect = 4;

int rotations = 0;
int step_count = (DEGREES/1.8)*MICROSTEP_COUNT;

void setup()
{
  SPI.begin();
  stepper.init(amisSlaveSelect);
  digitalWrite(amisStepPin, LOW);
  pinMode(amisStepPin, OUTPUT);
  digitalWrite(amisDirPin, LOW);
  pinMode(amisDirPin, OUTPUT);
  delay(1);

  // resets driver
  stepper.resetSettings();

  // Sets current limit
  stepper.setCurrentMilliamps(CURRENT_MAX);

  // Set the number of microsteps that correspond to one full step.
  stepper.setStepMode(MICROSTEP_COUNT);

  // Enable the motor outputs.
  stepper.enableDriver();
}

void loop(){
  if (rotations >= 1){
    setDirection(0);
    step(step_count);
  }
  delay(300);

}

// Sends a pulse on the NXT/STEP pin to tell the driver to take
// one step, and also delays to control the speed of the motor.
void step(int num_steps)
{
  for (unsigned int x = 0; x < num_steps; x++){
  // The NXT/STEP minimum high pulse width is 2 microseconds.
  digitalWrite(amisStepPin, HIGH);
  delayMicroseconds(3);
  digitalWrite(amisStepPin, LOW);
  delayMicroseconds(3);
  delayMicroseconds(SPEEDDELAY);
  }
  rotations += 1;
}

// Writes a high or low value to the direction pin to specify
// what direction to turn the motor.
void setDirection(bool dir)
{
  // The NXT/STEP pin must not change for at least 0.5
  // microseconds before and after changing the DIR pin.
  delayMicroseconds(1);
  digitalWrite(amisDirPin, dir);
  delayMicroseconds(1);
}
