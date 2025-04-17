/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: Xavion
 */

#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#include "LCD_Driver.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdbool.h>
#include "Game.h"

typedef enum {
	START_SCREEN,
	GAME_SCREEN,
	END_SCREEN
} Screen;

void ApplicationInit(void);
Screen handle_start_screen(void);
Screen handle_game_screen(void);
Screen handle_end_screen(void);

void LCD_Visual_Demo(void);

#if (COMPILE_TOUCH_FUNCTIONS == 1)
void LCD_Touch_Polling_Demo(void);
#endif // (COMPILE_TOUCH_FUNCTIONS == 1)

#endif /* INC_APPLICATIONCODE_H_ */
