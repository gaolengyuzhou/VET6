/**
 ****************************************************************************************************
 * @file        usmart_port.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V3.5
 * @date        2020-12-20
 * @brief       USMART ��ֲ�ļ�
 *
 *              ͨ���޸ĸ��ļ�,���Է���Ľ�USMART��ֲ����������
 *              ��:USMART_ENTIMX_SCAN == 0ʱ,����Ҫʵ��: usmart_get_input_string����.
 *              ��:USMART_ENTIMX_SCAN == 1ʱ,��Ҫ��ʵ��4������:
 *              usmart_timx_reset_time
 *              usmart_timx_get_time
 *              usmart_timx_init
 *              USMART_TIMX_IRQHandler
 *
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

#include "usmart.h"
#include "usmart_port.h"

TIM_HandleTypeDef g_timx_usmart_handle;      /* ��ʱ����� */

/**
 * @brief       ��ȡ����������(�ַ���)
 *   @note      USMARTͨ�������ú������ص��ַ����Ի�ȡ����������������Ϣ
 * @param       ��
 * @retval
 *   @arg       0,  û�н��յ�����
 *   @arg       ����,�������׵�ַ(������0)
 */
char *usmart_get_input_string(void)
{
    uint8_t len;
    char *pbuf = 0;

    if (g_usart_rx_sta & 0x8000)        /* ���ڽ�����ɣ� */
    {
        len = g_usart_rx_sta & 0x3fff;  /* �õ��˴ν��յ������ݳ��� */
        g_usart_rx_buf[len] = '\0';     /* ��ĩβ���������. */
        pbuf = (char*)g_usart_rx_buf;
        g_usart_rx_sta = 0;             /* ������һ�ν��� */
    }

    return pbuf;
}

/* ���ʹ���˶�ʱ��ɨ��, ����Ҫ�������º��� */
#if USMART_ENTIMX_SCAN == 1

/**
 * ��ֲע��:��������stm32Ϊ��,���Ҫ��ֲ������mcu,������Ӧ�޸�.
 * usmart_reset_runtime,�����������ʱ��,��ͬ��ʱ���ļ����Ĵ����Լ���־λһ������.��������װ��ֵΪ���,������޶ȵ��ӳ���ʱʱ��.
 * usmart_get_runtime,��ȡ��������ʱ��,ͨ����ȡCNTֵ��ȡ,����usmart��ͨ���жϵ��õĺ���,���Զ�ʱ���жϲ�����Ч,��ʱ����޶�
 * ֻ��ͳ��2��CNT��ֵ,Ҳ���������+���һ��,���������2��,û������,���������ʱ,������:2*������CNT*0.1ms.��STM32��˵,��:13.1s����
 * ������:USMART_TIMX_IRQHandler��Timer4_Init,��Ҫ����MCU�ص������޸�.ȷ������������Ƶ��Ϊ:10Khz����.����,��ʱ����Ҫ�����Զ���װ�ع���!!
 */

/**
 * @brief       ��λruntime
 *   @note      ��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
 * @param       ��
 * @retval      ��
 */
void usmart_timx_reset_time(void)
{
    __HAL_TIM_CLEAR_FLAG(&g_timx_usmart_handle, TIM_FLAG_UPDATE); /* ����жϱ�־λ */
    __HAL_TIM_SET_AUTORELOAD(&g_timx_usmart_handle, 0XFFFF);      /* ����װ��ֵ���õ���� */
    __HAL_TIM_SET_COUNTER(&g_timx_usmart_handle, 0);              /* ��ն�ʱ����CNT */
    usmart_dev.runtime = 0;
}

/**
 * @brief       ���runtimeʱ��
 *   @note      ��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
 * @param       ��
 * @retval      ִ��ʱ��,��λ:0.1ms,�����ʱʱ��Ϊ��ʱ��CNTֵ��2��*0.1ms
 */
uint32_t usmart_timx_get_time(void)
{
    if (__HAL_TIM_GET_FLAG(&g_timx_usmart_handle, TIM_FLAG_UPDATE) == SET)  /* �������ڼ�,�����˶�ʱ����� */
    {
        usmart_dev.runtime += 0XFFFF;
    }
    usmart_dev.runtime += __HAL_TIM_GET_COUNTER(&g_timx_usmart_handle);
    return usmart_dev.runtime;                                 /* ���ؼ���ֵ */
}

/**
 * @brief       ��ʱ����ʼ������
 * @param       arr:�Զ���װ��ֵ
 *              psc:��ʱ����Ƶϵ��
 * @retval      ��
 */ 
void usmart_timx_init(uint16_t arr, uint16_t psc)
{
    USMART_TIMX_CLK_ENABLE();
    
    g_timx_usmart_handle.Instance = USMART_TIMX;                 /* ͨ�ö�ʱ��4 */
    g_timx_usmart_handle.Init.Prescaler = psc;                   /* ��Ƶϵ�� */
    g_timx_usmart_handle.Init.CounterMode = TIM_COUNTERMODE_UP;  /* ���ϼ����� */
    g_timx_usmart_handle.Init.Period = arr;                      /* �Զ�װ��ֵ */
    g_timx_usmart_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&g_timx_usmart_handle);
    HAL_TIM_Base_Start_IT(&g_timx_usmart_handle);                /* ʹ�ܶ�ʱ���Ͷ�ʱ���ж� */
    HAL_NVIC_SetPriority(USMART_TIMX_IRQn,3,3);                  /* �����ж����ȼ�����ռ���ȼ�3�������ȼ�3 */
    HAL_NVIC_EnableIRQ(USMART_TIMX_IRQn);                        /* ����ITM�ж� */ 
}

/**
 * @brief       USMART��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void USMART_TIMX_IRQHandler(void)
{
    if(__HAL_TIM_GET_IT_SOURCE(&g_timx_usmart_handle,TIM_IT_UPDATE)==SET)/* ����ж� */
    {
        usmart_dev.scan();                                   /* ִ��usmartɨ�� */
        __HAL_TIM_SET_COUNTER(&g_timx_usmart_handle, 0);;    /* ��ն�ʱ����CNT */
        __HAL_TIM_SET_AUTORELOAD(&g_timx_usmart_handle, 100);/* �ָ�ԭ�������� */
    }
    
    __HAL_TIM_CLEAR_IT(&g_timx_usmart_handle, TIM_IT_UPDATE);/* ����жϱ�־λ */
}

#endif
















