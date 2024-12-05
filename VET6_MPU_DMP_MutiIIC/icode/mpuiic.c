/*
 * mpuiic.c
 *
 *  Created on: Nov 11, 2024
 *      Author: gy
 */
#include "i2c.h"
#include "mpuiic.h"

int8_t stm32_i2c1_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
        HAL_I2C_Mem_Write(&hi2c1, dev_addr>>1, reg_addr, 1, data, len, 100);
        while(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_BUSY);
        return 0;
}

int8_t stm32_i2c1_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
        HAL_I2C_Mem_Read(&hi2c1, dev_addr>>1, reg_addr, 1, data, len, 100);
        return 0;
}

u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    return stm32_i2c1_write(addr, reg, buf, len);
}

u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    return stm32_i2c1_read(addr, reg, buf, len);
}

u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
    return stm32_i2c1_write(addr, reg, &data, 1);
}
u8 MPU_Read_Byte(u8 addr,u8 reg,u8 *buf)
{
	return stm32_i2c1_read(addr, reg, buf, 1);
}

void mget_ms(unsigned long *time)
{
    *time=(unsigned long)HAL_GetTick();
}
