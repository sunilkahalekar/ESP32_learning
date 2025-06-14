#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

// === CONFIGURATION ===

// Motor driver pins (L298N - DC Gear Motors)
const int enA = 27; // Right side motor (ENA -> PWM)
const int in1 = 26;
const int in2 = 25;
const int enB = 5;  // Left side motor (ENB -> PWM)
const int in3 = 19;
const int in4 = 18;

int motorSpeed = 180;
bool motorsEnabled = true;

// WiFi AP credentials
const char* ssid = "Robot_Sun";
const char* password = "12345678";

WebServer server(80);

// === MOTOR CONTROL FUNCTIONS ===
void moveForward() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  Serial.println("Moving Forward...");
}

void moveBackward() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  Serial.println("Moving Backward...");
}

void turnLeft() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  Serial.println("Turning Left...");
}

void turnRight() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  Serial.println("Turning Right...");
}

void stopMotors() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  Serial.println("Motors Stopped.");
}

void disableMotors() {
  stopMotors();
  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  Serial.println("Motors Disabled.");
}

// === WEB PAGE ===
void handleRoot() {
  String page = R"rawliteral(
    <!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: sans-serif; text-align: center; background: #f5f5f5; }
      h1 { color: #333; }
      .btn {
        display: inline-block; margin: 8px; padding: 14px 22px; font-size: 16px;
        color: #fff; background: #007BFF; border: none; border-radius: 5px; text-decoration: none;
      }
      .btn:active { background: #0056b3; }
      .danger { background: #dc3545; }
      .grid { display: flex; flex-wrap: wrap; justify-content: center; }
      @media (max-width: 600px) { .btn { width: 80%; margin: 8px auto; } }
    </style>
    </head><body>
      <h1>Robot Controller</h1>
      <div class="grid">
        <a href="/forward" class="btn">Forward</a><br>
        <a href="/left" class="btn">Left</a>
        <a href="/stop" class="btn danger">Stop</a>
        <a href="/right" class="btn">Right Robot</a><br>
        <a href="/back" class="btn">Backward</a>
      </div>
      <h3>Motors: )rawliteral";
  page += (motorsEnabled) ? "Enabled" : "Disabled";
  page += R"rawliteral(</h3>
      <div class="grid">
        <a href="/enable" class="btn" style="background:#28a745;">Enable</a>
        <a href="/disable" class="btn danger">Disable</a>
      </div>
    </body></html>
  )rawliteral";

  server.send(200, "text/html", page);
}


// === SETUP ===
void setup() {
  Serial.begin(115200);

  // Setup motor pins
  pinMode(enA, OUTPUT); pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT); pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);
  disableMotors();

  // Setup WiFi AP
  WiFi.softAP(ssid, password);
  Serial.println("AP IP address: " + WiFi.softAPIP().toString());

  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/enable", [](){ motorsEnabled = true; handleRoot(); });
  server.on("/disable", [](){ motorsEnabled = false; disableMotors(); handleRoot(); });
  server.on("/forward", [](){ if(motorsEnabled) moveForward(); handleRoot(); });
  server.on("/back", [](){ if(motorsEnabled) moveBackward(); handleRoot(); });
  server.on("/left", [](){ if(motorsEnabled) turnLeft(); handleRoot(); });
  server.on("/right", [](){ if(motorsEnabled) turnRight(); handleRoot(); });
  server.on("/stop", [](){ if(motorsEnabled) stopMotors(); handleRoot(); });

  server.begin();
  Serial.println("Web Server started.");
}

// === LOOP ===
void loop() {
  server.handleClient();
}



