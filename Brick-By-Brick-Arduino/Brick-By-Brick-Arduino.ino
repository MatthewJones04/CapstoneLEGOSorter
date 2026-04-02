#include <AccelStepper.h>
#include <Wire.h>
#include <Servo.h>
#include "Brick-By-Brick-Arduino.h"

//MOTOR DECLARATIONS
AccelStepper inputConvey(AccelStepper::DRIVER, INPUT_CONVEY_STEP, INPUT_CONVEY_DIR);
AccelStepper identConvey(AccelStepper::DRIVER, IDENT_CONVEY_STEP, IDENT_CONVEY_DIR);

AccelStepper liftMotor(AccelStepper::DRIVER, LIFT_MOTOR_STEP, LIFT_MOTOR_DIR);
AccelStepper carouselMotor(AccelStepper::DRIVER, CAROUSEL_MOTOR_STEP, CAROUSEL_MOTOR_DIR);

//FLAG DECLARATIONS
bool vChannel1Active = false;
bool vChannel2Active = false;
bool vChannel3Active = false;

bool inputConveyActive = false;
bool identConveyActive = false;
bool liftActive = false;

//create angular servos
Servo bins[14];

bool binActive = false;
unsigned long binStartTime = 0;
int currentBin = -1;


void setup() {

  //set max speed of stepper motors
  inputConvey.setMaxSpeed(2000);
  inputConvey.setAcceleration(1000);

  identConvey.setMaxSpeed(2000);
  identConvey.setAcceleration(1000);

  liftMotor.setMaxSpeed(1500);
  liftMotor.setAcceleration(800);
  liftMotor.setSpeed(0);
  liftActive = false;

  carouselMotor.setMaxSpeed(2000);
  carouselMotor.setAcceleration(1000);

  pinMode(V1_IN1, OUTPUT);
  pinMode(V1_IN2, OUTPUT);
  pinMode(V1_ENA, OUTPUT);

  pinMode(V2_IN1, OUTPUT);
  pinMode(V2_IN2, OUTPUT);
  pinMode(V2_ENA, OUTPUT);

  pinMode(V3_IN1, OUTPUT);
  pinMode(V3_IN2, OUTPUT);
  pinMode(V3_ENA, OUTPUT);

  pinMode(LIFT_LIMIT_SWITCH, INPUT_PULLUP);

  setVChannel(1, 0);
  setVChannel(2, 0);
  setVChannel(3, 0);

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
    setAllMotorSpeeds(0);
    binActive = false;
  }

  if(vChannel1Active){
    setVChannel(1, 100);
  } else{
    setVChannel(1, 0);
  }

  if(vChannel2Active){
    setVChannel(2, 100);
  } else{
    setVChannel(2, 0);
  }

  if(vChannel3Active){
    setVChannel(3, 100);
  } else{
    setVChannel(3, 0);
  }

  if(liftActive){
    if(digitalRead(LIFT_LIMIT_SWITCH) == LOW){ // triggered
      liftMotor.setSpeed(0);
      liftActive = false;
    } else {
      liftMotor.setSpeed(1000);
    }
  }

  // Input conveyor
  if(inputConveyActive){
    inputConvey.setSpeed(2000);
  } else {
    inputConvey.setSpeed(0);
  }

  // Identifier conveyor
  if(identConveyActive){
    identConvey.setSpeed(2000);
  } else {
    identConvey.setSpeed(0);
  }

  // run stepper motors
  inputConvey.runSpeed();
  identConvey.runSpeed();
  carouselMotor.run();
  if(liftActive){
    liftMotor.runSpeed();
  } 
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

  if (strncmp(cmd, "IN", 2) == 0) {
    handleInputConvey(cmd);
  }
  else if (strncmp(cmd, "ID", 2) == 0) {
    handleIdentConvey(cmd);
  }
  else if (strncmp(cmd, "LI", 2) == 0) {
    handleLift(cmd);
  }
  else if (cmd[0] == 'B') {
    handleCarousel(cmd);
  }
  else if (strlen(cmd) >= 2 && cmd[1] == 'T'){
    handleTrapdoor(cmd);
  }
  else if (cmd[0] == 'V') {
    handleVChannel(cmd);
  }
  else {
    Serial.println("ERR:UNKNOWN_CMD");
  }
}

void handleInputConvey(char *cmd){
  if(strlen(cmd) >= 3){
    inputConveyActive = (cmd[2] == '1');
  }
}

void handleIdentConvey(char *cmd){
  if(strlen(cmd) >= 3){
    identConveyActive = (cmd[2] == '1');
  }
}

void handleLift(char *cmd){
  if(strlen(cmd) >= 3 && cmd[2] == '1'){
    liftActive = true;
    liftMotor.setSpeed(1000);
  }
}

void handleCarousel(char *cmd){
  int bin = atoi(cmd + 1);

  if(bin >= 0 && bin <= 23){
    int angle = map(bin, 0, 23, 0, 360);

    long steps = map(angle, 0, 360, 0, 1600); 
    // adjust 2000 based on your system

    carouselMotor.moveTo(steps);
  }
}

void handleTrapdoor(char *cmd){

  if(strlen(cmd) < 4) return;

  int state = cmd[0] - '0';   // 1 or 0
  int bin   = atoi(cmd + 2);  // 00–13

  if(bin >= 0 && bin <= 13){
    if(state == 1){
      setServo(bins[bin], 90);
    } else {
      setServo(bins[bin], 0);
    }
  }
}

void handleVChannel(char *cmd){

  if(strlen(cmd) < 4) return;

  int channel = cmd[1] - '0';
  int state   = cmd[3] - '0';

  bool on = (state == 1);

  switch(channel){
    case 1: vChannel1Active = on; break;
    case 2: vChannel2Active = on; break;
    case 3: vChannel3Active = on; break;
  }
}


///////////////////////////////////////////////////////////////
// MOVEMENT CODE //
///////////////////////////////////////////////////////////////

  //set all motors to the same speed
  void setAllMotorSpeeds(int speed){
    inputConvey.setSpeed(speed);
    identConvey.setSpeed(speed);
  }

  void setVChannel(int channel, int speedPercent) {

    int pwm = map(abs(speedPercent), 0, 100, 0, 255);

    int in1, in2, ena;

    switch(channel){
      case 1: in1 = V1_IN1; in2 = V1_IN2; ena = V1_ENA; break;
      case 2: in1 = V2_IN1; in2 = V2_IN2; ena = V2_ENA; break;
      case 3: in1 = V3_IN1; in2 = V3_IN2; ena = V3_ENA; break;
      default: return;
    }

    if(speedPercent > 0){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }
    else if(speedPercent < 0){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }
    else{
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }

    analogWrite(ena, pwm);
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

  //set a servo to a desired angle
  void setServo(Servo& servo, int angle){
    servo.write(angle);
  }
