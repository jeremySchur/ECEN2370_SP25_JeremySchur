/*
 * RNG_Driver.h
 *
 *  Created on: Apr 20, 2025
 *      Author: jeremyschur
 */

#ifndef INC_RNG_DRIVER_H_
#define INC_RNG_DRIVER_H_

#include "stm32f4xx_hal.h"
#include "Error_Handling.h"

void MX_RNG_Init(void);
uint8_t get_random_0_to_6();

#endif /* INC_RNG_DRIVER_H_ */
