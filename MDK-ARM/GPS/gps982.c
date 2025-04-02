#include "stm32f4xx_hal.h"

#include "gps982.h"
#include "lcd_spi_154.h"
#include "usart.h"

ReceDataFrame receDataFrame;

uint8_t gUart1RcecBuf[UART1RX_MAX_LENGTH];
uint16_t gReceCunt=0;
void If_valid(void)   // 判断是否有效
{
    char *point = 0;   // 逗号的地址指针
    char *nextPoint = 0;   // 下一个逗号的地址指针
    uint8_t tempVar = 0;   // 临时循环变量

		receDataFrame.isGetData = 0;   // 清除接收成功标志位

		// for循环解析接收帧
		for (tempVar = 0; tempVar < 7; tempVar++)
		{
				// 第一次循环
				if (tempVar == 0)
				{
						// 寻找第一个逗号
						if ((point = strstr(receDataFrame.Frame_Buffer, ",")) == NULL)
						{
								LCD_DisplayString(1,125,"Prase Errpr!");   // 解析错误
						}
				}
				else
				{
						point++;   // 防止重复找到同一个逗号

						// 寻找下一个逗号
						// 注意strstr函数的输入变量，是从上一个逗号之后开始找下一个逗号
						if ((nextPoint = strstr(point, ",")) != NULL)
						{
								// 存储信息
								switch (tempVar)
								{
										case 1:   // UTC时间
												memcpy(receDataFrame.UTCTime, point, nextPoint - point);
												receDataFrame.UTCTime[nextPoint - point] = '\0'; // 添加字符串结束符
												break;

										case 6:   // 数据有效标识
												memcpy(receDataFrame.UsefullFlag, point, nextPoint - point);
												receDataFrame.UsefullFlag[nextPoint - point] = '\0'; // 添加字符串结束符
												break;
								}
								point = nextPoint;   // 更新上一个逗号地址指针

								// 数据有效
								if (receDataFrame.UsefullFlag[0] == '1')
								{
										LCD_DisplayString(1,125,"Data is usefull!");
										receDataFrame.isValidData = 1;   // 数据解析完成
//												printf("Data is usefull!\r\n");
								}
								else if (receDataFrame.UsefullFlag[0] == '0')
								{
										LCD_DisplayString(1,125,"Data is invalid!");
									  receDataFrame.isValidData = 0;
//												printf("Data is invalid\r\n!");
								}
						}
						else
						{
								LCD_DisplayString(1,125,"Prase Errpr!");    // 解析错误
						}
				}
		}
}


// 接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		if (huart == &huart1)
    {
        if (gUart1RcecBuf[gReceCunt] == '\n') // 判断是否为结束符
        {
					HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
						// 确定是否收到"GMGGA"这一帧数据
						if (gUart1RcecBuf[0] == '$' && 
								gUart1RcecBuf[1] == 'G' &&
								gUart1RcecBuf[2] == 'N' &&
								gUart1RcecBuf[3] == 'G' &&
								gUart1RcecBuf[4] == 'G' && 
								gUart1RcecBuf[5] == 'A')
						{
	//                HAL_UART_Transmit_IT(&huart1, gUart1RcecBuf, gReceCunt + 1);
								// 是 $GPRMC 或者 $GNRMC，复制数据到接收数据帧结构体
								memcpy(receDataFrame.Frame_Buffer, gUart1RcecBuf, gReceCunt + 1);
								receDataFrame.Frame_Buffer[gReceCunt + 1] = '\0'; // 添加字符串结束符
								receDataFrame.isGetData = 1; // 设置接收成功标志
						}					
						// 重置接收索引
            gReceCunt = 0;
        }
        else
        {
            // 不是结束符，继续接收
            gReceCunt++;
            if (gReceCunt >= UART1RX_MAX_LENGTH)
            {
                // 缓冲区已满，重置接收索引
                gReceCunt = 0;
            }
        }

		// 再次开启接收中断，准备下一次接收
		HAL_UART_Receive_IT(&huart1, (uint8_t *)(gUart1RcecBuf + gReceCunt), 1);
		}
}    

/*
 *==============================================================================
 *函数名称：无
 *函数功能：无
 *输入参数：无
 *返回值：无
 *备  注：$GNGGA,
					 055234.00,
					 4004.73879510,
					 N,
					 11614.19821957,
					 E,
					 1,28,0.7,61.8089,
					 M,
					 -8.4923,
					 M,
						,
					 *50
 *==============================================================================
*/
// 从 $GNGGA 语句中提取纬度和经度信息
void extract_lat_lon(const char *sentence, double *lat, double *lon) {
    char *token = strtok((char *)sentence, ",");
    int field_count = 0;
    while (token != NULL) {
        if (field_count == 2) {
            *lat = atof(token);
        } else if (field_count == 4) {
            *lon = atof(token);
        }
        token = strtok(NULL, ",");
        field_count++;
    }
}
// 将度分格式转换为度分秒格式
void convert_to_dms(double coord, int *degrees, int *minutes, double *seconds) {
    *degrees = (int)coord / 100;
    double fractional_part = coord - (*degrees * 100);
    *minutes = (int)fractional_part;
    *seconds = (fractional_part - *minutes) * 60;
}
