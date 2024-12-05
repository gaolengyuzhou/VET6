/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RS485.h"
#include "control.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t RS485_buff[50]={0};
uint16_t size;
bool flg=0,re=0;
#ifdef __GNUC__ //串口重定向
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFFF);
return ch;
}


char *token;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_IT(&huart1,RS485_buff,50);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  HAL_UART_Transmit(&huart1, (uint8_t *)&RS485_buff, 100, 0);//可以接收到
//	  control_Send(CMD_ANGLE_CONTROL1,1,ang);
//	  control_Send(CMD_ANGLE_CONTROL1,2,1);/*用于测试*/
	  if(re){
		    // 假设 RS485_buff 中已经存储了接收到的数据，如 "10,5"
//		  int comma_found = 0;
		  int16_t value1 = 0, value2 = 0;
//	        for (int i = 0; i < size - 2; i++) {
//	            if (RS485_buff[i] >= '0' && RS485_buff[i] <= '9') {
//	                if (!comma_found) {
//	                    value1 = value1 * 10 + (RS485_buff[i] - '0');
//	                } else {
//	                    value2 = value2 * 10 + (RS485_buff[i] - '0');
//	                }
//	            } else if (RS485_buff[i] == ',') {
//	                comma_found = 1;
//	            }
//	        }
//		  printf("value1: %hd, value2: %hd\n", value1, value2);

	        // 假设 RS485_buff 中已经存储了接收到的数据，如 "-10,5"
	        char *token;
	        int16_t x = 0, y = 0, z = 0;
	        token = strtok((char *)RS485_buff, ",");
	        x = (int16_t)strtol(token, NULL, 10);
	        token = strtok(NULL, ",");
	        y = (int16_t)strtol(token, NULL, 10);
	        token = strtok(NULL, ",");
	        z = (int16_t)strtol(token, NULL, 10);

	        printf("value1: %hd, value2: %hd, value3: %hd\n", x, y, z);
	        double theta = atan2(y, x);
	        double degrees = theta * 180 / M_PI;

//	        double d = sqrt(x*x + y*y);
//	        double the = atan2(z, d);
//	        double deg = the * 180 / M_PI;


				value1 = (z+20) * 100;
				value2 = (degrees) * 100;
		control_Send(CMD_ANGLE_CONTROL1, 1, value1);
		HAL_Delay(50);
		control_Send(CMD_ANGLE_CONTROL1, 2, value2);
		re=0;
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*不定长数据帧接收中断*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
 {
	if (huart == &huart1)
	{
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
		re=1;
		size=Size;
		// 判断接收到的后两位是否为 '\r' �?? '\n'
//		if (Size >= 2 && RS485_buff[Size - 2] == RS485_END_FLAG1 && RS485_buff[Size - 1] == RS485_END_FLAG2)
//		{
//
//			if(flg){
//				// 将接收到的数据转为 int64_t value
//				int16_t value = 0;
//				for (int i = 0; i < Size - 2; i++)
//				{
//					if (RS485_buff[i] >= '0' && RS485_buff[i] <= '9')
//					{
//						value = value * 10 + (RS485_buff[i] - '0');
//					}
//				}
//				value = value*1000;
//				control_Send(CMD_ANGLE_CONTROL1,1,value);
//				flg=0;
//			}
//			else{
//		        // 将接收到的数据转为 int64_t value
//		        int16_t value = 0;
//		        for (int i = 0; i < Size - 2; i++)
//		        {
//		            if (RS485_buff[i] >= '0' && RS485_buff[i] <= '9')
//		            {
//		                value = value * 10 + (RS485_buff[i] - '0');
//		            }
//		        }
//		        value = value*1000;
//		        control_Send(CMD_ANGLE_CONTROL1,2,value);
//		        flg=1;
//			}
//		}
	//    else{
	//    	RS485_SendPacket(RS485_buff,Size);/*接受到什么就返回�??么，用于测试*/
	//    }
		HAL_UARTEx_ReceiveToIdle_IT(&huart1,RS485_buff,50);
	}
 }

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
