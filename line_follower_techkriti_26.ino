#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// ─────────────────────────────────────────────
//  MOTOR PINS
// ─────────────────────────────────────────────
#define lf 14
#define lb 27
#define rf 13
#define rb 12

// ─────────────────────────────────────────────
//  SENSOR PINS  (L4=leftmost, R4=rightmost)
// ─────────────────────────────────────────────
#define NUM_SENSORS 8
int L4 = 26, L3 = 25, L2 = 33, L1 = 32;
int R1 = 35, R2 = 34, R3 = 15, R4 = 2;
int sensorPins[NUM_SENSORS] = {L4, L3, L2, L1, R1, R2, R3, R4};

// Weights: L4=-35  L3=-25  L2=-15  L1=-5  R1=+5  R2=+15  R3=+25  R4=+35
const int WEIGHT[NUM_SENSORS] = {-35, -25, -15, -5, 5, 15, 25, 35};

// ─────────────────────────────────────────────
//  PD PARAMETERS  (tune via Bluetooth "C")
// ─────────────────────────────────────────────
float Kp        = 1.5f;
float Kd        = 10.0f;
float lastError = 0.0f;

// ─────────────────────────────────────────────
//  SPEED SETTINGS
// ─────────────────────────────────────────────
int baseSpeed = 200;
int maxSpeed  = 230;
int minSpeed  = 80;

// ─────────────────────────────────────────────
//  ROBOT STATE
// ─────────────────────────────────────────────
bool   running  = false;
String btBuffer = "";

// ─────────────────────────────────────────────
//  MISC
// ─────────────────────────────────────────────
#define BUTTON_PIN 4
#define led        16

// ═════════════════════════════════════════════
//  MOTOR CONTROL
// ═════════════════════════════════════════════
void stopMotors() {
  analogWrite(lf, 0); analogWrite(lb, 0);
  analogWrite(rf, 0); analogWrite(rb, 0);
}

// ═════════════════════════════════════════════
//  PD LINE FOLLOW
// ═════════════════════════════════════════════
void pdLineFollow() {
  long weightedSum = 0;
  int  activeCount = 0;

  for (int i = 0; i < NUM_SENSORS; i++) {
    if (digitalRead(sensorPins[i]) == 0) {  // 0 = on line
      weightedSum += WEIGHT[i];
      activeCount++;
    }
  }

  float error;
  if (activeCount == 0) {
    error = lastError;  // lost line — hold last direction
  } else {
    error = (float)weightedSum / (float)activeCount;
  }

  float derivative = error - lastError;
  float correction = (Kp * error) + (Kd * derivative);
  lastError = error;

  int leftSpeed  = constrain(baseSpeed + (int)correction, minSpeed, maxSpeed);
  int rightSpeed = constrain(baseSpeed - (int)correction, minSpeed, maxSpeed);

  analogWrite(lf, leftSpeed);  analogWrite(lb, 0);
  analogWrite(rf, rightSpeed); analogWrite(rb, 0);
}

// ═════════════════════════════════════════════
//  BLUETOOTH
// ═════════════════════════════════════════════
void processCommand(String cmd);
void enterTuningMode();

void handleBluetooth() {
  while (SerialBT.available()) {
    char c = (char)SerialBT.read();
    if (c == '\n' || c == '\r') {
      btBuffer.trim();
      if (btBuffer.length() > 0) processCommand(btBuffer);
      btBuffer = "";
    } else {
      btBuffer += c;
      if (btBuffer == "S" || btBuffer == "E" || btBuffer == "C") {
        processCommand(btBuffer);
        btBuffer = "";
      }
    }
  }
}

void processCommand(String cmd) {
  cmd.toUpperCase();
  if (cmd == "S") {
    running = true; lastError = 0.0f;
    SerialBT.println(">> STARTED");
  } else if (cmd == "E") {
    running = false; stopMotors();
    SerialBT.println(">> STOPPED");
  } else if (cmd == "C") {
    enterTuningMode();
  }
}

void enterTuningMode() {
  bool wasRunning = running;
  running = false;
  stopMotors();

  SerialBT.println("── PD TUNING ──");
  SerialBT.print("Kp="); SerialBT.print(Kp);
  SerialBT.print("  Kd="); SerialBT.println(Kd);

  // Read Kp
  SerialBT.println("Enter Kp then Enter:");
  String input = ""; unsigned long t = millis() + 15000;
  while (millis() < t) {
    if (SerialBT.available()) {
      char c = (char)SerialBT.read();
      if (c == '\n' || c == '\r') { input.trim(); if (input.length()) break; }
      else input += c;
    }
  }
  if (input.length()) { Kp = input.toFloat(); SerialBT.print("Kp="); SerialBT.println(Kp); }
  else SerialBT.println("Timeout — Kp unchanged.");

  // Read Kd
  SerialBT.println("Enter Kd then Enter:");
  input = ""; t = millis() + 15000;
  while (millis() < t) {
    if (SerialBT.available()) {
      char c = (char)SerialBT.read();
      if (c == '\n' || c == '\r') { input.trim(); if (input.length()) break; }
      else input += c;
    }
  }
  if (input.length()) { Kd = input.toFloat(); SerialBT.print("Kd="); SerialBT.println(Kd); }
  else SerialBT.println("Timeout — Kd unchanged.");

  SerialBT.println("── Done ──  S=Start  E=Stop  C=Tune");
  if (wasRunning) { running = true; lastError = 0.0f; SerialBT.println(">> Resuming..."); }
}

// ═════════════════════════════════════════════
//  SETUP
// ═════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  SerialBT.begin("esp_pid");

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  pinMode(lf, OUTPUT); pinMode(lb, OUTPUT);
  pinMode(rf, OUTPUT); pinMode(rb, OUTPUT);

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  while (digitalRead(BUTTON_PIN) == LOW) { delay(5); }
  digitalWrite(led, HIGH); delay(200); digitalWrite(led, LOW);

  SerialBT.println("Ready. S=Start  E=Stop  C=Tune Kp/Kd");
}

// ═════════════════════════════════════════════
//  MAIN LOOP
// ═════════════════════════════════════════════
void loop() {
  handleBluetooth();
  if (!running) return;
  pdLineFollow();
}
