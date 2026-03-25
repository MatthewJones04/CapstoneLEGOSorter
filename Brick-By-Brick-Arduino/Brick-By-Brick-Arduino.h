#ifndef BRICK_BY_BRICK_ARDUINO_H
#define BRICK_BY_BRICK_ARDUINO_H

//////////////////////////////////////////////
// DEFINE MOTOR PINS (DIGITAL ONLY)
//////////////////////////////////////////////

// Input Conveyor
#define INPUT_CONVEY_STEP        22
#define INPUT_CONVEY_DIR         23

// Identification Conveyor
#define IDENT_CONVEY_STEP        24
#define IDENT_CONVEY_DIR         25

// Lift Motor
#define LIFT_MOTOR_STEP          26
#define LIFT_MOTOR_DIR           27

// Carousel Motor
#define CAROUSEL_MOTOR_STEP      28
#define CAROUSEL_MOTOR_DIR       29

// V Channel 1
#define V1_IN1                   30
#define V1_IN2                   31
#define V1_ENA                   2    // PWM

// V Channel 2
#define V2_IN1                   32
#define V2_IN2                   33
#define V2_ENA                   3    // PWM

// V Channel 3
#define V3_IN1                   34
#define V3_IN2                   35
#define V3_ENA                   4    // PWM


//////////////////////////////////////////////
// DEFINE SERVO PINS (PWM)
//////////////////////////////////////////////
#define BIN_SIGNAL_0                    2
#define BIN_SIGNAL_1                    3
#define BIN_SIGNAL_2                    4
#define BIN_SIGNAL_3                    5
#define BIN_SIGNAL_4                    6
#define BIN_SIGNAL_5                    7
#define BIN_SIGNAL_6                    8
#define BIN_SIGNAL_7                    9
#define BIN_SIGNAL_8                    10
#define BIN_SIGNAL_9                    11
#define BIN_SIGNAL_10                   12
#define BIN_SIGNAL_11                   13
#define BIN_SIGNAL_12                   44
#define BIN_SIGNAL_13                   45


//////////////////////////////////////////////
// DEFINE SENSORS
//////////////////////////////////////////////
#define LIFT_LIMIT_SWITCH               40


//////////////////////////////////////////////
// DEFINE DEFINITIONS
//////////////////////////////////////////////
#define BIN_OPEN                        90
#define BIN_CLOSED                      0

#endif