#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* �û�������
 * ������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ�����)
 */

#include "sys.h"
#include "delay.h"
#include "lcd_spi_154.h"

extern void LCD_Test_Clear(void);
extern void LCD_Test_Text(void);
extern void LCD_Test_Variable (void);
extern void LCD_Test_Grahic(void);
extern void LCD_Test_Image(void);
extern void LCD_Test_Direction(void);

/* �������б���ʼ��(�û��Լ�����)
 * �û�ֱ������������Ҫִ�еĺ�����������Ҵ�
 */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* ���ʹ���˶�д���� */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr, uint32_t val)",
#endif
    (void *)delay_ms, "void delay_ms(uint16_t nms)",
    (void *)delay_us, "void delay_us(uint32_t nus)",
        
    (void *)LCD_Clear, "LCD_Clear(void)",
		(void *)LCD_Test_Clear,"void LCD_Test_Clear(void)",
		(void *)LCD_Test_Text,"void LCD_Test_Text(void)",
		(void *)LCD_Test_Variable,"void LCD_Test_Variable (void)",
		(void *)LCD_Test_Grahic,"void LCD_Test_Grahic(void)",
		(void *)LCD_Test_Image,"void LCD_Test_Image(void)",
		(void *)LCD_Test_Direction,"void LCD_Test_Direction(void)",

};

/******************************************************************************************/

/* �������ƹ�������ʼ��
 * �õ������ܿغ���������
 * �õ�����������
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* �������� */
    0,      /* �������� */
    0,      /* ����ID */
    1,      /* ������ʾ����,0,10����;1,16���� */
    0,      /* ��������.bitx:,0,����;1,�ַ��� */
    0,      /* ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ�� */
    0,      /* �����Ĳ���,��ҪPARM_LEN��0��ʼ�� */
};



















