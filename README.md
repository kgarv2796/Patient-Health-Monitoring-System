# 🏥 Wearable Health Monitoring Dashboard (ESP32 IoT)

A modern, responsive, and clinical-grade IoT biosignal telemetry dashboard engineered for an **ESP32-based wearable health monitoring prototype**.

---

## 📋 Table of Contents
1. [System Architecture](#-system-architecture)
2. [Hardware Components & Pinout Table](#-hardware-components--pinout-table)
3. [Wiring Schematic](#-wiring-schematic)
4. [Dashboard Features](#-dashboard-features)
5. [Firmware Setup (Arduino IDE)](#-firmware-setup-arduino-ide)
6. [Telemetry JSON Protocol](#-telemetry-json-protocol)
7. [Running the Dashboard](#-running-the-dashboard)
8. [Student Presentation & Demo Guide](#-student-presentation--demo-guide)
9. [Project Disclaimer](#-project-disclaimer)

---

## 🏗️ System Architecture

```
 +---------------------------------------------------------+
 |                 ESP32 Wearable Device                   |
 |                                                         |
 |  +-------------------+       +-----------------------+  |
 |  | MAX30100 Sensor   |       | DS18B20 Temp Sensor   |  |
 |  | (Heart Rate/SpO₂) |       | (Contact Body Temp)   |  |
 |  +---------+---------+       +-----------+-----------+  |
 |            | (I2C)                       | (OneWire)    |
 |            +-----------> ESP32 <---------+              |
 |                           |                             |
 |            +--------------+--------------+              |
 |            | (I2C)        | (SPI)        |              |
 |            v              v              v              |
 |       +---------+    +----------+    +-------+          |
 |       | SSD1306 |    | MicroSD  |    | Li-Po |          |
 |       |  OLED   |    | Logging  |    | Batt  |          |
 |       +---------+    +----------+    +-------+          |
 |                           |                             |
 |           +---------------+---------------+             |
 |           | Wi-Fi (WebSocket) / USB Serial|             |
 +-----------+-------------------------------+-------------+
                             |
                             v
 +---------------------------------------------------------+
 |             Responsive Web Dashboard                    |
 |                                                         |
 |  * Real-time Heart Rate, SpO₂, Temperature Cards        |
 |  * Interactive Canvas 2D Charts (Zoom, Hover, Bands)    |
 |  * Live Photoplethysmogram (PPG) Oscilloscope Canvas    |
 |  * Virtual 128x64 SSD1306 OLED Mirror Screen            |
 |  * Configurable Warning & Critical Threshold Engine     |
 |  * Audio Chimes (Web Audio API Synthesizer)             |
 |  * 24-Hour Historical Analytics & Date Picker           |
 |  * Data Recording, RFC 4180 CSV Export, JSON Download   |
 |  * Printable Clinical Summary Health Report             |
 |  * Dark Navy (#0B1220) & Clean Light Medical Themes     |
 +---------------------------------------------------------+
```

---

## 🔌 Hardware Components & Pinout Table

| Peripheral | Component | Protocol / Interface | ESP32 GPIO Pin | Details |
| :--- | :--- | :--- | :--- | :--- |
| **Microcontroller** | ESP32-WROOM-32 | System Core | &mdash; | Dual-Core 240 MHz, 520 KB SRAM |
| **Pulse Oximeter** | MAX30100 | I2C (Address `0x57`) | **GPIO 21 (SDA)**<br>**GPIO 22 (SCL)** | 3.3V VCC, Red & IR Photodiodes |
| **Body Temperature** | DS18B20 Waterproof | 1-Wire | **GPIO 4 (Data)** | Requires 4.7 kΩ pull-up to 3.3V |
| **Wrist Display** | SSD1306 0.96" OLED | I2C (Address `0x3C`) | **GPIO 21 (SDA)**<br>**GPIO 22 (SCL)** | 128 &times; 64 monochrome pixels |
| **Data Logging** | MicroSD Card Module | SPI | **GPIO 5 (CS)**<br>**GPIO 18 (SCK)**<br>**GPIO 19 (MISO)**<br>**GPIO 23 (MOSI)** | FAT32 formatted, 3.3V logic |
| **Battery Gauge** | 3.7V Li-Po Cell | Analog ADC | **GPIO 34 (ADC1_CH6)** | 2 &times; 100 kΩ voltage divider |
| **Heartbeat LED** | Onboard SMD LED | Digital Output | **GPIO 2** | Flashes on each detected systolic beat |

---

## ⚡ Wiring Schematic

```
          ESP32-WROOM-32
         +--------------+
         |          3V3 |--------+---------+---------+---------+ (3.3V Rail)
         |          GND |----+---|----+---|----+---|----+---| (GND Rail)
         |              |    |   |    |   |    |   |    |   |
         |       GPIO21 |----+---|----+---|----+---|    |   |
         |       GPIO22 |----+---|----+---|    |   |    |   |
         |       GPIO04 |----+---|    |   |    |   |    |   |
         |       GPIO05 |----+---|----+---|----+---|----+---|
         |       GPIO18 |----+---|----+---|----+---|
         |       GPIO19 |----+---|----+---|
         |       GPIO23 |----+---|
         +--------------+    |   |    |   |    |   |    |   |
                             |   |    |   |    |   |    |   |
MAX30100:                    |   |    |   |    |   |    |   |
  SDA -----------------------+   |    |   |    |   |    |   |
  SCL ---------------------------+    |   |    |   |    |   |
                                      |   |    |   |    |   |
SSD1306 OLED:                         |   |    |   |    |   |
  SDA --------------------------------+   |    |   |    |   |
  SCL ------------------------------------+    |   |    |   |
                                               |   |    |   |
DS18B20 Temp Sensor:                           |   |    |   |
  DATA ----------------------------------------+   |    |   |
  (Attach 4.7kΩ between DATA and 3.3V)             |    |   |
                                                   |    |   |
MicroSD Module:                                    |    |   |
  CS   --------------------------------------------+    |   |
  SCK  -------------------------------------------------+   |
  MISO -----------------------------------------------------+
  MOSI ---------------------------------------------------------
```

---

## 💻 Dashboard Features

### 1. Header & Live Telemetry
- **Device Status:** Online / Offline with animated beacon ring indicator.
- **ESP32 Connection:** Real-time Wi-Fi signal strength (`-62 dBm`), battery percentage (`88%`, `4.08V`), and relative sync timer (`Just now`, `3s ago`).
- **Telemetry Modes:**
  - 🎮 **Real-Time Simulator:** Fully functional offline simulation with realistic physiological noise and scenarios.
  - 🌐 **ESP32 WebSocket:** Direct bi-directional streaming over local Wi-Fi network.
  - 🔌 **Web Serial API:** 1-click direct connection to ESP32 over USB cable (Chrome / Edge).
  - 📡 **REST HTTP Polling:** Periodically polls `http://<ESP32_IP>/api/data`.

### 2. Main Health Cards
- **❤️ Heart Rate:** Current BPM, status pill (`NORMAL`, `TACHYCARDIA`, `BRADYCARDIA`), animated beating heart synchronized to exact pulse rate `(60 / BPM)s`, Min, Max, and Average.
- **🫁 SpO₂ (Oxygen Saturation):** Current %, circular SVG percentage progress gauge, Perfusion Index (PI: 4.2%), Min and Max stats.
- **🌡️ Body Temperature:** Current reading in °C with 1-click toggle to °F, visual thermometer gauge, fever warning thresholds, Min, Max, and Average stats.

### 3. Advanced Live Biosignal Oscilloscope
- **Live PPG Pulse Wave Canvas:** High-resolution photoplethysmogram at 50 Hz displaying systolic peak, dicrotic notch, and diastolic runoff with sweep bar and gain control (1x, 1.5x, 2x).
- **SSD1306 OLED Mirror Screen:** Live 128 &times; 64 pixel screen identical to the wearable wrist display.

### 4. Interactive Real-Time Graphs
- Separate charts for **Heart Rate vs. Time**, **SpO₂ vs. Time**, and **Temperature vs. Time**.
- Selectable time windows: **1m**, **5m**, **30m**, **1h**, and **24h**.
- Dynamic zooming (`+`, `-`, `Reset`) and wheel zooming.
- Interactive crosshairs and hover tooltips showing exact timestamp, value, and target baseline status.
- Shaded target health zones.

### 5. Health Status & Clinical Alert System
- Prominent status indicator: `NORMAL` (Green), `WARNING` (Amber), `CRITICAL ALERT` / `CHECK SENSOR` (Red).
- **Automated Critical Alert Warning Pop-up Modal:** Displays a centered, red-accented pulsing modal with abnormal vital readings, timestamp, reason, and Acknowledge button whenever entering a critical status.
- Audible medical chime alerts synthesized in real-time via Web Audio API.
- Filter, acknowledge, and dismiss alerts.
- Configurable threshold limits in the settings panel.

### 6. Historical Analytics & Data Logger
- 24-hour log explorer with date picker and morning/afternoon/evening time range filtering.
- One-click **Export CSV** (RFC 4180 standard), **Download JSON**, and **Print Clinical Summary Report**.
- MicroSD card storage capacity meter.

---

## 🚀 Firmware Setup (Arduino IDE)

1. Open **Arduino IDE** (v2.0 or newer).
2. Install the ESP32 Board Package:
   - Go to `File` &rarr; `Preferences`.
   - Add URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Go to `Tools` &rarr; `Board Manager` and search for **esp32** by Espressif Systems.
3. Install the required libraries from `Tools` &rarr; `Manage Libraries`:
   - `MAX30100lib` by OXullo Intersecans
   - `DallasTemperature` by Miles Burton
   - `OneWire` by Paul Stoffregen
   - `Adafruit SSD1306` & `Adafruit GFX Library`
   - `WebSockets` by Markus Sattler
   - `ArduinoJson` by Benoit Blanchon
4. Open `esp32_firmware/esp32_wearable_health_monitor.ino`.
5. Update your Wi-Fi credentials:
   ```cpp
   const char* WIFI_SSID     = "Your_WiFi_SSID";
   const char* WIFI_PASSWORD = "Your_WiFi_PASSWORD";
   ```
6. Select board **ESP32 Dev Module**, choose the correct COM port, and click **Upload**.

---

## 📡 Telemetry JSON Protocol

The ESP32 broadcasts a JSON packet every second over WebSocket, HTTP, and Serial:

```json
{
  "timestamp": 1696150240000,
  "hr": 75,
  "spo2": 98.4,
  "temp": 36.6,
  "battery": 88.0,
  "batteryVoltage": 4.08,
  "fingerDetected": true,
  "rssi": -62,
  "sensors": {
    "max30100": true,
    "tempSensor": true,
    "oled": true,
    "esp32": true,
    "wifi": true,
    "sdCard": true
  }
}
```

---

## 🌐 Running the Dashboard

### Option A: Local Browser (Zero Setup)
Simply double-click `index.html` to open it in Chrome, Edge, Safari, or Firefox. The dashboard will immediately start in **Simulator Demo Mode**.

### Option B: Direct USB Web Serial API
1. Connect the ESP32 to your computer using a micro-USB / USB-C cable.
2. In the dashboard top bar, select **Source: Web Serial (USB Cable)**.
3. Click **Connect USB** and select the ESP32 COM port.
4. Live telemetry will stream instantly without any Wi-Fi needed!

### Option C: Wi-Fi WebSocket Stream
1. Ensure your PC and the ESP32 are connected to the same Wi-Fi network.
2. Check the ESP32 IP address in the Arduino Serial Monitor (e.g. `192.168.1.145`).
3. In the dashboard top bar, click **Settings (⚙️)**, enter the IP address, and click **Save Settings**.
4. Select **Source: ESP32 WebSocket (Wi-Fi)**.

---

## 🎓 Student Presentation & Demo Guide

Use the built-in **Simulate Condition** selector during project demonstrations or oral viva presentations:

1. **Normal Baseline:** Demonstrates green indicators, 75 BPM, 98% SpO₂, and 36.6°C.
2. **Post-Exercise / Tachycardia:** Heart rate accelerates to 138 BPM, triggering an amber warning and speeding up the heartbeat animation.
3. **Hypoxia / Low Oxygen:** SpO₂ drops to 88%, triggering a high-priority red alert and audio chime.
4. **High Fever / Hyperthermia:** Temperature rises to 38.9°C, activating the thermometer gauge fever zone.
5. **Sensor Disconnect:** Simulates lifting the finger off the MAX30100 sensor, triggering `NO FINGER` status and flatline PPG waveform.

---

## ⚠️ Project Disclaimer

> [!IMPORTANT]
> **Educational & Prototype Notice:** This dashboard and embedded firmware are developed strictly for **academic, experimental, and educational demonstration purposes**. Sensor measurements are experimental readings from hobbyist/prototype sensors (MAX30100 and DS18B20). They are not intended for clinical medical diagnosis, patient life support, treatment decisions, or critical medical monitoring.
