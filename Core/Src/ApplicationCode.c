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
	LCD_DisplayChar(25, 80, 'S');
	LCD_DisplayChar(36, 80, 'i');
	LCD_DisplayChar(45, 80, 'n');
	LCD_DisplayChar(60, 80, 'g');
	LCD_DisplayChar(70, 80, 'l');
	LCD_DisplayChar(80, 80, 'e');

	LCD_DisplayChar(25, 100, 'P');
	LCD_DisplayChar(35, 100, 'l');
	LCD_DisplayChar(45, 100, 'a');
	LCD_DisplayChar(56, 100, 'y');
	LCD_DisplayChar(70, 100, 'e');
	LCD_DisplayChar(83, 100, 'r');

	LCD_Draw_Circle_Fill(60, 145, 20, LCD_COLOR_GREEN);

	// Multi-player button
	LCD_DisplayChar(152, 80,  'M');
	LCD_DisplayChar(169, 80,  'u');
	LCD_DisplayChar(181, 80,  'l');
	LCD_DisplayChar(189, 80,  't');
	LCD_DisplayChar(197, 80,  'i');

	LCD_DisplayChar(145, 100, 'P');
	LCD_DisplayChar(155, 100, 'l');
	LCD_DisplayChar(165, 100, 'a');
	LCD_DisplayChar(176, 100, 'y');
	LCD_DisplayChar(190, 100, 'e');
	LCD_DisplayChar(203, 100, 'r');

	LCD_Draw_Circle_Fill(180, 145, 20, LCD_COLOR_GREEN);

	HAL_Delay(100);


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

Screen handle_game_screen(void){
	game_init(&state);

	while (state.status == GAME_ONGOING){
		button_pressed = false;
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

	return END_SCREEN;
}

Screen handle_end_screen(void){
	LCD_Clear(0, LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	// Red Score
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DisplayChar(10, 10, 'R');
	LCD_DisplayChar(27, 10, 'E');
	LCD_DisplayChar(45, 10, 'D');
	LCD_DisplayChar(55, 8, ':');
	LCD_DisplayChar(70, 10, '5'); // THIS NEEDS TO BE THE ACTUAL SCORE

	// Blue Score
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_DisplayChar(137, 10, 'B');
	LCD_DisplayChar(152, 10, 'L');
	LCD_DisplayChar(167, 10, 'U');
	LCD_DisplayChar(185, 10, 'E');
	LCD_DisplayChar(195, 8, ':');
	LCD_DisplayChar(210, 10, '5'); // THIS NEEDS TO BE THE ACTUAL SCORE

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
