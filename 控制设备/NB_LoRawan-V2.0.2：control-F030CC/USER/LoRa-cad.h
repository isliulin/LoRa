/*
**************************************************************************************************************
*	@file	LoRa-cad.h
*	@author Ysheng
*	@version 
*	@date    
*	@brief	Ӧ�ò�ͷ�ļ�������MAC��
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORA_CAD_H
#define __LORA_CAD_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include <stdio.h>
#include <stdint.h>

extern uint32_t Send_time; ///ϵͳʱ��

extern TimerEvent_t RxWindowTimer1;
extern void OnRxWindow1TimerEvent( void ); ///���մ���
extern uint8_t Channel; ///�ŵ���

void LoRa_Cad_Init(void);

float SymbolTime(void);

void LoRa_Cad_Mode(void);

uint8_t Get_max(int8_t m,int16_t array[]);

uint8_t Bublesort(uint8_t a[],uint8_t n);


#endif /* __LoRa-cad_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
