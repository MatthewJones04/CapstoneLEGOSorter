#include <AccelStepper.h>
#include <Wire.h>
#include <Servo.h>
#include "Brick-By-Brick-Tester.h"

AccelStepper upperInConvey1(AccelStepper::DRIVER, UPPER_IN_CONVEY_1_STEP, UPPER_IN_CONVEY_1_DIR);
AccelStepper identConvey1(AccelStepper::DRIVER, IDENT_CONVEY_1_STEP, IDENT_CONVEY_1_DIR);

//create angular servos
Servo bins[1];

//create CR servos
Servo inputShakeCR;

bool binActive = false;
unsigned long binStartTime = 0;
int currentBin = -1;


void setup() {

  //set max speed of stepper motors
  upperInConvey1.setMaxSpeed(2000);
  upperInConvey1.setAcceleration(1000);

  identConvey1.setMaxSpeed(2000);
  identConvey1.setAcceleration(1000);

  //attach pins to servo
  bins[0].attach(BIN_SIGNAL_0);

  //attach pins to CR servo
  inputShakeCR.attach(INPUT_SHAKE_SIGNAL);

  //turn on serial communication for arduino mega
  Serial.begin(115200);
}

void loop() {
  // checkSerial();   // handle incoming bin commands

  // // handle timing for active bin
  // if (binActive && millis() - binStartTime >= 250) {
  //   setServo(bins[currentBin], BIN_CLOSED);
  //   runServo(inputShakeCR, 0);
  //   upperInConvey1.setSpeed(0);
  //   binActive = false;
  // }

  // if (Serial.available() >= 3) {
  //   char buffer[4];
  //   Serial.readBytes(buffer, 3);
  //   buffer[3] = '\0';  // Null terminate

  //   Serial.print("Received: ");
  //   Serial.println(buffer);
  // }

  upperInConvey1.setSpeed(500);
  identConvey1.setSpeed(500);

  // setServo(bins[0], 0);
  // delay(1500);
  // setServo(bins[0], 180);
  // delay(1500);


  // keep steppers running
  upperInConvey1.runSpeed();
  identConvey1.runSpeed();
} //end loop


///////////////////////////////////////////////////////////////
// BIN HANDLING CODE //
///////////////////////////////////////////////////////////////

  void handleBin(int binNumber) {
    currentBin = binNumber;

    setServo(bins[binNumber], BIN_OPEN);
    runServo(inputShakeCR, 100);
    upperInConvey1.setSpeed(2000);
    binStartTime = millis();
    binActive = true;
  }
  

  void checkSerial() {
    static char buffer[4];
    static uint8_t index = 0;

    while (Serial.available() > 0) {
      char c = Serial.read();

      if (c == '\n') {

        buffer[index] = '\0';

        if (buffer[0] == 'B' &&
            isDigit(buffer[1]) &&
            isDigit(buffer[2])) {

          int bin = (buffer[1] - '0') * 10 +
                    (buffer[2] - '0');

          if (bin >= 0 && bin <= 13) {
            if (!binActive) {
              handleBin(bin);
            }
          }
        }

        index = 0;
      }
      else {
        if (index < 3) {
          buffer[index++] = c;
        }
      }
    }
  }


///////////////////////////////////////////////////////////////
// MOVEMENT CODE //
///////////////////////////////////////////////////////////////

  //run the motor with an acceleration to a relative position
  void setRelativeMotor(AccelStepper& stepper, int acceleration, int relativePosition){
    stepper.setAcceleration(acceleration);
    stepper.moveTo(stepper.currentPosition() + relativePosition);
  }

  //run the motor with an acceleration to an absolute position
  void setAbsoluteMotor(AccelStepper& stepper, int acceleration, int absolutePosition){
    stepper.setAcceleration(acceleration);
    stepper.moveTo(absolutePosition);
  }

  //run a CR servo at a set speed
  void runServo(Servo& servo, int speedPercent){
    int speed = map(speedPercent, -100, 100, 0, 180);
    servo.write(speed);
  }

  //set a servo to a desired angle
  void setServo(Servo& servo, int angle){
    servo.write(angle);
  }

///////////////////////////////////////////////////////////////
// TEST CODE //
///////////////////////////////////////////////////////////////

  //test code for setting servo position - should set to 0 -> 90 -> 180 -> 90 -> 0 degrees
  void testServo(Servo& servo){
    //0 degrees
    setServo(servo, 0);
    delay(1000);

    //90 degrees
    setServo(servo, 90);
    delay(1000);
    
    //180 degrees
    setServo(servo, 180);
    delay(1000);

    //0 degrees
    setServo(servo, 90);
    delay(1000);
  }

  //test code for running continuous servo - should go 0 -> 100 -> 0 -> -100 -> 0 percent speed
  void testCRServo(Servo& servo){
    //servo off
    runServo(servo, 0);
    delay(1000);

    //full speed
    runServo(servo, 100);
    delay(1000);

    //servo off
    runServo(servo, 0);
    delay(1000);

    //full speed other direction
    runServo(servo, -100);
    delay(1000);
  }

  //test code for setting the motor to an absolute position - should go 0 -> 100 -> 0 -> -100 -> 0 absolute position
  void testAbsMotor(AccelStepper& stepper){
    //set absolute position to 0 with 100% acceleration
    setAbsoluteMotor(stepper, 100, 0);
    delay(1000);

    //set absolute position to 100 with 100% acceleration
    setAbsoluteMotor(stepper, 100, 100);
    delay(1000);

    //set absolute position to 0 with 100% acceleration
    setAbsoluteMotor(stepper, 100, 0);
    delay(1000);

    //set absolute position to -100 with 100% acceleration
    setAbsoluteMotor(stepper, 100, -100);
    delay(1000);
  }

  //test code for setting the motor to an absolute position - should go 0 -> 100 -> 200 -> 100 -> 0 absolute position, but by setting it with relative steps
  void testRelMotor(AccelStepper& stepper){
    //set relative position to 0 with 100% acceleration
    setRelativeMotor(stepper, 100, 0);
    delay(1000);

    //set relative position to 100 with 100% acceleration (100 abs)
    setRelativeMotor(stepper, 100, 100);
    delay(1000);

    //set relative position to 100 with 100% acceleration (200 abs)
    setRelativeMotor(stepper, 100, 100);
    delay(1000);

    //set relative position to -100 with 100% acceleration (100 abs)
    setRelativeMotor(stepper, 100, -100);
    delay(1000);

    //set relative position to -100 with 100% acceleration (0 abs)
    setRelativeMotor(stepper, 100, -100);
    delay(1000);
  }

  void testSerialConnection(Servo& servo){
    setServo(servo, 90);
    delay(1000);
    if (Serial.available()){
      char val = Serial.read(); // returns -1 if none, else 0..255
      if (val == 'H'){
        setServo(servo, 180);
        Serial.println(F("H was sent"));
        delay(1000);
      }
      else if (val == 'L'){
        setServo(servo, 0);
        Serial.println(F("L was sent"));
        delay(1000);
      }
    }
  }