//Purdue FIRST Programs - Purdue University

#include <SPI.h>
#include <XBOXRECV.h>

USB Usb;  //Create a USB instance to connect to the wireless dongle
XBOXRECV Xbox(&Usb); //Create wireless XBOX360 controller instance on the USB port

boolean last_connected;
boolean last_disabled;
boolean disabled;
int left_speed;
const int LEFT_PWM = 6;
const int RIGHT_PWM = 5;

void setup() {
  Serial.begin(9600);
  //pinMode(11,OUTPUT);
  if(Usb.Init() == -1) {
    Serial.print("Error starting wireless receiver library!");
    while (1);
  }
  Serial.println("Wireless receiver library loaded successfuly!");
  last_connected = false;
  last_disabled = false;
  disabled = true;
  left_speed = 0;
}

void loop() {
  Usb.Task();
  if (Xbox.XboxReceiverConnected) {
    //Serial.print("Disabled: "); Serial.print(disabled); Serial.print("Last_Disabled: "); Serial.println(last_disabled);
    if (Xbox.Xbox360Connected[0]) {
      if(!last_connected) {
        Serial.println("Controller connected!");
        last_connected = !last_connected;
      }
      if(disabled) {  //If the robot is disabled don't do anything
        if(last_disabled != disabled) {
          Xbox.setLedMode(ALTERNATING, 0);
          analogWrite(LEFT_PWM,127);
          analogWrite(RIGHT_PWM,127);
        }
      }
      else {    //Robot is not disabled, let it move
        //Collect Joystick Data
        Serial.print("Left Stick: "); Serial.print(Xbox.getAnalogHat(LeftHatY,0));
        left_speed = map(Xbox.getAnalogHat(LeftHatY,0),-32767,32768,0,255);
        Serial.print(" Adjusted: "); Serial.println(left_speed);
        analogWrite(LEFT_PWM,left_speed);
      }
      last_disabled = disabled;
      if(Xbox.getButtonClick(START, 0) /*&& Xbox.getButtonClick(R1, 0)*/) {
        Serial.println("Robot status toggled!");
        if(disabled == true) {
          Xbox.setLedOn(LED1,0);
        }
        disabled = !disabled;
      }
    }
    else {  //If there is a receiver but no controller
      if(last_connected){
        disabled = true;
        last_disabled = false;
        Serial.println("Controller disconnected!");
        last_connected = !last_connected;
      }
    }
  }

  delay(50);
}
