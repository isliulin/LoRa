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

typedef struct{
	bool Cad_Mode;
	bool Cad_Done;
	bool Cad_Detect;
	bool Iq_Invert;
	uint32_t Cad_Done_Count;
	float symbolTime;
	uint8_t UID[4]; ///���յ����豸ID
	uint32_t TimeOnAir; ///�տ�ʱ��
	int32_t timeover;
	int32_t randtime1; ///�������ʱ�䷶Χ1
	int32_t randtime2; ///�������ʱ�䷶Χ2
	uint8_t cad_counter; ///�ظ�ɨ�����
	uint8_t Channel_Num; ///�ŵ�ɨ�������8ͨ�� = 8��
	int16_t Rssi[8];
	uint8_t Channel_Scan[8];
	uint32_t cad_all_time;
}LoRaCad_t;

extern LoRaCad_t LoRaCad;

extern uint32_t LoRaCadtime; ///ϵͳʱ��

extern TimerEvent_t RxWindowTimer1;
extern uint8_t Channel; ///�ŵ���

extern TimerEvent_t CadTimer;
extern void OnCadUnusualTimerEvent( void );

extern TimerEvent_t CSMATimer;
extern volatile bool CSMATimerEvent;
extern void OnCsmaTimerEvent( void );

void LoRa_Cad_Init(void);

float SymbolTime(uint8_t Datarate);

void LoRa_Cad_Mode(void);

void LoRa_Detect_Mode(void);

uint8_t Get_max(int8_t m,int16_t array[]);

uint8_t Bublesort(uint8_t a[],uint8_t n);


#endif /* __LoRa-cad_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
