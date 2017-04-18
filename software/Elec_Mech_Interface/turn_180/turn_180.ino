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

// current in milliamps, DO NOT GO ABOVE 1000
#define CURRENT_MAX 800

// 4, 8, 16, 32, lower value will make smoother
#define TOTAL_DEGREES 180
#define MICROSTEP_COUNT 8
#define IMAGE_COUNT 15
#define MOVEMENT_TIME 1000

int step_degrees = TOTAL_DEGREES/IMAGE_COUNT; // degrees per image
int step_count = (step_degrees/1.8)*MICROSTEP_COUNT; // steps per image
int step_delay = MOVEMENT_TIME/step_count;

// set higher for higher resolution (based on vera's table set lower for faster (msec)
#define TRANSFER_DELAY 5000

const uint8_t amisDirPin = D0;
const uint8_t amisStepPin = D1;
const uint8_t amisSlaveSelect = D8;

int num_images = 0;

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
    //capture image
    if (num_images < IMAGE_COUNT){
     step(step_count, 0);
     delay(TRANSFER_DELAY);
    }
    ++num_images;
    
}

// Sends a pulse on the NXT/STEP pin to tell the driver to take
// one step, and also delays to control the speed of the motor.
void step(int num_steps, bool direction){
  /*
   * int num_steps = number of steps to send to driver
   * int direction (NEEDS TO BE A ZERO or a ONE, TRUE OR FALSE) 
   */
  
  setDirection(direction);
  for (unsigned int x = 0; x < num_steps; x++){
  digitalWrite(amisStepPin, HIGH);
  delayMicroseconds(3); // The NXT/STEP minimum high pulse width is 2 microseconds.
  digitalWrite(amisStepPin, LOW);
  delayMicroseconds(3);
  delay(step_delay);
  }
}

void setDirection(bool dir)
{
  /*
   * dir Writes a high or low value to the direction pin to specify what direction to turn the motor.
   */
  delayMicroseconds(1);
  digitalWrite(amisDirPin, dir);
  delayMicroseconds(1);
  // The NXT/STEP pin must not change for at least 0.5
  // microseconds before and after changing the DIR pin.
}
