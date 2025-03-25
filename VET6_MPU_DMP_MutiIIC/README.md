# STM32F407ZGT6读取多路MPU9250


#### 使用说明
```
/*三路IIC*/
typedef struct {
    float Pitch_9AX;
    float Roll_9AX;
    float Yaw_9AX;
    short Temp_9AX;
} MPUData;

extern MPUData mpu1Data;
extern MPUData mpu2Data;
extern MPUData mpu3Data;

typedef enum {
    MPU1 = 1,
    MPU2 = 2,
    MPU3 = 3
} MPUSENSOR_Type;

extern MPUSENSOR_Type mpu;
/*三路IIC*/
要是有多路IIC就使用MPUSENSOR_Type来限定是哪一路

在mpuiic添加：

//IIC连续写
//addr:器件地址
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    switch (mpu) {
        case MPU1:
        	return stm32_i2c1_write(addr, reg, buf, len);
            break;
        case MPU2:
			return stm32_i2c2_write(addr, reg, buf, len);
            break;
        case MPU3:
        	return stm32_i2c3_write(addr, reg, buf, len);
            break;
        default:
            break;
    }

}
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    switch (mpu) {
        case MPU1:
        	return stm32_i2c1_read(addr, reg, buf, len);
            break;
        case MPU2:
        	return stm32_i2c2_read(addr, reg, buf, len);
            break;
        case MPU3:
        	return stm32_i2c3_read(addr, reg, buf, len);
            break;
        default:
            break;
    }

}

```
#### 特别说明
```
https://agws5flvrp.feishu.cn/docx/Dc6odU4FCo5GrPxOmrec8li8nCh?from=from_copylink
https://agws5flvrp.feishu.cn/docx/Fkwvd01TbokH9nxDroccqy0Xnpb?from=from_copylink
```

