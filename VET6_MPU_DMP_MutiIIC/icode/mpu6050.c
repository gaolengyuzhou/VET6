/*
 * mpu6050.c
 *
 *  Created on: Nov 14, 2024
 *      Author: gy
 */

#include "mpu6050.h"
#include "mpuiic.h"

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"

#include <stdio.h>
#include "stm32f4xx_hal.h"

/***********************************初始化MPU*********************************************/
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
        return MPU_Write_Byte(MPU6050_ADDR, MPU_GYRO_CFG_REG,fsr<<3);
}

u8 MPU_Set_Accel_Fsr(u8 fsr)
{
        return MPU_Write_Byte(MPU6050_ADDR, MPU_ACCEL_CFG_REG,fsr<<3);
}

u8 MPU_Set_LPF(u16 lpf)
{
        u8 data=0;
        if(lpf>=188)data=1;
        else if(lpf>=98)data=2;
        else if(lpf>=42)data=3;
        else if(lpf>=20)data=4;
        else if(lpf>=10)data=5;
        else data=6;
        return MPU_Write_Byte(MPU6050_ADDR, MPU_CFG_REG,data);
}

u8 MPU_Set_Rate(u16 rate)
{
        u8 data;
        if(rate>1000)rate=1000;
        if(rate<4)rate=4;
        data=1000/rate-1;
        data=MPU_Write_Byte(MPU6050_ADDR, MPU_SAMPLE_RATE_REG,data);
        return MPU_Set_LPF(rate/2);
}

uint8_t MPU6050_Init(void)
{
        u8 res;
        MPU_Write_Byte(MPU6050_ADDR, MPU_PWR_MGMT1_REG,0x80);//复位从机
          int i=0,j=0;
          //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
          for(i=0;i<1000;i++)
          {
                for(j=0;j<1000;j++)
                {
                  ;
                }
          }
//        HAL_Delay(100);
        MPU_Write_Byte(MPU6050_ADDR, MPU_PWR_MGMT1_REG,0x00);//唤醒从机
        MPU_Set_Gyro_Fsr(3);//陀螺仪传感器 +-2000dps
        MPU_Set_Accel_Fsr(0);//加速度传感器 +-2g
        MPU_Set_Rate(50);
        MPU_Write_Byte(MPU6050_ADDR, MPU_INT_EN_REG,0x00);//关闭所有中断
        MPU_Write_Byte(MPU6050_ADDR, MPU_USER_CTRL_REG,0x00);//IIC主模式关闭
        MPU_Write_Byte(MPU6050_ADDR, MPU_FIFO_EN_REG,0x00);//关闭FIFO
        MPU_Write_Byte(MPU6050_ADDR, MPU_INTBP_CFG_REG,0x80);//INT低电平有效
        MPU_Read_Byte(MPU6050_ADDR, MPU_DEVICE_ID_REG, &res);
        if(res == MPU6050_ID)
        {
                MPU_Write_Byte(MPU6050_ADDR, MPU_PWR_MGMT1_REG,0x01);//设置CLKSEL PLL X轴为参考
                MPU_Write_Byte(MPU6050_ADDR, MPU_PWR_MGMT2_REG,0x00);//加速度计和陀螺仪使能
                MPU_Set_Rate(50);
        }
        else
                return 1;
        return 0;
}


/***********************************初始化DMP*********************************************/
/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};


static int run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x3) {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
    } else {
        return -1;
    }

    return 0;
}

int MPU6050_DMP_init(void)
{
    int res;
    struct int_param_s int_param;
        if(mpu_init(&int_param)==0)        //初始化MPU6050
        {
                res=mpu_set_sensors(INV_XYZ_GYRO|INV_XYZ_ACCEL);//设置所需要的传感器
                if(res)return 1;
                res=mpu_configure_fifo(INV_XYZ_GYRO|INV_XYZ_ACCEL);//设置FIFO
                if(res)return 2;
                res=mpu_set_sample_rate(DEFAULT_MPU_HZ);        //设置采样率
                if(res)return 3;
                res=dmp_load_motion_driver_firmware();                //加载dmp固件
                if(res)return 4;
                res=dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));//设置陀螺仪方向
                if(res)return 5;
                res=dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT|DMP_FEATURE_TAP|        //设置dmp功能
                    DMP_FEATURE_ANDROID_ORIENT|DMP_FEATURE_SEND_RAW_ACCEL|DMP_FEATURE_SEND_CAL_GYRO|
                    DMP_FEATURE_GYRO_CAL);
                if(res)return 6;
                res=dmp_set_fifo_rate(DEFAULT_MPU_HZ);        //设置DMP输出速率(最大不超过200Hz)
                if(res)return 7;
                res=run_self_test();                //自检
                if(res)return 8;
                res=mpu_set_dmp_state(1);        //使能DMP
                if(res)return 9;
        }else return 10;
        return 0;
}
/***********************************初始化函数*********************************************/
void MPU6050_DMP_InitFunc(void)
{
        u8 device_id = 0;
        u8 MPU_Res;
        u8 mpu_id = 0;
        HAL_Delay(500);

        do
        {
                MPU_Read_Len(MPU6050_ADDR, MPU_DEVICE_ID_REG, 1, &mpu_id);
                printf("MPU6050 Device ID: 0x%02X\r\n", mpu_id);
                HAL_Delay(1000);
        }while(device_id == MPU6050_ID);

        do
        {
                MPU_Res = MPU6050_Init();
            printf("MPU6050 init, ret = %d\r\n", MPU_Res);
            HAL_Delay(100);
        } while(MPU_Res);

        while(1)
        {MPU_Res = MPU6050_DMP_init();
                if(MPU_Res)
                {
                        printf("DMP Error:%d\r\n",MPU_Res);        HAL_Delay(500);
                }
                else
                        break;
        }
        printf("MPU_DMP Init Succeed\r\n");
}

/***********************************数据读取函数*********************************************/
u8 MPU6050_DMP_Get_Date(float *pitch,float *roll,float *yaw)
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
                q0 = quat[0] / q30;        //q30格式转换为浮点数
                q1 = quat[1] / q30;
                q2 = quat[2] / q30;
                q3 = quat[3] / q30;
                //计算得到俯仰角/横滚角/航向角
                *pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;        // pitch
                *roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;        // roll
                *yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;        //yaw
        }else return 2;
        return 0;
}

