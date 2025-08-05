/******** Sinric Pro Momentary Relay Control + Auto Mode + WiFiManager + 30s Delay ********/

#include <WiFiManager.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <Preferences.h>

// Sinric Pro credentials
#define APP_KEY       "c7303938-59fa-4ad2-a3de-e62d9bae9cec"
#define APP_SECRET    "c41c8855-b93b-496c-bf27-43f0f72c2dc8-4e754444-82cd-4acb-bb2c-c9cfe22b9b13"
#define DEVICE_ID_1   "6889cacbedeca866fe96e2ee"   // Relay 1 (Pump ON)
#define DEVICE_ID_2   "6889caffddd2551252ba8a70"   // Relay 2 (Pump OFF)
#define DEVICE_ID_3   "6890c55fedeca866fe994705"   // Auto Mode ON/OFF

// Pin configuration
#define RELAY1_PIN     19    // Active LOW Relay 1
#define RELAY2_PIN     21    // Active LOW Relay 2
#define AUTO_MODE_LED  22    // Auto mode status indicator
#define WIFI_LED_PIN    2    // WiFi status indicator

// Timing constants
#define PULSE_TIME     3000     // Relay ON duration in milliseconds
#define AUTO_DELAY     30000    // 30s delay for auto trigger

Preferences preferences;
bool autoMode = false;

// Relay setup
void setupRelay(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);  // OFF for active-low relay
}

// Relay pulse (momentary)
void pulseRelay(int pin, int time = PULSE_TIME) {
  digitalWrite(pin, LOW);
  delay(time);
  digitalWrite(pin, HIGH);
}

// Restore Auto Mode from Preferences
void restoreAutoMode() {
  preferences.begin("settings", false);
  autoMode = preferences.getBool("autoMode", false);
  preferences.end();

  digitalWrite(AUTO_MODE_LED, autoMode ? HIGH : LOW);

  if (autoMode) {
    Serial.println("Auto Mode ON: Waiting 30s before triggering Relay1...");
    delay(AUTO_DELAY);
    pulseRelay(RELAY1_PIN);
  }
}

// Sinric Pro callback
bool onPowerState(const String &deviceId, bool &state) {
  if (deviceId == DEVICE_ID_1 && state) {
    pulseRelay(RELAY1_PIN);
  } 
  else if (deviceId == DEVICE_ID_2 && state) {
    pulseRelay(RELAY2_PIN);
  } 
  else if (deviceId == DEVICE_ID_3) {
    autoMode = state;
    preferences.begin("settings", false);
    preferences.putBool("autoMode", autoMode);
    preferences.end();
    digitalWrite(AUTO_MODE_LED, autoMode ? HIGH : LOW);
    Serial.printf("Auto Mode %s\n", autoMode ? "Enabled" : "Disabled");
  }
  return true;
}

// Sinric Pro setup
void setupSinricPro() {
  SinricProSwitch &sw1 = SinricPro[DEVICE_ID_1];
  SinricProSwitch &sw2 = SinricPro[DEVICE_ID_2];
  SinricProSwitch &sw3 = SinricPro[DEVICE_ID_3];

  sw1.onPowerState(onPowerState);
  sw2.onPowerState(onPowerState);
  sw3.onPowerState(onPowerState);

  SinricPro.restoreDeviceStates(false);
  SinricPro.begin(APP_KEY, APP_SECRET);
 // SinricPro.setReconnectInterval(5000);  // Retry if connection drops
}

// Setup function
void setup() {
  Serial.begin(115200);
  delay(100);

  // Setup pins
  pinMode(WIFI_LED_PIN, OUTPUT);
  digitalWrite(WIFI_LED_PIN, LOW);

  setupRelay(RELAY1_PIN);
  setupRelay(RELAY2_PIN);
  pinMode(AUTO_MODE_LED, OUTPUT);
  digitalWrite(AUTO_MODE_LED, LOW);

  // Start WiFiManager
  WiFiManager wm;
  bool res = wm.autoConnect("Pump-Setup", "12345678"); // SSID, Password
  if (!res) {
    Serial.println("WiFi Failed. Restarting...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("WiFi Connected!");
  WiFi.setAutoReconnect(true);
  digitalWrite(WIFI_LED_PIN, HIGH);

  restoreAutoMode();  // Only after WiFi is ready
  setupSinricPro();
}

// Loop function
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WIFI_LED_PIN, HIGH);
    SinricPro.handle();
  } else {
    digitalWrite(WIFI_LED_PIN, LOW);
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > 10000) {
      Serial.println("WiFi lost. Reconnecting...");
      WiFi.begin();
      lastReconnectAttempt = millis();
    }
  }
}