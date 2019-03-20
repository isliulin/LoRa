/*
**************************************************************************************************************
*	@file	power.c
*	@author Ysheng
*	@version V0.0.1
*	@date    
*	@brief 延时函数
***************************************************************************************************************
*/

#include <stdint.h>
#include "power.h"
#include "debug.h"
#include "adc.h"

#define VREFINT_CAL_ADDR                   							((uint16_t*) ((uint32_t)0x1FF80078U)) /* Internal voltage reference, address of parameter VREFINT_CAL: VrefInt ADC raw data acquired at temperature 30 DegC (tolerance: +-5 DegC), Vref+ = 3.0 V (tolerance: +-10 mV). */
#define VREFINT_CAL_VREF                   							((uint32_t) 3U)                    /* Analog voltage reference (Vref+) value with which temperature sensor has been calibrated in production (tolerance: +-10 mV) (unit: mV). */
#define VDD_APPLI                      		 							((uint32_t) 1220U)    /* Value of analog voltage supply Vdda (unit: mV) */
#define VFULL																						((uint32_t) 4095U)


uint8_t CheckBattery(void)
{
	uint16_t adc[2] = {0};
	 
	adc[0] = AdcReadParameter(ADC_CHANNEL_1, 10);
	float VBAT = VREFINT_CAL_VREF*(*VREFINT_CAL_ADDR)*adc[0];
	
	adc[1] = AdcReadParameter(ADC_CHANNEL_VREFINT, 10);
	
	float temp = adc[1] * VFULL;
	
	DEBUG(2, "BAT = %d adc17 = %d adc1 = %d, VBAT = %.2fmV be = %.2fmV\r\n", *VREFINT_CAL_ADDR, adc[1], adc[0], (VBAT/temp)*2000, (((VBAT/temp)*2000 - 3600)/6));
	
	return (((VBAT/temp)*2000 - 3600)/6);	//电量百分几		
}
