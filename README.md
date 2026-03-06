# Smart Worker Safety Monitoring and Emergency Alert System
[![Ask DeepWiki](https://devin.ai/assets/askdeepwiki.png)](https://deepwiki.com/nesdan40/Smart-Worker-Safety-Monitoring-and-Emergency-Alert-System)

This project is an ESP32-based solution for monitoring the safety of workers in hazardous environments. It integrates multiple sensors to detect falls, harmful gases, and high temperatures. In case of an emergency, the system sends real-time alerts with GPS location data to a supervisor via Telegram and the Blynk mobile application.

## Features

*   **Fall Detection:** Uses an MPU6050 accelerometer to detect sudden impacts indicative of a fall.
*   **Environmental Monitoring:**
    *   Measures ambient temperature and humidity with a DHT11 sensor.
    *   Detects gas leaks or harmful air quality with an MQ2 gas sensor.
*   **GPS Tracking:** Provides the worker's real-time geographical location using a GPS module.
*   **Dual Alert System:**
    *   **Telegram:** Sends instant, detailed notifications including GPS location links.
    *   **Blynk:** Provides a mobile dashboard for monitoring sensor data and viewing alert statuses.
*   **Configurable Thresholds:** Easily adjustable thresholds for fall sensitivity, gas levels, and temperature to suit different working conditions.

## Hardware Requirements

*   **Controller:** ESP32 Development Board
*   **Sensors:**
    *   **Accelerometer/Gyroscope:** Adafruit MPU6050
    *   **Temperature/Humidity:** DHT11
    *   **Gas Sensor:** MQ2
    *   **GPS:** A GPS module compatible with TinyGPS++ (e.g., NEO-6M)
*   **Connectivity:** A Wi-Fi access point

## Software & Services

*   **IDE:** Arduino IDE or PlatformIO
*   **Platform:** Blynk (for mobile dashboard and notifications)
*   **Messaging:** Telegram (for emergency alerts)

### Required Libraries

*   `BlynkSimpleEsp32.h`
*   `WiFi.h`
*   `Wire.h`
*   `Adafruit_MPU6050.h`
*   `Adafruit_Sensor.h`
*   `DHT.h`
*   `TinyGPS++.h`
*   `HTTPClient.h`

## Configuration

Before uploading the sketch, you must configure the credentials and settings in the `WorkerSafety_project.ino` file.

### 1. Blynk Configuration

Update the following definitions with your Blynk project details:

```cpp
#define BLYNK_TEMPLATE_ID "TMPL3_hh1jA6g"
#define BLYNK_TEMPLATE_NAME "Worker Safety"
#define BLYNK_AUTH_TOKEN "eiHEjyS-5FjRplpQu53IsuZVMSGO6116"
```

### 2. Wi-Fi Credentials

Enter your Wi-Fi network's SSID and password:

```cpp
char ssid[] = "Meen";
char pass[] = "12345678";
```

### 3. Telegram Bot

Set your Telegram Bot Token and the recipient's Chat ID:

```cpp
String BOT_TOKEN = "8666052263:AAHsAiq7arVWD_JWQ7aO94u97fpEbG00MNk";
String CHAT_ID = "5306803052";
```

### 4. Alert Thresholds

Adjust the sensitivity of the alerts as needed:

```cpp
#define FALL_THRESHOLD 14     // Higher value for less sensitivity
#define GAS_THRESHOLD 200     // ADC value from the MQ2 sensor
#define TEMP_THRESHOLD 28     // Temperature in Celsius
```
### 5. Pinout

The project uses the following pin connections on the ESP32:

| Component     | Pin Function | ESP32 Pin |
|---------------|--------------|-----------|
| **DHT11**     | DATA         | GPIO 4    |
| **MQ2 Sensor**| A0 (Analog)  | GPIO 34   |
| **MPU6050**   | SDA          | GPIO 21   |
|               | SCL          | GPIO 22   |
| **GPS Module**| RX (ESP32)   | GPIO 16   |
|               | TX (ESP32)   | GPIO 17   |


## How It Works

The ESP32 connects to Wi-Fi and the Blynk service upon startup. It initializes all connected sensors.

A timer function (`sendData`) runs every 100 milliseconds to perform the following actions:
1.  **Read Sensor Data:** It gathers data from the MPU6050 (acceleration), DHT11 (temperature, humidity), and MQ2 (gas level).
2.  **Update Blynk:** The temperature, humidity, and gas values are sent to the Blynk dashboard on virtual pins V0, V1, and V2.
3.  **Check for Alerts:**
    *   **Fall:** Calculates the total acceleration vector. If it exceeds `FALL_THRESHOLD`, it triggers a fall alert. A Telegram message with GPS coordinates and a Google Maps link is sent. A Blynk alert is also triggered. The alert state is automatically reset after 15 seconds.
    *   **Gas & Temperature:** If the `gasValue` or `temperature` exceeds `GAS_THRESHOLD` or `TEMP_THRESHOLD` respectively, the system sends an alert to both Telegram and Blynk.

The GPS module continuously provides location data, which is included in the fall alert messages.

## Installation and Usage

1.  **Hardware Setup:** Connect all sensors to the ESP32 according to the pinout table.
2.  **Software Setup:**
    *   Open the `WorkerSafety_project.ino` file in the Arduino IDE.
    *   Install all the libraries listed under the [Required Libraries](#required-libraries) section.
    *   Update the configuration sections (Blynk, Wi-Fi, Telegram) with your own credentials.
3.  **Deploy:**
    *   Connect the ESP32 to your computer.
    *   Select the correct board and port in the Arduino IDE.
    *   Upload the sketch.
4.  **Monitor:**
    *   Open the Serial Monitor to view debug output and sensor readings.
    *   Use the Blynk app to monitor the worker's status in real-time.
    *   Check your Telegram chat for emergency alert notifications.
