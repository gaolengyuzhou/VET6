/*
 * control.c
 *
 *  Created on: Oct 24, 2024
 *      Author: gy
 */

#include <stdint.h>
#include <stdio.h>
#include "control.h"
#include "usart.h"
#include "RS485.h"

int8_t motorTemperature = 0;
int16_t motorPowerOrTorque = 0;
int16_t motorSpeed = 0;
uint16_t motorEncoder = 0;

/*********************************************************************************
* @brief        打包发送的数据
* @param        None
* @retval        None
* 这个函数接收一个缓冲区指针buffer、命令cmd、设备 IDid、数据长度size和数据指针data。
*********************************************************************************/
void control_PackCmd(uint8_t *buffer, uint8_t cmd, uint8_t id, uint8_t size, uint8_t *data)
{
        uint8_t i = 0;
        //首先，它将命令头、命令、设备 ID 和数据长度依次填入缓冲区的前四个位置。
        buffer[0] = CMD_HEAD;
        buffer[1] = cmd;
        buffer[2] = id;
        buffer[3] = size;
        //然后计算前四个字节的校验和，存储在缓冲区的第五个位置。
        buffer[4] = 0;        // 需要先清0
        for (i=0; i<4; i++)
                buffer[4] += buffer[i];
        //如果数据长度不为 0，将数据复制到缓冲区的后续位置，并计算数据的校验和，存储在缓冲区的最后一个位置。
        if (size != 0)
        {
                buffer[LEAST_FRAME_SIZE+size] = 0;// 需要先清0
                for (i=0; i<size; i++)        // 复制数据并计算校验值
                {
                        buffer[LEAST_FRAME_SIZE+i] = data[i];
                        buffer[LEAST_FRAME_SIZE+size] += buffer[LEAST_FRAME_SIZE+i];
                }
                //最后设置需要发送的数据总长度uart1TxDataSize。
                uart2TxDataSize = i + LEAST_FRAME_SIZE + 1;        // 需要发送的数据总长度
        }
        else
                uart2TxDataSize = LEAST_FRAME_SIZE ;        // 需要发送的数据总长度
}

/*********************************************************************************
* @brief	检查电机回复数据
* @param	None
* @retval	None
*********************************************************************************/
uint8_t control_CheckReceivedData(void)
{
	uint8_t receiveSuccess = 0;
	uint8_t temp = 0;
	uint8_t i = 0;

	HAL_UART_Receive(&huart2, uart2RxBuffer, uart2RxDataSize,1000);

	if (uart2RxBuffer[0] == CMD_HEAD)
	{
		temp = uart2RxBuffer[0] + uart2RxBuffer[1] + uart2RxBuffer[2] + uart2RxBuffer[3];
		if (uart2RxBuffer[4] == temp)
		{
			temp = uart2RxBuffer[5] + uart2RxBuffer[6] + uart2RxBuffer[7] + uart2RxBuffer[8] + uart2RxBuffer[9] + uart2RxBuffer[10] + uart2RxBuffer[11];
			if (uart2RxBuffer[12] == temp)
			{
				motorTemperature = (int8_t)uart2RxBuffer[5];
				motorPowerOrTorque = (int16_t)(uart2RxBuffer[6] + (uart2RxBuffer[7]<<8));
				motorSpeed = (int16_t)(uart2RxBuffer[8] + (uart2RxBuffer[9]<<8));
				motorEncoder = (int16_t)(uart2RxBuffer[10] + (uart2RxBuffer[11]<<8));
				receiveSuccess = 1;
			}
		}
	}

	for (i=0; i<uart2RxDataSize; i++)
		uart2RxBuffer[i] = 0;
	uart2RxDataSize = 0;

	printf("motorTemperature = %d, motorPowerOrTorque = %d, motorSpeed = %d, motorEncoder = %d\n", motorTemperature, motorPowerOrTorque, motorSpeed, motorEncoder);

	return receiveSuccess;
}

/*********************************************************************************
* @brief        发送控制命令
* @param        None
* @retval        None
*********************************************************************************/
void control_Send(uint8_t cmd, uint8_t id, int16_t value)
{
        uint8_t dataSize = 0;        // 命令数据长度
        int16_t openCtlData = 0;     // 开环控制数据
        int16_t torqueCtlData = 0;   // 力矩环控制数据
        int16_t speedCtlData = 0;    // 速度环控制数据
        int16_t angleCtlData = 0;    // 位置环控制数据

        if (cmd == CMD_OPEN_CONTROL)
        {
                openCtlData = value;
                dataSize = 2;

                control_PackCmd(uart2TxBuffer, cmd, id, dataSize, (uint8_t *)&openCtlData);
                uart2RxDataSize = LEAST_FRAME_SIZE + 7 + 1;
                RS485_SendPacket(uart2TxBuffer, uart2TxDataSize);
                control_CheckReceivedData();
        }
        else if (cmd == CMD_TORQUE_CONTROL)
        {
                torqueCtlData = value;
                dataSize = 2;

                control_PackCmd(uart2TxBuffer, cmd, id, dataSize, (uint8_t *)&torqueCtlData);
                uart2RxDataSize = LEAST_FRAME_SIZE + 7 + 1;
                RS485_SendPacket(uart2TxBuffer, uart2TxDataSize);
                control_CheckReceivedData();
        }
        else if (cmd == CMD_SPEED_CONTROL)
        {
                speedCtlData = value;
                dataSize = 4;

                control_PackCmd(uart2TxBuffer, cmd, id, dataSize, (uint8_t *)&speedCtlData);
                uart2RxDataSize = LEAST_FRAME_SIZE + 7 + 1;
                RS485_SendPacket(uart2TxBuffer, uart2TxDataSize);
                control_CheckReceivedData();
        }
        else if (cmd == CMD_ANGLE_CONTROL1)
        {
                angleCtlData = value;
                dataSize = 8;

                control_PackCmd(uart2TxBuffer, cmd, id, dataSize, (uint8_t *)&angleCtlData);
                uart2RxDataSize = LEAST_FRAME_SIZE + 7 + 1;
                RS485_SendPacket(uart2TxBuffer, uart2TxDataSize);
                control_CheckReceivedData();
        }
}

