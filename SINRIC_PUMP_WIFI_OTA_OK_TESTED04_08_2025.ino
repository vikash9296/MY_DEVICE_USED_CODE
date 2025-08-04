/******** Sinric Pro Momentary Relay Control + WiFiManager ********/

#include <WiFiManager.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

// Sinric Pro credentials
#define APP_KEY       "c7303938-59fa-4ad2-a3de-e62d9bae9cec"
#define APP_SECRET    "c41c8855-b93b-496c-bf27-43f0f72c2dc8-4e754444-82cd-4acb-bb2c-c9cfe22b9b13"
#define DEVICE_ID_1   "6889cacbedeca866fe96e2ee"
#define DEVICE_ID_2   "6889caffddd2551252ba8a70"
#define DEVICE_ID_3   "6889cb32edeca866fe96e336"

// Relay pins (active LOW)
#define RELAY1_PIN    19
#define RELAY2_PIN    21
#define RELAY3_PIN    22

// Push button pins
#define BUTTON1_PIN   13
#define BUTTON2_PIN   12
#define BUTTON3_PIN   14

// WiFi indicator LED
#define WIFI_LED_PIN  2

#define PULSE_TIME    3000  // milliseconds

unsigned long bootTime = 0;

void setupRelay(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); // OFF for active-low
}

void pulseRelay(int pin) {
  if (millis() - bootTime < 3000) return;
  digitalWrite(pin, LOW);
  delay(PULSE_TIME);
  digitalWrite(pin, HIGH);
}

void setupButton(int pin, void (*callback)()) {
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), callback, FALLING);
}

IRAM_ATTR void handleButton1() { pulseRelay(RELAY1_PIN); }
IRAM_ATTR void handleButton2() { pulseRelay(RELAY2_PIN); }
IRAM_ATTR void handleButton3() { pulseRelay(RELAY3_PIN); }

bool onPowerState(const String &deviceId, bool &state) {
  if (deviceId == DEVICE_ID_1) pulseRelay(RELAY1_PIN);
  else if (deviceId == DEVICE_ID_2) pulseRelay(RELAY2_PIN);
  else if (deviceId == DEVICE_ID_3) pulseRelay(RELAY3_PIN);
  return true;
}

void setupSinricPro() {
  SinricProSwitch &sw1 = SinricPro[DEVICE_ID_1];
  SinricProSwitch &sw2 = SinricPro[DEVICE_ID_2];
  SinricProSwitch &sw3 = SinricPro[DEVICE_ID_3];

  sw1.onPowerState(onPowerState);
  sw2.onPowerState(onPowerState);
  sw3.onPowerState(onPowerState);

  SinricPro.restoreDeviceStates(false);
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(115200);
  bootTime = millis();

  pinMode(WIFI_LED_PIN, OUTPUT);

  setupRelay(RELAY1_PIN);
  setupRelay(RELAY2_PIN);
  setupRelay(RELAY3_PIN);

  setupButton(BUTTON1_PIN, handleButton1);
  setupButton(BUTTON2_PIN, handleButton2);
  setupButton(BUTTON3_PIN, handleButton3);

  // === WiFiManager portal start ===
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("Pump-Setup", "12345678");  // AP name, password
  if (!res) {
    Serial.println("Failed to connect.");
    ESP.restart();
  }
  Serial.println("WiFi connected!");
  digitalWrite(WIFI_LED_PIN, HIGH);
  // ================================

  setupSinricPro();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    SinricPro.handle();
  } else {
    digitalWrite(WIFI_LED_PIN, LOW);
  }
}