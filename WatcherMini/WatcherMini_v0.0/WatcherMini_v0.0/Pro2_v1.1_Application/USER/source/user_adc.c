/*
 * user_adc.h	adc����ͷ�ļ������ڵ����ɼ������������ݲɼ�(ˮѹ)
*/

#include "user_adc.h"

extern ADC_HandleTypeDef hadc;

/*
 * AdcInit:				��ʼ��ADC
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t AdcInit(void)
{
	//ʹ�����´����ʼ������Ҫ��MX_ADC_Init()ע����
	hadc.Instance = ADC1;
	hadc.Init.OversamplingMode = DISABLE;
	hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;
	hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc.Init.LowPowerAutoWait = DISABLE;
	hadc.Init.LowPowerFrequencyMode = DISABLE;
	hadc.Init.LowPowerAutoPowerOff = DISABLE;
	if (HAL_ADC_Init(&hadc) != HAL_OK)
		return 0;
	return 1;
}

/*
 *AdcGetValue: 			��ȡADCĳͨ���ϵ�Adcƽ��ֵ
 *channel:				ͨ��,����ֵΪADC_CHANNEL_0~ADC_CHANNEL_18
 *time:					����
 *����ֵ:				��ȡ����ADCĳͨ���ϵ�ADCƽ��ֵ
*/
uint32_t AdcGetValue(uint32_t channel,uint8_t time)
{
	uint32_t ret_value = 0;
	uint32_t adc_val=0;
	ADC_ChannelConfTypeDef Adc_ChanCof;
	Adc_ChanCof.Channel=channel;
	Adc_ChanCof.Rank = ADC_RANK_CHANNEL_NUMBER;//��ӵ�������
	HAL_ADC_ConfigChannel(&hadc,&Adc_ChanCof);
	
    HAL_ADCEx_Calibration_Start(&hadc, 0); //У��
    
	for(uint8_t i = 0; i < time; i++){
		HAL_ADC_Start(&hadc);
		HAL_ADC_PollForConversion(&hadc, 100);
		while(!HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC));
		adc_val = HAL_ADC_GetValue(&hadc);
//		DEBUG("adc_val%d:%x\r\n",i,adc_val);
		ret_value += adc_val;
	}
    
    HAL_ADC_Stop(&hadc);
    
	Adc_ChanCof.Channel=channel;
	Adc_ChanCof.Rank = ADC_RANK_NONE;//��������ɾ��
	HAL_ADC_ConfigChannel(&hadc,&Adc_ChanCof);
	return ret_value/time;
}
/*
 *AdcGetVoltage: 		��ȡADCĳͨ���ϵĵ�ѹֵ,��Ե�ѹ�����ڲ���ѹ
 *channel:				ͨ��,����ֵΪADC_CHANNEL_0~ADC_CHANNEL_18
 *����ֵ:				��ȡ����ADCĳͨ���ϵĵ�ѹֵ,����0��ʾʧ��
*/
float AdcGetVoltage(uint32_t channel)
{
	uint32_t adc_val = AdcGetValue(channel,20);
	uint32_t adc_VREFINT = AdcGetValue(ADC_CHANNEL_VREFINT,20);//��Ե�ѹֵ
	if(adc_val == 0)
		return -1;
	if(adc_VREFINT == 0)
		return -1;
		
	float VREFINT=VREFINT_ADC*REFINT_VDD/4095.0;//�õ���׼�Ĳο���ѹֵ,Ҳ����ʹ��datasheet�ĵ���ֵ1.224v
//	DEBUG("VREFINT_ADC_val:%x,%u\r\n",VREFINT_ADC,VREFINT_ADC);
//	DEBUG("VREFINT_Voltage:%f\r\n",VREFINT);
//	DEBUG("adc_val:%x,%u\r\n",adc_val,adc_val);
	return  adc_val*VREFINT/adc_VREFINT;
}

/*
 *AdcGetBattery: 		��ȡ��ص�ѹ
 *Battery:				��ص�����0-100%
 *����ֵ:				��ص�����0-100%
 */
int32_t AdcGetBattery(int32_t *Battery)
{
	float a=AdcGetVoltage(ADC_CHANNEL_1)*2*1.0312;//���Լ���
	DEBUG("Battery Voltage:%f\r\n",a);
    if(a<3.6)
    {
        *Battery=0;
	}
    else
    {
        *Battery=(int32_t)((a-3.6)/(4.18-3.6)*100);//��·�����100K��100K����߳���ѹΪ4.2V
        DEBUG("Battery:%d��\r\n",*Battery);
	}
    if(*Battery>100)
        *Battery=100;
    return *Battery;
}

/*
 *AdcGetChargePower: 		��ȡ����ѹ
 *Battery:				��ص�����0-100%
 *����ֵ:				��ص�����0-100%
 */
float AdcGetChargePower(float ChargePower)
{
	ChargePower=(int32_t)(AdcGetVoltage(ADC_CHANNEL_0)*6.1);//��·�����100K��510K
	return ChargePower;
}

