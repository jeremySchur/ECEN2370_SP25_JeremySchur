/*
 * Gyro_Driver.c
 *
 *  Created on: Apr 27, 2025
 *      Author: jeremyschur
 */

#include "Gyro_Driver.h"

static SPI_HandleTypeDef hspi5;
static HAL_StatusTypeDef gyroHALStatus;

void Gyro_Init(){
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_SPI5_CLK_ENABLE();

	GPIO_InitTypeDef gpiof_pin;
	gpiof_pin.Pin = GYRO_MISO_Pin | GYRO_MOSI_Pin | GYRO_SCK_Pin;
	gpiof_pin.Mode = GPIO_MODE_AF_PP;
	gpiof_pin.Pull = GPIO_NOPULL;
	gpiof_pin.Speed = GPIO_SPEED_FREQ_LOW;
	gpiof_pin.Alternate = GPIO_AF5_SPI5;
	HAL_GPIO_Init(GPIOF, &gpiof_pin);

	GPIO_InitTypeDef gpioc_pin;
	gpioc_pin.Pin = GPIOC_SPI_NCS_MEMS;
	gpioc_pin.Mode = GPIO_MODE_OUTPUT_PP;
	gpioc_pin.Pull = GPIO_PULLDOWN;
	gpioc_pin.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &gpioc_pin);

	hspi5.Instance = SPI5;
	hspi5.Init.Mode = SPI_MODE_MASTER;
	hspi5.Init.Direction = SPI_DIRECTION_2LINES;
	hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi5.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi5.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi5.Init.NSS = SPI_NSS_SOFT;
	hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi5.Init.CRCPolynomial = 10;
	gyroHALStatus = HAL_SPI_Init(&hspi5);
	Gyro_VerifyHalStatus();

	Gyro_Power();
	HAL_Delay(GYRO_HAL_DELAY);
	Gyro_ConfigureRegisters(CTRL_REG5, GYRO_FIFO_ENABLE);
}

int16_t Gyro_Read_X_Axis(){
	if (Gyro_ReadRegisters(0x27)){
		uint8_t y_low = Gyro_ReadRegisters(OUT_Y_L);
		uint8_t y_high = Gyro_ReadRegisters(OUT_Y_H);

		int16_t y = (int16_t)((y_high << 8) | y_low);
		return y;
	} else {
		return 0;
	}
}

void Gyro_Power(){
	uint16_t dataToSend = 0x1F;

	Gyro_ConfigureRegisters(CTRL_REG1, dataToSend);
}

void Gyro_ConfigureRegisters(uint8_t reg, uint8_t data){
	uint16_t commandToSend = ((data << 8) | (GYRO_WRITE | reg));

	Gyro_EnableSlaveCommunication();
	while (HAL_GPIO_ReadPin(GPIOC, GPIOC_SPI_NCS_MEMS) != GPIO_PIN_RESET);
	gyroHALStatus = HAL_SPI_Transmit(&hspi5, (uint8_t *) &commandToSend, 2, TESTING_TIMEOUT);

	Gyro_VerifyHalStatus();

	Gyro_DisableSlaveCommunication();
}

uint16_t Gyro_ReadRegisters(uint8_t reg){
	uint8_t commandToSend = (GYRO_READ | reg);
	uint16_t recievedData = 0x0000;

	Gyro_EnableSlaveCommunication();
	while (HAL_GPIO_ReadPin(GPIOC, GYRO_MISO_Pin) != GPIO_PIN_RESET);
	gyroHALStatus = HAL_SPI_TransmitReceive(&hspi5, &commandToSend, (uint8_t *) &recievedData, 2, TESTING_TIMEOUT);

	Gyro_VerifyHalStatus();

	uint16_t dataReturned = (0xFF00 & recievedData) >> 8;
	Gyro_DisableSlaveCommunication();
	return dataReturned;
}

void Gyro_VerifyHalStatus(){
	if (gyroHALStatus != HAL_OK) {
		Error_Handler();
	}
}

void Gyro_EnableSlaveCommunication(){
	HAL_GPIO_WritePin(GPIOC, GPIOC_SPI_NCS_MEMS, GPIO_PIN_RESET);
}

void Gyro_DisableSlaveCommunication(){
	HAL_GPIO_WritePin(GPIOC, GPIOC_SPI_NCS_MEMS, GPIO_PIN_SET);
}
