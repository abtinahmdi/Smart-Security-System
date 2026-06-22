## Smart RFID-Based Security System with Multi-Sensor Integration
This repository features a comprehensive, Arduino-based Smart Security and Intrusion Detection System. Designed to safeguard a designated space, the system integrates multiple hardware sensors with real-time feedback mechanisms, featuring dynamic adjustments based on ambient environment constraints.

##  Key Features
* Secure Access Control: Utilizes an MFRC522 RFID module for robust system activation (arming) and deactivation (disarming) via pre-authorized key fobs or cards.

* Dual-Layer Motion Detection: * PIR Motion Sensor: Scans for any human movement within its detection cone.

* HC-SR04 Ultrasonic Sensor: Implements advanced signal filtering using a 5-sample averaging algorithm to eliminate jitter and detect real physical distance fluctuations (e.g., a door opening).

* Context-Aware Environmental Adaptation: Features a Digital Photoresistor (LDR) that tracks ambient light levels to toggle between Day Mode and Night Mode.

* Adaptive Audio-Visual Alarm: When an intrusion is triggered, the system shifts outputs based on the time of day:

* Day Mode: Discharges a milder buzzer frequency and longer visual strobe intervals.

* Night Mode: Emits a piercing, high-frequency buzzer tone combined with tight LED flashing intervals for elevated urgency.

## Hardware Stack
* Microcontroller: Arduino Uno / Nano (AVR-based)

* Access: MFRC522 RFID Module (SPI)

* Sensing: HC-SR04 Ultrasonic Sensor, PIR Motion Sensor, Digital LDR Module

* Outputs: Active Buzzer, High-Brightness Red/Green/Blue LEDs

## Repository Structure

media/ → Photos and a short video on the demonstration of this project.

src/ → Arduino/C source code