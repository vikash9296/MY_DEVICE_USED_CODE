/**********************************************************************************
 *  TITLE: Blynk 2.0 + IR + Manual Button control 8 Relays using ESP32 with DHT11 sensor, LDR, Realtime Feedback & No WiFi control
 *  YouTube Video: https://youtu.be/Vgyj3cCAioI
 *  Related Blog : https://iotcircuithub.com/esp32-projects/
 **********************************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL3nj5PHliQ"
#define BLYNK_TEMPLATE_NAME "WIFI PUMP CONTROL OTA"
#define BLYNK_DEVICE_NAME "ESP32_8Relay_Controller"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial

#include <DHT.h>  
#include <IRremote.h>

#define IR_RECV_PIN 35 // D35 (IR receiver pin)
IRrecv irrecv(IR_RECV_PIN);
decode_results results;

#define DHTTYPE DHT11
#define DHTPIN 16 //D16  pin connected with DHT
#define LDR_PIN 34 //D34  pin connected with LDR

// define the GPIO connected with Relays and switches
#define RelayPin1 19
#define RelayPin2 21
#define RelayPin3 22
#define RelayPin4 23
#define RelayPin5 18
#define RelayPin6 5
#define RelayPin7 25
#define RelayPin8 26

#define SwitchPin1 13
#define SwitchPin2 12
#define SwitchPin3 14
#define SwitchPin4 27
#define SwitchPin5 33
#define SwitchPin6 32
#define SwitchPin7 15
#define SwitchPin8 4

#define wifiLed 2 // D2

#define VPIN_BUTTON_1 V1
#define VPIN_BUTTON_2 V2
#define VPIN_BUTTON_3 V3
#define VPIN_BUTTON_4 V4
#define VPIN_BUTTON_5 V5
#define VPIN_BUTTON_6 V6
#define VPIN_BUTTON_7 V7
#define VPIN_BUTTON_8 V8
#define VPIN_BUTTON_C V9
#define VPIN_TEMPERATURE V10
#define VPIN_HUMIDITY V11
#define VPIN_LDR V12

bool toggleState_1 = LOW;
bool toggleState_2 = LOW;
bool toggleState_3 = LOW;
bool toggleState_4 = LOW;
bool toggleState_5 = LOW;
bool toggleState_6 = LOW;
bool toggleState_7 = LOW;
bool toggleState_8 = LOW;

float temperature1 = 0;
float humidity1 = 0;
int ldrVal;

#include "BlynkEdgent.h"
BlynkTimer timer2;
DHT dht(DHTPIN, DHTTYPE);

void readSensor() {
  ldrVal = map(analogRead(LDR_PIN), 0, 4095, 10, 0);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } else {
    humidity1 = h;
    temperature1 = t;
  }  
}

void sendSensor() {
  readSensor();
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity1);
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1);
  Blynk.virtualWrite(VPIN_LDR, ldrVal);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, toggleState_1 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, toggleState_2 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, toggleState_3 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  digitalWrite(RelayPin4, toggleState_4 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_5) {
  toggleState_5 = param.asInt();
  digitalWrite(RelayPin5, toggleState_5 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_6) {
  toggleState_6 = param.asInt();
  digitalWrite(RelayPin6, toggleState_6 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_7) {
  toggleState_7 = param.asInt();
  digitalWrite(RelayPin7, toggleState_7 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_8) {
  toggleState_8 = param.asInt();
  digitalWrite(RelayPin8, toggleState_8 ? LOW : HIGH);
}

BLYNK_WRITE(VPIN_BUTTON_C) {
  all_SwitchOff();
}

void all_SwitchOff() {
  toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  toggleState_3 = 0; digitalWrite(RelayPin3, HIGH); Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  toggleState_4 = 0; digitalWrite(RelayPin4, HIGH); Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  toggleState_5 = 0; digitalWrite(RelayPin5, HIGH); Blynk.virtualWrite(VPIN_BUTTON_5, toggleState_5);
  toggleState_6 = 0; digitalWrite(RelayPin6, HIGH); Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_6);
  toggleState_7 = 0; digitalWrite(RelayPin7, HIGH); Blynk.virtualWrite(VPIN_BUTTON_7, toggleState_7);
  toggleState_8 = 0; digitalWrite(RelayPin8, HIGH); Blynk.virtualWrite(VPIN_BUTTON_8, toggleState_8);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity1);
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1);
  Blynk.virtualWrite(VPIN_LDR, ldrVal);
}

void setup() {
  Serial.begin(115200);
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);
  pinMode(RelayPin7, OUTPUT);
  pinMode(RelayPin8, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);
  pinMode(SwitchPin5, INPUT_PULLUP);
  pinMode(SwitchPin6, INPUT_PULLUP);
  pinMode(SwitchPin7, INPUT_PULLUP);
  pinMode(SwitchPin8, INPUT_PULLUP);

  // During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);
  digitalWrite(RelayPin5, HIGH);
  digitalWrite(RelayPin6, HIGH);
  digitalWrite(RelayPin7, HIGH);
  digitalWrite(RelayPin8, HIGH);

  irrecv.enableIRIn(); // Enabling IR sensor
  BlynkEdgent.begin();
  dht.begin();
  timer2.setInterval(2000L, sendSensor);
}

void loop() {
  BlynkEdgent.run();
  // manual_control(); // Add your manual control logic here
  // ir_remote();      // Add your IR control logic here
  timer2.run();
}
