/*
 * Game.h
 *
 *  Created on: Apr 16, 2025
 *      Author: jeremyschur
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include <stdint.h>
#include <stdbool.h>
#include "LCD_Driver.h"

#define ROWS 6
#define COLS 7
#define TOP_MARGIN 40
#define CHIP_RADIUS 15
#define CELL_WIDTH (LCD_PIXEL_WIDTH / COLS)
#define CELL_HEIGHT ((LCD_PIXEL_HEIGHT - TOP_MARGIN) / ROWS)

typedef enum {
    EMPTY,
    RED,
    BLUE
} Cell;

typedef enum {
    GAME_ONGOING,
    GAME_DRAW,
    GAME_WIN_RED,
    GAME_WIN_BLUE
} GameStatus;

typedef enum {
	SINGLE_PLAYER,
	MULTI_PLAYER
} GameMode;

typedef struct {
	Cell board[ROWS][COLS];
	uint8_t current_player; // 1 represents RED's turn, 2 represents BLUE's turn.
	GameStatus status;
	GameMode mode;
} GameState;

typedef struct {
	uint8_t row;
	uint8_t col;
} ChipState;

void game_init(GameState *state);
bool game_check_move(GameState *state, uint8_t col);
void game_make_move(GameState *state, uint8_t col);
void game_check_winner(GameState *state);
void draw_board(GameState *state);
void draw_chip(uint16_t x, uint16_t y, uint16_t Color);

#endif /* INC_GAME_H_ */
