/*
 *   turn_180.ino
 *   Master MCU code for controlling the following components
 *   + stepper motor
 *   + driver
 *
 *   Copyright (c) 2016-2017 Daniel Hull

*/

#define TOTAL_DEGREES 180
#define IMAGE_COUNT 15
#define MOVEMENT_TIME 1000
#define MICROSTEP_COUNT 16

int step_degrees = TOTAL_DEGREES/IMAGE_COUNT; // degrees per image
int step_count = (step_degrees/1.8)*MICROSTEP_COUNT; // steps per image
int step_delay = MOVEMENT_TIME/step_count;

// set higher for higher resolution (based on vera's table set lower for faster (msec)
#define TRANSFER_DELAY 5000

const uint8_t dir_pin = 13;
const uint8_t step_pin = 12;
const uint8_t sleep_pin = 11;
const uint8_t ms1_pin = 10;
const uint8_t ms2_pin = 9;
const uint8_t ms3_pin = 8;


int num_images = 0;

void setup()
{
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(sleep_pin, OUTPUT);

  digitalWrite(step_pin, LOW);
  digitalWrite(dir_pin, LOW);
  digitalWrite(sleep_pin, LOW);

  setStep(MICROSTEP_COUNT);

  delay(1);
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
  digitalWrite(sleep_pin, HIGH);
  setDirection(direction);
  for (unsigned int x = 0; x < num_steps; x++){
  digitalWrite(step_pin, HIGH);
  delayMicroseconds(10); // The NXT/STEP minimum high pulse width is 2 microseconds.
  digitalWrite(step_pin, LOW);
  delayMicroseconds(10);
  delay(step_delay);
  }
  digitalWrite(sleep_pin, LOW);

}

void setDirection(bool dir)
{
  /*
   * dir Writes a high or low value to the direction pin to specify what direction to turn the motor.
   */
  delayMicroseconds(1);
  digitalWrite(dir_pin, dir);
  delayMicroseconds(1);
  // The NXT/STEP pin must not change for at least 0.5
  // microseconds before and after changing the DIR pin.
}
