/*
 * 485.c
 *
 *  Created on: Nov 6, 2024
 *      Author: gy
 */
#include "RS485.h"
#include "usart.h"



void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart2, &Byte, 1, 0xFFFF);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void RS485_SendPacket(uint8_t *packet, uint16_t length)
{
    Serial_SendArray(packet, length);
}

