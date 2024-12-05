/*
 * mpu9250.c
 *
 *  Created on: Nov 11, 2024
 *      Author: gy
 */
#include <mpu9250.h>

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"

#include "mpl.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "data_builder.h"
#include "eMPL_outputs.h"
#include "mltypes.h"
#include "mpu.h"
#include "log.h"
#include "packet.h"

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

#include <stdio.h>
#include "stm32f4xx_hal.h"

/*三路IIC*/
MPUData mpu1Data = {0.0f, 0.0f, 0.0f, 0};
MPUData mpu2Data = {0.0f, 0.0f, 0.0f, 0};
MPUData mpu3Data = {0.0f, 0.0f, 0.0f, 0};

MPUSENSOR_Type mpu = MPU1;
/*三路IIC*/

/********************************************************************************************/
/* 							PART1: MPU9250 初始化配置										*/
/********************************************************************************************/
/* 设置MPU9250陀螺仪传感器满量程范围					*/
/* fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps	*/
/* 返回值:0,设置成功									*/
/*    其他,设置失败 									*/
uint8_t MPU9250_SetGyroFsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU6500_ADDR, MPU_GYRO_CFG_REG, fsr<<3);	//设置陀螺仪满量程范围
}

/* 设置MPU92506050加速度传感器满量程范围	*/
/* fsr:0,±2g;1,±4g;2,±8g;3,±16g		*/
/* 返回值:0,设置成功						*/
/*    其他,设置失败 						*/
uint8_t MPU9250_SetAccelFsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU6500_ADDR, MPU_ACCEL_CFG_REG, fsr<<3);	//设置加速度传感器满量程范围
}

/* 设置MPU92506050的数字低通滤波器		*/
/* lpf:数字低通滤波频率(Hz)				*/
/* 返回值:0,设置成功						*/
/*    其他,设置失败 						*/
uint8_t MPU9250_SetLPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)
		data=1;
	else if(lpf>=98)
		data=2;
	else if(lpf>=42)
		data=3;
	else if(lpf>=20)
		data=4;
	else if(lpf>=10)
		data=5;
	else
		data=6;
	return MPU_Write_Byte(MPU6500_ADDR, MPU_CFG_REG, data);			//设置数字低通滤波器
}

/* 设置MPU9250的采样率(假定Fs=1KHz)		*/
/* rate:4~1000(Hz)						*/
/* 返回值:0,设置成功						*/
/*    其他,设置失败 						*/
uint8_t MPU9250_SetRate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)
		rate=1000;
	if(rate<4)
		rate=4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU6500_ADDR, MPU_SAMPLE_RATE_REG, data);	//设置数字低通滤波器
 	return MPU9250_SetLPF(rate / 2);											//自动设置LPF为采样率的一半
}

uint8_t MPU9250_Init(void)
{
	uint8_t res = 0;
	MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0x80);//复位MPU9250
	HAL_Delay(100);
	MPU_Write_Byte(MPU9250_ADDR, MPU_PWR_MGMT2_REG, 0x00);//唤醒MPU9250
	MPU9250_SetGyroFsr(MPU9250_GYRO_FSR);					//陀螺仪传感器,±2000dps
	MPU9250_SetAccelFsr(MPU9250_ACCEL_FSR);				//加速度传感器,±2g
	MPU9250_SetRate(MPU9250_RATE);							//设置采样率50Hz
	MPU_Write_Byte(MPU9250_ADDR, MPU_INT_EN_REG, 0x00);		//关闭所有中断
	MPU_Write_Byte(MPU9250_ADDR, MPU_USER_CTRL_REG, 0x00);	//I2C主模式关闭
	MPU_Write_Byte(MPU9250_ADDR, MPU_FIFO_EN_REG, 0x00);	//关闭FIFO
	MPU_Write_Byte(MPU9250_ADDR, MPU_INTBP_CFG_REG, 0x82);	//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
	MPU_Read_Byte(MPU9250_ADDR, MPU_DEVICE_ID_REG, &res);	//读取MPU6500的ID
	if (res == MPU6500_ID)
	{
		MPU_Write_Byte(MPU6500_ADDR, MPU_PWR_MGMT1_REG, 0x01);//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU6500_ADDR, MPU_PWR_MGMT2_REG, 0x00);
	}
	else
	{
		printf("MPU9250 Init failed 1, res = %X\r\n", res);
		return 1;
	}
	MPU_Read_Byte(AK8963_ADDR, MAG_WIA,&res);
	if (res == AK8963_ID)
	{
		MPU_Write_Byte(AK8963_ADDR, MAG_CNTL1, 0x11);		// 连续测量模式
		printf("MPU9250 Init success.\r\n");
	}
	else
	{

		printf("MPU9250 Init failed 2, res = %X\r\n", res);
		return 1;
	}
	return 0;
}


/********************************************************************************************/
/* 							PART2: DMP 初始化配置										*/
/********************************************************************************************/
//q30，q16格式,long转float时的除数.
#define q30  1073741824.0f
#define q16  65536.0f

//陀螺仪方向设置
static signed char gyro_orientation[9] = { 1, 0, 0,
                                           0, 1, 0,
                                           0, 0, 1};
//磁力计方向设置
static signed char comp_orientation[9] = { 0, 1, 0,
                                           1, 0, 0,
                                           0, 0,-1};
//MPU9250自测试
//返回值:0,正常
//    其他,失败
u8 run_self_test(void)
{
	int result;
	//char test_packet[4] = {0};
	long gyro[3], accel[3];
	result = mpu_run_6500_self_test(gyro, accel,1);
	if (result == 0x7)
	{
		/* Test passed. We can trust the gyro data here, so let's push it down
		* to the DMP.
		*/
        unsigned short accel_sens;
		float gyro_sens;

		mpu_get_gyro_sens(&gyro_sens);
		gyro[0] = (long)(gyro[0] * gyro_sens);
		gyro[1] = (long)(gyro[1] * gyro_sens);
		gyro[2] = (long)(gyro[2] * gyro_sens);
        //inv_set_gyro_bias(gyro, 3);
		dmp_set_gyro_bias(gyro);
		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
       // inv_set_accel_bias(accel, 3);
		dmp_set_accel_bias(accel);
		return 0;
	}else return 1;
}

//mpu9250,dmp初始化
//返回值:0,正常
//    其他,失败
u8 mpu_dmp_init(void)
{
	u8 res=0;
    struct int_param_s int_param;
    unsigned char accel_fsr;
    unsigned short gyro_rate, gyro_fsr;
    unsigned short compass_fsr;

	if(mpu_init(&int_param)==0)	//初始化MPU9250
	{
        res=inv_init_mpl();     //初始化MPL
        if(res)return 1;
        inv_enable_quaternion();
        inv_enable_9x_sensor_fusion();
        inv_enable_fast_nomot();
        inv_enable_gyro_tc();
        inv_enable_vector_compass_cal();
        inv_enable_magnetic_disturbance();
        inv_enable_eMPL_outputs();
        res=inv_start_mpl();    //开启MPL
        if(res)return 1;
		res=mpu_set_sensors(INV_XYZ_GYRO|INV_XYZ_ACCEL|INV_XYZ_COMPASS);//设置所需要的传感器
		if(res)return 2;
		res=mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);   //设置FIFO
		if(res)return 3;
		res=mpu_set_sample_rate(DEFAULT_MPU_HZ);	            //设置采样率
		if(res)return 4;
        res=mpu_set_compass_sample_rate(1000/COMPASS_READ_MS);  //设置磁力计采样率
        if(res)return 5;
        mpu_get_sample_rate(&gyro_rate);
        mpu_get_gyro_fsr(&gyro_fsr);
        mpu_get_accel_fsr(&accel_fsr);
        mpu_get_compass_fsr(&compass_fsr);
        inv_set_gyro_sample_rate(1000000L/gyro_rate);
        inv_set_accel_sample_rate(1000000L/gyro_rate);
        inv_set_compass_sample_rate(COMPASS_READ_MS*1000L);
        inv_set_gyro_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_orientation),(long)gyro_fsr<<15);
        inv_set_accel_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_orientation),(long)accel_fsr<<15);
        inv_set_compass_orientation_and_scale(
            inv_orientation_matrix_to_scalar(comp_orientation),(long)compass_fsr<<15);


		res=dmp_load_motion_driver_firmware();		             //加载dmp固件
		if(res)return 6;
		res=dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));//设置陀螺仪方向
		if(res)return 7;
		res=dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT|DMP_FEATURE_TAP|	            //设置dmp功能
		    DMP_FEATURE_ANDROID_ORIENT|DMP_FEATURE_SEND_RAW_ACCEL|DMP_FEATURE_SEND_CAL_GYRO|
		    DMP_FEATURE_GYRO_CAL);
		if(res)return 8;
		res=dmp_set_fifo_rate(DEFAULT_MPU_HZ);	//设置DMP输出速率(最大不超过200Hz)
		if(res)return 9;
		res=run_self_test();		//自检
		if(res)return 10;
		res=mpu_set_dmp_state(1);	//使能DMP
		if(res)return 11;
	}
	return 0;
}

void MPU9250_DMP_InitFunc(void)
{
	u8 device_id;
	u8 MPU_Res;
	u8 mpu_id = 0;
	HAL_Delay(500);
	MPU_Res = MPU_Read_Len(MPU9250_ADDR, MPU_DEVICE_ID_REG, 1, &mpu_id);	// 判断IIC实物接线是否有问题
	HAL_Delay(1000);
	if(MPU_Res)																// 同时需要判断MPU9250
	{																		// AD0引脚是否接错
		printf("Read Error:%d\r\n",MPU_Res);
		while(1);
	}
	printf("MPU9250 Device ID: 0x%02X\n", mpu_id);
	while(device_id != 0x71)
	{	HAL_Delay(1000);
		MPU_Res = MPU_Read_Len(MPU9250_ADDR, MPU_DEVICE_ID_REG, 1, &device_id);
		printf("MPU9250 Device ID: 0x%02X\r\n", mpu_id);
	}
	while(1)
	{MPU_Res = mpu_dmp_init();
		if(MPU_Res)
		{
			printf("DMP Error:%d\r\n",MPU_Res);	HAL_Delay(500);
		}
		else
			break;
	}
	printf("MPU_DMP Init Succeed\r\n");
}

//得到dmp处理后的数据(注意,本函数需要比较多堆栈,局部变量有点多)
//pitch:俯仰角 精度:0.1°   范围:-90.0° <---> +90.0°
//roll:横滚角  精度:0.1°   范围:-180.0°<---> +180.0°
//yaw:航向角   精度:0.1°   范围:-180.0°<---> +180.0°
//返回值:0,正常
//    其他,失败
u8 mpu_dmp_get_data(float *pitch,float *roll,float *yaw)
{
	float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
	unsigned long sensor_timestamp;
	short gyro[3], accel[3], sensors;
	unsigned char more;
	long quat[4];
	if(dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more))return 1;
	/* Gyro and accel data are written to the FIFO by the DMP in chip frame and hardware units.
	 * This behavior is convenient because it keeps the gyro and accel outputs of dmp_read_fifo and mpu_read_fifo consistent.
	**/
	/*if (sensors & INV_XYZ_GYRO )
	send_packet(PACKET_TYPE_GYRO, gyro);
	if (sensors & INV_XYZ_ACCEL)
	send_packet(PACKET_TYPE_ACCEL, accel); */
	/* Unlike gyro and accel, quaternions are written to the FIFO in the body frame, q30.
	 * The orientation is set by the scalar passed to dmp_set_orientation during initialization.
	**/
	if(sensors&INV_WXYZ_QUAT)
	{
		q0 = quat[0] / q30;	//q30格式转换为浮点数
		q1 = quat[1] / q30;
		q2 = quat[2] / q30;
		q3 = quat[3] / q30;
		//计算得到俯仰角/横滚角/航向角
		*pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;	// pitch
		*roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;	// roll
		*yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
	}else return 2;
	return 0;
}

//得到mpl处理后的数据(注意,本函数需要比较多堆栈,局部变量有点多)
//pitch:俯仰角 精度:0.1°   范围:-90.0° <---> +90.0°
//roll:横滚角  精度:0.1°   范围:-180.0°<---> +180.0°
//yaw:航向角   精度:0.1°   范围:-180.0°<---> +180.0°
//返回值:0,正常
//    其他,失败
u8 mpu_mpl_get_data(float *pitch,float *roll,float *yaw)
{
	unsigned long sensor_timestamp,timestamp;
	short gyro[3], accel_short[3],compass_short[3],sensors;
	unsigned char more;
	long compass[3],accel[3],quat[4],temperature;
    long data[9];
    int8_t accuracy;

	if(dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors,&more))return 1;

    if(sensors&INV_XYZ_GYRO)
    {
        inv_build_gyro(gyro,sensor_timestamp);          //把新数据发送给MPL
        mpu_get_temperature(&temperature,&sensor_timestamp);
        inv_build_temp(temperature,sensor_timestamp);   //把温度值发给MPL，只有陀螺仪需要温度值
    }

    if(sensors&INV_XYZ_ACCEL)
    {
        accel[0] = (long)accel_short[0];
        accel[1] = (long)accel_short[1];
        accel[2] = (long)accel_short[2];
        inv_build_accel(accel,0,sensor_timestamp);      //把加速度值发给MPL
    }

    if (!mpu_get_compass_reg(compass_short, &sensor_timestamp))
    {
        compass[0]=(long)compass_short[0];
        compass[1]=(long)compass_short[1];
        compass[2]=(long)compass_short[2];
        inv_build_compass(compass,0,sensor_timestamp); //把磁力计值发给MPL
    }
    inv_execute_on_data();
    inv_get_sensor_type_euler(data,&accuracy,&timestamp);

    *roll  = (data[0]/q16);
    *pitch = (data[1]/q16);
    *yaw   = data[2] / q16;
	return 0;
}


/********************************************************************************************/
/* 							PART3: MPU 原始数据										*/
/********************************************************************************************/
/**
  * @brief   读取MPU6050的ID
  * @param
  * @retval  正常返回1，异常返回0
  */
uint8_t MPU9250ReadID(void)
{
    unsigned char Re = 0;
    MPU_Read_Len(MPU9250_ADDR, MPU_DEVICE_ID_REG,1,&Re);    //读器件地址
     if (Re != 0x68) {
         printf("检测不到MPU9250模块，请检查模块与开发板的接线");
         return 0;
     } else {
         printf("MPU9250 ID = %d\r\n",Re);
         return 1;
     }

 }

//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    u8 buf[2];
    short raw;
	float temp;
	MPU_Read_Len(MPU9250_ADDR,MPU_TEMP_OUTH_REG,2,buf);
    raw=((u16)buf[0]<<8)|buf[1];
    temp=21+((double)raw)/333.87;
    return temp*100;;
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;
	res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];
		*gy=((u16)buf[2]<<8)|buf[3];
		*gz=((u16)buf[4]<<8)|buf[5];
	}
    return res;;
}

//得到加速度值(原始值)
//ax,ay,az:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;
	res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];
		*ay=((u16)buf[2]<<8)|buf[3];
		*az=((u16)buf[4]<<8)|buf[5];
	}
    return res;;
}

//得到磁力计值(原始值)
//mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    u8 buf[6],res;
	res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res==0)
	{
		*mx=((u16)buf[1]<<8)|buf[0];
		*my=((u16)buf[3]<<8)|buf[2];
		*mz=((u16)buf[5]<<8)|buf[4];
	}
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963每次读完以后都需要重新设置为单次测量模式
    return res;;
}





float acc_[3];
float gyro_[3];
short mag_[3];
/********************************************************************************************/
/* 							PART4: 校准函数													*/
/********************************************************************************************/

/* 校准加速度 			*/
/* 采用简单的六面校准 	*/

void CaliAccelData(float acc[3])
{
	static const float acc_offset[3] = {0.0144, 0.0311, 1.3100};		// 多次测量加速度计后估计的偏移值
	float acc_norm;											// 加速度模值
	const float g = 9.8;									// 重力加速度

	acc[0] = -acc[0] - acc_offset[0];						// 去除偏移，此处添加负号使得与机体坐标系一致
	acc[1] = acc[1] - acc_offset[1];
	acc[2] = acc[2] - acc_offset[2];

	acc_norm = sqrt(acc[0]*acc[0] + acc[1]*acc[1] + acc[2]*acc[2]);
	acc[0] = acc[0] / acc_norm * g;							// 归一化到 [-g, g]
	acc[1] = acc[1] / acc_norm * g;
	acc[2] = acc[2] / acc_norm * g;
}


/* 校准陀螺仪 	*/
/* 去除零偏		*/
void CaliGyroData(float gyro[3])
{
	static const float gyro_offset[3] = {0.685492,  -0.836520,  0.721489};	// 测量陀螺仪数据后估计的陀螺仪偏移值

	gyro[0] = -(gyro[0] - gyro_offset[0]);
	gyro[1] = -(gyro[1] - gyro_offset[1]);
	gyro[2] = -(gyro[2] - gyro_offset[2]);
}

/* 校准磁力计	*/
/* 校准零偏		*/
void CaliMagData(short mag[3])
{
	static const short mag_offset[3] = {-80,  196, -101};		// 校准磁力计使用的偏移，多次测量计算而得

	mag[0] -= mag_offset[0];
	mag[1] -= mag_offset[1];
	mag[2] -= mag_offset[2];
}
/********************************************************************************************/
/* 							PART5: IIC读取数据任务											*/
/********************************************************************************************/

/* 获取加速度数据，获取加速度		*/

void GetAccelTask(void *arg)
{
	uint8_t buf[6], res;
	short raw_acc[3];				// 原始的加速度数据，此处必须用short, 取值范围 -32767~32736，占2字节。int取值范围也相同，但是占4字节
	float fsr;						// 满量程
	const float g = 9.8;

	while (1)
	{
		res = MPU_Read_Len(MPU6500_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);

		if(res==0)
		{
			raw_acc[0] = ((uint16_t)buf[0]<<8) | buf[1];  			// x 方向加速度原始值
			raw_acc[1] = ((uint16_t)buf[2]<<8) | buf[3];  			// y 方向加速度原始值
			raw_acc[2] = ((uint16_t)buf[4]<<8) | buf[5];  			// z 方向加速度原始值
		}

		if (MPU9250_ACCEL_FSR == MPU9250_ACCEL_FSR_2g)						// 选择量程
			fsr = 2.0 * g;
		else if (MPU9250_ACCEL_FSR == MPU9250_ACCEL_FSR_4g)
			fsr = 4.0 * g;
		else if (MPU9250_ACCEL_FSR == MPU9250_ACCEL_FSR_8g)
			fsr = 8.0 * g;
		else if (MPU9250_ACCEL_FSR == MPU9250_ACCEL_FSR_16g)
			fsr = 16.0 * g;

		acc_[0] = (raw_acc[0]) / 100.0 * (fsr / 327.67);		// x 方向加速度，dof9 于 attitude.c 定义
		acc_[1] = (raw_acc[1]) / 100.0 * (fsr / 327.67);		// y 方向加速度
		acc_[2] = (raw_acc[2]) / 100.0 * (fsr / 327.67);		// z 方向加速度

		CaliAccelData(acc_);									// 校准

		HAL_Delay(20);
	}
}

/* 获取陀螺仪数据，获取角速度					*/
/* 逆时针旋转角速度为正，顺时针旋转角速度为负	*/

void GetGyroTask(void *arg)
{
	uint8_t buf[6], res;
	float fsr;						// Full Scale Range 满量程
	short raw_gyro[3];				// 原始的陀螺仪数据，此处必须用short, 取值范围 -32767~32736，占2字节。int取值范围也相同，但是占4字节

	while (1)
	{
		res = MPU_Read_Len(MPU6500_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
		if(res==0)
		{
			raw_gyro[0] = ((uint16_t)buf[0]<<8) | buf[1];  			// 原始 gyro x
			raw_gyro[1] = ((uint16_t)buf[2]<<8) | buf[3];  			// 原始 gyro y
			raw_gyro[2] = ((uint16_t)buf[4]<<8) | buf[5];			// 原始 gyro z
		}

		if (MPU9250_GYRO_FSR == MPU9250_GYRO_FSR_250)
			fsr = 250.0;
		else if (MPU9250_GYRO_FSR == MPU9250_GYRO_FSR_500)
			fsr = 500.0;
		else if (MPU9250_GYRO_FSR == MPU9250_GYRO_FSR_1000)
			fsr = 1000.0;
		else if (MPU9250_GYRO_FSR == MPU9250_GYRO_FSR_2000)
			fsr = 2000.0;

		gyro_[0] = (float)raw_gyro[0] * (fsr / 3276.70) / -10.0;// 添加负号，使得绕 x 轴逆时针转动为正
		gyro_[1] = (float)raw_gyro[1] * (fsr / 3276.70) / 10.0;	// 乘以 fsr 除以 32767，为了乘积太大引起误差，先除以3276.7再除以10.0。
		gyro_[2] = (float)raw_gyro[2] * (fsr / 3276.70) / 10.0;

		CaliGyroData(gyro_);									// 校准
		HAL_Delay(20);
	}
}

/* 获取磁力计数据任务							*/
/* mag[0]~mag[2] 分别为x,z,y方向16位磁力计数据	*/
/* !!! 是 x,z,y方向数据，不是 x,y,z 方向，因为HMC5883读取数据顺序就是 x,z,y !!!*/
/* short为2字节，读取的buf[0],buf[1]也是两字节。int为四字节，(int)65534=65534, (short)65534=-2*/
void GetMagTask(void *arg)	// 获取磁力计原始数据
{
	uint8_t buf[6] ={0}, res;
	while (1)
	{
		res = MPU_Read_Len(AK8963_ADDR, MAG_XOUT_L, 6, buf);	// IIC读取磁力计原始数据

		if (res == 0)
		{
			mag_[0] = ((uint16_t)buf[1]<<8) | buf[0];		// 磁力计 x方向原始数据
			mag_[1] = ((uint16_t)buf[3]<<8) | buf[2];		// 磁力计 y方向原始数据
			mag_[2] = ((uint16_t)buf[5]<<8) | buf[4];		// 磁力计 z方向原始数据
		}
		MPU_Write_Byte(AK8963_ADDR, MAG_CNTL1, 0x11);		// 单次测量模式

		CaliMagData(mag_);

		HAL_Delay(20);
	}
}

/* 得到温度值			*/
/* 返回值:温度值（°C）	*/
/* 此处为使用到			*/
float GetAccelTemperature(void)
{
    uint8_t buf[2];
    short raw_tmp;		// 温度的原始数据
	float tmp;			// 温度值
	MPU_Read_Len(MPU6500_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
    raw_tmp = ((uint16_t)buf[0]<<8) | buf[1];
    tmp = 36.53 + ((double)raw_tmp) / 340.0;
    return tmp;
}

/*********************************************-***********************************************/
/* 							PART6: 外部接口													*/
/********************************************************************************************/
/* 获取磁力计数据接口	*/
void GetMagDataApi(short mag[3])
{
	mag[0] = mag_[0];
	mag[1] = mag_[1];
	mag[2] = mag_[2];
}

/* 获取三轴加速度接口	*/
void GetAccelDataApi(float acc[3])
{
	acc[0] = acc_[0];
	acc[1] = acc_[1];
	acc[2] = acc_[2];
}

/* 获取三轴角速度接口	*/
void GetGyroDataApi(float gyro[3])
{
	gyro[0] = gyro_[0];
	gyro[1] = gyro_[1];
	gyro[2] = gyro_[2];
}
