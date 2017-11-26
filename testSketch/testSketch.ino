
///////////////
// Libraries //
///////////////

#include <Wire.h>
#include "Adafruit_MotorShield.h"
//#include "utility/Adafruit_MS_PWMServoDriver.h"


/////////////////////
// Pin Definitions //
/////////////////////

// Microswitches
#define MICROSWITCH_0 4
#define MICROSWITCH_1 5
#define MICROSWITCH_2 6
#define MICROSWITCH_3 7

// Ultrasonic Rangefinders
#define RANGEFINDER_0 A8
#define RANGEFINDER_1 A9
#define RANGEFINDER_2 A10
#define RANGEFINDER_3 A11
#define RANGEFINDER_4 A12

// Rangefinder0 RX pin
#define RANGEFINDER_0_RX 24

// Start Button
#define START_BUTTON 2


/////////////////////
// Motor Variables //
/////////////////////

// Time Stamps
volatile int prevTime = 0;
volatile int curTime = 0;

// Encoder Counts
volatile int motor0_encoder = 0;
volatile int motor1_encoder = 0;
volatile int motor2_encoder = 0;
volatile int motor3_encoder = 0;

// Velocity Averages
double motor0_encoderVelocities[5] = {0, 0, 0, 0, 0};
double motor1_encoderVelocities[5] = {0, 0, 0, 0, 0};
double motor2_encoderVelocities[5] = {0, 0, 0, 0, 0};
double motor3_encoderVelocities[5] = {0, 0, 0, 0, 0};

// Motor Velocities (mm/sec)
double motor0_velocity = 0;
double motor1_velocity = 0;
double motor2_velocity = 0;
double motor3_velocity = 0;

// Command Velocities (mm/sec)
uint8_t motor0_commandVelocity = 0;
uint8_t motor1_commandVelocity = 0;
uint8_t motor2_commandVelocity = 0;
uint8_t motor3_commandVelocity = 0;

int locations[3] = {0, 0, 0};


////////////////////////////
// Motor Shield Variables //
////////////////////////////

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor0 = AFMS.getMotor(1);
Adafruit_DCMotor *motor1 = AFMS.getMotor(2);
Adafruit_DCMotor *motor2 = AFMS.getMotor(3);
Adafruit_DCMotor *motor3 = AFMS.getMotor(4);

// PID if necessary
// PID FL_PID(&vel, &pwm, &cmd_vel, Kp, Ki, Kd, DIRECT);



void setup() {
    Serial.begin(9600);
    
    // Initialize Microswitch Pins
    pinMode(MICROSWITCH_0, INPUT_PULLUP);
    pinMode(MICROSWITCH_1, INPUT_PULLUP);
    pinMode(MICROSWITCH_2, INPUT_PULLUP);
    pinMode(MICROSWITCH_3, INPUT_PULLUP);
    pinMode(START_BUTTON, INPUT_PULLUP);
    
    // Initialize MotorShield and Motors
    AFMS.begin();
    motor0->setSpeed(0);
    motor1->setSpeed(0);
    motor2->setSpeed(0);
    motor3->setSpeed(0);    
    motor0->run(FORWARD);
    motor1->run(FORWARD);
    motor2->run(FORWARD);
    motor3->run(FORWARD);
    motor0->run(RELEASE);
    motor1->run(RELEASE);
    motor2->run(RELEASE);
    motor3->run(RELEASE);

    pinMode(RANGEFINDER_0_RX, OUTPUT);
    
    digitalWrite(RANGEFINDER_0_RX, 1);
    delay(35);
    digitalWrite(RANGEFINDER_0_RX, 0);
    
    delay(300);
}

void loop() {
    moveBackward(255);

    while(!digitalRead(MICROSWITCH_2) || !digitalRead(MICROSWITCH_3)) {
        if(digitalRead(MICROSWITCH_2)) {
            turnLeft(127);
        } else if(digitalRead(MICROSWITCH_3)) {
            turnRight(127);
        } else {
            moveBackward(255);
        }
    }

    // Probably isn't necessary
    stopRobot();
    
    // Move left towards Stage A    
    while((analogRead(RANGEFINDER_1) - 3) / 2 + 3 < 33) {
        if(!digitalRead(MICROSWITCH_3)) {
            slideBackLeft(255);
        } else {
            moveLeft(255);
        }
        
        digitalWrite(RANGEFINDER_0_RX, 1);
        delay(35);
        digitalWrite(RANGEFINDER_0_RX, 0);
        
        delay(300);
    }

    // Slow down when ~5 in away
    while((analogRead(RANGEFINDER_1) - 3) / 2 + 3 < 38) {
        if(!digitalRead(MICROSWITCH_2) || !digitalRead(MICROSWITCH_3)) {
            slideBackLeft(127);
        } else {
            moveLeft(127);
        }
        
        digitalWrite(RANGEFINDER_0_RX, 1);
        delay(35);
        digitalWrite(RANGEFINDER_0_RX, 0);
        delay(300);
    }
    
    moveBackward(255);
    while(!digitalRead(MICROSWITCH_2) || !digitalRead(MICROSWITCH_3)) {
        if(digitalRead(MICROSWITCH_2)) {
            turnLeft(127);
        } else if(digitalRead(MICROSWITCH_3)) {
            turnRight(127);
        } else {
            moveBackward(255);
        }
    }
    
    stopRobot();
    while(1) {
        Serial.println((analogRead(RANGEFINDER_1) - 3) / 2 + 3);
        
        digitalWrite(RANGEFINDER_0_RX, 1);
        delay(35);
        digitalWrite(RANGEFINDER_0_RX, 0);
        
        delay(250);
    }
}


void decodeLED() {
    // FIXME: Returns a random number for now
    for(int i = 0; i < 3; i++) {
        locations[i] = random(0, 2);
    }
    Serial.print("Locations: ");
    Serial.print(locations[0]);
    Serial.print(", ");
    Serial.print(locations[1]);
    Serial.print(", ");
    Serial.println(locations[2]);
}


////////////////////
// Command Motors //
////////////////////

void commandMotors() {
    motor0->setSpeed(motor0_commandVelocity);
    motor1->setSpeed(motor1_commandVelocity);
    motor2->setSpeed(motor2_commandVelocity);
    motor3->setSpeed(motor3_commandVelocity);
}


////////////////////////
// Move Robot Forward //
////////////////////////

void moveForward(int velocity) {
    motor0_commandVelocity = velocity;
    motor1_commandVelocity = velocity;
    motor2_commandVelocity = velocity;
    motor3_commandVelocity = velocity;
    motor0->run(FORWARD);
    motor1->run(FORWARD);
    motor2->run(FORWARD);
    motor3->run(FORWARD);

    commandMotors();
}


/////////////////////////
// Move Robot Backward //
/////////////////////////

void moveBackward(int velocity) {
    motor0_commandVelocity = velocity;
    motor1_commandVelocity = velocity;
    motor2_commandVelocity = velocity;
    motor3_commandVelocity = velocity;
    motor0->run(BACKWARD);
    motor1->run(BACKWARD);
    motor2->run(BACKWARD);
    motor3->run(BACKWARD);

    commandMotors();
}


/////////////////////
// Move Robot Left //
/////////////////////

void moveLeft(int velocity) {
    motor0_commandVelocity = velocity;
    motor1_commandVelocity = velocity;
    motor2_commandVelocity = velocity;
    motor3_commandVelocity = velocity;
    motor0->run(BACKWARD);
    motor1->run(FORWARD);
    motor2->run(BACKWARD);
    motor3->run(FORWARD);

    commandMotors();
}


//////////////////////
// Move Robot Right //
//////////////////////

void moveRight(int velocity) {
    motor0_commandVelocity = velocity;
    motor1_commandVelocity = velocity;
    motor2_commandVelocity = velocity;
    motor3_commandVelocity = velocity;
    motor0->run(FORWARD);
    motor1->run(BACKWARD);
    motor2->run(FORWARD);
    motor3->run(BACKWARD);

    commandMotors();
}


/////////////////////
// Turn Robot Left //
/////////////////////

void turnLeft(int velocity) {
    motor1_commandVelocity = velocity;
    motor2_commandVelocity = velocity;
    motor1->run(FORWARD);
    motor2->run(FORWARD);

    commandMotors();
}


//////////////////////
// Turn Robot Right //
//////////////////////

void turnRight(int velocity) {
    motor0_commandVelocity = velocity;
    motor3_commandVelocity = velocity;
    motor0->run(FORWARD);
    motor3->run(FORWARD);

    commandMotors();
}


/////////////////////
// Move Robot Left //
/////////////////////

void slideBackLeft(int velocity) {
    motor0_commandVelocity = velocity;
    motor1_commandVelocity = velocity / 2;
    motor2_commandVelocity = velocity;
    motor3_commandVelocity = velocity / 2;
    motor0->run(BACKWARD);
    motor1->run(FORWARD);
    motor2->run(BACKWARD);
    motor3->run(FORWARD);

    commandMotors();
}


////////////////
// Stop Robot //
////////////////

void stopRobot() {
    motor0_commandVelocity = 0;
    motor1_commandVelocity = 0;
    motor2_commandVelocity = 0;
    motor3_commandVelocity = 0;
    motor0->run(RELEASE);
    motor1->run(RELEASE);
    motor2->run(RELEASE);
    motor3->run(RELEASE);

    commandMotors();
}

