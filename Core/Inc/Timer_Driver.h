/*
 * Timer_Driver.h
 *
 *  Created on: Apr 19, 2025
 *      Author: jeremyschur
 */

#ifndef INC_TIMER_DRIVER_H_
#define INC_TIMER_DRIVER_H_

#include "stm32f4xx_hal.h"
#include "Error_Handling.h"

void MX_TIM2_Init(void);
void TIM2_Start_Timer();
void TIM2_Stop_Timer();
uint32_t TIM2_Get_Counter();

#endif /* INC_TIMER_DRIVER_H_ */
