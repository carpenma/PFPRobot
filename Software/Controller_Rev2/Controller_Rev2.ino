#include <Servo.h>
#include <SPI.h>
#include <XBOXRECV.h>

#define FALSE 0
#define TRUE 1

USB Usb;  //Create a USB instance to connect to the wireless dongle
XBOXRECV Xbox(&Usb); //Create wireless XBOX360 controller instance on the USB port

//Create servo instances to represent the motor controllers
Servo leftSide;
Servo rightSide;

boolean isEnabled;
boolean last_connected;
const int LEFT_PWM = 9;
const int RIGHT_PWM = 10;
const int DEADZONE_L = 0;
const int DEADZONE_R = 0;
const unsigned long TIMEOUT = 500;  // Time allowed between commands (in ms) before the robot automatically disables

int leftStick_Y;
int rightStick_Y;
int leftSpeed;
int rightSpeed;
unsigned long lastData;

void setup() {
  Serial.begin(9600);
  isEnabled = FALSE;  //Robot starts disabled for safety

  if(Usb.Init() == -1) {
    Serial.print("Error starting wireless receiver library!");
    while (1);
  }
  Serial.println("Wireless receiver library loaded successfuly!");
  delay(250);
  int leftSpeed = 90;
  int rightSpeed = 90;

  lastData = millis();
}

void loop() {
  Usb.Task();
  if (Xbox.XboxReceiverConnected) {
    if (Xbox.Xbox360Connected[0]) {
      if(!last_connected) Serial.println("Controller Connected!");
      //Serial.print("isEnabled: "); Serial.println(isEnabled);
      //delay(50);
      //Serial.println(millis() - lastData);
      if(isEnabled && millis() - lastData <= TIMEOUT && millis() > 500) {
        Serial.println("Running Robot!");
        runRobot();
      }
      else {
        disable();
      }
      if(Xbox.getButtonClick(START, 0)) {
        //Serial.println("Start Button Detected!");
        if(isEnabled) disable();
        else enable();
      }
    }
    else {
      Serial.println("Error: No Controller!");
      disable(); // Lost contact with controller
      delay(500);
    }
  }
  else {
    Serial.println("Error: No Receiver!");
    disable();  // Lost contact with receiver
    delay(1000);
  }
  last_connected = Xbox.Xbox360Connected[0];
}

void enable() {
  //Serial.print("Enabling! - "); Serial.println(isEnabled);
  leftSide.attach(LEFT_PWM);
  rightSide.attach(RIGHT_PWM);

  if(!isEnabled && Xbox.XboxReceiverConnected && Xbox.Xbox360Connected[0]) {
    //Serial.println("Enabled Transition!");
    Xbox.setLedOn(LED1,0);  //May change this so that different LEDs come on for different speeds/modes etc.
    Xbox.setRumbleOn(60000,60000,0);
    delay(200);
    Xbox.setRumbleOff(0);
  }
  isEnabled = TRUE;
}

void disable() {
  //Serial.print("Disabling! - "); Serial.println(isEnabled);
  leftSide.detach();
  rightSide.detach();

  if(isEnabled && Xbox.XboxReceiverConnected && Xbox.Xbox360Connected[0]) {
    //Serial.println("Disabled Transition!");
    Xbox.setLedMode(ALTERNATING, 0);
    Xbox.setRumbleOn(60000,60000,0);
    delay(200);
    Xbox.setRumbleOff(0);
  }
  isEnabled = FALSE;
  lastData = millis();
}

void runRobot() {
  getData();
  moveRobot();
}

// Y Axis = Up/Down
// X Axis = Left/Right
void moveRobot() {
  leftSpeed = map(leftStick_Y,-32767,32768,0,180);
  rightSpeed = map(rightStick_Y,-32767,32768,0,180);

  if (leftSpeed <= (90 + DEADZONE_L) & leftSpeed >= (90 - DEADZONE_L)) {
    leftSpeed = 90;
  }
  if(rightSpeed <= (90 + DEADZONE_R) & rightSpeed >= (90 - DEADZONE_R)) {
    rightSpeed = 90;
  }

  leftSide.write(leftSpeed);
  rightSide.write(rightSpeed);
}

void getData() {
  leftStick_Y = Xbox.getAnalogHat(LeftHatY,0);
  rightStick_Y = Xbox.getAnalogHat(RightHatY,0);
  lastData = millis();//Update data timestamp
}
