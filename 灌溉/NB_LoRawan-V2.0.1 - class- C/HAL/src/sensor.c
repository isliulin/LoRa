/*
**************************************************************************************************************
*	@file	watersensor.c
*	@author 
*	@version 
*	@date    
*	@brief ���������ݴ����
***************************************************************************************************************
*/

#include "sensor.h"
#include "debug.h"
#include "adc.h"

#define VREFINT_CAL_ADDR                   							((uint16_t*) ((uint32_t)0x1FF80078U)) /* Internal voltage reference, address of parameter VREFINT_CAL: VrefInt ADC raw data acquired at temperature 30 DegC (tolerance: +-5 DegC), Vref+ = 3.0 V (tolerance: +-10 mV). */
#define VREFINT_CAL_VREF                   							((uint32_t) 3U)                    /* Analog voltage reference (Vref+) value with which temperature sensor has been calibrated in production (tolerance: +-10 mV) (unit: mV). */
#define VDD_APPLI                      		 						((uint32_t) 1220U)    /* Value of analog voltage supply Vdda (unit: mV) */
#define VFULL														((uint32_t) 4095U)

Sensor_t WaterSensorsData = {0};


/*SamplingData����ȡ��������Ϣ��ˮѹ������
*���������			ˮѹ����������
*���ز�����			��
*/
void SamplingData(uint16_t adctemp[2])
{	 
	///WaterSensorsData.temp[1]*1.6Pa/5v
	uint16_t adc[3];
	
	adc[0] = AdcReadParameter(ADC_CHANNEL_1, 10);
	float VBAT = VREFINT_CAL_VREF*(*VREFINT_CAL_ADDR)*adc[0];

	adc[1] = AdcReadParameter(ADC_CHANNEL_2, 10);
	float Water_Gage = VREFINT_CAL_VREF*(*VREFINT_CAL_ADDR)*adc[1];
	
	adc[2] = AdcReadParameter(ADC_CHANNEL_VREFINT, 1);
	float temp = adc[2] * VFULL;
	
	adctemp[0] = ((VBAT/temp)*2000 - 3600)/6; //�����ٷּ�
	
	float water_pressure = ((Water_Gage/temp)*2000);
	
	adctemp[1] = ((uint16_t)(water_pressure*16))/50; //pa�Ŵ�1000��	water_pressure*1.6/5000
	DEBUG(2, "BAT = %d adc17 = %d adc2 = %d, VBAT = %.2fmV  Water_Gage = %.2fmV\r\n", *VREFINT_CAL_ADDR, adc[1], adc[0], (VBAT/temp)*2000, (Water_Gage/temp)*2000);
	DEBUG(2, "adctemp[0] = %d�� water_pressure = %.4fMpa adctemp[1] = %d Kpa\r\n", adctemp[0], (water_pressure*16)/50000, adctemp[1]);
}

/*ˮ������������ʼ��
*��������
*���أ���
*/
void WaterFlowInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();               		 //����GPIOBʱ��

	GPIO_Initure.Pin=WaterFlow_IO;  
	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      			//�����ش���
	GPIO_Initure.Pull=GPIO_PULLDOWN;
	HAL_GPIO_Init(WaterFlow_PORT,&GPIO_Initure);
	
	HAL_NVIC_SetPriority(EXTI0_1_IRQn,5,0);       //��ռ���ȼ�Ϊ5�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
	
	WaterSensorsData.pulsecount = 0; 
}


