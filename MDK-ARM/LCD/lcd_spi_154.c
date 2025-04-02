/***
	************************************************************************************************************************************************************************************************
	*	@version V1.0
	*	@author  ¹С��Ƽ�
	*	@brief   SPI������ʾ������Ļ������ ST7789
   **********************************************************************************************************************************************************************************************
   *  @description
	*
	*	ʵ��ƽ̨��¹С��STM32F407ZGT6���İ� ���ͺţ�LXB407ZG-P1��
	* �ͷ�΢�ţ�19949278543
	*
>>>>> ��Ҫ˵����
	*
	*  1.��Ļ����Ϊ16λRGB565��ʽ
	*  2.SPIͨ���ٶ�Ϊ 21M  
   *
>>>>> ����˵����
	*
	*	1. �����ֿ�ʹ�õ���С�ֿ⣬���õ��˶�Ӧ�ĺ�����ȥȡģ���û����Ը����������������ɾ��
	*	2. ���������Ĺ��ܺ�ʹ�ÿ��Բο�������˵��
	*
	*********************************************************************************************************************************************************************************************LXB*****
***/

#include "lcd_spi_154.h"

SPI_HandleTypeDef hspi3;			// SPI_HandleTypeDef �ṹ�����

#define  LCD_SPI hspi3           // SPI�ֲ��꣬�����޸ĺ���ֲ

static pFONT *LCD_AsciiFonts;		// Ӣ�����壬ASCII�ַ���
static pFONT *LCD_CHFonts;		   // �������壨ͬʱҲ����Ӣ�����壩

// ��Ϊ����SPI����Ļ��ÿ�θ�����ʾʱ����Ҫ����������������д�Դ棬
// ����ʾ�ַ�ʱ�������һ������ȥд����д�Դ棬��ǳ�����
// ��˿���һƬ���������Ƚ���Ҫ��ʾ������д�������������������д���Դ档
// �û����Ը���ʵ�����ȥ�޸Ĵ˴��������Ĵ�С��
// ���磬�û���Ҫ��ʾ32*32�ĺ���ʱ����Ҫ�Ĵ�СΪ 32*32*2 = 2048 �ֽڣ�ÿ�����ص�ռ2�ֽڣ�
uint16_t  LCD_Buff[1024];        // LCD��������16λ��ÿ�����ص�ռ2�ֽڣ�


struct	//LCD��ز����ṹ��
{
	uint32_t Color;  				//	LCD��ǰ������ɫ
	uint32_t BackColor;			//	����ɫ
   uint8_t  ShowNum_Mode;		// ������ʾģʽ
	uint8_t  Direction;			//	��ʾ����
   uint16_t Width;            // ��Ļ���س���
   uint16_t Height;           // ��Ļ���ؿ��	
   uint8_t  X_Offset;         // X����ƫ�ƣ�����������Ļ���������Դ�д�뷽ʽ
   uint8_t  Y_Offset;         // Y����ƫ�ƣ�����������Ļ���������Դ�д�뷽ʽ
}LCD;



/*************************************************************************************************
*	�� �� ��:	HAL_SPI_MspInit
*	��ڲ���:	hspi - SPI_HandleTypeDef����ı���������ʾ����� SPI ���
*	�� �� ֵ:	��
*	��������:	��ʼ�� SPI3 ����
*	˵    ��:	��		
*************************************************************************************************/

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};
   if(hspi->Instance==SPI3)
   {
		__HAL_RCC_SPI3_CLK_ENABLE();	// ʹ��SPI3ʱ��
		
      GPIO_LCD_SCK_CLK;    	 		// ʹ�� SCK          ����ʱ��	
      GPIO_LCD_SDA_CLK;    			// ʹ�� SDA          ����ʱ��	
		GPIO_LCD_CS_CLK;					// ʹ�� CS           ����ʱ��	
      GPIO_LCD_Backlight_CLK;   		// ʹ�� ����         ����ʱ��
      GPIO_LCD_DC_CLK;          		// ʹ�� ����ָ��ѡ�� ����ʱ��
		
// ��ʼ�� SCK��MOSI
      GPIO_InitStruct.Pin 		   = LCD_SCK_PIN;      	// SCK����
      GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;           	 	// �����������
      GPIO_InitStruct.Pull 		= GPIO_NOPULL;                	// ��������
      GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;  	// ����ٶȵȼ�
		GPIO_InitStruct.Alternate 	= GPIO_AF6_SPI3;				
      HAL_GPIO_Init(LCD_SCK_PORT, &GPIO_InitStruct);

      GPIO_InitStruct.Pin 		   = LCD_SDA_PIN;      	// SCK��MOSI����		
      HAL_GPIO_Init(LCD_SDA_PORT, &GPIO_InitStruct);
		

// ��ʼ��Ƭѡ���ţ�ʹ��Ӳ�� SPI Ƭѡ

		GPIO_InitStruct.Pin 		= LCD_CS_PIN;						// ���� ����
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;					   // ��������
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_HIGH;			// �ٶȵȼ���
		HAL_GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);			// ��ʼ��  
  
// ��ʼ�� ���� ����  
		GPIO_InitStruct.Pin 		= LCD_Backlight_PIN;				// ���� ����
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;					   // ��������
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;			// �ٶȵȼ���
		HAL_GPIO_Init(LCD_Backlight_PORT, &GPIO_InitStruct);	// ��ʼ��  

// ��ʼ�� ����ָ��ѡ�� ����  
		GPIO_InitStruct.Pin 		= LCD_DC_PIN;				      // ����ָ��ѡ�� ����
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;						// ��������
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_HIGH;			// �ٶȵȼ���
		HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStruct);	      // ��ʼ��  
		
		LCD_DC_Data;			// DC�������ߣ�Ĭ�ϴ���д����״̬
		LCD_CS_H;				// ����Ƭѡ����ֹͨ��
		LCD_Backlight_OFF;  	// �ȹرձ��⣬��ʼ�����֮���ٴ�		
   }
}

	  
/*************************************************************************************************
*	�� �� ��:	MX_SPI3_Init
*	��ڲ���:	��
*	�� �� ֵ:	��
*	��������:	��ʼ��SPI����
*	˵    ��:	ʹ��Ӳ��Ƭѡ	 
*************************************************************************************************/

void MX_SPI3_Init(void)
{
	LCD_SPI.Instance 									= SPI3;							   					//	ʹ��SPI3
	LCD_SPI.Init.Mode 								= SPI_MODE_MASTER;            					//	����ģʽ
	LCD_SPI.Init.Direction 							= SPI_DIRECTION_1LINE;       					   //	����
	LCD_SPI.Init.DataSize 							= SPI_DATASIZE_8BIT;          					//	8λ���ݿ��
	LCD_SPI.Init.CLKPolarity 						= SPI_POLARITY_LOW;           					//	CLK����ʱ���ֵ͵�ƽ
	LCD_SPI.Init.CLKPhase 							= SPI_PHASE_1EDGE;            					//	������CLK��һ��������Ч
	LCD_SPI.Init.NSS 									= SPI_NSS_SOFT;        								//	���Ƭѡ   
	
// SPI3 ������APB1���ߣ�����ʱ��42MHz	
// ����Ϊ2��Ƶ���õ�21MHz��SPI����ʱ��
	LCD_SPI.Init.BaudRatePrescaler 				= SPI_BAUDRATEPRESCALER_2;							// 2��Ƶ
	
	LCD_SPI.Init.FirstBit	 						= SPI_FIRSTBIT_MSB;									//	��λ����
	LCD_SPI.Init.TIMode 								= SPI_TIMODE_DISABLE;         					//	��ֹTIģʽ
	LCD_SPI.Init.CRCCalculation					= SPI_CRCCALCULATION_DISABLE; 					//	��ֹCRC
	LCD_SPI.Init.CRCPolynomial 					= 0x0;                        					// CRCУ��������ò���				

   HAL_SPI_Init(&LCD_SPI);       // ��ʼ��SPI

// Ϊ�˴���Ч�ʣ������̵�SPIͨ�Ų��üĴ�������������HAL���Դ����շ�������������Ҫ������������2��   
   __HAL_SPI_ENABLE(&LCD_SPI);   // ʹ��SPI
   SPI_1LINE_TX(&LCD_SPI);       // ���߷���ģʽ
}


/*****************************************************************************************
*	�� �� ��: LCD_WriteCMD
*	��ڲ���: CMD - ��Ҫд��Ŀ���ָ��
*	�� �� ֵ: ��
*	��������: ����д�������
*	˵    ��: ��
******************************************************************************************/

void  LCD_WriteCommand(uint8_t lcd_command)
{

	while((LCD_SPI.Instance->SR & 0x0080) != RESET); 	// ���ж�SPI�Ƿ���У��ȴ�ͨ�����
	
	LCD_DC_Command;	//	DC��������ͣ�����дָ��	

	(&LCD_SPI)->Instance->DR = lcd_command;		// ��������
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����

	LCD_DC_Data;	//	DC��������ߣ�����д����		
}

/****************************************************************************************************************************************
*	�� �� ��: LCD_WriteData_8bit
*
*	��ڲ���: lcd_data - ��Ҫд������ݣ�8λ
*
*	��������: д��8λ����
*	
****************************************************************************************************************************************/

void  LCD_WriteData_8bit(uint8_t lcd_data)
{
	LCD_SPI.Instance->DR = lcd_data;									// ��������
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
}

/****************************************************************************************************************************************
*	�� �� ��: LCD_WriteData_16bit
*
*	��ڲ���: lcd_data - ��Ҫд������ݣ�16λ
*
*	��������: д��16λ����
*	
****************************************************************************************************************************************/

void  LCD_WriteData_16bit(uint16_t lcd_data)
{
	LCD_SPI.Instance->DR = lcd_data>>8;								// �������ݣ���8λ
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	LCD_SPI.Instance->DR = lcd_data;									// �������ݣ���8λ
	while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
}

/****************************************************************************************************************************************
*	�� �� ��: LCD_WriteBuff
*
*	��ڲ���: DataBuff - ��������DataSize - ���ݳ���
*
*	��������: ����д�����ݵ���Ļ
*	
****************************************************************************************************************************************/

void  LCD_WriteBuff(uint16_t *DataBuff, uint16_t DataSize)
{
	uint32_t i;

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// �ر�SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // �л���16λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;					// ʹ��SPI
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	for(i=0;i<DataSize;i++)				
	{

		LCD_SPI.Instance->DR = DataBuff[i];
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������

	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// �ر�SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// �л���8λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;	// ʹ��SPI	
}

/****************************************************************************************************************************************
*	�� �� ��: SPI_LCD_Init
*
*	��������: ��ʼ��SPI�Լ���Ļ�������ĸ��ֲ���
*	
****************************************************************************************************************************************/

void SPI_LCD_Init(void)
{
   MX_SPI3_Init();               // ��ʼ��SPI�Ϳ�������

   HAL_Delay(10);               // ��Ļ����ɸ�λʱ�������ϵ縴λ������Ҫ�ȴ�5ms���ܷ���ָ��

	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC����ʼͨ��

 	LCD_WriteCommand(0x36);       // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
	LCD_WriteData_8bit(0x00);     // ���ó� ���ϵ��¡������ң�RGB���ظ�ʽ

	LCD_WriteCommand(0x3A);			// �ӿ����ظ�ʽ ָ���������ʹ�� 12λ��16λ����18λɫ
	LCD_WriteData_8bit(0x05);     // �˴����ó� 16λ ���ظ�ʽ

// �������ܶ඼�ǵ�ѹ����ָ�ֱ��ʹ�ó��Ҹ��趨ֵ
 	LCD_WriteCommand(0xB2);			
	LCD_WriteData_8bit(0x0C);
	LCD_WriteData_8bit(0x0C); 
	LCD_WriteData_8bit(0x00); 
	LCD_WriteData_8bit(0x33); 
	LCD_WriteData_8bit(0x33); 			

	LCD_WriteCommand(0xB7);		   // դ����ѹ����ָ��	
	LCD_WriteData_8bit(0x35);     // VGH = 13.26V��VGL = -10.43V

	LCD_WriteCommand(0xBB);			// ������ѹ����ָ��
	LCD_WriteData_8bit(0x19);     // VCOM = 1.35V

	LCD_WriteCommand(0xC0);
	LCD_WriteData_8bit(0x2C);

	LCD_WriteCommand(0xC2);       // VDV �� VRH ��Դ����
	LCD_WriteData_8bit(0x01);     // VDV �� VRH ���û���������

	LCD_WriteCommand(0xC3);			// VRH��ѹ ����ָ��  
	LCD_WriteData_8bit(0x12);     // VRH��ѹ = 4.6+( vcom+vcom offset+vdv)
				
	LCD_WriteCommand(0xC4);		   // VDV��ѹ ����ָ��	
	LCD_WriteData_8bit(0x20);     // VDV��ѹ = 0v

	LCD_WriteCommand(0xC6); 		// ����ģʽ��֡�ʿ���ָ��
	LCD_WriteData_8bit(0x0F);   	// ������Ļ��������ˢ��֡��Ϊ60֡    

	LCD_WriteCommand(0xD0);			// ��Դ����ָ��
	LCD_WriteData_8bit(0xA4);     // ��Ч���ݣ��̶�д��0xA4
	LCD_WriteData_8bit(0xA1);     // AVDD = 6.8V ��AVDD = -4.8V ��VDS = 2.3V

	LCD_WriteCommand(0xE0);       // ������ѹ٤��ֵ�趨
	LCD_WriteData_8bit(0xD0);
	LCD_WriteData_8bit(0x04);
	LCD_WriteData_8bit(0x0D);
	LCD_WriteData_8bit(0x11);
	LCD_WriteData_8bit(0x13);
	LCD_WriteData_8bit(0x2B);
	LCD_WriteData_8bit(0x3F);
	LCD_WriteData_8bit(0x54);
	LCD_WriteData_8bit(0x4C);
	LCD_WriteData_8bit(0x18);
	LCD_WriteData_8bit(0x0D);
	LCD_WriteData_8bit(0x0B);
	LCD_WriteData_8bit(0x1F);
	LCD_WriteData_8bit(0x23);

	LCD_WriteCommand(0xE1);      // ������ѹ٤��ֵ�趨
	LCD_WriteData_8bit(0xD0);
	LCD_WriteData_8bit(0x04);
	LCD_WriteData_8bit(0x0C);
	LCD_WriteData_8bit(0x11);
	LCD_WriteData_8bit(0x13);
	LCD_WriteData_8bit(0x2C);
	LCD_WriteData_8bit(0x3F);
	LCD_WriteData_8bit(0x44);
	LCD_WriteData_8bit(0x51);
	LCD_WriteData_8bit(0x2F);
	LCD_WriteData_8bit(0x1F);
	LCD_WriteData_8bit(0x1F);
	LCD_WriteData_8bit(0x20);
	LCD_WriteData_8bit(0x23);

	LCD_WriteCommand(0x21);       // �򿪷��ԣ���Ϊ����ǳ����ͣ�������Ҫ������

 // �˳�����ָ�LCD�������ڸ��ϵ硢��λʱ�����Զ���������ģʽ ����˲�����Ļ֮ǰ����Ҫ�˳�����  
	LCD_WriteCommand(0x11);       // �˳����� ָ��
   HAL_Delay(120);               // ��Ҫ�ȴ�120ms���õ�Դ��ѹ��ʱ�ӵ�·�ȶ�����

 // ����ʾָ�LCD�������ڸ��ϵ硢��λʱ�����Զ��ر���ʾ 
	LCD_WriteCommand(0x29);       // ����ʾ   	

	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����		

// ���½���һЩ������Ĭ������
   LCD_SetDirection(Direction_V);  	      //	������ʾ����
	LCD_SetBackColor(LCD_BLACK);           // ���ñ���ɫ
   LCD_SetColor(LCD_WHITE);               // ���û���ɫ  
	LCD_Clear();                           // ����

   LCD_SetAsciiFont(&ASCII_Font24);       // ����Ĭ������
   LCD_ShowNumMode(Fill_Zero);	      	// ���ñ�����ʾģʽ������λ���ո������0

// ȫ���������֮�󣬴򿪱���	
   LCD_Backlight_ON;  // ��������ߵ�ƽ��������
	
}

/****************************************************************************************************************************************
*	�� �� ��:	 LCD_SetAddress
*
*	��ڲ���:	 x1 - ��ʼˮƽ����   y1 - ��ʼ��ֱ����  
*              x2 - �յ�ˮƽ����   y2 - �յ㴹ֱ����	   
*	
*	��������:   ������Ҫ��ʾ����������		 			 
*****************************************************************************************************************************************/

void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)		
{
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	LCD_WriteCommand(0x2a);			//	�е�ַ���ã���X����
	LCD_WriteData_16bit(x1+LCD.X_Offset);
	LCD_WriteData_16bit(x2+LCD.X_Offset);

	LCD_WriteCommand(0x2b);			//	�е�ַ���ã���Y����
	LCD_WriteData_16bit(y1+LCD.Y_Offset);
	LCD_WriteData_16bit(y2+LCD.Y_Offset);

	LCD_WriteCommand(0x2c);			//	��ʼд���Դ棬��Ҫ��ʾ����ɫ����
	
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����		
}

/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetColor
*
*	��ڲ���:	Color - Ҫ��ʾ����ɫ��ʾ����0x0000FF ��ʾ��ɫ
*
*	��������:	�˺����������û��ʵ���ɫ��������ʾ�ַ������㻭�ߡ���ͼ����ɫ
*
*	˵    ��:	1. Ϊ�˷����û�ʹ���Զ�����ɫ����ڲ��� Color ʹ��24λ RGB888����ɫ��ʽ���û����������ɫ��ʽ��ת��
*					2. 24λ����ɫ�У��Ӹ�λ����λ�ֱ��Ӧ R��G��B  3����ɫͨ��
*
*****************************************************************************************************************************************/

void LCD_SetColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //������ɫͨ����ֵ

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // ת���� 16λ ��RGB565��ɫ
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	LCD.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value);  // ����ɫд��ȫ��LCD����		
}

/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetBackColor
*
*	��ڲ���:	Color - Ҫ��ʾ����ɫ��ʾ����0x0000FF ��ʾ��ɫ
*
*	��������:	���ñ���ɫ,�˺������������Լ���ʾ�ַ��ı���ɫ
*
*	˵    ��:	1. Ϊ�˷����û�ʹ���Զ�����ɫ����ڲ��� Color ʹ��24λ RGB888����ɫ��ʽ���û����������ɫ��ʽ��ת��
*					2. 24λ����ɫ�У��Ӹ�λ����λ�ֱ��Ӧ R��G��B  3����ɫͨ��
*
*****************************************************************************************************************************************/

void LCD_SetBackColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //������ɫͨ����ֵ

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // ת���� 16λ ��RGB565��ɫ
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	LCD.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value);	// ����ɫд��ȫ��LCD����			   	
}

/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetDirection
*
*	��ڲ���:	direction - Ҫ��ʾ�ķ���
*
*	��������:	����Ҫ��ʾ�ķ���
*
*	˵    ��:   1. ��������� Direction_H ��Direction_V ��Direction_H_Flip ��Direction_V_Flip        
*              2. ʹ��ʾ�� LCD_DisplayDirection(Direction_H) ����������Ļ������ʾ
*
*****************************************************************************************************************************************/

void LCD_SetDirection(uint8_t direction)
{
	LCD.Direction = direction;    // д��ȫ��LCD����

	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
			
   if( direction == Direction_H )   // ������ʾ
   {
      LCD_WriteCommand(0x36);    		// �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0x70);        // ������ʾ
      LCD.X_Offset   = 0;             // ���ÿ���������ƫ����
      LCD.Y_Offset   = 0;   
      LCD.Width      = LCD_Height;		// ���¸�ֵ������
      LCD.Height     = LCD_Width;		
   }
   else if( direction == Direction_V )
   {
      LCD_WriteCommand(0x36);    		// �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0x00);        // ��ֱ��ʾ
      LCD.X_Offset   = 0;              // ���ÿ���������ƫ����
      LCD.Y_Offset   = 0;     
      LCD.Width      = LCD_Width;		// ���¸�ֵ������
      LCD.Height     = LCD_Height;						
   }
   else if( direction == Direction_H_Flip )
   {
      LCD_WriteCommand(0x36);   			 // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0xA0);         // ������ʾ�������·�ת��RGB���ظ�ʽ
      LCD.X_Offset   = 80;              // ���ÿ���������ƫ����
      LCD.Y_Offset   = 0;      
      LCD.Width      = LCD_Height;		 // ���¸�ֵ������
      LCD.Height     = LCD_Width;				
   }
   else if( direction == Direction_V_Flip )
   {
      LCD_WriteCommand(0x36);    		// �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
      LCD_WriteData_8bit(0xC0);        // ��ֱ��ʾ �������·�ת��RGB���ظ�ʽ
      LCD.X_Offset   = 0;              // ���ÿ���������ƫ����
      LCD.Y_Offset   = 80;     
      LCD.Width      = LCD_Width;		// ���¸�ֵ������
      LCD.Height     = LCD_Height;				
   }   
	   
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����			
}


/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetAsciiFont
*
*	��ڲ���:	*fonts - Ҫ���õ�ASCII����
*
*	��������:	����ASCII���壬��ѡ��ʹ�� 3216/2412/2010/1608/1206 ���ִ�С������
*
*	˵    ��:	1. ʹ��ʾ�� LCD_SetAsciiFont(&ASCII_Font24) �������� 2412�� ASCII����
*					2. �����ģ����� lcd_fonts.c 			
*
*****************************************************************************************************************************************/

void LCD_SetAsciiFont(pFONT *Asciifonts)
{
  LCD_AsciiFonts = Asciifonts;
}


/****************************************************************************************************************************************
*	�� �� ��:	LCD_Clear
*
*	��������:	������������LCD���Ϊ LCD.BackColor ����ɫ
*
*	˵    ��:	���� LCD_SetBackColor() ����Ҫ����ı���ɫ���ٵ��øú�����������
*
*****************************************************************************************************************************************/

void LCD_Clear(void)
{
	uint32_t i;

	LCD_SetAddress(0,0,LCD.Width-1,LCD.Height-1);			//��������	

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// �ر�SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // �л���16λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;					// ʹ��SPI
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	for(i=0;i<LCD.Width*LCD.Height;i++)				
	{

		LCD_SPI.Instance->DR = LCD.BackColor;
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// �ر�SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// �л���8λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;	// ʹ��SPI
}


/****************************************************************************************************************************************
*	�� �� ��:	LCD_ClearRect
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					width  - Ҫ�������ĺ��򳤶�
*					height - Ҫ��������������
*
*	��������:	�ֲ�������������ָ��λ�ö�Ӧ���������Ϊ LCD.BackColor ����ɫ
*
*	˵    ��:	1. ���� LCD_SetBackColor() ����Ҫ����ı���ɫ���ٵ��øú�����������
*				   2. ʹ��ʾ�� LCD_ClearRect( 10, 10, 100, 50) ���������(10,10)��ʼ�ĳ�100��50������
*
*****************************************************************************************************************************************/

void LCD_ClearRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	uint16_t i;

   LCD_SetAddress( x, y, x+width-1, y+height-1);	// ��������	

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// �ر�SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // �л���16λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;					// ʹ��SPI
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	for(i=0;i<width*height;i++)				
	{

		LCD_SPI.Instance->DR = LCD.BackColor;
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// �ر�SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// �л���8λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;	// ʹ��SPI
}


/****************************************************************************************************************************************
*	�� �� ��:	LCD_DrawPoint
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					color  - Ҫ���Ƶ���ɫ��ʹ�� 24λ RGB888 ����ɫ��ʽ���û����������ɫ��ʽ��ת��
*
*	��������:	��ָ���������ָ����ɫ�ĵ�
*
*	˵    ��:	ʹ��ʾ�� LCD_DrawPoint( 10, 10, 0x0000FF) ��������(10,10)������ɫ�ĵ�
*
*****************************************************************************************************************************************/

void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{
	LCD_SetAddress(x,y,x,y);	//	�������� 
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC

	LCD_WriteData_16bit(LCD.Color)	;
	
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����		
} 


/****************************************************************************************************************************************
*	�� �� ��:	LCD_DisplayChar
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					c  - ASCII�ַ�
*
*	��������:	��ָ��������ʾָ�����ַ�
*
*	˵    ��:	1. ������Ҫ��ʾ�����壬����ʹ�� LCD_SetAsciiFont(&ASCII_Font24) ����Ϊ 2412��ASCII����
*					2.	������Ҫ��ʾ����ɫ������ʹ�� LCD_SetColor(0xff0000FF) ����Ϊ��ɫ
*					3. �����ö�Ӧ�ı���ɫ������ʹ�� LCD_SetBackColor(0x000000) ����Ϊ��ɫ�ı���ɫ
*					4. ʹ��ʾ�� LCD_DisplayChar( 10, 10, 'a') ��������(10,10)��ʾ�ַ� 'a'
*
*****************************************************************************************************************************************/

void LCD_DisplayChar(uint16_t x, uint16_t y,uint8_t c)
{
	uint16_t  index = 0, counter = 0 ,i = 0, w = 0;		// ��������
   uint8_t   disChar;		//�洢�ַ��ĵ�ַ

	c = c - 32; 	// ����ASCII�ַ���ƫ��
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC


	for(index = 0; index < LCD_AsciiFonts->Sizes; index++)	
	{
		disChar = LCD_AsciiFonts->pTable[c*LCD_AsciiFonts->Sizes + index]; //��ȡ�ַ���ģֵ
		for(counter = 0; counter < 8; counter++)
		{ 
			if(disChar & 0x01)	
			{		
            LCD_Buff[i] =  LCD.Color;			// ��ǰģֵ��Ϊ0ʱ��ʹ�û���ɫ���
			}
			else		
			{		
            LCD_Buff[i] = LCD.BackColor;		//����ʹ�ñ���ɫ���Ƶ�
			}
			disChar >>= 1;
			i++;
         w++;
 			if( w == LCD_AsciiFonts->Width ) // ���д������ݴﵽ���ַ���ȣ����˳���ǰѭ��
			{								   // ������һ�ַ���д��Ļ���
				w = 0;
				break;
			}        
		}	
	}		
   LCD_SetAddress( x, y, x+LCD_AsciiFonts->Width-1, y+LCD_AsciiFonts->Height-1);	   // ��������	
   LCD_WriteBuff(LCD_Buff,LCD_AsciiFonts->Width*LCD_AsciiFonts->Height);          // д���Դ�
}

/**************************************************************************************************************************************
*	�� �� ��:	LCD_DisplayString
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					p - ASCII�ַ������׵�ַ
*
*	��������:	��ָ��������ʾָ�����ַ���
*
*	˵    ��:	1. ������Ҫ��ʾ�����壬����ʹ�� LCD_SetAsciiFont(&ASCII_Font24) ����Ϊ 2412��ASCII����
*					2.	������Ҫ��ʾ����ɫ������ʹ�� LCD_SetColor(0x0000FF) ����Ϊ��ɫ
*					3. �����ö�Ӧ�ı���ɫ������ʹ�� LCD_SetBackColor(0x000000) ����Ϊ��ɫ�ı���ɫ
*					4. ʹ��ʾ�� LCD_DisplayString( 10, 10, "LXB") ������ʼ����Ϊ(10,10)�ĵط���ʾ�ַ���"LXB"
*
*****************************************************************************************************************************************/

void LCD_DisplayString( uint16_t x, uint16_t y, char *p) 
{  
	while ((x < LCD.Width) && (*p != 0))	//�ж���ʾ�����Ƿ񳬳���ʾ�������ַ��Ƿ�Ϊ���ַ�
	{
		 LCD_DisplayChar( x,y,*p);
		 x += LCD_AsciiFonts->Width; //��ʾ��һ���ַ�
		 p++;	//ȡ��һ���ַ���ַ
	}
}

/****************************************************************************************************************************************
*	�� �� ��:	LCD_SetTextFont
*
*	��ڲ���:	*fonts - Ҫ���õ��ı�����
*
*	��������:	�����ı����壬�������ĺ�ASCII�ַ���
*
*	˵    ��:	1. ��ѡ��ʹ�� 3232/2424/2020/1616/1212 ���ִ�С���������壬
*						���Ҷ�Ӧ������ASCII����Ϊ 3216/2412/2010/1608/1206
*					2. �����ģ����� lcd_fonts.c 
*					3. �����ֿ�ʹ�õ���С�ֿ⣬���õ��˶�Ӧ�ĺ�����ȥȡģ
*					4. ʹ��ʾ�� LCD_SetTextFont(&CH_Font24) �������� 2424�����������Լ�2412��ASCII�ַ�����
*
*****************************************************************************************************************************************/

void LCD_SetTextFont(pFONT *fonts)
{
	LCD_CHFonts = fonts;		// ������������
	switch(fonts->Width )
	{
		case 12:	LCD_AsciiFonts = &ASCII_Font12;	break;	// ����ASCII�ַ�������Ϊ 1206
		case 16:	LCD_AsciiFonts = &ASCII_Font16;	break;	// ����ASCII�ַ�������Ϊ 1608
		case 20:	LCD_AsciiFonts = &ASCII_Font20;	break;	// ����ASCII�ַ�������Ϊ 2010	
		case 24:	LCD_AsciiFonts = &ASCII_Font24;	break;	// ����ASCII�ַ�������Ϊ 2412
		case 32:	LCD_AsciiFonts = &ASCII_Font32;	break;	// ����ASCII�ַ�������Ϊ 3216		
		default: break;
	}
}
/******************************************************************************************************************************************
*	�� �� ��:	LCD_DisplayChinese
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					pText - �����ַ�
*
*	��������:	��ָ��������ʾָ���ĵ��������ַ�
*
*	˵    ��:	1. ������Ҫ��ʾ�����壬����ʹ�� LCD_SetTextFont(&CH_Font24) ����Ϊ 2424�����������Լ�2412��ASCII�ַ�����
*					2.	������Ҫ��ʾ����ɫ������ʹ�� LCD_SetColor(0xff0000FF) ����Ϊ��ɫ
*					3. �����ö�Ӧ�ı���ɫ������ʹ�� LCD_SetBackColor(0xff000000) ����Ϊ��ɫ�ı���ɫ
*					4. ʹ��ʾ�� LCD_DisplayChinese( 10, 10, "��") ��������(10,10)��ʾ�����ַ�"��"
*
*****************************************************************************************************************************************/

void LCD_DisplayChinese(uint16_t x, uint16_t y, char *pText) 
{
	uint16_t  i=0,index = 0, counter = 0;	// ��������
	uint16_t  addr;	// ��ģ��ַ
   uint8_t   disChar;	//��ģ��ֵ
	uint16_t  Xaddress = 0; //ˮƽ����

	while(1)
	{		
		// �Ա������еĺ��ֱ��룬���Զ�λ�ú�����ģ�ĵ�ַ		
		if ( *(LCD_CHFonts->pTable + (i+1)*LCD_CHFonts->Sizes + 0)==*pText && *(LCD_CHFonts->pTable + (i+1)*LCD_CHFonts->Sizes + 1)==*(pText+1) )	
		{   
			addr=i;	// ��ģ��ַƫ��
			break;
		}				
		i+=2;	// ÿ�������ַ�����ռ���ֽ�

		if(i >= LCD_CHFonts->Table_Rows)	break;	// ��ģ�б�������Ӧ�ĺ���	
	}	
	i=0;
	for(index = 0; index <LCD_CHFonts->Sizes; index++)
	{	
		disChar = *(LCD_CHFonts->pTable + (addr)*LCD_CHFonts->Sizes + index);	// ��ȡ��Ӧ����ģ��ַ
		
		for(counter = 0; counter < 8; counter++)
		{ 
			if(disChar & 0x01)	
			{		
            LCD_Buff[i] =  LCD.Color;			// ��ǰģֵ��Ϊ0ʱ��ʹ�û���ɫ���
			}
			else		
			{		
            LCD_Buff[i] = LCD.BackColor;		// ����ʹ�ñ���ɫ���Ƶ�
			}
         i++;
			disChar >>= 1;
			Xaddress++;  //ˮƽ�����Լ�
			
			if( Xaddress == LCD_CHFonts->Width ) 	//	���ˮƽ����ﵽ���ַ���ȣ����˳���ǰѭ��
			{														//	������һ�еĻ���
				Xaddress = 0;
				break;
			}
		}	
	}	
   LCD_SetAddress( x, y, x+LCD_CHFonts->Width-1, y+LCD_CHFonts->Height-1);	   // ��������	
   LCD_WriteBuff(LCD_Buff,LCD_CHFonts->Width*LCD_CHFonts->Height);            // д���Դ�
}

/*****************************************************************************************************************************************
*	�� �� ��:	LCD_DisplayText
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					pText - �ַ�����������ʾ���Ļ���ASCII�ַ�
*
*	��������:	��ָ��������ʾָ�����ַ���
*
*	˵    ��:	1. ������Ҫ��ʾ�����壬����ʹ�� LCD_SetTextFont(&CH_Font24) ����Ϊ 2424�����������Լ�2412��ASCII�ַ�����
*					2.	������Ҫ��ʾ����ɫ������ʹ�� LCD_SetColor(0xff0000FF) ����Ϊ��ɫ
*					3. �����ö�Ӧ�ı���ɫ������ʹ�� LCD_SetBackColor(0xff000000) ����Ϊ��ɫ�ı���ɫ
*					4. ʹ��ʾ�� LCD_DisplayChinese( 10, 10, "¹С��Ƽ�STM32") ��������(10,10)��ʾ�ַ���"¹С��Ƽ�STM32"
*
**********************************************************************************************************************************LXB*******/

void LCD_DisplayText(uint16_t x, uint16_t y, char *pText) 
{  
 	
	while(*pText != 0)	// �ж��Ƿ�Ϊ���ַ�
	{
		if(*pText<=0x7F)	// �ж��Ƿ�ΪASCII��
		{
			LCD_DisplayChar(x,y,*pText);	// ��ʾASCII
			x+=LCD_AsciiFonts->Width;				// ˮƽ���������һ���ַ���
			pText++;								// �ַ�����ַ+1
		}
		else					// ���ַ�Ϊ����
		{			
			LCD_DisplayChinese(x,y,pText);	// ��ʾ����
			x+=LCD_CHFonts->Width;				// ˮƽ���������һ���ַ���
			pText+=2;								// �ַ�����ַ+2�����ֵı���Ҫ2�ֽ�
		}
	}	
}



/*****************************************************************************************************************************************
*	�� �� ��:	LCD_ShowNumMode
*
*	��ڲ���:	mode - ���ñ�������ʾģʽ
*
*	��������:	���ñ�����ʾʱ����λ��0���ǲ��ո񣬿�������� Fill_Space ���ո�Fill_Zero �����
*
*	˵    ��:   1. ֻ�� LCD_DisplayNumber() ��ʾ���� �� LCD_DisplayDecimals()��ʾС�� �����������õ�
*					2. ʹ��ʾ�� LCD_ShowNumMode(Fill_Zero) ���ö���λ���0������ 123 ������ʾΪ 000123
*
*****************************************************************************************************************************************/

void LCD_ShowNumMode(uint8_t mode)
{
	LCD.ShowNum_Mode = mode;
}


/*****************************************************************************************************************************************
*	�� �� ��:	LCD_DisplayNumber
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					number - Ҫ��ʾ������,��Χ�� -2147483648~2147483647 ֮��
*					len - ���ֵ�λ�������λ������len��������ʵ�ʳ�������������Ҫ��ʾ��������Ԥ��һ��λ�ķ�����ʾ�ռ�
*
*	��������:	��ָ��������ʾָ������������
*
*	˵    ��:	1. ������Ҫ��ʾ�����壬����ʹ�� LCD_SetAsciiFont(&ASCII_Font24) ����Ϊ��ASCII�ַ�����
*					2.	������Ҫ��ʾ����ɫ������ʹ�� LCD_SetColor(0x0000FF) ����Ϊ��ɫ
*					3. �����ö�Ӧ�ı���ɫ������ʹ�� LCD_SetBackColor(0x000000) ����Ϊ��ɫ�ı���ɫ
*					4. ʹ��ʾ�� LCD_DisplayNumber( 10, 10, a, 5) ��������(10,10)��ʾָ������a,�ܹ�5λ������λ��0��ո�
*						���� a=123 ʱ������� LCD_ShowNumMode()����������ʾ  123(ǰ�������ո�λ) ����00123
*						
*****************************************************************************************************************************************/

void  LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number, uint8_t len) 
{  
	char   Number_Buffer[15];				// ���ڴ洢ת������ַ���

	if( LCD.ShowNum_Mode == Fill_Zero)	// ����λ��0
	{
		sprintf( Number_Buffer , "%0.*d",len, number );	// �� number ת�����ַ�����������ʾ		
	}
	else			// ����λ���ո�
	{	
		sprintf( Number_Buffer , "%*d",len, number );	// �� number ת�����ַ�����������ʾ		
	}
	
	LCD_DisplayString( x, y,(char *)Number_Buffer) ;  // ��ת���õ����ַ�����ʾ����
	
}

/***************************************************************************************************************************************
*	�� �� ��:	LCD_DisplayDecimals
*
*	��ڲ���:	x - ��ʼˮƽ����
*					y - ��ʼ��ֱ����
*					decimals - Ҫ��ʾ������, double��ȡֵ1.7 x 10^��-308��~ 1.7 x 10^��+308����������ȷ��׼ȷ����Чλ��Ϊ15~16λ
*
*       			len - ������������λ��������С����͸��ţ�����ʵ�ʵ���λ��������ָ������λ��������ʵ�ʵ��ܳ���λ�����
*							ʾ��1��С�� -123.123 ��ָ�� len <=8 �Ļ�����ʵ���ճ���� -123.123
*							ʾ��2��С�� -123.123 ��ָ�� len =10 �Ļ�����ʵ�����   -123.123(����ǰ����������ո�λ) 
*							ʾ��3��С�� -123.123 ��ָ�� len =10 �Ļ��������ú��� LCD_ShowNumMode() ����Ϊ���0ģʽʱ��ʵ����� -00123.123 
*
*					decs - Ҫ������С��λ������С����ʵ��λ��������ָ����С��λ����ָ���Ŀ�������������
*							 ʾ����1.12345 ��ָ�� decs Ϊ4λ�Ļ�����������Ϊ1.1235
*
*	��������:	��ָ��������ʾָ���ı���������С��
*
*	˵    ��:	1. ������Ҫ��ʾ�����壬����ʹ�� LCD_SetAsciiFont(&ASCII_Font24) ����Ϊ��ASCII�ַ�����
*					2.	������Ҫ��ʾ����ɫ������ʹ�� LCD_SetColor(0x0000FF) ����Ϊ��ɫ
*					3. �����ö�Ӧ�ı���ɫ������ʹ�� LCD_SetBackColor(0x000000) ����Ϊ��ɫ�ı���ɫ
*					4. ʹ��ʾ�� LCD_DisplayDecimals( 10, 10, a, 5, 3) ��������(10,10)��ʾ�ֱ���a,�ܳ���Ϊ5λ�����б���3λС��
*						
*****************************************************************************************************************************************/

void  LCD_DisplayDecimals( uint16_t x, uint16_t y, double decimals, uint8_t len, uint8_t decs) 
{  
	char  Number_Buffer[20];				// ���ڴ洢ת������ַ���
	
	if( LCD.ShowNum_Mode == Fill_Zero)	// ����λ���0ģʽ
	{
		sprintf( Number_Buffer , "%0*.*lf",len,decs, decimals );	// �� number ת�����ַ�����������ʾ		
	}
	else		// ����λ���ո�
	{
		sprintf( Number_Buffer , "%*.*lf",len,decs, decimals );	// �� number ת�����ַ�����������ʾ		
	}
	
	LCD_DisplayString( x, y,(char *)Number_Buffer) ;	// ��ת���õ����ַ�����ʾ����
}


/***************************************************************************************************************************************
*	�� �� ��: LCD_DrawLine
*
*	��ڲ���: x1 - ��� ˮƽ����
*			 	 y1 - ��� ��ֱ����
*
*				 x2 - �յ� ˮƽ����
*            y2 - �յ� ��ֱ����
*
*	��������: ������֮�仭��
*
*	˵    ��: �ú�����ֲ��ST�ٷ������������
*						 
*****************************************************************************************************************************************/

#define ABS(X)  ((X) > 0 ? (X) : -(X))    

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;

	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */

	if (x2 >= x1)                 /* The x-values are increasing */
	{
	 xinc1 = 1;
	 xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
	 xinc1 = -1;
	 xinc2 = -1;
	}

	if (y2 >= y1)                 /* The y-values are increasing */
	{
	 yinc1 = 1;
	 yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
	 yinc1 = -1;
	 yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
	 xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
	 yinc2 = 0;                  /* Don't change the y for every iteration */
	 den = deltax;
	 num = deltax / 2;
	 numadd = deltay;
	 numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
	 xinc2 = 0;                  /* Don't change the x for every iteration */
	 yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
	 den = deltay;
	 num = deltay / 2;
	 numadd = deltax;
	 numpixels = deltay;         /* There are more y-values than x-values */
	}
	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
	 LCD_DrawPoint(x,y,LCD.Color);             /* Draw the current pixel */
	 num += numadd;              /* Increase the numerator by the top of the fraction */
	 if (num >= den)             /* Check if numerator >= denominator */
	 {
		num -= den;               /* Calculate the new numerator value */
		x += xinc1;               /* Change the x as appropriate */
		y += yinc1;               /* Change the y as appropriate */
	 }
	 x += xinc2;                 /* Change the x as appropriate */
	 y += yinc2;                 /* Change the y as appropriate */
	}  
}


/***************************************************************************************************************************************
*	�� �� ��: LCD_DrawLine_V
*
*	��ڲ���: x - ˮƽ����
*			 	 y - ��ֱ����
*				 height - ��ֱ���
*
*	��������: ��ָ��λ�û���ָ������� ��ֱ ��
*
*	˵    ��: 1. �ú�����ֲ��ST�ٷ������������
*				 2. Ҫ���Ƶ������ܳ�����Ļ����ʾ����		
*            3. ���ֻ�ǻ���ֱ���ߣ�����ʹ�ô˺������ٶȱ� LCD_DrawLine ��ܶ�
*  ���ܲ��ԣ�
*****************************************************************************************************************************************/

void LCD_DrawLine_V(uint16_t x, uint16_t y, uint16_t height)
{
   uint16_t i ; // ��������

	for (i = 0; i < height; i++)
	{
       LCD_Buff[i] =  LCD.Color;  // д�뻺����
   }   
   LCD_SetAddress( x, y, x, y+height-1);	     // ��������	

   LCD_WriteBuff(LCD_Buff,height);          // д���Դ�
}

/***************************************************************************************************************************************
*	�� �� ��: LCD_DrawLine_H
*
*	��ڲ���: x - ˮƽ����
*			 	 y - ��ֱ����
*				 width  - ˮƽ���
*
*	��������: ��ָ��λ�û���ָ������� ˮƽ ��
*
*	˵    ��: 1. �ú�����ֲ��ST�ٷ������������
*				 2. Ҫ���Ƶ������ܳ�����Ļ����ʾ����		
*            3. ���ֻ�ǻ� ˮƽ ���ߣ�����ʹ�ô˺������ٶȱ� LCD_DrawLine ��ܶ�
*  ���ܲ��ԣ�
**********************************************************************************************************************************LXB*******/

void LCD_DrawLine_H(uint16_t x, uint16_t y, uint16_t width)
{
   uint16_t i ; // ��������

	for (i = 0; i < width; i++)
	{
       LCD_Buff[i] =  LCD.Color;  // д�뻺����
   }   
   LCD_SetAddress( x, y, x+width-1, y);	     // ��������	

   LCD_WriteBuff(LCD_Buff,width);          // д���Դ�
}

/***************************************************************************************************************************************
*	�� �� ��: LCD_DrawRect
*
*	��ڲ���: x - ˮƽ����
*			 	 y - ��ֱ����
*			 	 width  - ˮƽ���
*				 height - ��ֱ���
*
*	��������: ��ָ��λ�û���ָ������ľ�������
*
*	˵    ��: 1. �ú�����ֲ��ST�ٷ������������
*				 2. Ҫ���Ƶ������ܳ�����Ļ����ʾ����
*						 
*****************************************************************************************************************************************/

void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
   // ����ˮƽ��
   LCD_DrawLine_H( x,  y,  width);           
   LCD_DrawLine_H( x,  y+height-1,  width);

   // ���ƴ�ֱ��
   LCD_DrawLine_V( x,  y,  height);
   LCD_DrawLine_V( x+width-1,  y,  height);
}



/***************************************************************************************************************************************
*	�� �� ��: LCD_DrawCircle
*
*	��ڲ���: x - Բ�� ˮƽ����
*			 	 y - Բ�� ��ֱ����
*			 	 r  - �뾶
*
*	��������: ������ (x,y) ���ư뾶Ϊ r ��Բ������
*
*	˵    ��: 1. �ú�����ֲ��ST�ٷ������������
*				 2. Ҫ���Ƶ������ܳ�����Ļ����ʾ����
*
*****************************************************************************************************************************************/

void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r)
{
	int Xadd = -r, Yadd = 0, err = 2-2*r, e2;
	do {   

		LCD_DrawPoint(x-Xadd,y+Yadd,LCD.Color);
		LCD_DrawPoint(x+Xadd,y+Yadd,LCD.Color);
		LCD_DrawPoint(x+Xadd,y-Yadd,LCD.Color);
		LCD_DrawPoint(x-Xadd,y-Yadd,LCD.Color);
		
		e2 = err;
		if (e2 <= Yadd) {
			err += ++Yadd*2+1;
			if (-Xadd == Yadd && e2 <= Xadd) e2 = 0;
		}
		if (e2 > Xadd) err += ++Xadd*2+1;
    }
    while (Xadd <= 0);   
}


/***************************************************************************************************************************************
*	�� �� ��: LCD_DrawEllipse
*
*	��ڲ���: x - Բ�� ˮƽ����
*			 	 y - Բ�� ��ֱ����
*			 	 r1  - ˮƽ����ĳ���
*				 r2  - ��ֱ����ĳ���
*
*	��������: ������ (x,y) ����ˮƽ����Ϊ r1 ��ֱ����Ϊ r2 ����Բ����
*
*	˵    ��: 1. �ú�����ֲ��ST�ٷ������������
*				 2. Ҫ���Ƶ������ܳ�����Ļ����ʾ����
*
*****************************************************************************************************************************************/

void LCD_DrawEllipse(int x, int y, int r1, int r2)
{
  int Xadd = -r1, Yadd = 0, err = 2-2*r1, e2;
  float K = 0, rad1 = 0, rad2 = 0;
   
  rad1 = r1;
  rad2 = r2;
  
  if (r1 > r2)
  { 
    do {
      K = (float)(rad1/rad2);
		 
		LCD_DrawPoint(x-Xadd,y+(uint16_t)(Yadd/K),LCD.Color);
		LCD_DrawPoint(x+Xadd,y+(uint16_t)(Yadd/K),LCD.Color);
		LCD_DrawPoint(x+Xadd,y-(uint16_t)(Yadd/K),LCD.Color);
		LCD_DrawPoint(x-Xadd,y-(uint16_t)(Yadd/K),LCD.Color);     
		 
      e2 = err;
      if (e2 <= Yadd) {
        err += ++Yadd*2+1;
        if (-Xadd == Yadd && e2 <= Xadd) e2 = 0;
      }
      if (e2 > Xadd) err += ++Xadd*2+1;
    }
    while (Xadd <= 0);
  }
  else
  {
    Yadd = -r2; 
    Xadd = 0;
    do { 
      K = (float)(rad2/rad1);

		LCD_DrawPoint(x-(uint16_t)(Xadd/K),y+Yadd,LCD.Color);
		LCD_DrawPoint(x+(uint16_t)(Xadd/K),y+Yadd,LCD.Color);
		LCD_DrawPoint(x+(uint16_t)(Xadd/K),y-Yadd,LCD.Color);
		LCD_DrawPoint(x-(uint16_t)(Xadd/K),y-Yadd,LCD.Color);  
		 
      e2 = err;
      if (e2 <= Xadd) {
        err += ++Xadd*3+1;
        if (-Yadd == Xadd && e2 <= Yadd) e2 = 0;
      }
      if (e2 > Yadd) err += ++Yadd*3+1;     
    }
    while (Yadd <= 0);
  }
}


/***************************************************************************************************************************************
*	�� �� ��: LCD_FillCircle
*
*	��ڲ���: x - Բ�� ˮƽ����
*			 	 y - Բ�� ��ֱ����
*			 	 r  - �뾶
*
*	��������: ������ (x,y) ���뾶Ϊ r ��Բ������
*
*	˵    ��: 1. �ú�����ֲ��ST�ٷ������������
*				 2. Ҫ���Ƶ������ܳ�����Ļ����ʾ����
*
*****************************************************************************************************************************************/

void LCD_FillCircle(uint16_t x, uint16_t y, uint16_t r)
{
  int32_t  D;    /* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (r << 1);
  
  CurX = 0;
  CurY = r;
  
  while (CurX <= CurY)
  {
    if(CurY > 0) 
    { 
      LCD_DrawLine_V(x - CurX, y - CurY,2*CurY);
      LCD_DrawLine_V(x + CurX, y - CurY,2*CurY);
    }
    
    if(CurX > 0) 
    {
		// LCD_DrawLine(x - CurY, y - CurX,x - CurY,y - CurX + 2*CurX);
		// LCD_DrawLine(x + CurY, y - CurX,x + CurY,y - CurX + 2*CurX); 	

      LCD_DrawLine_V(x - CurY, y - CurX,2*CurX);
      LCD_DrawLine_V(x + CurY, y - CurX,2*CurX);
    }
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
  LCD_DrawCircle(x, y, r);  
}

/***************************************************************************************************************************************
*	�� �� ��: LCD_FillRect
*
*	��ڲ���: x - ˮƽ����
*			 	 y - ��ֱ����
*			 	 width  - ˮƽ���
*				 height -��ֱ���
*
*	��������: ������ (x,y) ���ָ�������ʵ�ľ���
*
*	˵    ��: Ҫ���Ƶ������ܳ�����Ļ����ʾ����
*						 
*****************************************************************************************************************************************/

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	uint16_t i;

   LCD_SetAddress( x, y, x+width-1, y+height-1);	// ��������	

   LCD_SPI.Instance->CR1 &= 0xFFBF;					// �ر�SPI
   LCD_SPI.Instance->CR1 |= 0x0800;	            // �л���16λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;					// ʹ��SPI
	
	LCD_CS_L;	// Ƭѡ���ͣ�ʹ��IC
	
	for(i=0;i<width*height;i++)				
	{

		LCD_SPI.Instance->DR = LCD.Color;
	   while( (LCD_SPI.Instance->SR & 0x0002) == 0);		// �ȴ����ͻ��������
	}
	while( (LCD_SPI.Instance->SR & 0x0080) != RESET);	//	�ȴ�ͨ�����
	LCD_CS_H;	// Ƭѡ����	
	
	LCD_SPI.Instance->CR1 &= 0xFFBF;	// �ر�SPI
   LCD_SPI.Instance->CR1 &= 0xF7FF;	// �л���8λ���ݸ�ʽ
   LCD_SPI.Instance->CR1 |= 0x0040;	// ʹ��SPI
}


/***************************************************************************************************************************************
*	�� �� ��: LCD_DrawImage
*
*	��ڲ���: x - ��ʼˮƽ����
*				 y - ��ʼ��ֱ����
*			 	 width  - ͼƬ��ˮƽ���
*				 height - ͼƬ�Ĵ�ֱ���
*				*pImage - ͼƬ���ݴ洢�����׵�ַ
*
*	��������: ��ָ�����괦��ʾͼƬ
*
*	˵    ��: 1.Ҫ��ʾ��ͼƬ��Ҫ���Ƚ���ȡģ����ϤͼƬ�ĳ��ȺͿ��
*            2.ʹ�� LCD_SetColor() �������û���ɫ��LCD_SetBackColor() ���ñ���ɫ
*						 
*****************************************************************************************************************************************/

void 	LCD_DrawImage(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t *pImage) 
{  
   uint8_t   disChar;	         // ��ģ��ֵ
	uint16_t  Xaddress = x;       // ˮƽ����
 	uint16_t  Yaddress = y;       // ��ֱ����  
	uint16_t  i=0,j=0,m=0;        // ��������
	uint16_t  BuffCount = 0;      // ����������
   uint16_t  Buff_Height = 0;    // ������������

// ��Ϊ��������С���ޣ���Ҫ�ֶ��д��
   Buff_Height = (sizeof(LCD_Buff)/2) / height;    // ���㻺�����ܹ�д��ͼƬ�Ķ�����

	for(i = 0; i <height; i++)             // ѭ������д��
	{
		for(j = 0; j <(float)width/8; j++)  
		{
			disChar = *pImage;

			for(m = 0; m < 8; m++)
			{ 
				if(disChar & 0x01)	
				{		
               LCD_Buff[BuffCount] =  LCD.Color;			// ��ǰģֵ��Ϊ0ʱ��ʹ�û���ɫ���
				}
				else		
				{		
				   LCD_Buff[BuffCount] = LCD.BackColor;		//����ʹ�ñ���ɫ���Ƶ�
				}
				disChar >>= 1;     // ģֵ��λ
				Xaddress++;        // ˮƽ�����Լ�
				BuffCount++;       // ����������       
				if( (Xaddress - x)==width ) // ���ˮƽ����ﵽ���ַ���ȣ����˳���ǰѭ��,������һ�еĻ���		
				{											 
					Xaddress = x;				                 
					break;
				}
			}	
			pImage++;			
		}
      if( BuffCount == Buff_Height*width  )  // �ﵽ�������������ɵ��������ʱ
      {
         BuffCount = 0; // ������������0

         LCD_SetAddress( x, Yaddress , x+width-1, Yaddress+Buff_Height-1);	// ��������	
         LCD_WriteBuff(LCD_Buff,width*Buff_Height);          // д���Դ�     

         Yaddress = Yaddress+Buff_Height;    // ������ƫ�ƣ���ʼд����һ��������
      }     
      if( (i+1)== height ) // �������һ��ʱ
      {
         LCD_SetAddress( x, Yaddress , x+width-1,i+y);	   // ��������	
         LCD_WriteBuff(LCD_Buff,width*(i+1+y-Yaddress));    // д���Դ�     
      }
	}	
}


/*************************************************************************************************
*	�� �� ��:	LCD_Test_Clear
*
*	��������:	��������
*************************************************************************************************/
void LCD_Test_Clear(void)
{
	uint8_t	i = 0;			// ��������
			
	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_BLACK);				// ���û�����ɫ

	for(i=0;i<8;i++)
	{
		switch (i)		// �л�����ɫ
		{
			case 0: LCD_SetBackColor(LIGHT_RED); 		break;	
			case 1: LCD_SetBackColor(LIGHT_GREEN); 	break;				
			case 2: LCD_SetBackColor(LIGHT_BLUE); 		break;
			case 3: LCD_SetBackColor(LIGHT_YELLOW); 	break;
			case 4: LCD_SetBackColor(LIGHT_CYAN);		break;
			case 5: LCD_SetBackColor(LIGHT_GREY); 		break;
			case 6: LCD_SetBackColor(LIGHT_MAGENTA); 	break;
			case 7: LCD_SetBackColor(LCD_WHITE); 		break;			
			default:	break;			
		}
		LCD_Clear();		// ����
		LCD_DisplayText(13, 70,"STM32 ˢ������");
		LCD_DisplayText(13,106,"��Ļ�ֱ���:240*240");
		LCD_DisplayText(13,142,"������:ST7789");	
		HAL_Delay(1000);	// ��ʱ
	}
}

/*************************************************************************************************
*	�� �� ��:	LCD_Test_Text
*
*	��������:	�ı���ʾ����
*
*	˵    ��:	��ʾ�ı����������������С�����ĺ�ASCII�ַ� 
*************************************************************************************************/
void LCD_Test_Text(void)
{
	LCD_SetBackColor(LCD_BLACK); 			//	���ñ���ɫ
	LCD_Clear(); 								// ����
	
	LCD_SetColor(LCD_WHITE);
	LCD_SetAsciiFont(&ASCII_Font32); LCD_DisplayString(0, 0,"!#$'()*+,-.0123"); 						    		
	LCD_SetAsciiFont(&ASCII_Font24); LCD_DisplayString(0,32,"!#$'()*+,-.012345678"); 				   
	LCD_SetAsciiFont(&ASCII_Font20); LCD_DisplayString(0,56,"!#$'()*+,-.0123456789:;<");      	
	LCD_SetAsciiFont(&ASCII_Font16); LCD_DisplayString(0,76,"!#$'()*+,-.0123456789:;<=>?@AB"); 	
	LCD_SetAsciiFont(&ASCII_Font12); LCD_DisplayString(0,92,"!#$'()*+,-.0123456789:;<=>?@ABCDEFGHIJKL"); 	
																																		
	LCD_SetColor(LCD_CYAN);                                                                             
	LCD_SetAsciiFont(&ASCII_Font12); LCD_DisplayString(0,104,"!#&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKL"); 	
	LCD_SetAsciiFont(&ASCII_Font16); LCD_DisplayString(0,116,"!#&'()*+,-.0123456789:;<=>?@AB"); 	
	LCD_SetAsciiFont(&ASCII_Font20); LCD_DisplayString(0,132,"!#&'()*+,-.0123456789:;<");		  	
	LCD_SetAsciiFont(&ASCII_Font24); LCD_DisplayString(0,152,"!#&'()*+,-.012345678"); 				  	
	LCD_SetAsciiFont(&ASCII_Font32); LCD_DisplayString(0,176,"!#&'()*+,-.0123"); 							  		

	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_YELLOW);				// ���û��ʣ���ɫ
	LCD_DisplayText(0, 216,"ASCII�ַ���");

	HAL_Delay(2000);	// ��ʱ�ȴ�
	LCD_Clear(); 								// ����

	LCD_SetTextFont(&CH_Font12);			// ����1212��������,ASCII�����ӦΪ1206
	LCD_SetColor(0X8AC6D1);					// ���û���
	LCD_DisplayText(14, 10,"1212:¹С��Ƽ�");	
	
	LCD_SetTextFont(&CH_Font16);			// ����1616��������,ASCII�����ӦΪ1608
	LCD_SetColor(0XC5E1A5);					// ���û���
	LCD_DisplayText(14, 30,"1616:¹С��Ƽ�");		
	
	LCD_SetTextFont(&CH_Font20);			// ����2020��������,ASCII�����ӦΪ2010
	LCD_SetColor(0XFFB549);					// ���û���
	LCD_DisplayText(14, 60,"2020:¹С��Ƽ�");		

	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(0XFF585D);					// ���û���
	LCD_DisplayText(14, 90,"2424:¹С��Ƽ�");		

	LCD_SetTextFont(&CH_Font32);			// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(0xFFB6B9);					// ���û���
	LCD_DisplayText(14, 130,"3232:¹С��Ƽ�");		

	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_WHITE);
 	LCD_DisplayText(14, 180,"������ʾ");	  

	HAL_Delay(2000);	// ��ʱ�ȴ�
}
/************************************************************************************************
*	�� �� ��:	LCD_Test_Variable
*
*	��������:	������ʾ������������С��
*************************************************************************************************/
void LCD_Test_Variable (void)
{
	uint16_t i;					// ��������
	int32_t	a = 0;			// �����������������ڲ���
	int32_t	b = 0;			// �����������������ڲ���
	int32_t	c = 0;			// �����������������ڲ���

	double p = 3.1415926;	// ���帡�������������ڲ���
	double f = -1234.1234;	// ���帡�������������ڲ���
	
	LCD_SetBackColor(LCD_BLACK); 			//	���ñ���ɫ
	LCD_Clear(); 								// ����
	
   LCD_SetTextFont(&CH_Font24);     
	LCD_SetColor(LIGHT_CYAN);					// ���û��ʣ�����ɫ		
	LCD_DisplayText(0,10,"����:");				
	LCD_DisplayText(0,40,"����:");					
				
	LCD_SetColor(LIGHT_YELLOW);				// ���û��ʣ�����ɫ		
	LCD_DisplayText(0, 80,"���ո�:");	
	LCD_DisplayText(0,110,"���0:");	
	
	LCD_SetColor(LIGHT_RED);					// ���û���	������ɫ		
	LCD_DisplayText(0, 150,"��С��:");	
	LCD_DisplayText(0, 180,"��С��:");		
	
	for(i=0;i<100;i++)
   {
		LCD_SetColor(LIGHT_CYAN);								// ���û���	������ɫ	
		LCD_ShowNumMode(Fill_Space);							// ����λ���ո�
		LCD_DisplayNumber( 80,10, b+i*10, 4) ;				// ��ʾ����			
		LCD_DisplayNumber( 80,40, c-i*10, 4) ;				// ��ʾ����			
		
		LCD_SetColor(LIGHT_YELLOW);								// ���û��ʣ�����ɫ	

		LCD_ShowNumMode(Fill_Space);								// ����λ��� �ո�
		LCD_DisplayNumber( 130, 80, a+i*150, 8) ;				// ��ʾ����		

		LCD_ShowNumMode(Fill_Zero);								// ����λ���0      
		LCD_DisplayNumber( 130,110, b+i*150, 8) ;				// ��ʾ����			
		
		LCD_SetColor(LIGHT_RED);									// ���û��ʣ�����ɫ			
		LCD_ShowNumMode(Fill_Space);								// ����λ���ո�		
		LCD_DisplayDecimals( 100, 150, p+i*0.1,  6,3);		// ��ʾС��	
		LCD_DisplayDecimals( 100, 180, f+i*0.01, 11,4);		// ��ʾС��		
		
		HAL_Delay(15);				
   }
	HAL_Delay(2500);		
}
/*************************************************************************************************
*	�� �� ��:	LCD_Test_Color
*
*	��������:	��ɫ��
*************************************************************************************************/
void LCD_Test_Color(void)
{
	uint16_t i;					// ��������
	uint16_t y;
// ��ɫ����>>>>>	
	LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
	LCD_Clear(); //������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font20);			// ��������
	LCD_SetColor(LCD_WHITE);				// ���û�����ɫ
	LCD_DisplayText(0,0,"RGB����ɫ:");

	//ʹ�û��ߺ�����������ɫɫ��
	for(i=0;i<240;i++)
	{
		LCD_SetColor( LCD_RED-(i<<16) );
      LCD_DrawLine_V(0+i,  20,10);
	}
	for(i=0;i<240;i++)
	{
		LCD_SetColor( LCD_GREEN-(i<<8) );
      LCD_DrawLine_V(0+i,  35,10);
	}
	for(i=0;i<240;i++)
	{
		LCD_SetColor( LCD_BLUE-i );
      LCD_DrawLine_V(0+i,  50,10);
	}	

   y = 70;
   LCD_SetColor(LIGHT_CYAN);    LCD_FillRect(150,y+5     ,90,10);  LCD_DisplayString(0,y     ,"LIGHT_CYAN");	   
	LCD_SetColor(LIGHT_MAGENTA); LCD_FillRect(150,y+20*1+5,90,10);  LCD_DisplayString(0,y+20*1,"LIGHT_MAGENTA");	
	LCD_SetColor(LIGHT_YELLOW);  LCD_FillRect(150,y+20*2+5,90,10);  LCD_DisplayString(0,y+20*2,"LIGHT_YELLOW");	
	LCD_SetColor(LIGHT_GREY);    LCD_FillRect(150,y+20*3+5,90,10);  LCD_DisplayString(0,y+20*3,"LIGHT_GREY");  	

   LCD_SetColor(DARK_CYAN);     LCD_FillRect(150,y+20*4+5,90,10);  LCD_DisplayString(0,y+20*4,"DARK_CYAN");		
	LCD_SetColor(DARK_MAGENTA);  LCD_FillRect(150,y+20*5+5,90,10);  LCD_DisplayString(0,y+20*5,"DARK_MAGENTA");	
	LCD_SetColor(DARK_YELLOW);   LCD_FillRect(150,y+20*6+5,90,10);  LCD_DisplayString(0,y+20*6,"DARK_YELLOW");	
	LCD_SetColor(DARK_GREY);     LCD_FillRect(150,y+20*7+5,90,10);	 LCD_DisplayString(0,y+20*7,"DARK_GREY");	

	HAL_Delay(2000);
}

/*************************************************************************************************
*	�� �� ��:	LCD_Test_Grahic
*
*	��������:	2Dͼ�λ���
*************************************************************************************************/
void LCD_Test_Grahic(void)
{
	LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
	LCD_Clear(); //������ˢ����ɫ	

	LCD_SetColor(LCD_WHITE);	
	LCD_DrawRect(0,0,240,240); 			//���ƾ���

	LCD_SetColor(LCD_RED);    LCD_FillCircle(140,50,30);		//���Բ��
	LCD_SetColor(LCD_GREEN);  LCD_FillCircle(170,50,30); 	
	LCD_SetColor(LCD_BLUE);   LCD_FillCircle(200,50,30);  	
	
	LCD_SetColor(LCD_YELLOW);
	LCD_DrawLine(26,26,113,64);				//��ֱ��
	LCD_DrawLine(35,22,106,81);				//��ֱ��
	LCD_DrawLine(45,20, 93,100);				//��ֱ��
	LCD_DrawLine(52,16, 69,108);				//��ֱ��
	LCD_DrawLine(62,16, 44,108);				//��ֱ��
	
	LCD_SetColor(LIGHT_CYAN);
	LCD_DrawCircle(120,170,30);			//����Բ��
	LCD_DrawCircle(120,170,20);   

	LCD_SetColor(LIGHT_RED);	
	LCD_DrawEllipse(120,170,90,40); 	//������Բ 
	LCD_DrawEllipse(120,170,70,40); 	//������Բ    
	LCD_SetColor(LIGHT_MAGENTA);	
	LCD_DrawEllipse(120,170,100,50); 	//������Բ
	LCD_DrawEllipse(120,170,110,60);  

	HAL_Delay(2000);		
}
/*************************************************************************************************
*	�� �� ��:	LCD_Test_Image
*
*	��������:	ͼƬ��ʾ����
*************************************************************************************************/
void LCD_Test_Image(void)
{
	LCD_SetBackColor(LCD_BLACK); 			//	���ñ���ɫ
	LCD_Clear(); 								// ����
	
	LCD_SetColor( 0xffF6E58D);
	LCD_DrawImage( 19, 21, 83, 83, Image_Android_83x83) ;	   // ��ʾͼƬ

	LCD_SetColor( 0xffDFF9FB);
	LCD_DrawImage( 141, 21, 83, 83, Image_Message_83x83) ;	// ��ʾͼƬ
	
	LCD_SetColor( 0xff9DD3A8);
	LCD_DrawImage( 19, 140, 83, 83, Image_Toys_83x83) ;		// ��ʾͼƬ
	
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage( 141, 140, 83, 83, Image_Video_83x83) ;		// ��ʾͼƬ

	HAL_Delay(2000);	
}
/*************************************************************************************************
*	�� �� ��:	LCD_Test_Direction
*
*	��������:	������ʾ����
*************************************************************************************************/
void  LCD_Test_Direction(void)
{
	for(int i=0;i<4;i++)
	{  
      LCD_SetBackColor(LCD_BLACK); 			//	���ñ���ɫ
      LCD_Clear(); 								// ����
      LCD_SetTextFont(&CH_Font24);  
	   LCD_SetColor( 0xffDFF9FB);         
		switch (i)		// �л�����ɫ
		{
			case 0:  
            LCD_SetDirection(Direction_V);		   
            LCD_DisplayText(20,20,"Direction_V"); 
         break;	

			case 1:  
            LCD_SetDirection(Direction_H); 	
            LCD_DisplayText(20,20,"Direction_H"); 
         break;	

			case 2:  
            LCD_SetDirection(Direction_V_Flip); 
            LCD_DisplayText(20,20,"Direction_V_Flip"); 
         break;
			case 3: 
            LCD_SetDirection(Direction_H_Flip); 	
            LCD_DisplayText(20,20,"Direction_H_Flip"); 
         break;
	
			default:	break;			
		}
      LCD_SetColor( 0xffF6E58D);
      LCD_DrawImage( 19, 80, 83, 83, Image_Android_83x83) ;	   // ��ʾͼƬ
      LCD_SetTextFont(&CH_Font32);
      LCD_SetColor( 0xff9DD3A8);  
      LCD_DisplayText(130,90,"¹С��");
      LCD_DisplayText(130,130,"�Ƽ�");
 
      HAL_Delay(1000);	// ��ʱ
	}
}

/**************************************************************************************************************************************************************************************************************************************¹С�� STM32F4���İ�******************************LXB***/
// ʵ��ƽ̨��¹С�� STM32F4���İ�
//


