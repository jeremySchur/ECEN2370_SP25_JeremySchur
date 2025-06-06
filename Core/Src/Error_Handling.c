/*
 * Error_Handling.c
 *
 *  Created on: Apr 19, 2025
 *      Author: jeremyschur
 */

#include "Error_Handling.h"

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
