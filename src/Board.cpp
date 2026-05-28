#include "Board.h"
/**
 * Initializes the LED matrix and resets the game board.
 */
void Board::begin() {
  pixel.begin();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixel.setPixelColor(i, 0);
  }
  pixel.setBrightness(5);
  pixel.show();
  resetBoard();
}
/**
 * Moves the cursor to the right.
 */
void Board::moveCursor() { cursorCol = (cursorCol + 1) % COLS; }
/**
 * Moves the cursor to the left.
 */
void Board::moveCursorBack() { cursorCol = (cursorCol - 1) % COLS; }
/**
 * Drops a chip into the specified column.
 * @param col The column to drop the chip into.
 * @return true if the chip was dropped successfully, false otherwise.
 */
bool Board::dropChip(int col) {
  for (int row = ROWS - 1; row >= 0; row--) {
    if (gameBoard[row][col] == 0) {
      insertChip(row, col, current_player);
      current_player = (current_player == RED) ? YELLOW : RED;
      return true;
    }
  }
  return false;
}
/**
 * Inserts a chip of the specified color into the board at the given position.
 * @param row The row to insert the chip into.
 * @param col The column to insert the chip into.
 * @param color The color of the chip to insert.
 */
void Board::insertChip(int row, int col, current_player_t color) {
  gameBoard[row][col] = color;
  Serial.printf("%s at (%u,%u)\n", color == RED ? "🔴" : "🟡", row, col);

  board_update_t update;
  update.col = col;
  update.color = color;
  xQueueSend(output_queue, &update, 0);

  if (checkWinner(row, col, color)) {
    victoryDance(row, col, color);
    Serial.printf("%s wins!\n", color == RED ? "RED" : "YELLOW");
    resetBoard();
  }
  printBoard();
}
/**
 * Draws the game board on the LED matrix.
 */
void Board::drawBoard() {
  for (int col = 0; col < WIDTH; col++) {
    pixel.setPixelColor(pixelIndex[0][col], blue);
    pixel.setPixelColor(pixelIndex[HEIGHT - 1][col], blue);
  }
  for (int row = 0; row < HEIGHT; row++) {
    pixel.setPixelColor(pixelIndex[row][0], blue);
    pixel.setPixelColor(pixelIndex[row][WIDTH - 1], blue);
  }

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      int idx = pixelIndex[row + BOARD_ROW_OFFSET][col + BOARD_COL_OFFSET];
      if (gameBoard[row][col] == 1) {
        pixel.setPixelColor(idx, red);
      } else if (gameBoard[row][col] == 2) {
        pixel.setPixelColor(idx, yellow);
      } else {
        pixel.setPixelColor(idx, 0);
      }
    }
  }
  uint32_t cursorColor = (current_player == RED) ? red : yellow;
  pixel.setPixelColor(pixelIndex[0][cursorCol + BOARD_COL_OFFSET], cursorColor);

  pixel.show();
}
/**
 * Checks if there is a winning line in the specified direction.
 * @param row The row to check.
 * @param col The column to check.
 * @param dRow The row direction to check.
 * @param dCol The column direction to check.
 * @param color The color of the chips to check for.
 * @return true if a winning line is found, false otherwise.
 */
bool Board::checkDirection(int row, int col, int dRow, int dCol,
                           current_player_t color) {
  int count = 1;  // count the placed piece itself

  // forward direction
  for (int i = 1; i < 4; i++) {
    int r = row + dRow * i;
    int c = col + dCol * i;
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) break;
    if (gameBoard[r][c] != color) break;
    count++;
  }

  // backward direction
  for (int i = 1; i < 4; i++) {
    int r = row - dRow * i;
    int c = col - dCol * i;
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) break;
    if (gameBoard[r][c] != color) break;
    count++;
  }

  return count >= 4;
}
/**
 * Checks if the specified player has won the game after placing a chip at the
 * given position.
 * @param row The row where the chip was placed.
 * @param col The column where the chip was placed.
 * @param color The color of the chip that was placed.
 * @return true if the player has won, false otherwise.
 */
bool Board::checkWinner(int row, int col, current_player_t color) {
  for (int i = 0; i < 4; i++) {
    if (checkDirection(row, col, direction[i][0], direction[i][1], color)) {
      return true;
    }
  }
  return false;
}
/**
 * Performs a victory dance by flashing the LED matrix in the color of the
 * winning player.
 * @param row The row where the winning chip was placed.
 * @param col The column where the winning chip was placed.
 * @param color The color of the winning player.
 * @param delayMs The delay in milliseconds between flashes (default is 400ms).
 */
void Board::victoryDance(int row, int col, current_player_t color,
                         int delayMs) {
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 64; i++) {
      pixel.setPixelColor(i, color == RED ? red : yellow);
    }
    pixel.show();
    delay(delayMs);
    for (int i = 0; i < 64; i++) {
      pixel.setPixelColor(i, 0);
    }
    pixel.show();
    delay(delayMs);
  }
}
/**
 * Resets the game board to its initial state.
 */
void Board::resetBoard() { memset(gameBoard, 0, sizeof(gameBoard)); }

current_player_t Board::printBoard() {
  Serial.println();

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      uint8_t boardSpace = gameBoard[row][col];
      Serial.printf("(%u,%u)", row, col);
      switch (boardSpace) {
        case RED:
          Serial.print("🔴");
          break;
        case YELLOW:
          Serial.print("🟡");
          break;
        default:
          Serial.print("⚪️");
          break;
      }
      Serial.print(" ");
    }

    Serial.println();
  }
  Serial.println();
  return RED;
}