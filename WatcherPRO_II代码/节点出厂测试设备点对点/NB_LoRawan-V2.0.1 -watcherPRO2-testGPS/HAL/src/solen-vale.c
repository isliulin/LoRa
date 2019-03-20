/*
**************************************************************************************************************
*	@file	  solen-vale.c
*	@author Ysheng
*	@version 
*	@date    
*	@brief  ��ŷ�
***************************************************************************************************************
*/

#include <stdint.h>
#include "solen-vale.h"

SolenoidState Solenoid_State;	//��ŷ���ǰ״̬

void solen_vale_init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	__HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��

	GPIO_Initure.Pin=BI_PIN|SOLEN_POWER0N;    
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_Initure.Pin=FI_PIN;    
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(FI_GPIO_PORT,&GPIO_Initure);

	HAL_GPIO_WritePin(BI_GPIO_PORT,BI_PIN,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(FI_GPIO_PORT,FI_PIN,GPIO_PIN_RESET);	
	Solenoid_State = SLND_CLOSE;
}

/*
 * PowerOn:		�򿪵�ŷ���Դ����
 * ������			��
 * ���ز����� ��
*/
void SolenoidPowerOn(void)
{
	//12V��ŷ���Դ����
	HAL_GPIO_WritePin(BI_GPIO_PORT,SOLEN_POWER0N,GPIO_PIN_SET);	 	
}

/*
 * PowerOn:	  �رյ�ŷ���Դ����
 * ������			��
 * ���ز����� ��
*/
void SolenoidPowerOff(void)
{
	HAL_GPIO_WritePin(BI_GPIO_PORT,SOLEN_POWER0N,GPIO_PIN_RESET);	 	
}

void solen_vale_open(void)
{
	//100ms������
	HAL_GPIO_WritePin(FI_GPIO_PORT,FI_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BI_GPIO_PORT,BI_PIN,GPIO_PIN_SET);
	HAL_Delay(100);	
	HAL_GPIO_WritePin(BI_GPIO_PORT,BI_PIN,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(FI_GPIO_PORT,FI_PIN,GPIO_PIN_RESET);	
	Solenoid_State = SLND_OPEN;
}

void solen_vale_close(void)
{
	//100ms������
	HAL_GPIO_WritePin(BI_GPIO_PORT, BI_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FI_GPIO_PORT, FI_PIN,GPIO_PIN_SET);
	HAL_Delay(100);	
	HAL_GPIO_WritePin(BI_GPIO_PORT, BI_PIN,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(FI_GPIO_PORT, FI_PIN,GPIO_PIN_RESET);	
	Solenoid_State = SLND_CLOSE;
}

/*
 * GetSolenoidState:	��ȡ��ǰ��ŷ�����״̬
 * ������				 			��
 * ����ֵ��				    ��ǰ��ŷ�����״̬
*/
SolenoidState GetSolenoidState(void)
{
	return Solenoid_State;
}

/*
 * SetSolenoidState:	���õ�ǰ��ŷ�����״̬
 * ������				 			��
 * ����ֵ��				    ��ǰ��ŷ�����״̬
*/
SolenoidState SetSolenoidState(SolenoidState state)
{
	Solenoid_State=state;
	return Solenoid_State;
}
