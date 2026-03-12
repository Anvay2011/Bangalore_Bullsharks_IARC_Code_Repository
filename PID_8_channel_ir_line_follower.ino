#include <Arduino.h>

#define NUM_SENSORS 8
#define LEFT_MOTOR_FORWARD 5
#define LEFT_MOTOR_BACKWARD 6
#define RIGHT_MOTOR_FORWARD 9
#define RIGHT_MOTOR_BACKWARD 10
#define BASE_SPEED 100 // Adjust speed as needed
#define Kp 2.0  // Proportional gain
#define Ki 0.0001  // Integral gain
#define Kd 1.0  // Derivative gain

int sensorPins[NUM_SENSORS] = {2, 3, 4, 5, 6, 7, 8, 9};
int sensorValues[NUM_SENSORS];
int lastError = 0;
int integral = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LEFT_MOTOR_FORWARD, OUTPUT);
    pinMode(LEFT_MOTOR_BACKWARD, OUTPUT);
    pinMode(RIGHT_MOTOR_FORWARD, OUTPUT);
    pinMode(RIGHT_MOTOR_BACKWARD, OUTPUT);
    for (int i = 0; i < NUM_SENSORS; i++) {
        pinMode(sensorPins[i], INPUT);
    }
}

int readSensors() {
    int weightedSum = 0;
    int sum = 0;
    for (int i = 0; i < NUM_SENSORS; i++) {
        sensorValues[i] = digitalRead(sensorPins[i]);
        weightedSum += sensorValues[i] * i * 100;
        sum += sensorValues[i];
    }
    return (sum == 0) ? (lastError * 100) : (weightedSum / sum - 350); // 350 centers the error around 0
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {
    leftSpeed = constrain(leftSpeed, -255, 255);
    rightSpeed = constrain(rightSpeed, -255, 255);
    
    if (leftSpeed > 0) {
        analogWrite(LEFT_MOTOR_FORWARD, leftSpeed);
        analogWrite(LEFT_MOTOR_BACKWARD, 0);
    } else {
        analogWrite(LEFT_MOTOR_FORWARD, 0);
        analogWrite(LEFT_MOTOR_BACKWARD, -leftSpeed);
    }
    
    if (rightSpeed > 0) {
        analogWrite(RIGHT_MOTOR_FORWARD, rightSpeed);
        analogWrite(RIGHT_MOTOR_BACKWARD, 0);
    } else {
        analogWrite(RIGHT_MOTOR_FORWARD, 0);
        analogWrite(RIGHT_MOTOR_BACKWARD, -rightSpeed);
    }
}

void loop() {
    int position = readSensors();
    int error = position;
    integral += error;
    int derivative = error - lastError;
    int correction = (Kp * error) + (Ki * integral) + (Kd * derivative);
    lastError = error;
    
    int leftSpeed = BASE_SPEED - correction;
    int rightSpeed = BASE_SPEED + correction;
    
    setMotorSpeed(leftSpeed, rightSpeed);
}