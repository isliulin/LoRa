
#ifndef __USER_BAT_H__
#define __USER_BAT_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "user_config.h"

/* ���Ͷ��� ------------------------------------------------------------------*/


/* �궨�� --------------------------------------------------------------------*/


/* �������� ------------------------------------------------------------------*/


/* �������� ------------------------------------------------------------------*/
uint8_t BatInit(void);

void BatEnableCharge(void);

void BatDisableCharge(void);

uint8_t BatCheck(uint8_t *bat);

uint8_t BatGetBattery(float *Battery_V);

float BatGetChargePower(float *ChargePower);

#endif /* __USER_BAT_H__ */
