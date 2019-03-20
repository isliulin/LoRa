/*
**************************************************************************************************************
*	@file	  solen-vale.h
*	@author Ysheng
*	@version 
*	@date    
*	@brief  ��ŷ�
***************************************************************************************************************
*/

#ifndef __SOLEN_VALE__H
#define __SOLEN_VALE__H

#include "stm32l0xx_hal.h"

#define  FI_GPIO_PORT						 GPIOA
#define  BI_GPIO_PORT						 GPIOB

#define  SOLEN_POWER0N     			 GPIO_PIN_3
#define  FI_PIN								   GPIO_PIN_8
#define  BI_PIN        					 GPIO_PIN_15

/*
 *SOLENOIDSTATE:	��ŷ�״̬�ṹ��
*/
typedef enum SOLENOIDSTATE
{
	SLND_CLOSE	=0,		//��
	SLND_OPEN	=1		//��
}SolenoidState;


void solen_vale_init(void);

void SolenoidPowerOn(void);

void SolenoidPowerOff(void);

void solen_vale_open(void);

void solen_vale_close(void);

/*
 * GetSolenoidState:	��ȡ��ǰ��ŷ�����״̬
 * ������				 			��
 * ����ֵ��				    ��ǰ��ŷ�����״̬
*/
SolenoidState GetSolenoidState(void);

/*
 * SetSolenoidState:	���õ�ǰ��ŷ�����״̬
 * ������				 			��
 * ����ֵ��				    ��ǰ��ŷ�����״̬
*/
SolenoidState SetSolenoidState(SolenoidState state);


#endif
