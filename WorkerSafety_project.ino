#define BLYNK_TEMPLATE_ID "TMPL3_hh1jA6g"
#define BLYNK_TEMPLATE_NAME "Worker Safety"
#define BLYNK_AUTH_TOKEN "eiHEjyS-5FjRplpQu53IsuZVMSGO6116"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// WiFi
char ssid[] = "Meen";
char pass[] = "12345678";

// Sensors
Adafruit_MPU6050 mpu;

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MQ2_PIN 34

// GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// Telegram
String BOT_TOKEN = "8666052263:AAHsAiq7arVWD_JWQ7aO94u97fpEbG00MNk";
String CHAT_ID = "5306803052";

// Thresholds
#define FALL_THRESHOLD 14
#define GAS_THRESHOLD 200
#define TEMP_THRESHOLD 28

// State variables
bool fallAlertActive = false;
bool gasAlertActive = false;
bool tempAlertActive = false;

unsigned long fallTime = 0;
const int ALERT_DURATION = 15000;

BlynkTimer timer;

// ================= TELEGRAM FUNCTION =================
void sendTelegram(String message) {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;

  message.replace(" ", "%20");
  message.replace("\n", "%0A");

  String url = "https://api.telegram.org/bot" + BOT_TOKEN +
               "/sendMessage?chat_id=" + CHAT_ID +
               "&text=" + message;

  Serial.println("Sending Telegram...");
  Serial.println(url);

  https.begin(client, url);
  int httpCode = https.GET();

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  https.end();
}

// ================= MAIN FUNCTION =================
void sendData() {

  // MPU
  sensors_event_t a, g, temp_mpu;
  mpu.getEvent(&a, &g, &temp_mpu);

  float acc_total = sqrt(
    a.acceleration.x * a.acceleration.x +
    a.acceleration.y * a.acceleration.y +
    a.acceleration.z * a.acceleration.z
  );

  // Other sensors
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gasValue = analogRead(MQ2_PIN);

  // Send to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, gasValue);

  Serial.print("ACC: ");
  Serial.println(acc_total);

  // ================= FALL =================
  if (acc_total > FALL_THRESHOLD && !fallAlertActive) {

    Serial.println("🚨 FALL DETECTED");

    fallAlertActive = true;
    fallTime = millis();

    Blynk.virtualWrite(V3, 1);

    String message = "🚨 FALL DETECTED\n\n";
    message += "🌡 Temp: " + String(temperature, 1) + " °C\n";
    message += "💧 Humidity: " + String(humidity, 0) + " %\n";
    message += "🔥 Gas: " + String(gasValue) + "\n\n";

    if (gps.location.isValid()) {
      float lat = gps.location.lat();
      float lng = gps.location.lng();

      String link = "https://maps.google.com/?q=" +
                    String(lat, 6) + "," + String(lng, 6);

      message += "📍 Location:\n" + link;
      Blynk.virtualWrite(V6, link);
    } else {
      message += "📍 Location: GPS unavailable";
      Blynk.virtualWrite(V6, "GPS unavailable");
    }

    sendTelegram(message);
    Blynk.logEvent("fall_alert", "Fall detected!");
  }

  // Auto reset fall
  if (fallAlertActive && (millis() - fallTime > ALERT_DURATION)) {
    fallAlertActive = false;
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V6, "Normal");
    Serial.println("⏱ Fall Reset");
  }

  if (fallAlertActive) {
    Blynk.virtualWrite(V3, 1);
  }

  // ================= GAS =================
  if (gasValue > GAS_THRESHOLD && !gasAlertActive) {

    gasAlertActive = true;
    Blynk.virtualWrite(V4, 1);

    Serial.println("🔥 GAS ALERT");

    sendTelegram("🔥 GAS ALERT\nGas level high!");
    Blynk.logEvent("gas_alert", "Gas level exceeded!");
  }
  else if (gasValue <= GAS_THRESHOLD && gasAlertActive) {

    gasAlertActive = false;
    Blynk.virtualWrite(V4, 0);

    Serial.println("Gas normal");
  }

  // ================= TEMP =================
  if (temperature > TEMP_THRESHOLD && !tempAlertActive) {

    tempAlertActive = true;
    Blynk.virtualWrite(V5, 1);

    Serial.println("🌡 TEMP ALERT");

    sendTelegram("🌡 HIGH TEMP ALERT\nTemperature exceeded!");
    Blynk.logEvent("temp_alert", "High temperature!");
  }
  else if (temperature <= TEMP_THRESHOLD && tempAlertActive) {

    tempAlertActive = false;
    Blynk.virtualWrite(V5, 0);

    Serial.println("Temp normal");
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 STARTED");

  WiFi.begin(ssid, pass);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  Serial.println("Blynk Connected!");

  Wire.begin(21, 22);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  dht.begin();

  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  timer.setInterval(100L, sendData);
}

// ================= LOOP =================
void loop() {
  Blynk.run();
  timer.run();

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }
}