
#ifndef __USER_BAT_H__
#define __USER_BAT_H__

#include "stm32l0xx_hal.h"

/* ����ͷ�ļ� ----------------------------------------------------------------*/

/* ���Ͷ��� ------------------------------------------------------------------*/


/* �궨�� --------------------------------------------------------------------*/


/* �������� ------------------------------------------------------------------*/


/* �������� ------------------------------------------------------------------*/
/*
 * Bq24195Init:			��ʼ�������ԴоƬ
 * ����:				��
 * ����ֵ:				��
*/
void  Bq24195Init(void);


void BatEnableCharge(void);

void BatDisableCharge(void);

uint8_t BatCheck(uint8_t *bat);

uint8_t BatGetBattery(float *Battery_V);

float BatGetChargePower(float *ChargePower);

#endif /* __USER_BAT_H__ */
