#include <WiFi.h>
#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Performing Factory Reset...");

  // Erase Wi-Fi credentials
  WiFi.disconnect(true, true);
  delay(1000);
  Serial.println("Wi-Fi credentials erased.");

  // Erase custom preferences
  preferences.begin("my_app", false);  // Replace "my_app" with your namespace
  preferences.clear();
  preferences.end();
  Serial.println("Preferences cleared.");

  // Optionally reset GPIOs
  for (int pin = 0; pin < 40; pin++) {
    pinMode(pin, INPUT);  // Set all pins to input mode
  }
  Serial.println("GPIOs reset to INPUT.");

  Serial.println("Factory Reset Done. Restarting in 3 seconds...");
  delay(3000);
  ESP.restart();  // Reboot the device
}

void loop() {
  // Do nothing
}
