/*
 * user_adc.h	adc����ͷ�ļ������ڵ����ɼ������������ݲɼ�(ˮѹ)
*/

#ifndef __USER_ADC_H__
#define __USER_ADC_H__


#include "user_config.h"

#define VREFINT_ADC (*((uint16_t *)0x1FF80078)) //�ο���ѹ��ADCֵ
#define REFINT_VDD 3							//���Բο���ѹ��ADCֵʱ�Ĺ����ѹ��datasheet����д

/*
 * AdcInit:				��ʼ��ADC
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t AdcInit(void);

/*
 *AdcGetValue: 			��ȡADCĳͨ���ϵ�ƽ��ֵ
 *channel:				ͨ��,����ֵΪADC_CHANNEL_0~ADC_CHANNEL_18
 *time:					����
 *����ֵ:				��ȡ����ADCĳͨ���ϵ�ƽ��ֵ
*/
uint32_t AdcGetValue(uint32_t channel,uint8_t time);

/*
 *AdcGetVoltage: 		��ȡADCĳͨ���ϵĵ�ѹֵ,��Ե�ѹ�����ڲ���ѹ
 *channel:				ͨ��,����ֵΪADC_CHANNEL_0~ADC_CHANNEL_18
 *����ֵ:				��ȡ����ADCĳͨ���ϵĵ�ѹֵ,����0��ʾʧ��
*/
float AdcGetVoltage(uint32_t channel);

int32_t AdcGetBattery(int32_t *Battery);
/*
 *AdcGetChargePower: 		��ȡ����ѹ
 *Battery:				��ص�����0-100%
 *����ֵ:				��ص�����0-100%
 */
float AdcGetChargePower(float ChargePower);
#endif /* __ADC_H__ */

