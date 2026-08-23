#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

namespace {
constexpr char kDeviceName[] = "DC Assistant";
constexpr char kFirmwareVersion[] = "DC 0.3";
constexpr uint8_t kStatusLed = 48;
Preferences preferences;
}

void setup() {
  Serial.begin(115200);
  pinMode(kStatusLed, OUTPUT);
  preferences.begin("dcassistant", false);
  digitalWrite(kStatusLed, HIGH);
  Serial.printf("%s | %s | ESP32-S3 ready\n", kDeviceName, kFirmwareVersion);
}

void loop() {
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 5000) {
    lastHeartbeat = millis();
    Serial.printf("Wi-Fi: %s | NVS: ready | Audio: standby\n", WiFi.isConnected() ? "connected" : "not configured");
  }
  delay(25);
}
