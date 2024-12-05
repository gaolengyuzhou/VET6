/*
 * 485.h
 *
 *  Created on: Nov 6, 2024
 *      Author: gy
 */

#ifndef RS485_H_
#define RS485_H_

#include "stm32f4xx_hal.h"

#define RS485_END_FLAG1	'\r'			//RS485一桢数据结束标志1
#define RS485_END_FLAG2	'\n'			//RS485一桢数据结束标志2

void RS485_SendPacket(uint8_t *packet, uint16_t length);

#endif /* RS485_H_ */
