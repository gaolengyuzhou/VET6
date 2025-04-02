/**
 ****************************************************************************************************
 * @file        usmart_port.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V3.5
 * @date        2020-12-20
 * @brief       USMART ���ڵ������
 *
 *              USMART����ALIENTEK������һ�����ɵĴ��ڵ��Ի������,ͨ�� ��,�����ͨ���������ֵ��ó�
 *              ��������κκ���,��ִ��.���,�����������ĺ������������(֧������(10/16����,֧�ָ���)
 *              ���ַ�����������ڵ�ַ����Ϊ����),�����������֧��10���������,��֧�ֺ����� ��ֵ��ʾ.
 *              V2.1�汾�Ժ�����hex��dec����ָ��.���ǿ����������ú�����������ʾ��ʽ.Ҳ�����������ݵĽ�
 *              ��ת��,����:
 *              ����"hex 100"  ���ڴ��ڵ��������Ͽ��� HEX 0X64.
 *              ����"dec 0X64" ���ڴ��ڵ��������Ͽ��� DEC 100.
 *   @note
 *              USMART��Դռ�����@MDK 3.80A@2.0�汾��
 *              FLASH:4K~K�ֽ�(ͨ��USMART_USE_HELP��USMART_USE_WRFUNS����)
 *              SRAM:72�ֽ�(���ٵ������)
 *              SRAM���㹫ʽ:   SRAM=PARM_LEN+72-4  ����PARM_LEN������ڵ���4.
 *              Ӧ�ñ�֤��ջ��С��100���ֽ�.
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 * 
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵�� 
 * 
 * V3.4֮ǰ�汾��ϸ�޸�˵����USMART�ļ����µ�:readme.txt
 * 
 * V3.4 20200324
 * 1, ����usmart_port.c��usmart_port.h,���ڹ���USMART����ֲ,�����޸�
 * 2, �޸ı���������ʽΪ: uint8_t, uint16_t, uint32_t
 * 3, �޸�usmart_reset_runtimeΪusmart_timx_reset_time
 * 4, �޸�usmart_get_runtimeΪusmart_timx_get_time
 * 5, �޸�usmart_scan����ʵ�ַ�ʽ,�ĳ���usmart_get_input_string��ȡ������
 * 6, �޸�printf����ΪUSMART_PRINTF�궨��
 * 7, �޸Ķ�ʱɨ����غ���,���ú궨�巽ʽ,������ֲ
 *
 * V3.5 20201220
 * 1���޸Ĳ��ִ�����֧��AC6������
 *
 ****************************************************************************************************
 */
 
#ifndef __USMART_PORT_H
#define __USMART_PORT_H

#include "sys.h"
#include "usart.h"




/******************************************************************************************/
/* �û����ò��� */


#define MAX_FNAME_LEN           30      /* ��������󳤶ȣ�Ӧ������Ϊ��С����������ĳ��ȡ� */
#define MAX_PARM                10      /* ���Ϊ10������ ,�޸Ĵ˲���,�����޸�usmart_exe��֮��Ӧ. */
#define PARM_LEN                200     /* ���в���֮�͵ĳ��Ȳ�����PARM_LEN���ֽ�,ע�⴮�ڽ��ղ���Ҫ��֮��Ӧ(��С��PARM_LEN) */


#define USMART_ENTIMX_SCAN      1       /* ʹ��TIM�Ķ�ʱ�ж���ɨ��SCAN����,�������Ϊ0,��Ҫ�Լ�ʵ�ָ�һ��ʱ��ɨ��һ��scan����.
                                         * ע��:���Ҫ��runtimeͳ�ƹ���,��������USMART_ENTIMX_SCANΪ1!!!!
                                         */

#define USMART_USE_HELP         1       /* ʹ�ð�������ֵ��Ϊ0�����Խ�ʡ��700���ֽڣ����ǽ������޷���ʾ������Ϣ�� */
#define USMART_USE_WRFUNS       1       /* ʹ�ö�д����,ʹ������,���Զ�ȡ�κε�ַ��ֵ,������д�Ĵ�����ֵ. */

#define USMART_PRINTF           printf  /* ����printf��� */

/******************************************************************************************/
/* USMART��ʱ�� ���� */

# if USMART_ENTIMX_SCAN == 1    /* ������ʹ�ܶ�ʱ��ɨ��,����Ҫ���¶��� */

/* TIMX �ж϶��� 
 * ���ڶ�ʱ����usmart.scan����ɨ�贮������,��ִ����ز���
 * ע��: ͨ���޸���4���궨��,����֧��TIM1~TIM17����һ����ʱ��.
 */
#define USMART_TIMX                     TIM4
#define USMART_TIMX_IRQn                TIM4_IRQn
#define USMART_TIMX_IRQHandler          TIM4_IRQHandler
#define USMART_TIMX_CLK_ENABLE()        do{ __HAL_RCC_TIM4_CLK_ENABLE(); }while(0)  /* TIMX ʱ��ʹ�� */

#endif

/******************************************************************************************/


/* ���û�ж���uint32_t,���� */
#ifndef uint32_t
typedef unsigned           char uint8_t;
typedef unsigned short     int  uint16_t;
typedef unsigned           int  uint32_t;
#endif



char * usmart_get_input_string(void);        /* ��ȡ���������� */
void usmart_timx_reset_time(void);              /* ��λ����ʱ�� */
uint32_t usmart_timx_get_time(void);            /* ��ȡ����ʱ�� */
void usmart_timx_init(uint16_t arr, uint16_t psc);   /* ��ʼ����ʱ�� */

#endif



























