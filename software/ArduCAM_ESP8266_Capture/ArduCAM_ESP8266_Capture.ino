// ArduCAM Mini demo (C)2017 Lee
// Web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with ArduCAM ESP8266 2MP/5MP camera.
// This demo was made for ArduCAM ESP8266 2MP/5MP Camera.
// It can take photo and send to the Web.
// It can take photo continuously as video streaming and send to the Web.
// The demo sketch will do the following tasks:
// 1. Set the camera to JPEG output mode.
// 2. if server.on("/capture", HTTP_GET, serverCapture),it can take photo and send to the Web.
// 3.if server.on("/stream", HTTP_GET, serverStream),it can take photo continuously as video 
//streaming and send to the Web.

// This program requires the ArduCAM V4.0.0 (or later) library and ArduCAM ESP8266 2MP/5MP camera
// and use Arduino IDE 1.6.8 compiler or above

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#if !(defined ESP8266 )
#error Please select the ArduCAM ESP8266 UNO board in the Tools/Board
#endif

//This demo can only work on OV2640_MINI_2MP or ARDUCAM_SHIELD_V2 platform.
#if !(defined (OV2640_MINI_2MP)||defined (OV5640_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP_PLUS) \
    || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) \
    ||(defined (ARDUCAM_SHIELD_V2) && (defined (OV2640_CAM) || defined (OV5640_CAM) || defined (OV5642_CAM))))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif
// set GPIO16 as the slave select :
const int CS = 16;
//Version 1,set GPIO1 as the slave select :
const int SD_CS = 1;

ArduCAM myCAM(OV5642, CS);

void myCAMSaveToFile(){
  Serial.println("Running myCAMSaveToSDFile");
  char str[8];
  byte buf[256];
  static int i = 0;
  static int k = 0;
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  bool is_header = false;
  //Flush the FIFO
  myCAM.flush_fifo();
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Start capture
  myCAM.start_capture();
  Serial.println(F("Start Capture"));
  while(!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
  Serial.println(F("Capture Done."));  

  length = myCAM.read_fifo_length();
  Serial.print(F("The fifo length is :"));
  Serial.println(length, DEC);
  if (length >= MAX_FIFO_SIZE) {  //8M
    Serial.println(F("Over size."));
  }
  if (length == 0 ) {  //0kb
    Serial.println(F("Size is 0."));
  }

  i = 0;
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  while ( length-- ){
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) {
    //If find the end ,break while
        buf[i++] = temp;  //save the last  0XD9     
       //Write the remain bytes in the buffer
        myCAM.CS_HIGH();
        //outFile.write(buf, i);
        for (int z = 0; z < i; z++) {
          	Serial.write(buf[z]);
        } 
        Serial.println(F("Image save OK."));
        is_header = false;
        i = 0;
    }  
    if (is_header == true){ 
       //Write image data to buffer if not full
        if (i < 256)
        buf[i++] = temp;
        else
        {
          //Write 256 bytes image data to file
          myCAM.CS_HIGH();
          //outFile.write(buf, 256);
          for (int z = 0; z < 256; z++) {
          	Serial.write(buf[z]);
          }
          i = 0;
          buf[i++] = temp;
          myCAM.CS_LOW();
          myCAM.set_fifo_burst();
        }        
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF)){
      is_header = true;
      buf[i++] = temp_last;
      buf[i++] = temp;   
    } 
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
	Serial.begin(9600);
	Serial.println(F("ArduCAM Start!"));

	// set the CS as an output:
	pinMode(CS, OUTPUT);

	// initialize SPI:
	SPI.begin();
	SPI.setFrequency(4000000); //4MHz
	delay(1000);

	//Check if the ArduCAM SPI bus is OK
	myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
	temp = myCAM.read_reg(ARDUCHIP_TEST1);
	if (temp != 0x55){
	Serial.println(F("SPI1 interface Error!"));
	while(1);
	}
	#if defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
	//Check if the camera module type is OV5640
	myCAM.wrSensorReg16_8(0xff, 0x01);
	myCAM.rdSensorReg16_8(OV5640_CHIPID_HIGH, &vid);
	myCAM.rdSensorReg16_8(OV5640_CHIPID_LOW, &pid);
	if((vid != 0x56) || (pid != 0x40))
	Serial.println(F("Can't find OV5640 module!"));
	else
	Serial.println(F("OV5640 detected."));
	#elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
	//Check if the camera module type is OV5642
	myCAM.wrSensorReg16_8(0xff, 0x01);
	myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
	myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
	if((vid != 0x56) || (pid != 0x42)){
	Serial.println(F("Can't find OV5642 module!"));
	}
	else
	Serial.println(F("OV5642 detected."));
	#endif


	//Change to JPEG capture mode and initialize the OV2640 module
	myCAM.set_format(JPEG);
	myCAM.InitCAM();
	#if defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
	myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
	myCAM.OV5640_set_JPEG_size(OV5640_320x240);
	#elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
	myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
	myCAM.OV5642_set_JPEG_size(OV5642_2592x1944);  
	#endif

	myCAM.clear_fifo_flag();

}

void loop() {
	Serial.println(F("Looping"));
	delay(5000);
	myCAMSaveToFile();
}

