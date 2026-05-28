#pragma once
#include <Arduino.h>
#include <ArduinoOTA.h>

#include "secrets.h"

class OTA {
 public:
  void begin();
  void handle();
};