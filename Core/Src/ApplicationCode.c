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
		// Add interrupt handler for button press and add logic to drop chip

	}

	return END_SCREEN;
}

void LCD_Visual_Demo(void)
{
	visualDemo();
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void)
{
	LCD_Clear(0,LCD_COLOR_GREEN);
	while (1) {
		/* If touch pressed */
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			LCD_Clear(0, LCD_COLOR_RED);
		} else {
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_GREEN);
		}
	}
}
#endif // COMPILE_TOUCH_FUNCTIONS

