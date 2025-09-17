#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <Preferences.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include <map>

// Sinric Pro Credentials
#define APP_KEY           "4100bfe8-ea8a-464d-b268-97fb70ae2912"
#define APP_SECRET        "4090f94e-62d1-4226-93e8-bed0586df4d7-8a7015a4-76a4-4cc1-a47f-c9df04a7c7d9"
#define device_ID_1       "68ca2b8bc6e948341599afc5"
#define device_ID_2       "68ca2bc6ad5e91d6f4375ac7"
#define device_ID_3       "68ca2bf2c6e948341599b05b"
#define device_ID_4       "SWITCH_ID_NO_4_HERE"  // Put your 4th device ID

#define RelayPin1 23
#define RelayPin2 22
#define RelayPin3 21
#define RelayPin4 19
#define SwitchPin1 13
#define SwitchPin2 12
#define SwitchPin3 14
#define SwitchPin4 27
#define wifiLed   2

#define DEBOUNCE_TIME 250
#define DNS_PORT 53

DNSServer dnsServer;
WebServer webServer(80);
Preferences preferences;

IPAddress apIP(192, 168, 4, 1);
String ssid, pass;

struct DevicePins {
  int relayPIN;
  int flipSwitchPIN;
};

std::map<String, DevicePins> devices = {
  {device_ID_1, {RelayPin1, SwitchPin1}},
  {device_ID_2, {RelayPin2, SwitchPin2}},
  {device_ID_3, {RelayPin3, SwitchPin3}},
  {device_ID_4, {RelayPin4, SwitchPin4}}
};

struct flipSwitchConfig_t {
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
};

std::map<int, flipSwitchConfig_t> flipSwitches;

unsigned long previousMillis = 0;
const long interval = 500;
bool ledState = false;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>WiFi Config</title></head><body>";
  html += "<h2>WiFi Configuration</h2>";
  html += "<form method='POST' action='/save'>";
  html += "SSID: <input name='ssid'><br>";
  html += "Password: <input name='pass'><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form></body></html>";

  webServer.send(200, "text/html", html);
}

void handleSave() {
  ssid = webServer.arg("ssid");
  pass = webServer.arg("pass");

  preferences.begin("wifiCreds", false);
  preferences.putString("ssid", ssid);
  preferences.putString("pass", pass);
  preferences.end();

  webServer.send(200, "text/html", "<h2>Credentials saved.</h2><p>Restart ESP now.</p>");
}

void handleNotFound() {
  webServer.sendHeader("Location", "/");
  webServer.send(302, "text/plain", "");
}

void startConfigPortal() {
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("SINRIC_ESP32_RELAY_WIFI_SETUP");
  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.on("/", handleRoot);
  webServer.on("/save", HTTP_POST, handleSave);
  webServer.onNotFound(handleNotFound);
  webServer.begin();

  Serial.println("[Config Portal]: Started at " + apIP.toString());

  while (true) {
    dnsServer.processNextRequest();
    webServer.handleClient();
    delay(10);
  }
}

void setupRelays() {
  for (auto &device : devices) {
    int relayPIN = device.second.relayPIN;
    pinMode(relayPIN, OUTPUT);
    digitalWrite(relayPIN, HIGH);  // Active Low
  }
}

void setupFlipSwitches() {
  for (auto &device : devices) {
    flipSwitchConfig_t config;
    config.deviceId = device.first;
    config.lastFlipSwitchChange = 0;
    config.lastFlipSwitchState = true;
    flipSwitches[device.second.flipSwitchPIN] = config;
    pinMode(device.second.flipSwitchPIN, INPUT_PULLUP);
  }
}

bool onPowerState(String deviceId, bool &state) {
  Serial.printf("[SinricPro]: %s -> %s\n", deviceId.c_str(), state ? "ON" : "OFF");
  int relayPIN = devices[deviceId].relayPIN;
  digitalWrite(relayPIN, !state);
  return true;
}

void handleFlipSwitches() {
  unsigned long now = millis();
  for (auto &flipSwitch : flipSwitches) {
    if (now - flipSwitch.second.lastFlipSwitchChange > DEBOUNCE_TIME) {
      int pin = flipSwitch.first;
      bool lastState = flipSwitch.second.lastFlipSwitchState;
      bool state = digitalRead(pin);

      if (state != lastState) {
        flipSwitch.second.lastFlipSwitchChange = now;
        String deviceId = flipSwitch.second.deviceId;
        int relayPIN = devices[deviceId].relayPIN;
        bool newRelayState = !digitalRead(relayPIN);
        digitalWrite(relayPIN, newRelayState);
        SinricProSwitch &mySwitch = SinricPro[deviceId];
        mySwitch.sendPowerStateEvent(!newRelayState);
        flipSwitch.second.lastFlipSwitchState = state;
      }
    }
  }
}

void setupWiFi() {
  preferences.begin("wifiCreds", false);
  ssid = preferences.getString("ssid", "");
  pass = preferences.getString("pass", "");
  preferences.end();

  if (ssid == "") {
    startConfigPortal();
  } else {
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
      startConfigPortal();
    } else {
      digitalWrite(wifiLed, HIGH);
      Serial.printf("\n[WiFi]: Connected! IP=%s\n", WiFi.localIP().toString().c_str());
    }
  }
}

void setupSinricPro() {
  for (auto &device : devices) {
    SinricProSwitch &mySwitch = SinricPro[device.first.c_str()];
    mySwitch.onPowerState(onPowerState);
  }
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup() {
  Serial.begin(9600);
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, LOW);

  setupRelays();
  setupFlipSwitches();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  static unsigned long lastReconnectAttempt = 0;
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    // Blink WiFi LED when offline
    if (now - previousMillis >= interval) {
      previousMillis = now;
      ledState = !ledState;
      digitalWrite(wifiLed, ledState ? HIGH : LOW);
    }

    if (now - lastReconnectAttempt >= 5000) {
      lastReconnectAttempt = now;
      WiFi.begin(ssid.c_str(), pass.c_str());
      Serial.println("[WiFi]: Reconnecting...");
    }
  } else {
    // Solid LED when online
    digitalWrite(wifiLed, HIGH);
  }

  SinricPro.handle();
  handleFlipSwitches();
}