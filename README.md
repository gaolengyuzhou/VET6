# ģ��ӿ�
![](./����GPS.png)
> COM3������������Ҳ���Խӵ���

> COM1������Ҫ���ڵ�Ƭ���ϵģ�����ͨ��

> COM2��ʹ�û�վ���ƶ�վʱ���õģ�������GPS�豸��ͨ����
# ���ô���һ
![](./�첽.png)
![](./115200.png)
![](./NVIC.png)
## ���usart.c
```c
/* USER CODE BEGIN 0 */
#include "stdio.h"
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	 HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x0001);  
	return ch;
}
/* USER CODE END 0 */
```
## ��дgps982���ղ���
### ���ڽ��պ���`gUart1RcecBuf`
1. ��Դ�ļ����ʼ��
```c
uint8_t gUart1RcecBuf[UART1RX_MAX_LENGTH];
uint16_t gReceCunt=0;
```
2. ��ͷ�ļ�������Ϊ�ⲿ�ɵ���
```c
#define UART1RX_MAX_LENGTH 256
extern uint8_t gUart1RcecBuf[UART1RX_MAX_LENGTH];
```
3. ��main�￪���ж�
```c
HAL_UART_Receive_IT(&huart1, (uint8_t *)gUart1RcecBuf, 1);
```
> 1���ǽ��յ�һ���ֽھͽ���ص�����

> ������ǻص��������ص�������ֱ�����յ��س�����'\n'�Ž�ֹ��
4. �ص�����
```c
// ������ɻص�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
    {
        if (gUart1RcecBuf[gReceCunt] == '\n') // �ж��Ƿ�Ϊ������
        {
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
            /*���ݴ���*/
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
```
## ��дgps982�������ݲ���
### ����GPS���ݵĽṹ�岢������Ϊ�ⲿ�ɵ���
```c
/*gps982.h*/
typedef struct {
    uint8_t isGetData;
    uint8_t isValidData;
    char Frame_Buffer[256];
    char UTCTime[20];
    char UsefullFlag[2];
    char latitude[20];
    char N_S[2];
    char longitude[20];
    char E_W[2];
} ReceDataFrame;

extern ReceDataFrame receDataFrame;
```
### main�мǵó�ʼ��
```c
memset(&receDataFrame, 0, sizeof(ReceDataFrame));
```
## ���յ�����Ϣ
```
$GNRMC,015135.000,A,4159.65553,N,12136.79345,E,0.52,0.00,191123,,,A*7F
```
1. ����
```c
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
```
2. ��λת��
```c
// ���ȷָ�ʽת��Ϊ�ȷ����ʽ
void convert_to_dms(double coord, int *degrees, int *minutes, double *seconds) {
    *degrees = (int)coord / 100;
    double fractional_part = coord - (*degrees * 100);
    *minutes = (int)fractional_part;
    *seconds = (fractional_part - *minutes) * 60;
}
```
3. �ж��н���
```c
// ȷ���Ƿ��յ�"GMGGA"��һ֡����
if (gUart1RcecBuf[0] == '$' && 
        gUart1RcecBuf[1] == 'G' &&
        gUart1RcecBuf[2] == 'N' &&
        gUart1RcecBuf[3] == 'G' &&
        gUart1RcecBuf[4] == 'G' && 
        gUart1RcecBuf[5] == 'A')
{
    // �� $GPRMC ���� $GNRMC���������ݵ���������֡�ṹ��
    memcpy(receDataFrame.Frame_Buffer, gUart1RcecBuf, gReceCunt + 1);
    receDataFrame.Frame_Buffer[gReceCunt + 1] = '\0'; // ����ַ���������
    receDataFrame.isGetData = 1; // ���ý��ճɹ���־
}	
```
4. �������еĴ����߼�:���յ�--����Чֵ--������LCD��ʾ
```c
// ������ݽ��ճɹ�
if (receDataFrame.isGetData)
{// �ж��Ƿ���Ч
    If_valid();
}
if (receDataFrame.isValidData)
{
    extract_lat_lon(receDataFrame.Frame_Buffer, &lat, &lon);
    convert_to_dms(lat, &lat_degrees, &lat_minutes, &lat_seconds);
    convert_to_dms(lon, &lon_degrees, &lon_minutes, &lon_seconds);

    char lat_str[50];
    char lon_str[50];
    sprintf(lat_str, "%d^%d'%f\"", lat_degrees, lat_minutes, lat_seconds);
    sprintf(lon_str, "%d^%d'%f\"", lon_degrees, lon_minutes, lon_seconds);

    LCD_DisplayString(1, 5, "lat:");LCD_DisplayString(1, 35, lat_str);
    LCD_DisplayString(1, 65, "lon:");LCD_DisplayString(1, 95, lon_str);

    receDataFrame.isValidData = 0;
}
```
# ���û�վ
```
# com2��ͼ���ӿ�
mode base time 60 1.5 2.5
rtcm1006 com2 10       
rtcm1033 com2 10       
rtcm1074 com2 1
rtcm1124 com2 1
rtcm1084 com2 1
rtcm1094 com2 1
com com2 38400
saveconfig
```
GGA(Global positioning system Fix Data)��һ�ְ���ȫ��λϵͳ(GPS)λ����Ϣ��NMEA��׼�������ͣ����а������ȡ���ȸ߶ȡ��������������ݡ�GGA��Ϣ����һ����λ״ָ̬ʾ����ָʾ����ֵΪ0��ʾû�ж�λ��ֵΪ1��ʾ��׼��λ��ֵΪ2��ʾ��ֶ�λ��

RMC(Recommended Minimum Navigation Information)����һ�ְ���GPSλ����Ϣ��NMEA��׼�������ͣ����а������ȡ�γ�ȡ��ٶȡ���������ݡ�RMC��Ϣ����һ����λ״ָ̬ʾ����ָʾ����ֵΪA��ʾ��׼��λ��
```c
GPGGA COM1 1
GPRMC COM1 1
SAVECONFIG
```
## �ȷָ�ʽ
```
39��6��20.646��ת��Ϊ
�ȷָ�ʽ��3906.3441
�ȸ�ʽ��39.105735��
```
## �Ƕȸ�ʽ
## �ȷ����ʽ