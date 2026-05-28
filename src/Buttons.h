#pragma once
#include <Arduino.h>
/**
 * Buttons class manages the state of two buttons, allowing for detection of
 * single presses, double presses, and holds.
 */
class Buttons {
 public:
  struct Button {
    Button(uint8_t pin = 0, unsigned long doublePressTime = 300,
           unsigned long holdTime = 800);

    void begin();
    void update();

    bool wasPressed();
    bool wasDoublePressed();
    bool wasHeld();

   private:
    enum class State : uint8_t { IDLE, DOWN, HELD, WAIT_SECOND, SECOND_DOWN };

    uint8_t pin;
    unsigned long doublePressTime;
    unsigned long holdTime;

    bool lastLevel = HIGH;

    State state = State::IDLE;

    unsigned long pressStartTime = 0;
    unsigned long firstTapStartTime = 0;

    bool pressedFlag = false;
    bool doublePressedFlag = false;
    bool heldFlag = false;
  };

  Button Left;
  Button Right;

  Buttons(uint8_t leftPin, uint8_t rightPin);

  void begin();
  void update();
};