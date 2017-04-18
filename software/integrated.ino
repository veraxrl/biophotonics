/*
 *   integrated.ino
 *   Master MCU code for controlling the following components
 *   + stepper motor
 *
 *   adapted from amis-30543 examples
 *   http://pololu.github.io/amis-30543-arduino/
 *   Copyright (c) 2016-2017 Daniel Hull

*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Wire.h>
#include <ArduCAM.h>
#include "memorysaver.h"

#include <SPI.h>
#include <AMIS30543.h>
AMIS30543 stepper;

// current in milliamps, DO NOT GO ABOVE 1000
#define CURRENT_MAX 800

// 4, 8, 16, 32, lower value will make smoother
#define DEGREES 180
#define MICROSTEP_COUNT 8
int step_count = (DEGREES/1.8)*MICROSTEP_COUNT;

// set higher for slower, set lower for faster (msec)
int speed_delay = 7500/step_count;

const uint8_t amisDirPin = D0;
const uint8_t amisStepPin = D1;
const uint8_t amisSlaveSelect = D8;

int rotations = 0;
int iter = 0;

// Mark Camera status: 
int camera_status = 1;
// 2: have reached the end
// 1: stopped, can capture image
// 0: capture done, ready to move


// set GPIO16 as the slave select :
const int CS = 16;
int wifiType = 0;

//Station mode you should put your ssid and password
const char* ssid = "DukeOpen"; // Put your SSID here
const char* password = "TC8715D27E252"; // Put your PASSWORD here

ESP8266WebServer server(80);
ArduCAM myCAM(OV5642, CS);

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
   myCAM.OV5642_set_JPEG_size(OV5642_2592x1944);
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


  // Step motor Setup: SPI.begin() already 
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


// Variables related to state machine

int meas_time = 0;
int stop_count = 0;

void loop(){

  /*When the camera has come to the last stop*/
  if (camera_status == 2) { 
    int total_step = 15;
    stepBackward(total_step);   //The function should have its own delay to make sure enough time is given for its completion
    meas_time = 0;
    stop_count = 0;
  }

  /*When the camera stops and ready for capture */
  else if (camera_status == 1) {
    server.handleClient();
    if (millis() - meas_time > 10000) {    //Give 10s for capture here, can be adjusted 
      camera_status = 0;
      meas_time = millis();
    }
  }

  /*When the camera is done with capture and ready to move to next stage*/
  else {
    int one_step = 1;
    stepForward(one_step);
    if (stop_count == total_step) {
      camera_status = 2;
    }
    else {
      camera_status = 1;
      meas_time = millis();
      stop_count++;
    }
  }

}


// Sends a pulse on the NXT/STEP pin to tell the driver to take
// one step, and also delays to control the speed of the motor.
void stepForward(int num_steps){
  setDirection(0);
  for (unsigned int x = 0; x < num_steps; x++){
    // The NXT/STEP minimum high pulse width is 2 microseconds.
    digitalWrite(amisStepPin, HIGH);
    delayMicroseconds(3);
    digitalWrite(amisStepPin, LOW);
    delayMicroseconds(3);
    delay(speed_delay);
  }
  ++iter;
  ++rotations;
}

void stepBackward(int num_steps){
  setDirection(1);
  for (unsigned int x = 0; x < num_steps; x++){
    // The NXT/STEP minimum high pulse width is 2 microseconds.
    digitalWrite(amisStepPin, HIGH);
    delayMicroseconds(3);
    digitalWrite(amisStepPin, LOW);
    delayMicroseconds(3);
    delay(speed_delay);
  }
  --rotations;
}

// Writes a high or low value to the direction pin to specify
// what direction to turn the motor.
void setDirection(bool dir){
  // The NXT/STEP pin must not change for at least 0.5
  // microseconds before and after changing the DIR pin.
  delayMicroseconds(1);
  digitalWrite(amisDirPin, dir);
  delayMicroseconds(1);
}
