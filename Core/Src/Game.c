/*
 * Game.c
 *
 *  Created on: Apr 16, 2025
 *      Author: jeremyschur
 */

#include "Game.h"

void game_init(GameState *state){
	state->red_score = 0;
	state->blue_score = 0;
}

void game_reset(GameState *state){
	for (uint8_t r = 0; r < ROWS; r++){
		for (uint8_t c = 0; c < COLS; c++){
			state->board[r][c] = EMPTY;
		}
	}
	state->current_player = 1;
	state->status = GAME_ONGOING;
	state->chips_placed = 0;
	state->time = 0;
}

void draw_board(GameState *state){
	LCD_Clear(0, LCD_COLOR_BLACK);

	// Draw vertical grid lines
	for (uint8_t c = 1; c < COLS; c++){
		uint16_t x = c * CELL_WIDTH;
		LCD_Draw_Vertical_Line(x, TOP_MARGIN, LCD_PIXEL_HEIGHT - TOP_MARGIN, LCD_COLOR_WHITE);
	}

	// Draw horizontal grid lines
	for (uint8_t r = 1; r < ROWS; r++){
		uint16_t y = TOP_MARGIN + r * CELL_HEIGHT;
		LCD_Draw_Horizontal_Line(0, y, LCD_PIXEL_WIDTH, LCD_COLOR_WHITE);
	}

	// Draw chips
	for (uint8_t r = 0; r < ROWS; r++){
		for (uint8_t c = 0; c < COLS; c++){
			if (state->board[r][c] != EMPTY){
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
		uint8_t row_dir = directions[d][0];
		uint8_t col_dir = directions[d][1];

		// Check in the positive direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			uint8_t r = row + row_dir * i;
			uint8_t c = col + col_dir * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != player_chip){
				break;
			}
			count++;
		}

		// Check in the negative direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			uint8_t r = row - row_dir * i;
			uint8_t c = col - col_dir * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != player_chip){
				break;
			}
			count++;
		}

		if (count >= WIN_COUNT){
			if (player_chip == RED){
				state->status = GAME_WIN_RED;
				state->red_score += 1;
			} else {
				state->status = GAME_WIN_BLUE;
				state->blue_score += 1;
			}
			return;
		}
	}

	// Check for draw
	if (state->chips_placed == MAX_CHIPS){
		state->status = GAME_DRAW;
	}
}

uint8_t get_random_move(GameState *state){
	return get_random_0_to_6();
}

uint8_t get_best_move(GameState *state, uint8_t ai_player){
	int16_t best_score = -32768;
	uint8_t best_col = 0;

	uint8_t ai_chip = (ai_player == 1) ? RED : BLUE;
	uint8_t opp_chip = (ai_player == 1) ? BLUE : RED;

	for (uint8_t col = 0; col < COLS; col++){
		if (!game_check_move(state, col)){
			continue;
		}

		// Find row chip will be placed in column c
		int8_t row = -1;
		for (int8_t r = ROWS - 1; r >= 0; r--){
			if (state->board[r][col] == EMPTY){
				row = r;
				break;
			}
		}

		int16_t score = 0;

		// Check for AI win
		state->board[row][col] = ai_chip;
		if (check_win(state, row, col, ai_chip)){
			score += 1000;
		}

		// Check for opponent win
		state->board[row][col] = opp_chip;
		if (check_win(state, row, col, opp_chip)){
			score += 800;
		}

		/*
		 * Find next "best" column:
		 	 - Prioritizes columns near the center of the board
		 	 - Favors positions adjacent to other same-color chips
		 */
		state->board[row][col] = ai_chip;
		score += 6 - ((col > COLS / 2) ? (col - COLS / 2) : (COLS / 2 - col));
		score += count_adjacent(state, row, col, ai_chip) * 10;

		state->board[row][col] = EMPTY;

		if (score > best_score){
			best_score = score;
			best_col = col;
		}
	}

	return best_col;
}

bool check_win(GameState *state, int8_t row, int8_t col, uint8_t chip){
	uint8_t directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

	for (uint8_t d = 0; d < 4; d++){
		uint8_t count = 1;
		int8_t row_dir = directions[d][0];
		int8_t col_dir = directions[d][1];

		// Positive direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			int8_t r = row + row_dir * i;
			int8_t c = col + col_dir * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != chip){
				break;
			}
			count++;
		}

		// Negative direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			int8_t r = row - row_dir * i;
			int8_t c = col - col_dir * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != chip){
				break;
			}
			count++;
		}

		if (count >= WIN_COUNT){
			return true;
		}
	}

	return false;
}

int16_t count_adjacent(GameState *state, int8_t row, int8_t col, uint8_t chip){
	int16_t total = 0;

	uint8_t directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

	for (uint8_t d = 0; d < 4; d++){
		uint8_t count = 0;
		int8_t row_dir = directions[d][0];
		int8_t col_dir = directions[d][1];

		// Positive direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			int8_t r = row + row_dir * i;
			int8_t c = col + col_dir * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != chip){
				break;
			}
			count++;
		}

		// Negative direction
		for (uint8_t i = 1; i < WIN_COUNT; i++){
			int8_t r = row - row_dir * i;
			int8_t c = col - col_dir * i;
			if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state->board[r][c] != chip){
				break;
			}
			count++;
		}

		total += count;
	}

	return total;
}
