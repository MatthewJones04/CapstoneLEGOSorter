#include <AccelStepper.h>
#include <Wire.h>
#include <Servo.h>
#include "Brick-By-Brick-Arduino.h"

//MOTOR DECLARATIONS
AccelStepper upperInConvey1(AccelStepper::DRIVER, UPPER_IN_CONVEY_1_STEP, UPPER_IN_CONVEY_1_DIR);
AccelStepper upperInConvey2(AccelStepper::DRIVER, UPPER_IN_CONVEY_2_STEP, UPPER_IN_CONVEY_2_DIR);
AccelStepper identConvey1(AccelStepper::DRIVER, IDENT_CONVEY_1_STEP, IDENT_CONVEY_1_DIR);
AccelStepper identConvey2(AccelStepper::DRIVER, IDENT_CONVEY_2_STEP, IDENT_CONVEY_2_DIR);
AccelStepper identifyMotor1(AccelStepper::DRIVER, IDENTIFY_MOTOR_1_STEP, IDENTIFY_MOTOR_1_DIR);
AccelStepper outputMotor1(AccelStepper::DRIVER, OUTPUT_MOTOR_1_STEP, OUTPUT_MOTOR_1_DIR);

//FLAG DECLARATIONS
bool vChannel1Active = false;
bool vChannel2Active = false;
bool rampActive = false;

//create angular servos
Servo bins[14];

//create CR servos
Servo inputShakeCR;

bool binActive = false;
unsigned long binStartTime = 0;
int currentBin = -1;


void setup() {

  //set max speed of stepper motors
  upperInConvey1.setMaxSpeed(2000);
  upperInConvey1.setAcceleration(1000);

  upperInConvey2.setMaxSpeed(2000);
  upperInConvey2.setAcceleration(1000);

  identConvey1.setMaxSpeed(2000);
  identConvey1.setAcceleration(1000);

  identConvey2.setMaxSpeed(2000);
  identConvey2.setAcceleration(1000);

  identifyMotor1.setMaxSpeed(1500);
  identifyMotor1.setAcceleration(800);

  outputMotor1.setMaxSpeed(2000);
  outputMotor1.setAcceleration(1000);

  //attach pins to servo
  bins[0].attach(BIN_SIGNAL_0);
  bins[1].attach(BIN_SIGNAL_1);
  bins[2].attach(BIN_SIGNAL_2);
  bins[3].attach(BIN_SIGNAL_3);
  bins[4].attach(BIN_SIGNAL_4);
  bins[5].attach(BIN_SIGNAL_5);
  bins[6].attach(BIN_SIGNAL_6);
  bins[7].attach(BIN_SIGNAL_7);
  bins[8].attach(BIN_SIGNAL_8);
  bins[9].attach(BIN_SIGNAL_9);
  bins[10].attach(BIN_SIGNAL_10);
  bins[11].attach(BIN_SIGNAL_11);
  bins[12].attach(BIN_SIGNAL_12);
  bins[13].attach(BIN_SIGNAL_13);

  //attach pins to CR servo
  inputShakeCR.attach(INPUT_SHAKE_SIGNAL);

  //turn on serial communication for arduino mega
  Serial.begin(115200);
}

void loop() {
  static char cmd[16];
  if(readCommand(cmd)){
    processCommand(cmd);
  }

  // bin timing logic
  if(binActive && millis() - binStartTime >= 250){
    setServo(bins[currentBin], BIN_CLOSED);
    runServo(inputShakeCR, 0);
    setAllMotorSpeeds(0);
    binActive = false;
  }

  if(vChannel2Active){
    upperInConvey1.setSpeed(2000);
    upperInConvey2.setSpeed(2000);
    } else{
      upperInConvey1.setSpeed(0);
      upperInConvey2.setSpeed(0);
    }

  if(vChannel1Active){
    identConvey1.setSpeed(2000);
    identConvey2.setSpeed(2000);
  } else{
    identConvey1.setSpeed(0);
    identConvey2.setSpeed(0);
  }

  if(rampActive){
    identifyMotor1.setSpeed(2000);
  } else{
    identifyMotor1.setSpeed(0);
  }

  // run motors
  upperInConvey1.runSpeed();
  upperInConvey2.runSpeed();
  identConvey1.runSpeed();
  identConvey2.runSpeed();
  identifyMotor1.runSpeed();
  outputMotor1.runSpeed();
}


///////////////////////////////////////////////////////////////
// COMMAND HANDLING CODE //
///////////////////////////////////////////////////////////////

bool readCommand(char *cmd) {
  static uint8_t index = 0;

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      cmd[index] = '\0';
      index = 0;
      return true;
    }

    if (index < 15) {
      cmd[index++] = c;
    } else {
      index = 0; // overflow protection
    }
  }
  return false;
}

void processCommand(char *cmd) {

  switch (cmd[0]) {

    case 'B':
      handleBinCommand(cmd);
      break;

    case 'V':
      handleVChannel(cmd);
      break;

    case 'R':
      handleRamp(cmd);
      break;

    default:
      Serial.println("ERR:UNKNOWN_CMD");
      break;
  }
}

void handleBinCommand(char *cmd) {
  int bin = atoi(cmd + 1);

  if (bin >= 0 && bin <= 13) {
    if (!binActive) {
      handleBin(bin);
    }
  }
}

void handleBin(int binNumber){
  currentBin = binNumber;

  setServo(bins[binNumber], BIN_OPEN);
  runServo(inputShakeCR, 100);
  setAllMotorSpeeds(2000);

  binStartTime = millis();
  binActive = true;
}

void handleVChannel(char *cmd){
  int channel = cmd[1] - '0';
  int state   = cmd[3] - '0';

  bool on = (state == '1' || state == 1);

  switch (channel){
    case 1:
      vChannel1Active = on;
    break;

    case 2:
      vChannel2Active = on;
    break;
  }
}

void handleRamp(char *cmd){
  int state = cmd[3] - '0';

  bool on = (state == '1' || state == 1);

  rampActive = on;

}

///////////////////////////////////////////////////////////////
// MOVEMENT CODE //
///////////////////////////////////////////////////////////////

  //set all motors to the same speed
  void setAllMotorSpeeds(int speed){
    upperInConvey1.setSpeed(speed);
    upperInConvey2.setSpeed(speed);
    identConvey1.setSpeed(speed);
    identConvey2.setSpeed(speed);
    identifyMotor1.setSpeed(speed);
    outputMotor1.setSpeed(speed);
  }

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
