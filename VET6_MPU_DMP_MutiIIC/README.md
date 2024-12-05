# STM32F407ZGT6读取多路MPU9250

#### 介绍
{**以下是 Gitee 平台说明，您可以替换此简介**
Gitee 是 OSCHINA 推出的基于 Git 的代码托管平台（同时支持 SVN）。专为开发者提供稳定、高效、安全的云端软件开发协作平台
无论是个人、团队、或是企业，都能够用 Gitee 实现代码托管、项目管理、协作开发。企业项目请看 [https://gitee.com/enterprises](https://gitee.com/enterprises)}

#### 软件架构
软件架构说明

1.  xxxx
2.  xxxx
3.  xxxx

#### 使用说明

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


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
