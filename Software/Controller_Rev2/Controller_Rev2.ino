#include <Servo.h>
#include <SPI.h>
#include <XBOXRECV.h>

USB Usb;  //Create a USB instance to connect to the wireless dongle
XBOXRECV Xbox(&Usb); //Create wireless XBOX360 controller instance on the USB port

//Create servo instances to represent the motor controllers
Servo leftSide;
Servo rightSide;

boolean isEnabled;
const int LEFT_PWM = 9;
const int RIGHT_PWM = 10;
const int DEADZONE_L = 0;
const int DEADZONE_R = 0;
const unsigned long TIMEOUT = 250;  // Time allowed between commands (in ms) before the robot automatically disables

int leftStick_Y;
int rightStick_Y;
int leftSpeed;
int rightSpeed;
unsigned long lastData = 250;

void setup() {
  Serial.begin(9600);
  isEnabled = FALSE;  //Robot starts disabled for safety

  if(Usb.Init() == -1) {
    Serial.print("Error starting wireless receiver library!");
    while (1);
  }
  Serial.println("Wireless receiver library loaded successfuly!");

  int leftSpeed = 90;
  int rightSpeed = 90;
}

void loop() {
  Usb.Task();
  if (Xbox.XboxReceiverConnected) {
    if (Xbox.Xbox360Connected[0]) {
      if(isEnabled & millis() - lastData <= TIMEOUT) {
        runRobot();
      }
      if(Xbox.getButtonClick(START, 0) {
        if(isEnabled) disable();
        else enable();
      }
    }
    else {
      disable(); // Lost contact with controller
      delay(500);
    }
  }
  else {
    disable();  // Lost contact with receiver
    delay(1000);
  }
}

void enable() {
  isEnabled = TRUE;
  leftSide.attach(LEFT_PWM);
  rightSide.attach(RIGHT_PWM);

  //Change status of controller LEDs if status just changed
  //Vibrate?
}

void disable() {
  isEnabled = FALSE;
  leftSide.detach(LEFT_PWM);
  rightSide.detach(RIGHT_PWM);

  //Change status of the controller LEDs if robot was just disabled
  // Vibrate?

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

  //Deadzone calc

  leftSide.write(leftSpeed);
  rightSide.write(rightSpeed);
}

void getData() {
  leftStick_Y = Xbox.getAnalogHat(LeftHatY,0);
  rightStick_Y = Xbox.getAnalogHat(RightHatY,0);
  lastData = millis();//Update data timestamp
}
