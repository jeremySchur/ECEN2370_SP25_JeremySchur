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
