/*
 * mpuiic.h
 *
 *  Created on: Nov 11, 2024
 *      Author: gy
 */

#ifndef MPUIIC_H_
#define MPUIIC_H_

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned char u8;
typedef unsigned char u16;

u8 MPU_Write_Byte(u8 devaddr,u8 reg,u8 data);
u8 MPU_Read_Byte(u8 addr,u8 reg,u8 *buf);

u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);

void mget_ms(unsigned long *time);

#endif /* MPUIIC_H_ */
