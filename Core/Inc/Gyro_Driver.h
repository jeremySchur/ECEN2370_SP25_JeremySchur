/*
 * Gyro_Driver.h
 *
 *  Created on: Apr 27, 2025
 *      Author: jeremyschur
 */

#ifndef INC_GYRO_DRIVER_H_
#define INC_GYRO_DRIVER_H_

#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "Error_Handling.h"

#define USE_GYRO 0

#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define CTRL_REG5 0x24
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B


#define GYRO_SCK_Pin GPIO_PIN_7
#define GYRO_MISO_Pin GPIO_PIN_8
#define GYRO_MOSI_Pin GPIO_PIN_9
#define GPIOC_SPI_NCS_MEMS GPIO_PIN_1

#define GYRO_READ (1 << 7)
#define GYRO_WRITE (0 << 7)

#define GYRO_FIFO_ENABLE 0x40
#define TESTING_TIMEOUT 0xFFFFF
#define GYRO_HAL_DELAY 100

#define GYRO_THRESHOLD 500

void Gyro_Init();
int16_t Gyro_Read_X_Axis();
void Gyro_Power();
void Gyro_ConfigureRegisters(uint8_t reg, uint8_t data);
uint16_t Gyro_ReadRegisters(uint8_t reg);
void Gyro_VerifyHalStatus();
void Gyro_EnableSlaveCommunication();
void Gyro_DisableSlaveCommunication();

#endif /* INC_GYRO_DRIVER_H_ */
