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
const int LEFT_PWM = 3;
const int RIGHT_PWM = 4;
const int DEADZONE_L = 10;
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

  wdt_enable(WDTO_1S);  // 1 Second watcdog timeout

  if(Usb.Init() == -1) {
    Serial.print("Error starting wireless receiver library!");
    while (1);  // Wait here for watchdog to catch us
  }
  Serial.println("Wireless receiver library loaded successfuly!");

  int leftSpeed = 90;
  int rightSpeed = 90;

  wdt_reset();
  lastData = millis();
}

void loop() {
  Usb.Task();
  if(millis() - lastData > 300) { // Allow the microcontroller to spin (while still feeding watchdog) to allow Xbox receiver to initialize
    if (Xbox.XboxReceiverConnected) {
      if (Xbox.Xbox360Connected[0]) {
        if(!last_connected) Serial.println("Controller Connected!");
        //Serial.print("isEnabled: "); Serial.println(isEnabled);
        //delay(50);
        //Serial.println(millis() - lastData);
        if(isEnabled && millis() - lastData <= TIMEOUT && millis() > 500) {
          //Serial.println("Running Robot!");
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
      else {
        Serial.println("Error: No Controller!");
        disable(); // Lost contact with controller
      }
    } // End receiver connected case
    else {
      Serial.println("Error: No Receiver!");
      disable();  // Lost contact with receiver
    }
    last_connected = Xbox.Xbox360Connected[0];
  } // End Xbox initialization waiting case
  wdt_reset();  //Feed the watchdog
}

void enable() {
  //Serial.print("Enabling! - "); Serial.println(isEnabled);
  leftSide.attach(LEFT_PWM);
  rightSide.attach(RIGHT_PWM);

  if(!isEnabled && Xbox.XboxReceiverConnected && Xbox.Xbox360Connected[0]) {
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
    Xbox.setLedBlink(ALL,0);
    Xbox.setRumbleOn(60000,60000,0);
    delay(200);
    Xbox.setRumbleOff(0);
  }
  isEnabled = FALSE;
  lastData = millis();
}

void runRobot() {
  Xbox.setLedOn(LED1,0);  // Because the controller needs to be reminded periodically how the LEDs should look for some reason..
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

  //Serial.print("Writing to Jags: Left: "); Serial.print(leftSpeed); Serial.print(" | Right: "); Serial.println(rightSpeed);
  leftSide.write(leftSpeed);
  rightSide.write(rightSpeed);
}

void getData() {
  leftStick_Y = Xbox.getAnalogHat(LeftHatY,0);
  rightStick_Y = Xbox.getAnalogHat(RightHatY,0);
  lastData = millis();//Update data timestamp
}
