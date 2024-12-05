/*
 * control.h
 *
 *  Created on: Oct 24, 2024
 *      Author: gy
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#define CMD_OPEN_CONTROL                              0xA0        // 开环控制MS
#define CMD_TORQUE_CONTROL                            0xA1        // 力矩控制MF MG
#define CMD_SPEED_CONTROL                             0xA2        // 速度控制
#define CMD_ANGLE_CONTROL1                            0xA3        // 位置控制

extern int8_t motorTemperature;
extern int16_t motorPowerOrTorque;
extern int16_t motorSpeed;
extern uint16_t motorEncoder;

// 最小帧长
#define LEAST_FRAME_SIZE					  5		// 最小帧长度 header + cmd + id + dataLength + headCheckSum
// HEAD
#define CMD_HEAD							 0x3E	// 头

void control_Send(uint8_t cmd, uint8_t id, int16_t value);

#endif /* CONTROL_H_ */
