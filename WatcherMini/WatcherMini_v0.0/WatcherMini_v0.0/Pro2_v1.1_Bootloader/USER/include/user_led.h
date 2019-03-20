/*
 * user_solenoid.h	LED����ͷ�ļ�
*/

#ifndef __USER_LED_H__
#define __USER_LED_H__

#include "stm32l0xx_hal.h"

/*
 * InitLed:				��ʼ��LED
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t InitLed(void);

/*
 * LedPowerOn:				��LED�����Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t LedPowerOn(void);

/*
 * LedPowerOff:			��LED�����Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t LedPowerOff(void);

/*
 * OpenLed:				����Led��
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t OpenLed(void);

/*
 * CloseLed:			Ϩ��Led��
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t CloseLed(void);

/*
 * FlashLed:			��˸Led��
 * rate:				��˸Ƶ��Hz ��ЧֵΪ1~1000
 * count:				��˸����,0��ʾһֱ��˸
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t FlashLed(uint32_t rate , uint32_t count);

#endif /* __USER_LED_H__ */
