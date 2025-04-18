/*
 * Game.c
 *
 *  Created on: Apr 16, 2025
 *      Author: jeremyschur
 */

#include "Game.h"

void game_init(GameState *state) {
	for (uint8_t r = 0; r < ROWS; r++) {
		for (uint8_t c = 0; c < COLS; c++) {
			state->board[r][c] = EMPTY;
		}
	}
	state->current_player = 1;
	state->status = GAME_ONGOING;
}

void draw_board(GameState *state) {
	LCD_Clear(0, LCD_COLOR_BLACK);

	// Draw vertical grid lines
	for (uint8_t c = 1; c < COLS; c++) {
		uint16_t x = c * CELL_WIDTH;
		LCD_Draw_Vertical_Line(x, TOP_MARGIN, LCD_PIXEL_HEIGHT - TOP_MARGIN, LCD_COLOR_WHITE);
	}

	// Draw horizontal grid lines
	for (uint8_t r = 1; r < ROWS; r++) {
		uint16_t y = TOP_MARGIN + r * CELL_HEIGHT;
		LCD_Draw_Horizontal_Line(0, y, LCD_PIXEL_WIDTH, LCD_COLOR_WHITE);
	}

	// Draw chips
	for (uint8_t r = 0; r < ROWS; r++) {
		for (uint8_t c = 0; c < COLS; c++) {
			if (state->board[r][c] != EMPTY) {
				uint16_t color = (state->board[r][c] == RED) ? LCD_COLOR_RED : LCD_COLOR_BLUE;

				uint16_t x = c * CELL_WIDTH + CELL_WIDTH / 2;
				uint16_t y = TOP_MARGIN + r * CELL_HEIGHT + CELL_HEIGHT / 2;

				draw_chip(x, y, color);
			}
		}
	}
}

void draw_chip(uint16_t x, uint16_t y, uint16_t color) {
	LCD_Draw_Circle_Fill(x, y, CHIP_RADIUS, color);
}

bool game_check_move(GameState *state, uint8_t col){
	for (uint8_t r = 0; r < ROWS; r++){
		if (state->board[r][col] == EMPTY){
			return true;
		}
	}
	return false;
}

uint8_t game_make_move(GameState *state, uint8_t col){
	for (uint8_t r = ROWS - 1; r >= 0; r--){
		if (state->board[r][col] == EMPTY){
			state->board[r][col] = (state->current_player == 1) ? RED : BLUE;
			state->chips_placed += 1;
			return r;
		}
	}
}

void game_check_winner(GameState *state, uint8_t col, uint8_t row){
	Cell player_chip = state->board[row][col];

	uint8_t directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

	for (uint8_t d = 0; d < 4; d++){
		uint8_t count = 1;
		uint8_t row_direction = directions[d][0];
		uint8_t col_direction = directions[d][1];

		// Check in the positive direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			uint8_t r = row + row_direction * i;
			uint8_t c = col + col_direction * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != player_chip){
				break;
			}
			count++;
		}

		// Check in the negative direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			uint8_t r = row - row_direction * i;
			uint8_t c = col - col_direction * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != player_chip){
				break;
			}
			count++;
		}

		if (count >= WIN_COUNT){
			state->status = (player_chip == RED) ? GAME_WIN_RED : GAME_WIN_BLUE;
			return;
		}
	}

	// Check for draw
	if (state->chips_placed == MAX_CHIPS){
		state->status = GAME_DRAW;
	}
}
