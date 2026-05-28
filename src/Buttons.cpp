#include "Buttons.h"

/**
 *  Constructor for Button struct, initializes pin and timing parameters.
 *  @param pin The GPIO pin number the button is connected to.
 *  @param doublePressTime The maximum time (in milliseconds) between two
 * presses to be considered a double press (default is 300ms).
 *  @param holdTime The time (in milliseconds) a button must be held down to be
 * considered a hold (default is 800ms).
 */
Buttons::Button::Button(uint8_t pin, unsigned long doublePressTime,
                        unsigned long holdTime)
    : pin(pin), doublePressTime(doublePressTime), holdTime(holdTime) {}
/**
 * Initializes the button by setting the pin mode and resetting state variables.
 */
void Buttons::Button::begin() {
  pinMode(pin, INPUT_PULLUP);  // active-LOW button
  lastLevel = HIGH;
  state = State::IDLE;
  pressedFlag = doublePressedFlag = heldFlag = false;
}
/**
 * Updates the button state machine.
 */
void Buttons::Button::update() {
  bool level = digitalRead(pin);  // HIGH = up, LOW = down
  unsigned long now = millis();

  bool falling = (level == LOW && lastLevel == HIGH);  // press down
  bool rising = (level == HIGH && lastLevel == LOW);   // release

  switch (state) {
    case State::IDLE:
      if (falling) {
        pressStartTime = now;
        firstTapStartTime = now;
        state = State::DOWN;
      }
      break;

    case State::DOWN:
      // Hold detection while still down
      if (level == LOW) {
        if (now - pressStartTime >= holdTime) {
          heldFlag = true;      // fire once when threshold crossed
          state = State::HELD;  // stay here until release
        }
      }
      // Released before hold threshold -> candidate for single/double
      if (rising) {
        state = State::WAIT_SECOND;  // start window for a second press
      }
      break;

    case State::HELD:
      // Ignore single/double paths; we've already fired "held".
      if (rising) {
        state = State::IDLE;
      }
      break;

    case State::WAIT_SECOND:
      // If second press happens within double window -> double press
      if (falling) {
        if (now - firstTapStartTime <= doublePressTime) {
          doublePressedFlag = true;    // fire immediately on second press down
          state = State::SECOND_DOWN;  // wait for release to return to IDLE
        } else {
          // Too late: finalize the previous tap as a single, and start a new
          // press
          pressedFlag = true;
          pressStartTime = now;
          firstTapStartTime = now;
          state = State::DOWN;
        }
      } else {
        // No second press yet; if window expires, it's a single press
        if (now - firstTapStartTime > doublePressTime) {
          pressedFlag = true;
          state = State::IDLE;
        }
      }
      break;

    case State::SECOND_DOWN:
      // After double detected, just wait for release to cleanly return to IDLE
      if (rising) {
        state = State::IDLE;
      }
      break;
  }

  lastLevel = level;
}
/**
 * Checks if the button was pressed since the last call to this function.
 * Returns true if it was, and resets the pressed flag.
 */
bool Buttons::Button::wasPressed() {
  if (pressedFlag) {
    pressedFlag = false;
    return true;
  }
  return false;
}
/**
 * Checks if the button was double pressed since the last call to this function.
 * Returns true if it was, and resets the double pressed flag.
 */
bool Buttons::Button::wasDoublePressed() {
  if (doublePressedFlag) {
    doublePressedFlag = false;
    return true;
  }
  return false;
}
/**
 * Checks if the button was held since the last call to this function.
 * Returns true if it was, and resets the held flag.
 */
bool Buttons::Button::wasHeld() {
  if (heldFlag) {
    heldFlag = false;
    return true;
  }
  return false;
}
/**
 * Constructor for the Buttons class.
 */
Buttons::Buttons(uint8_t leftPin, uint8_t rightPin)
    : Left(leftPin), Right(rightPin) {}
/**
 * Initializes both buttons by calling their begin() methods.
 */
void Buttons::begin() {
  Left.begin();
  Right.begin();
}
/**
 * Updates both buttons by calling their update() methods.
 */
void Buttons::update() {
  Left.update();
  Right.update();
}