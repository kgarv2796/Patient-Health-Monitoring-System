#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>
#include "MAX30105.h"
#include "heartRate.h"

// ==================================================
// WIFI SETTINGS - ENTER YOUR NEW CREDENTIALS
// ==================================================

const char* WIFI_SSID = "WIFI_SSD";
const char* WIFI_PASSWORD = "WIFI_PASS";

// ==================================================
// ADAFRUIT IO SETTINGS
// ==================================================

#define IO_USERNAME "ADAFRUIT_Username"
#define IO_KEY "Adafruit_key"

const char* MQTT_HOST = "io.adafruit.com";
const int MQTT_PORT = 8883;

// Existing Adafruit IO feeds
const char* TOPIC_HW =
  "GouravSingh/feeds/hw-827-heart-rate";

const char* TOPIC_MAX =
  "GouravSingh/feeds/max30102-heart-rate";

const char* TOPIC_TEMP =
  "GouravSingh/feeds/temperature";

const char* TOPIC_HUMIDITY =
  "GouravSingh/feeds/humidity";

// ==================================================
// DIGICERT ROOT CA - ADAFRUIT IO TLS
// ==================================================

const char* adafruitio_root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEjTCCA3WgAwIBAgIQDQd4KhM/xvmlcpbhMf/ReTANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0xNzExMDIxMjIzMzdaFw0yNzExMDIxMjIzMzdaMGAxCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpY2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xHzAdBgNVBAMTFkdlb1RydXN0IFRMUyBSU0EgQ0EgRzEwggEiMA0GCSqGSIb3
DQEBAQUAA4IBDwAwggEKAoIBAQC+F+jsvikKy/65LWEx/TMkCDIuWegh1Ngwvm4Q
yISgP7oU5d79eoySG3vOhC3w/3jEMuipoH1fBtp7m0tTpsYbAhch4XA7rfuD6whU
gajeErLVxoiWMPkC/DnUvbgi74BJmdBiuGHQSd7LwsuXpTEGG9fYXcbTVN5SATYq
DfbexbYxTMwVJWoVb6lrBEgM3gBBqiiAiy800xu1Nq07JdCIQkBsNpFtZbIZhsDS
fzlGWP4wEmBQ3O67c+ZXkFr2DcrXBEtHam80Gp2SNhou2U5U7UesDL/xgLK6/0d7
6TnEVMSUVJkZ8VeZr+IUIlvoLrtjLbqugb0T3OYXW+CQU0kBAgMBAAGjggFAMIIB
PDAdBgNVHQ4EFgQUlE/UXYvkpOKmgP792PkA76O+AlcwHwYDVR0jBBgwFoAUTiJU
IBiV5uNu5g/6+rkS7QYXjzkwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsG
AQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/AgEAMDQGCCsGAQUFBwEB
BCgwJjAkBggrBgEFBQcwAYYYaHR0cDovL29jc3AuZGlnaWNlcnQuY29tMEIGA1Ud
HwQ7MDkwN6A1oDOGMWh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEds
b2JhbFJvb3RHMi5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEW
HGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwDQYJKoZIhvcNAQELBQADggEB
AIIcBDqC6cWpyGUSXAjjAcYwsK4iiGF7KweG97i1RJz1kwZhRoo6orU1JtBYnjzB
c4+/sXmnHJk3mlPyL1xuIAt9sMeC7+vreRIF5wFBC0MCN5sbHwhNN1JzKbifNeP5
ozpZdQFmkCo+neBiKR6HqIA+LMTMCMMuv2khGGuPHmtDze4GmEGZtYLyF8EQpa5Y
jPuV6k2Cr/N3XxFpT3hRpt/3usU/Zb9wfKPtWpoznZ4/44c1p9rzFcZYrWkj3A+7
TNBJE0GmP2fhXhP1D/XVfIW/h0yCJGEiV9Glm/uGOa3DXHlmbAcxSyCRraG+ZBkA
7h4SeM6Y8l/7MBRpPCz6l8Y=
-----END CERTIFICATE-----
)EOF";

// ==================================================
// PIN DEFINITIONS
// ==================================================

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define HW827_PIN 34

#define SDA_PIN 21
#define SCL_PIN 22

// ==================================================
// SENSOR OBJECTS
// ==================================================

DHT dht(DHT_PIN, DHT_TYPE);
MAX30105 maxSensor;

// ==================================================
// NETWORK OBJECTS
// ==================================================

WiFiClientSecure secureClient;
PubSubClient mqtt(secureClient);

// ==================================================
// MAX30102 VARIABLES
// ==================================================

float averageBPM = 0;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];

byte rateSpot = 0;
byte beatCount = 0;

unsigned long lastMaxBeat = 0;
unsigned long lastMaxValidTime = 0;

bool maxValid = false;
// SENSOR RAW VALUES
long irValue = 0;
int hwValue = 0;

// ==================================================
// HW-827 VARIABLES
// ==================================================

float baseline = 0;
float hwBPM = 0;

unsigned long lastHWBeat = 0;
unsigned long lastHWValidTime = 0;

bool pulseDetected = false;
bool hwValid = false;

// ==================================================
// DHT11 VARIABLES
// ==================================================

float temperature = 0;
float humidity = 0;

bool dhtValid = false;

unsigned long lastDHTRead = 0;
unsigned long lastDHTSuccess = 0;

// ==================================================
// TIMERS
// ==================================================

unsigned long lastPrint = 0;
unsigned long lastPublish = 0;

unsigned long lastWiFiAttempt = 0;
unsigned long lastMQTTAttempt = 0;

const unsigned long PUBLISH_INTERVAL = 2500;
const unsigned long DHT_INTERVAL = 2000;

byte feedIndex = 0;

// ==================================================
// WIFI CONNECTION
// ==================================================

void ensureWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  if (millis() - lastWiFiAttempt >= 10000) {

    lastWiFiAttempt = millis();

    Serial.println("Connecting to WiFi...");

    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}

// ==================================================
// MQTT CONNECTION
// ==================================================

void ensureMQTT() {

  if (WiFi.status() != WL_CONNECTED ||
      mqtt.connected()) {
    return;
  }

  if (millis() - lastMQTTAttempt < 5000) {
    return;
  }

  lastMQTTAttempt = millis();

  Serial.println("Connecting to Adafruit IO...");

  String clientId =
    "ESP32-" +
    String((uint32_t)ESP.getEfuseMac(), HEX);

  if (mqtt.connect(
        clientId.c_str(),
        IO_USERNAME,
        IO_KEY
      )) {

    Serial.println("Adafruit IO MQTT connected!");

  } else {

    Serial.print("MQTT connection failed. State: ");
    Serial.println(mqtt.state());
  }
}

// ==================================================
// PUBLISH ONE FEED
// ==================================================

void publishFeed(
  const char* topic,
  float value,
  const char* feedName
) {

  if (!mqtt.connected()) {
    return;
  }

  char payload[20];

  snprintf(
    payload,
    sizeof(payload),
    "%.1f",
    value
  );

  if (mqtt.publish(topic, payload)) {

    Serial.print(feedName);
    Serial.print(" published: ");
    Serial.println(payload);

  } else {

    Serial.print(feedName);
    Serial.println(" publish failed!");
  }
}

// ==================================================
// PUBLISH SENSOR DATA
// ==================================================

void publishSensorData() {

  if (!mqtt.connected()) {
    return;
  }

  switch (feedIndex) {

    case 0:

      if (hwValid) {
        publishFeed(
          TOPIC_HW,
          hwBPM,
          "HW-827 Heart Rate"
        );
      } else {
        Serial.println("HW-827: No valid reading");
      }

      break;

    case 1:

      if (maxValid) {
        publishFeed(
          TOPIC_MAX,
          averageBPM,
          "MAX30102 Heart Rate"
        );
      } else {
        Serial.println("MAX30102: No valid reading");
      }

      break;

    case 2:

      if (dhtValid) {
        publishFeed(
          TOPIC_TEMP,
          temperature,
          "Temperature"
        );
      } else {
        Serial.println("DHT11: No valid reading");
      }

      break;

    case 3:

      if (dhtValid) {
        publishFeed(
          TOPIC_HUMIDITY,
          humidity,
          "Humidity"
        );
      } else {
        Serial.println("DHT11: No valid reading");
      }

      break;
  }

  feedIndex++;

  if (feedIndex >= 4) {
    feedIndex = 0;
  }
}

// ==================================================
// READ DHT11
// ==================================================

void readDHT() {

  unsigned long now = millis();

  if (now - lastDHTRead < DHT_INTERVAL) {
    return;
  }

  lastDHTRead = now;

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t) && !isnan(h)) {

    temperature = t;
    humidity = h;

    dhtValid = true;
    lastDHTSuccess = now;

  } else {

    Serial.println("DHT11 reading error!");
  }

  // Expire old readings after 15 seconds.
  if (dhtValid &&
      now - lastDHTSuccess > 15000) {
    dhtValid = false;
  }
}

// ==================================================
// READ MAX30102
// ==================================================

void readMAX30102() {

  unsigned long now = millis();

  irValue = maxSensor.getIR();

  if (checkForBeat(irValue)) {

    if (lastMaxBeat > 0) {

      unsigned long interval =
        now - lastMaxBeat;

      float bpm = 60000.0 / interval;

      if (bpm >= 40 && bpm <= 200) {

        rates[rateSpot] = (byte)bpm;

        rateSpot =
          (rateSpot + 1) % RATE_SIZE;

        if (beatCount < RATE_SIZE) {
          beatCount++;
        }

        float sum = 0;

        for (byte i = 0; i < beatCount; i++) {
          sum += rates[i];
        }

        averageBPM = sum / beatCount;

        maxValid = true;
        lastMaxValidTime = now;
      }
    }

    lastMaxBeat = now;
  }

  // No finger or no valid pulse for 8 seconds.
  if (maxValid &&
      now - lastMaxValidTime > 8000) {

    averageBPM = 0;
    maxValid = false;

    beatCount = 0;
    rateSpot = 0;
    lastMaxBeat = 0;
  }

  if (irValue < 50000) {

    averageBPM = 0;
    maxValid = false;

    beatCount = 0;
    rateSpot = 0;
    lastMaxBeat = 0;
  }
}

// ==================================================
// READ HW-827
// ==================================================

void readHW827() {

  unsigned long now = millis();

  hwValue = analogRead(HW827_PIN);

  if (baseline == 0) {
    baseline = hwValue;
  }

  baseline =
    baseline * 0.99 +
    hwValue * 0.01;

  float threshold = baseline + 120;

  if (hwValue > threshold && !pulseDetected) {

    if (lastHWBeat > 0) {

      unsigned long interval =
        now - lastHWBeat;

      if (interval >= 300 &&
          interval <= 2000) {

        hwBPM = 60000.0 / interval;

        hwValid = true;
        lastHWValidTime = now;
      }
    }

    lastHWBeat = now;
    pulseDetected = true;
  }

  if (hwValue < baseline + 40) {
    pulseDetected = false;
  }

  // Expire old heart-rate readings.
  if (hwValid &&
      now - lastHWValidTime > 5000) {

    hwBPM = 0;
    hwValid = false;
  }
}

// ==================================================
// SERIAL MONITOR
// ==================================================

void printSensorData() {

  unsigned long now = millis();

  if (now - lastPrint < 3000) {
    return;
  }

  lastPrint = now;

  Serial.println();
  Serial.println("========== SENSOR DATA ==========");

  Serial.print("Temperature: ");

  if (dhtValid) {
    Serial.print(temperature);
    Serial.println(" C");
  } else {
    Serial.println("Unavailable");
  }

  Serial.print("Humidity: ");

  if (dhtValid) {
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.println("Unavailable");
  }

  Serial.println("--------------------------------");

  Serial.print("MAX30102 IR: ");
  Serial.println(irValue);

  Serial.print("MAX30102 Heart Rate: ");

  if (maxValid) {
    Serial.print(averageBPM);
    Serial.println(" BPM");
  } else {
    Serial.println("Place finger on sensor");
  }

  Serial.println("--------------------------------");

  Serial.print("HW-827 Analog: ");
  Serial.println(hwValue);

  Serial.print("HW-827 Heart Rate: ");

  if (hwValid) {
    Serial.print(hwBPM);
    Serial.println(" BPM");
  } else {
    Serial.println("Waiting for pulse...");
  }

  Serial.println("--------------------------------");

  Serial.print("WiFi: ");

  Serial.println(
    WiFi.status() == WL_CONNECTED
      ? "Connected"
      : "Disconnected"
  );

  Serial.print("Adafruit IO MQTT: ");

  Serial.println(
    mqtt.connected()
      ? "Connected"
      : "Disconnected"
  );

  Serial.println("================================");
}

// ==================================================
// SETUP
// ==================================================

void setup() {

  Serial.begin(115200);

  // DHT11
  dht.begin();

  // HW-827
  analogReadResolution(12);

  analogSetPinAttenuation(
    HW827_PIN,
    ADC_11db
  );

  // MAX30102
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!maxSensor.begin(Wire, I2C_SPEED_FAST)) {

    Serial.println("MAX30102 not found!");

    while (true) {
      delay(1000);
    }
  }

  maxSensor.setup();

  maxSensor.setPulseAmplitudeRed(0x0A);
  maxSensor.setPulseAmplitudeIR(0x1F);
  maxSensor.setPulseAmplitudeGreen(0);

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  // Proper TLS certificate verification.
  secureClient.setCACert(adafruitio_root_ca);

  // MQTT
  mqtt.setServer(
    MQTT_HOST,
    MQTT_PORT
  );

  mqtt.setBufferSize(512);
  mqtt.setKeepAlive(30);

  Serial.println();
  Serial.println("==================================");
  Serial.println("PATIENT HEALTH MONITORING SYSTEM");
  Serial.println("ESP32 + ADAFRUIT IO");
  Serial.println("==================================");

  Serial.println("Sensors initialized!");
  Serial.println("Starting WiFi and MQTT...");
}

// ==================================================
// MAIN LOOP
// ==================================================

void loop() {

  unsigned long currentTime = millis();

  // Network
  ensureWiFi();
  ensureMQTT();

  if (mqtt.connected()) {
    mqtt.loop();
  }

  // Sensors
  readDHT();
  readMAX30102();
  readHW827();

  // Publish one feed every 2.5 seconds.
  if (currentTime - lastPublish >= PUBLISH_INTERVAL) {

    lastPublish = currentTime;

    publishSensorData();
  }

  // Serial Monitor
  printSensorData();
}