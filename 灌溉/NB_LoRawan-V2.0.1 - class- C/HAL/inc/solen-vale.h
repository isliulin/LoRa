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
#include "user-app.h"

#define  FI_GPIO_PORT						 GPIOA
#define  FI_PIN								 GPIO_PIN_12

#define  BI_GPIO_PORT						 GPIOA
#define  BI_PIN        						 GPIO_PIN_11

#define  CTRL_12V_PORT					 	 GPIOA
#define  CTRL_12V_POWERON   				 GPIO_PIN_8


/*
 *SOLENOIDSTATE:	��ŷ�״̬�ṹ��
*/
typedef enum SOLENOIDSTATE
{
	SLND_CLOSE	=0,		//��
	SLND_OPEN	=1		//��
}SolenoidState;

/*
 *solen_vale_init����ŷ���ʼ��
 *������		 ��
 *����ֵ��		 ��
*/
void solen_vale_init(void);

/*
 * Ctrl_12V_PowerInit: ��ʼ��12V��Դ����
 * ������	 		   ��
 * ���ز�����		   ��
*/
void Ctrl_12V_PowerInit(void);

/*
 * PowerOn:		�򿪵�ŷ���Դ����
 * ������		��
 * ���ز�����	��
*/
void Ctrl_12V_PowerOn(void);

/*
 * PowerOn:	  �رյ�ŷ���Դ����
 * ������	  ��
 * ���ز����� ��
*/
void Ctrl_12V_PowerOff(void);

/*
 * solen_vale_open:	 �򿪵�ŷ�
 * ������	 		 ��
 * ���ز�����		 ��
*/
void solen_vale_open(void);

/*
 * solen_vale_close: �رյ�ŷ�
 * ������	 		 ��
 * ���ز�����		 ��
*/
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

/*
 *WaterMCounter��ˮ���ֶ��رմ�������
 *������		 ��
 *����ֵ��		 ��
*/
void WaterMCounter(void);

/*
 *WaterMOCounter��ˮ���ֶ��򿪴�������
 *������		  ��
 *����ֵ��		  ��
*/
void WaterMOCounter(void);

/*
 *OnLoRaHeartDeal��LoRa����������
 *������		   ��
 *����ֵ��		   ��
*/
void OnLoRaHeartDeal(void);

/*
 *OnLoRaAckDeal��LoRa ACKӦ����
 *������		 ��
 *����ֵ��		 ��
*/
void OnLoRaAckDeal(void);

/*
 *OnWaterAutopen��ˮ���Զ����ƿ�����
 *������		  ��
 *����ֵ��		  ��
*/
void OnWaterAutopen(void);

/*
 *OnWaterAutClose��ˮ���Զ�ģʽ�رղ���
 *������		   ��
 *����ֵ��		   ��
*/
void OnWaterAutClose(void);

/*
 *WaterManOpen��ˮ���ֶ�ģʽ�򿪲���
 *������		��
 *����ֵ��		��
*/
void WaterManOpen(void);

/*
 *WaterManClose��ˮ���ֶ�ģ�رղ���
 *������		 ��
 *����ֵ��		 ��
*/
void WaterManClose(void);


#endif
