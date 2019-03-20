/*
**************************************************************************************************************
*	@file	bq24195.c
*	@author 
*	@version 
*	@date    
*	@brief	��������bq24195��Դ����оƬ
***************************************************************************************************************
*/

#include "i2c.h"
#include "bq24195.h"
#include "user_bq24195.h"
#include "delay.h"
#include "debug.h"
#include "power.h"
#include "user-app.h"

#define BATTERY_DEVICE		1	//����豸
#define NON_BATTERY_DEVICE	0	//�ǵ���豸
#define SET_BQ24195			1		

uint8_t device_type = BATTERY_DEVICE;//�豸����

void InitPower(void)
{
	InitBq24195();				//��Դ�����ʼ������ʼ��i2c�Լ���Ӧ�жϽ�
	
	for(uint8_t i = 0; i < 5; i++)
	{
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_15);	//PB9��0 
		delay_ms(200);
	}
	Bq24195EnableCharge( );	
	
#if SET_BQ24195   ///��ʹ��������ó��IC����������
	
	//161208��¼����ǿ���趨�豸����Ϊ����豸�������ĵ�һ��watcher��gprs v2��lora�ڵ��Ϊ�������ֱ������Դ���豸��
	device_type = BATTERY_DEVICE;

	uint8_t state = 0;
			
	state = SetHighImpedanceMode(DISABLE_CONTROL);		//�������̬״̬	
	
	DEBUG(3,"state = %02x\r\n",state);
	
	SetMiniSysVoltage(SYSTEM_VOLTAGE_3000MV		//������Сϵͳ��ѹ3.7v
		+SYSTEM_VOLTAGE_400MV
		+SYSTEM_VOLTAGE_200MV
		+SYSTEM_VOLTAGE_100MV);
	SetWdgTimer(WATCHER_DOG_TIMER_DISABLE);		//����оƬ��д���Ź�
	SetInputVoltageLimit(INPUT_VOLTAGE_3880MV
		+INPUT_VOLTAGE_640MV
		+INPUT_VOLTAGE_320MV
		+INPUT_VOLTAGE_160MV
		+INPUT_VOLTAGE_80MV);
	SetInputCurrentLimit(INPUT_CURRENT_2000MA);	//���������������2A
	SetEnableChargeTimer(DISABLE_CONTROL);		//���ó�糬ʱ���Ź�
#endif

	if (device_type==BATTERY_DEVICE)			//����Ǵ�����豸
	{
			//SetChargeType(CHARGER_BATTERY);	
			//������Ƶ�ѹ=4.016v
			//SetChargeVoltageLimit(CHARGE_VOLTAGE_3504MV+CHARGE_VOLTAGE_512MV);
			//���³����ֵΪ��300mv����3.716v��
		SetBatteryRechargeThreshold(BTATTERY_RECHARGE_300MV);
		if (CheckBattery()==0)					//��ص�����
		{
			DEBUG(2,"battery extremely low;enter standby\r\n");
//			Into_Low_Power( ); 					//��ص����ر�ͣ�ֱ�����ý�������
		}
	}
	else
	{
		SetChargeType(CHARGER_DISABLE);			//���ǵ���豸����ó��
	}
}
  
/*
*PB8  PB9�ֿ���ʼ�������������жϡ�GPIO��ʼ������
*/
void InitBq24195(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	__HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOBʱ��

	GPIO_Initure.Pin=GPIO_PIN_9; //PB9 
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_NOPULL;          //����
	GPIO_Initure.Speed=GPIO_SPEED_LOW;     //����
	
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);  

	GPIO_Initure.Pin=GPIO_PIN_8;  
	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      			//�����ش����������ж�Ӱ����
	GPIO_Initure.Pull=GPIO_PULLDOWN;
	
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_15; //PA15 
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);	//PB9��0 
	
		//�ж���4-PC15
//	HAL_NVIC_SetPriority(EXTI4_15_IRQn,1,0);       //��ռ���ȼ�Ϊ0�������ȼ�Ϊ0
//	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);             //ʹ���ж���

}

void EnableCharge(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_Delay(100);  						
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
}
