/**
 ****************************************************************************************************
 * @file        usmart.h
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
 ****************************************************************************************************
 */

#ifndef __USMART_H
#define __USMART_H

#include "usmart_port.h"


#define USMART_OK               0       /* �޴��� */
#define USMART_FUNCERR          1       /* �������� */
#define USMART_PARMERR          2       /* �������� */
#define USMART_PARMOVER         3       /* ������� */
#define USMART_NOFUNCFIND       4       /* δ�ҵ�ƥ�亯�� */

#define SP_TYPE_DEC             0       /* 10���Ʋ�����ʾ */
#define SP_TYPE_HEX             1       /* 16���Ʋ�����ʾ */


/* �������б� */
struct _m_usmart_nametab
{
    void *func;             /* ����ָ�� */
    const char *name;       /* ������(���Ҵ�) */
};

/* usmart���ƹ����� */
struct _m_usmart_dev
{
    struct _m_usmart_nametab *funs;     /* ������ָ�� */

    void (*init)(uint16_t tclk);        /* ��ʼ�� */
    uint8_t (*cmd_rec)(char *str);   /* ʶ������������ */
    void (*exe)(void);                  /* ִ��  */
    void (*scan)(void);                 /* ɨ�� */
    uint8_t fnum;                       /* �������� */
    uint8_t pnum;                       /* �������� */
    uint8_t id;                         /* ����id */
    uint8_t sptype;                     /* ������ʾ����(���ַ�������):0,10����;1,16����; */
    uint16_t parmtype;                  /* ���������� */
    uint8_t  plentbl[MAX_PARM];         /* ÿ�������ĳ����ݴ�� */
    uint8_t  parm[PARM_LEN];            /* �����Ĳ��� */
    uint8_t runtimeflag;                /* 0,��ͳ�ƺ���ִ��ʱ��;1,ͳ�ƺ���ִ��ʱ��,ע��:�˹��ܱ�����USMART_ENTIMX_SCANʹ�ܵ�ʱ��,������ */
    uint32_t runtime;                   /* ����ʱ��,��λ:0.1ms,�����ʱʱ��Ϊ��ʱ��CNTֵ��2��*0.1ms */
};

extern struct _m_usmart_nametab usmart_nametab[];   /* ��usmart_config.c���涨�� */
extern struct _m_usmart_dev usmart_dev;             /* ��usmart_config.c���涨�� */


void usmart_init(uint16_t tclk);        /* ��ʼ�� */
uint8_t usmart_cmd_rec(char*str);    	/* ʶ�� */
void usmart_exe(void);                  /* ִ�� */
void usmart_scan(void);                 /* ɨ�� */
uint32_t read_addr(uint32_t addr);      /* ��ȡָ����ַ��ֵ */
void write_addr(uint32_t addr,uint32_t val);/* ��ָ����ַд��ָ����ֵ */

#endif






























