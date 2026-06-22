#include <SPI.h>
#include <MFRC522.h>
#include <NewPing.h>

// RFID Pins
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

// LED Pins
const int redLED = 8;
const int greenLED = 6;
const int blueLED = 7;

// Sensor Pins
const int pirPin = 2;
const int trigPin = 3; 
const int echoPin = 4; 
const int LDR_PIN = 5;

// Buzzer Pin
const int buzzerPin = A0;

// Ultrasonic Sensor
#define MAX_DISTANCE 200
NewPing sonar(trigPin, echoPin, MAX_DISTANCE);

// System State Variables
bool systemArmed = false;
bool nightMode = false;
bool alarmTriggered = false;
unsigned long lastBuzzerToggle = 0;
unsigned long lastRedLEDToggle = 0;
int initialDistance = 0; 

// Your authorized card UID
byte authorizedUID[] = {0x13, 0xB6, 0x44, 0x29};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(LDR_PIN, INPUT);

  // Add a warm-up delay for the sensors to prevent immediate false alarms
  Serial.println("System starting up... Please wait.");
  delay(5000); // 5 seconds warm-up delay
  
  // ثبت فاصله اولیه با منطق تصحیح شده
  initialDistance = getAverageDistance();
  Serial.print("Initial Distance Calibrated: ");
  Serial.print(initialDistance);
  Serial.println(" cm");
  
  Serial.println("System ready to be activated.");
}

void loop() {
  // Check for RFID card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (checkRFIDCard()) {
      handleCardTap();
      delay(500); 
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

  // Check light level (Digital Photoresistor)
  int lightLevel = digitalRead(LDR_PIN);
  if (lightLevel == HIGH) { // HIGH when dark
    nightMode = true;
    digitalWrite(blueLED, HIGH);
  } 
  else {
    nightMode = false;
    digitalWrite(blueLED, LOW);
  }

  if (systemArmed) {
    // Green LED is solid ON when armed
    digitalWrite(greenLED, HIGH);

    // Check for PIR motion
    if (digitalRead(pirPin) == HIGH) {
      triggerAlarm();
    }

    // پایش فاصله تصحیح شده
    int currentDistance = getAverageDistance();
    int distanceChange = abs(currentDistance - initialDistance);
    
    if (currentDistance > 0 && currentDistance < MAX_DISTANCE && distanceChange >= 15 && systemArmed) { 
      Serial.print("Ultrasonic Triggered! Change detected: ");
      Serial.print(distanceChange);
      Serial.println(" cm");
      triggerAlarm();
    }

    // Alarm logic
    if (alarmTriggered) {
      activateAlarm();
    }
  } 
  else {
    // System is disarmed
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzerPin, LOW);
    alarmTriggered = false;
  }
}

void handleCardTap() {
  if (systemArmed) {
    if (alarmTriggered) {
      // If alarm is on, turn it off but keep system armed
      alarmTriggered = false;
      digitalWrite(redLED, LOW);
      digitalWrite(buzzerPin, LOW);
      Serial.println("Alarm Deactivated. System Remains Armed.");
    } 
    else {
      // If system is armed but no alarm, deactivate it completely
      systemArmed = false;
      Serial.println("System Deactivated.");
    }
  } 
  else {
    // If system is disarmed, activate it
    systemArmed = true;
    initialDistance = getAverageDistance(); // کالیبراسیون مجدد در لحظه قفل شدن
    Serial.println("System Activated.");
  }
}

void triggerAlarm() {
  if (!alarmTriggered) {
    alarmTriggered = true;
    Serial.println("Intrusion detected! ALARM!");
  }
}

bool checkRFIDCard() {
  byte buffer[mfrc522.uid.size];
  for (int i = 0; i < mfrc522.uid.size; i++) {
    buffer[i] = mfrc522.uid.uidByte[i];
  }

  if (memcmp(buffer, authorizedUID, sizeof(authorizedUID)) == 0) {
    Serial.println("Access Granted!");
    return true;
  } 
  else {
    Serial.println("Access Denied!");
    return false;
  }
}

void activateAlarm() {
  // Flash red LED
  if (millis() - lastRedLEDToggle > 250) {
    digitalWrite(redLED, !digitalRead(redLED));
    lastRedLEDToggle = millis();
  }

  // Buzzer pattern
  int buzzDelay = nightMode ? 50 : 100;
  if (millis() - lastBuzzerToggle > buzzDelay) {
    digitalWrite(buzzerPin, !digitalRead(buzzerPin));
    lastBuzzerToggle = millis();
  }
}

// تابع اصلاح شده برای معکوس کردن منطق برعکس سنسور شما
int getAverageDistance() {
  long sum = 0;
  int validSamples = 0;
  
  for (int i = 0; i < 5; i++) {
    int dist = sonar.ping_cm();
    
    if (dist > 2 && dist < MAX_DISTANCE) {
      // تغییر اصلی اینجاست: با کم کردن مقدار از MAX_DISTANCE، جهت عدد کاملاً درست و مستقیم می‌شود
      int correctedDist = dist; 
      sum += correctedDist;
      validSamples++;
    }
    delay(35); 
  }
  
  if (validSamples > 0) {
    return sum / validSamples;
  }
  
  return initialDistance; 
}