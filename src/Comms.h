#pragma once

#include <Arduino.h>

extern QueueHandle_t input_queue;
extern QueueHandle_t output_queue;
/**
 * Structure representing an update to the game board, containing the column and
 * color of the chip that was placed. The column is stored in the lower 4 bits,
 * and the color is stored in the upper 4 bits.
 */
typedef struct {
  uint8_t col : 4;
  uint8_t color : 4;
} board_update_t;