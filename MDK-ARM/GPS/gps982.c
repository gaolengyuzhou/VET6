#include "stm32f4xx_hal.h"

#include "gps982.h"
#include "lcd_spi_154.h"
#include "usart.h"

#define _USE_MATH_DEFINES
#include <math.h>

ReceDataFrame receDataFrame;

uint8_t gUart1RcecBuf[UART1RX_MAX_LENGTH];
uint16_t gReceCunt=0;
void If_valid(void)   // �ж��Ƿ���Ч
{
    char *point = 0;   // ���ŵĵ�ַָ��
    char *nextPoint = 0;   // ��һ�����ŵĵ�ַָ��
    uint8_t tempVar = 0;   // ��ʱѭ������

		receDataFrame.isGetData = 0;   // ������ճɹ���־λ
		
		// forѭ����������֡
		for (tempVar = 0; tempVar < 7; tempVar++)
		{
				// ��һ��ѭ��
				if (tempVar == 0)
				{
						// Ѱ�ҵ�һ������
						if ((point = strstr(receDataFrame.Frame_Buffer, ",")) == NULL)
						{
								LCD_DisplayString(1,185,"Prase Errpr!");   // ��������
						}
				}
				else
				{
						point++;   // ��ֹ�ظ��ҵ�ͬһ������

						// Ѱ����һ������
						// ע��strstr����������������Ǵ���һ������֮��ʼ����һ������
						if ((nextPoint = strstr(point, ",")) != NULL)
						{
								// �洢��Ϣ
								switch (tempVar)
								{
										case 1:   // UTCʱ��
												memcpy(receDataFrame.UTCTime, point, nextPoint - point);
												receDataFrame.UTCTime[nextPoint - point] = '\0'; // ����ַ���������
												break;

										case 6:   // ������Ч��ʶ
												memcpy(receDataFrame.UsefullFlag, point, nextPoint - point);
												receDataFrame.UsefullFlag[nextPoint - point] = '\0'; // ����ַ���������
												break;
								}
								point = nextPoint;   // ������һ�����ŵ�ַָ��

								// ������Ч
								if (receDataFrame.UsefullFlag[0] == '1')
								{
										receDataFrame.isValidData = 1;
//												printf("Data is usefull!\r\n");
								}
								else if (receDataFrame.UsefullFlag[0] == '0')
								{
										LCD_DisplayString(1,185,"Data is invalid!");
									  receDataFrame.isValidData = 0;
//												printf("Data is invalid\r\n!");
								}
						}
						else
						{
								LCD_DisplayString(1,185,"Prase Errpr!");    // ��������
						}
				}
		}
}


// ������ɻص�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		if (huart == &huart1)
    {
        if (gUart1RcecBuf[gReceCunt] == '\n') // �ж��Ƿ�Ϊ������
        {
					HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
						// ȷ���Ƿ��յ�"GMGGA"��һ֡����
						if (gUart1RcecBuf[0] == '$' && 
								gUart1RcecBuf[1] == 'G' &&
								gUart1RcecBuf[2] == 'N' &&
								gUart1RcecBuf[3] == 'G' &&
								gUart1RcecBuf[4] == 'G' && 
								gUart1RcecBuf[5] == 'A')
						{
	//                HAL_UART_Transmit_IT(&huart1, gUart1RcecBuf, gReceCunt + 1);
								// �� $GPRMC ���� $GNRMC���������ݵ���������֡�ṹ��
								memcpy(receDataFrame.Frame_Buffer, gUart1RcecBuf, gReceCunt + 1);
								receDataFrame.Frame_Buffer[gReceCunt + 1] = '\0'; // ����ַ���������
								receDataFrame.isGetData = 1; // ���ý��ճɹ���־
						}					
						// ���ý�������
            gReceCunt = 0;
        }
        else
        {
            // ���ǽ���������������
            gReceCunt++;
            if (gReceCunt >= UART1RX_MAX_LENGTH)
            {
                // ���������������ý�������
                gReceCunt = 0;
            }
        }

		// �ٴο��������жϣ�׼����һ�ν���
		HAL_UART_Receive_IT(&huart1, (uint8_t *)(gUart1RcecBuf + gReceCunt), 1);
		}
}    

/*
 *==============================================================================
 *�������ƣ���
 *�������ܣ���
 *�����������
 *����ֵ����
 *��  ע��$GNGGA,
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
// �� $GNGGA �������ȡγ�Ⱥ;�����Ϣ
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
// ���ȷָ�ʽת��Ϊ�ȷ����ʽ
void dm_to_dms(double coord, int *degrees, int *minutes, double *seconds) {
    *degrees = (int)coord / 100;
    double fractional_part = coord - (*degrees * 100);
    *minutes = (int)fractional_part;
    *seconds = (fractional_part - *minutes) * 60;
}
// ���ȷָ�ʽת��Ϊ�ȸ�ʽ
double dm_to_deg(double dm) {
    int degrees = (int)(dm / 100);
    double minutes = dm - degrees * 100;
    return degrees + minutes / 60;
}
// ���ȷ����ʽת��Ϊ�ȷָ�ʽ
double dms_to_dm(int degrees, int minutes, double seconds) {
    return degrees * 100 + minutes + seconds / 60;
}
// ���㵱ǰ��γ���������γ��֮��Ĳ��
void calculate_distance_difference(double current_lat_dm, double current_lon_dm, double target_lat, double target_lon) {
    char lat_str[50];
		char lon_str[50];
	
	  // ����ǰ��γ��ת��Ϊ�ȸ�ʽ
    double current_lat_deg = dm_to_deg(current_lat_dm);
    double current_lon_deg = dm_to_deg(current_lon_dm);
	  double target_lat_deg = dm_to_deg(target_lat);
    double target_lon_deg = dm_to_deg(target_lon);

    // ����γ�Ȳ�;��Ȳ�
    double lat_diff = current_lat_deg - target_lat_deg;
    double lon_diff = current_lon_deg - target_lon_deg;

    // ����γ�Ȳ��Ӧ�ľ��루�ף�
    double lat_diff_meters = lat_diff * LATITUDE_PER_DEGREE_METERS;

    // ���㵱ǰγ�ȵ�����ֵ
    double current_lat_rad = current_lat_deg * M_PI / 180;
    double longitude_per_degree_meters = LATITUDE_PER_DEGREE_METERS * cos(current_lat_rad);
    // ���㾭�Ȳ��Ӧ�ľ��루�ף�
    double lon_diff_meters = lon_diff * longitude_per_degree_meters;

//    printf("��ǰ��γ��: %.6f, %.6f\n", current_lat_deg, current_lon_deg);
//    printf("Ŀ�꾭γ��: %.6f, %.6f\n", target_lat, target_lon);
//    printf("γ�Ȳ�: %.2f ��\n", lat_diff_meters);
//    printf("���Ȳ�: %.2f ��\n", lon_diff_meters);
		sprintf(lat_str, "%.6fm", lat_diff_meters);
		sprintf(lon_str, "%.6fm", lon_diff_meters);

		LCD_DisplayString(1, 65, lat_str);
		LCD_DisplayString(1, 155, lon_str);
}
