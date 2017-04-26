/*
 *   integrated.ino
 *   Master MCU code for controlling the following components
 *   + stepper motor
 *
 *   adapted from amis-30543 examples
 *   http://pololu.github.io/amis-30543-arduino/
 *   Copyright (c) 2016-2017 Daniel Hull & Vera Xu

*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Wire.h>
#include <ArduCAM.h>
#include "memorysaver.h"

#include <SPI.h>

#define TOTAL_DEGREES 180
#define IMAGE_COUNT 15
#define MOVEMENT_TIME 5000
#define MICROSTEP_COUNT 16
#define TRANSFER_DELAY 7000

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


// set GPIO16 as the slave select :
const int CS = 16;
int wifiType = 0;

//Station mode you should put your ssid and password
const char* ssid = "DukeOpen"; // Put your SSID here
const char* password = "TC8715D27E252"; // Put your PASSWORD here

ESP8266WebServer server(80);
ArduCAM myCAM(OV5642, CS);

void setup() {
  uint8_t vid, pid;
  uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  Serial.begin(115200);
  Serial.println("ArduCAM Start!");

  // set the CS as an output:
  pinMode(CS, OUTPUT);

  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(4000000); //4MHz

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55){
    Serial.println("SPI1 interface Error!");
    while(1);
  }
  //Check if the camera module type is OV5642
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
   if((vid != 0x56) || (pid != 0x42)){
   Serial.println("Can't find OV5642 module!");
   while(1);
   }
   else
   Serial.println("OV5642 detected.");


  //Change to JPEG capture mode and initialize the OV5642 module
  myCAM.set_format(JPEG);
   myCAM.InitCAM();
   myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
   myCAM.OV5642_set_JPEG_size(OV5642_1600x1200);
   delay(1000);
  if(!strcmp(ssid,"SSID")){
    Serial.println("Please set your SSID");
    while(1);
  }
  if(!strcmp(password,"PASSWORD")){
    Serial.println("Please set your PASSWORD");
    while(1);
  }
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/capture", HTTP_GET, serverCapture);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Server started");

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

// Variables related to state machine
int meas_time = 0;
int images_taken = 0;
// Mark Camera status:
int camera_status = 1;
// 2: have reached the end
// 1: stopped, can capture image
// 0: capture done, ready to move


void loop(){
  int start_time = millis();
  server.handleClient();
  // Picture Captured
  if (millis() - start_time > 3000) {
    // Move motor here
    step(step_count,0);
  }

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

void setStep(int microsteps){
  switch (microsteps){
    case 16:
      digitalWrite(ms1_pin, HIGH);
      digitalWrite(ms2_pin, HIGH);
      digitalWrite(ms3_pin, HIGH);
    case 8:
      digitalWrite(ms1_pin, HIGH);
      digitalWrite(ms2_pin, HIGH);
      digitalWrite(ms3_pin, LOW);
    case 4:
      digitalWrite(ms1_pin, LOW);
      digitalWrite(ms2_pin, HIGH);
      digitalWrite(ms3_pin, LOW);
    case 2:
      digitalWrite(ms1_pin, HIGH);
      digitalWrite(ms2_pin, LOW);
      digitalWrite(ms3_pin, LOW);
    case 1:
      digitalWrite(ms1_pin, LOW);
      digitalWrite(ms2_pin, LOW);
      digitalWrite(ms3_pin, LOW);
  }
}


void start_capture(){
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM){
  WiFiClient client = server.client();

  size_t len = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE){
    Serial.println("Over size.");
    return;
  }else if (len == 0 ){
    Serial.println("Size is 0.");
    return;
  }

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  #if !(defined (OV5642_MINI_5MP_PLUS) ||(defined (ARDUCAM_SHIELD_V2) && defined (OV5642_CAM)))
  SPI.transfer(0xFF);
  #endif
  if (!client.connected()) return;
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-Length: " + String(len) + "\r\n\r\n";
  server.sendContent(response);
  static const size_t bufferSize = 4096;
  static uint8_t buffer[bufferSize] = {0xFF};
  while (len) {
      size_t will_copy = (len < bufferSize) ? len : bufferSize;
      myCAM.transferBytes(&buffer[0], &buffer[0], will_copy);
      if (!client.connected()) break;
      client.write(&buffer[0], will_copy);
      len -= will_copy;
  }

  myCAM.CS_HIGH();
}

void serverCapture(){
  start_capture();
  Serial.println("CAM Capturing");

  int total_time = 0;
  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  Serial.print("capture total_time used (in miliseconds):");
  Serial.println(total_time, DEC);
  total_time = 0;
  Serial.println("CAM Capture Done!");
  total_time = millis();
  camCapture(myCAM);
  total_time = millis() - total_time;
  Serial.print("send total_time used (in miliseconds):");
  Serial.println(total_time, DEC);
  Serial.println("CAM send Done!");
}

void handleNotFound(){
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text/plain", message);

  if (server.hasArg("ql")){
    int ql = server.arg("ql").toInt();
    myCAM.OV5642_set_JPEG_size(ql);
    delay(1000);
    Serial.println("QL change to: " + server.arg("ql"));
  }
}
