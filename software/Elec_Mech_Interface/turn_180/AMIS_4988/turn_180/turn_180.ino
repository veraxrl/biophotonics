/*
 *   turn_180.ino
 *   Master MCU code for ESP8266 for controlling the following components
 *   + stepper motor
 *   + driver (Big EasyDriver)
 *
 *   Copyright (c) 2016-2017 Daniel Hull

*/

#define TOTAL_DEGREES 180
#define IMAGE_COUNT 15
#define MOVEMENT_TIME 1000
#define MICROSTEP_COUNT 16
#define TRANSFER_DELAY 10000

int step_degrees = TOTAL_DEGREES/IMAGE_COUNT; // degrees per image
int step_count = (step_degrees/1.8)*MICROSTEP_COUNT; // steps per image
int step_delay = MOVEMENT_TIME/step_count;

// set higher for higher resolution (based on vera's table set lower for faster (msec)

const uint16_t dir_pin = D0;
const uint16_t step_pin = D1;
const uint16_t sleep_pin = D2;
const uint16_t ms1_pin = D3;
const uint16_t ms2_pin = D4;
const uint16_t ms3_pin = D5;

int num_images = 0;

void setup()
{
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(sleep_pin, OUTPUT);
  pinMode(ms1_pin, OUTPUT);
  pinMode(ms2_pin, OUTPUT);
  pinMode(ms3_pin, OUTPUT);

  digitalWrite(step_pin, LOW);
  digitalWrite(dir_pin, LOW);
  digitalWrite(sleep_pin, HIGH);

  digitalWrite(ms1_pin, HIGH);
  digitalWrite(ms2_pin, HIGH);
  digitalWrite(ms3_pin, HIGH);

  delay(1);
}


void loop(){
    //capture image
    if (num_images < 15){
    step(step_count,0);
    delay(TRANSFER_DELAY);
    ++num_images;
    }
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
  digitalWrite(step_pin, HIGH);
  delayMicroseconds(2); // The NXT/STEP minimum high pulse width is 1 microseconds.
  digitalWrite(step_pin, LOW);
  delayMicroseconds(2);
  delay(step_delay);
  }

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

void setStep(int microsteps){
  delay(1);
  switch (microsteps){
    case 16:
      digitalWrite(ms1_pin, HIGH);
      digitalWrite(ms2_pin, HIGH);
      digitalWrite(ms3_pin, HIGH);
      break;
    case 8:
      digitalWrite(ms1_pin, HIGH);
      digitalWrite(ms2_pin, HIGH);
      digitalWrite(ms3_pin, LOW);
      break;
    case 4:
      digitalWrite(ms1_pin, LOW);
      digitalWrite(ms2_pin, HIGH);
      digitalWrite(ms3_pin, LOW);
      break;
    case 2:
      digitalWrite(ms1_pin, HIGH);
      digitalWrite(ms2_pin, LOW);
      digitalWrite(ms3_pin, LOW);
      break;
    case 1:
      digitalWrite(ms1_pin, LOW);
      digitalWrite(ms2_pin, LOW);
      digitalWrite(ms3_pin, LOW);
      break;
    default:
      digitalWrite(ms1_pin, LOW);
      digitalWrite(ms2_pin, LOW);
      digitalWrite(ms3_pin, LOW);
      break;
  }
}
