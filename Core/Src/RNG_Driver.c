/*
 * RNG_Driver.c
 *
 *  Created on: Apr 20, 2025
 *      Author: jeremyschur
 */

#include "RNG_Driver.h"

RNG_HandleTypeDef hrng;

/**
 * @brief RNG Initialization Function
 * @param None
 * @retval None
 */
void MX_RNG_Init(void) {

	/* USER CODE BEGIN RNG_Init 0 */

	/* USER CODE END RNG_Init 0 */

	/* USER CODE BEGIN RNG_Init 1 */

	/* USER CODE END RNG_Init 1 */
	hrng.Instance = RNG;
	if (HAL_RNG_Init(&hrng) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN RNG_Init 2 */

	/* USER CODE END RNG_Init 2 */

}

uint8_t get_random_0_to_6(){
    uint32_t random;
    while (HAL_RNG_GenerateRandomNumber(&hrng, &random) != HAL_OK);
    return (uint8_t)(random % 7);
}
