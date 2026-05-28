#include "Arduino.h"
#include "Bluetooth.h"
#include "Board.h"
#include "Buttons.h"
#include "Comms.h"
#include "OTA.h"

#define BUTTON_ZERO 0
#define BUTTON_ONE 1

Buttons buttons(BUTTON_ZERO, BUTTON_ONE);
Board board;
OTA ota;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUTTON_ZERO, INPUT_PULLUP);
  buttons.begin();
  board.begin();
  ota.begin();

  output_queue = xQueueCreate(10, sizeof(board_update_t));
  xTaskCreate(output_task, "output_task", 4096, NULL, 1, NULL);
}

void loop() {
  buttons.update();
  if (buttons.Left.wasPressed()) {
    board.moveCursor();
  }
  if (buttons.Left.wasDoublePressed()) {
    board.dropChip(board.cursorCol);
  }
  if (buttons.Left.wasHeld()) {
    board.moveCursorBack();
  }

  if (Serial.available()) {
    String s = Serial.readString();
    if (s.startsWith("rst")) {
      ESP.restart();
      return;
    }
    // row,col,color ex. 0,0,1
    int firstComma = s.indexOf(',');
    int secondComma = s.indexOf(',', firstComma + 1);
    int row = s.substring(0, firstComma).toInt();
    int col = s.substring(firstComma + 1, secondComma).toInt();
    int color = s.substring(secondComma + 1).toInt();
    board.insertChip(row, col, (current_player_t)color);
  }

  board.drawBoard();
  ota.handle();
}