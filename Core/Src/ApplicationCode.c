/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */


extern void initialise_monitor_handles(void); 

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS

static GameState state;
static ChipState chip;
static bool button_pressed;
static bool screen_pressed;

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

    game_init(&state);

    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();

	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;

	#endif // COMPILE_TOUCH_FUNCTIONS
}

Screen handle_start_screen(void){
	LCD_Clear(0, LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font16x24);

	// Single Player Button
	LCD_DisplayString(15, 80, "Single");
	LCD_DisplayString(15, 100, "Player");

	LCD_Draw_Circle_Fill(60, 145, 20, LCD_COLOR_GREEN);

	// Multi-player button
	LCD_DisplayString(142, 80, "Multi");
	LCD_DisplayString(135, 100, "Player");

	LCD_Draw_Circle_Fill(180, 145, 20, LCD_COLOR_GREEN);

	HAL_Delay(100);

	// Logic for Mode Selection
	while (1) {
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			if (StaticTouchData.x < LCD_PIXEL_WIDTH / 2) {
				state.mode = SINGLE_PLAYER;
			} else {
				state.mode = MULTI_PLAYER;
			}
			break;
		}
	}

	return GAME_SCREEN;
}

Screen handle_game_screen(){
	game_reset(&state);

	uint32_t start_time = HAL_GetTick();
	while (state.status == GAME_ONGOING){
		button_pressed = false;

		// "AI" Turn Logic
		if (state.mode == SINGLE_PLAYER && state.current_player == 2){
			uint8_t col = get_best_move(&state, state.current_player);
			if (game_check_move(&state, col)){
				uint8_t row = game_make_move(&state, col);
				game_check_winner(&state, col, row);

				state.current_player = (state.current_player == 1) ? 2 : 1;
			}
		} else { // Player Turn Logic
			while(!button_pressed) {
				uint16_t color = (state.current_player == 1) ? LCD_COLOR_RED : LCD_COLOR_BLUE;

				uint16_t x = chip.col * CELL_WIDTH + CELL_WIDTH / 2;
				uint16_t y = chip.row * CELL_HEIGHT + CELL_HEIGHT / 2;

				draw_board(&state);
				draw_chip(x, y, color);
				HAL_Delay(100);

				screen_pressed = false;
				while(!screen_pressed){
					if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
						if (StaticTouchData.x < LCD_PIXEL_WIDTH / 2) {
							chip.col = (chip.col == 0) ? 0 : chip.col - 1;
						} else {
							chip.col = (chip.col == COLS - 1) ? COLS - 1 : chip.col + 1;
						}
						screen_pressed = true;
					}
				}
			}

			if (game_check_move(&state, chip.col)){
				uint8_t row = game_make_move(&state, chip.col);
				game_check_winner(&state, chip.col, row);

				state.current_player = (state.current_player == 1) ? 2 : 1;
			}
		}
	}

	uint32_t end_time = HAL_GetTick();
	state.time = (end_time - start_time) / 1000;

	return END_SCREEN;
}

Screen handle_end_screen(){
	LCD_Clear(0, LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	char score[4];
	char time[12];

	// Red Score
	sprintf(score, "%u", state.red_score);

	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DisplayString(10, 10, "RED:");
	LCD_DisplayString(75, 10, score);

	// Blue Score
	sprintf(score, "%u", state.blue_score);

	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_DisplayString(120, 10, "BLUE:");
	LCD_DisplayString(200, 10, score);

	// Previous Game Time
	sprintf(time, "%lus", state.time);

	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_DisplayString(45, 45, "TIME:");
	LCD_DisplayString(125, 45, time);

	// Logic for Win Display
	if (state.status == GAME_WIN_RED){
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DisplayString(50, 160, "RED WINS!");
	} else if (state.status == GAME_WIN_BLUE) {
		LCD_SetTextColor(LCD_COLOR_BLUE);
		LCD_DisplayString(40, 160, "BLUE WINS!");
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_DisplayString(90, 160, "TIE!");
	}

	// Play Again Button
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_Draw_Rectangle_Fill(30, 270, 170, 40, LCD_COLOR_GREEN);
	LCD_DisplayString(35, 280, "PLAY AGAIN");

	// Logic for Play Again Button
	while (1) {
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			if (StaticTouchData.y < LCD_PIXEL_HEIGHT / 2) {
				break;
			}
		}
	}

	return START_SCREEN;
}

void EXTI0_IRQHandler(void){
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	EXTI_HandleTypeDef hexti;
	hexti.Line = EXTI_LINE_0;

	screen_pressed = true;
	button_pressed = true;

	HAL_EXTI_ClearPending(&hexti, EXTI_TRIGGER_RISING_FALLING);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
