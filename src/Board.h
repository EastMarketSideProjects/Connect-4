#pragma once
#include "Adafruit_NeoPixel.h"
#include "Comms.h"

#define PIXEL_PIN 14
#define WIDTH 8
#define HEIGHT 8
#define ROWS 6
#define COLS 6
#define BOARD_ROW_OFFSET 1
#define BOARD_COL_OFFSET 1
#define NUMPIXELS (WIDTH * HEIGHT)

typedef enum { EMPTY, RED, YELLOW } current_player_t;
/**
 *  Board class manages the game state and LED display for Connect 4.
 */
class Board {
 public:
  current_player_t gameBoard[ROWS][COLS];
  current_player_t current_player = RED;
  Adafruit_NeoPixel pixel =
      Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_RGB + NEO_KHZ800);
  uint32_t red = pixel.Color(255, 0, 0);
  uint32_t yellow = pixel.Color(255, 255, 0);
  uint32_t blue = pixel.Color(0, 0, 50);
  int cursorCol = 0;

  const uint8_t pixelIndex[HEIGHT][WIDTH] = {
      {0, 1, 2, 3, 4, 5, 6, 7},         {8, 9, 10, 11, 12, 13, 14, 15},
      {16, 17, 18, 19, 20, 21, 22, 23}, {24, 25, 26, 27, 28, 29, 30, 31},
      {32, 33, 34, 35, 36, 37, 38, 39}, {40, 41, 42, 43, 44, 45, 46, 47},
      {48, 49, 50, 51, 52, 53, 54, 55}, {56, 57, 58, 59, 60, 61, 62, 63}};

  void begin();
  void moveCursor();
  void moveCursorBack();
  bool dropChip(int col);
  void insertChip(int row, int col, current_player_t color);
  void drawBoard();
  const int direction[4][2] = {
      {0, 1},  // horizontal
      {1, 0},  // vertical
      {1, 1},  // diagonal
      {1, -1}  // diagonal
  };

  bool checkDirection(int row, int col, int dRow, int dCol,
                      current_player_t color);

  bool checkWinner(int row, int col, current_player_t color);

  void victoryDance(int row, int col, current_player_t color,
                    int delayMs = 400);
  void resetBoard();
  current_player_t printBoard();
};