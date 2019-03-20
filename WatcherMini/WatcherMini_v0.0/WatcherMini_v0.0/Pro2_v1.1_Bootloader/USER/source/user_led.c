/*
 * led.c	LED�����ļ�
*/

#include "user_led.h"

/*
 * InitLed:				��ʼ��LED
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t InitLed(void)
{
	//MX_GPIO_Init();�ѳ�ʼ��
	
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	HAL_GPIO_WritePin(GPIOA,LED_Pin, GPIO_PIN_RESET);
//	GPIO_InitStruct.Pin |= LED_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	return 1;
}

/*
 * LedPowerOn:			��LED�����Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t LedPowerOn(void)
{
	//led�޵�Դ���أ�ֱ����VDD����
	return 1;
}

/*
 * LedPowerOff:			��LED�����Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t LedPowerOff(void)
{
	//led�޵�Դ���أ�ֱ����VDD���ӣ������޷��ر�Led�ĵ�Դ
	return 0;
}

/*
 * OpenSolenoid:		��Led
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t OpenLed(void)
{
	//led�޵�Դ���أ�ֱ����VDD���ӣ��͵�ƽ��
	HAL_GPIO_WritePin(Out_LED_GPIO_Port,Out_LED_Pin, GPIO_PIN_SET);
	return 1;
}

/*
 * CloseLed:			�ر�Led
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t CloseLed(void)
{
	//led�޵�Դ���أ�ֱ����VDD���ӣ��ߵ�ƽ�ر�
	HAL_GPIO_WritePin(Out_LED_GPIO_Port,Out_LED_Pin, GPIO_PIN_RESET);
	return 1;
}

/*
 * FlashLed:			��˸Led
 * rate:				��˸Ƶ��Hz ��ЧֵΪ1~1000
 * count:				��˸����,0��ʾһֱ��˸
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t FlashLed(uint32_t rate , uint32_t count)
{
	for(uint32_t i=0;i<count;i++)
	{
		uint32_t time=1000/rate;
		OpenLed();
		HAL_Delay(time/2);
		CloseLed();
		HAL_Delay(time/2);
	}
	return 1;
}
