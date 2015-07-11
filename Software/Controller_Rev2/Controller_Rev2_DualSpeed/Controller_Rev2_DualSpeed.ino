/*  PFP Promotional Robot Code v2.0 Dual Speed
    Purdue FIRST Programs - Purdue University
    This program is designed to take input from a wireless XBox controller, convert it to servo speeds and
    output it via PWM to control the PFP promotional robot.  Note that this code version supports two
    different speed modes (half and full) which are toggled using the BACK button on the controller.

    For assistance contact Matt Carpenter carpenma@gmail.com
*/
#include <Servo.h>
#include <SPI.h>
#include <XBOXRECV.h>
#include <avr/wdt.h>

#define FALSE 0
#define TRUE 1

USB Usb;  //Create a USB instance to connect to the wireless dongle
XBOXRECV Xbox(&Usb); //Create wireless XBOX360 controller instance on the USB port

//Create servo instances to represent the motor controllers
Servo leftSide;
Servo rightSide;

boolean isEnabled;
boolean last_connected;
boolean fullSpeed;
const int LEFT_PWM = 2;     // Pin declarations for left and right PWM outputs
const int RIGHT_PWM = 3;
const int DEADZONE_L = 10;  // Range of values (+/-) on the controller which will translate to centered
const int DEADZONE_R = 10;
const unsigned long TIMEOUT = 500;  // Time allowed between commands (in ms) before the robot automatically disables

int leftStick_Y;
int rightStick_Y;
int leftSpeed;
int rightSpeed;
unsigned long lastData;

void setup() {
  Serial.begin(115200);
  isEnabled = FALSE;  //Robot starts disabled for safety
  fullSpeed = FALSE; // Also a safety precaution, robot starts in half-speed mode
  Serial.println("Initializing...");
  wdt_enable(WDTO_1S);  // 1 Second watcdog timeout

  if(Usb.Init() == -1) {
    Serial.print("Error starting wireless receiver library!");
    while (1);  // Wait here for watchdog to catch us
  }
  Serial.println("Wireless receiver library loaded successfuly!");

  int leftSpeed = 90;
  int rightSpeed = 90;

  wdt_reset();
  Serial.println("=== PFP Robot Code v2.0 Dual Speed Variant ===");
  lastData = millis();
}

void loop() {
  Usb.Task();
  if(millis() - lastData > 300) { // Allow the microcontroller to spin (while still feeding watchdog) to allow Xbox receiver to initialize
    if (Xbox.XboxReceiverConnected) {   // The wireless receiver was found
      if (Xbox.Xbox360Connected[0]) {   // Controller connected to the receiver
        if(!last_connected) Serial.println("Controller Connected!");
        if(isEnabled && millis() - lastData <= TIMEOUT && millis() > 500) {
          if(Xbox.getButtonClick(BACK, 0)) {  // Check if buttons are clicked to toggle the speed mode and flip it if so
            fullSpeed = !fullSpeed;
            if (fullSpeed) Xbox.setLedOn(LED2,0);
            else Xbox.setLedOn(LED1,0);
          } // End change drive speed case
          runRobot();
        }
        else {
          disable();
        }
        if(Xbox.getButtonClick(START, 0)) {
          if(isEnabled) disable();
          else enable();
        }
      } // End controller connected case
      else {  // Lost contact with controller
        Serial.println("Error: No Controller!");
        disable();
      }
    } // End receiver connected case
    else {  // Lost contact with receiver
      Serial.println("Error: No Receiver!");
      disable();
    }
    last_connected = Xbox.Xbox360Connected[0];
  } // End Xbox initialization waiting case
  wdt_reset();  //Feed the watchdog
}

void enable() {
  if(!isEnabled) Serial.println("Enabling!");
  fullSpeed = FALSE;  // Whenever the robot is enabled it starts in half-speed mode
  leftSide.attach(LEFT_PWM);
  rightSide.attach(RIGHT_PWM);

  if(!isEnabled && Xbox.XboxReceiverConnected && Xbox.Xbox360Connected[0]) {
    if (fullSpeed) Xbox.setLedOn(LED2,0);
    else Xbox.setLedOn(LED1,0);
    Xbox.setRumbleOn(60000,60000,0);
    delay(200);
    Xbox.setRumbleOff(0);
  }
  isEnabled = TRUE;
}

void disable() {
  if(isEnabled) Serial.println("Disabling!");
  leftSide.detach();
  rightSide.detach();

  if(isEnabled && Xbox.XboxReceiverConnected && Xbox.Xbox360Connected[0]) {
    Xbox.setLedBlink(ALL,0);
    Xbox.setRumbleOn(60000,60000,0);
    delay(200);
    Xbox.setRumbleOff(0);
  }
  isEnabled = FALSE;
  fullSpeed = FALSE;
  lastData = millis();
}

void runRobot() {
  getData();
  moveRobot();
}

// Y Axis = Up/Down
// X Axis = Left/Right
void moveRobot() {
  if (fullSpeed) {
    leftSpeed = map(leftStick_Y,-32767,32768,0,180);
    rightSpeed = map(rightStick_Y,-32767,32768,0,180);
  }
  else {  // Only use the middle of the speed range
    leftSpeed = map(leftStick_Y,-32767,32768,45,135);
    rightSpeed = map(rightStick_Y,-32767,32768,45,135);
  }

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
