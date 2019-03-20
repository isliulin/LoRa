/*
 * user_solenoid.h	LED����ͷ�ļ�
*/

#ifndef __USER_LED_H__
#define __USER_LED_H__

#include "user_config.h"

typedef enum 
{
    E_LED_INIT          =0,  // �����������ʼ��,��ȡ����������
    E_LED_CONNECT       ,    // ����������
    E_LED_DISCONNECT    ,    // ��������
    E_LED_LOCATION      ,    // GPS��λ
    E_LED_ERROR         ,    // �����쳣��SIMģ���𻵡�SIM�����𻵣�
    E_LED_SENDOK        ,    // �������
}E_LedState;

void InitLed(void);

void LedOpen(void);

void LedClose(void);

void LedChangeState(E_LedState State);

void LedDisplay(uint16_t BaseTime);

#endif /* __USER_LED_H__ */
