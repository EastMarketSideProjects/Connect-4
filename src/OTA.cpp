#include "OTA.h"

void OTA::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("connect4");
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() { /* optional: stop motors, save state, etc */ });
  ArduinoOTA.onError([](ota_error_t error) { /* optional error handler */ });
  ArduinoOTA.begin();
}

void OTA::handle() { ArduinoOTA.handle(); }