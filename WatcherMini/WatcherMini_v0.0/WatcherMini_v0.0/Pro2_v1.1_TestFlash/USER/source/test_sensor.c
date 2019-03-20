#include "user_main.h"
#include "user_bq24195.h"
#include "user_sim.h"
#include "user_led.h"
#include "user_adc.h"
#include "user_flash_L072.h"
#include "user_sensor_pro2.h"
#include "user_gps.h"
#include "bootloader_config.h"
#include <string.h>

#define STRUCT_VERSION	3			//ͨ�����ݰ汾��

extern UART_HandleTypeDef hlpuart1;	//GPRS
extern UART_HandleTypeDef huart1;	//����
extern UART_HandleTypeDef huart2;	//GPS
extern UART_HandleTypeDef huart5;	//485

void UserMain()
{
    char SendData[128]	={0};				//256̫������ջ������128�㹻��
    SensorData sensor_data[8]={0};			//���������նȡ�������ʪ�ȼ���5�����������ݣ������8������
    	uint16_t len=0;
    	int32_t Battery;
        
    	if(AdcInit()==0)
    {
		printf("AdcInit Fail\r\n");
        HAL_NVIC_SystemReset();
    }
    else
    {
		AdcGetBattery(&Battery);
        if(Battery==0)
        {
            printf("����������\r\n");
            return;
        }
	}
    
    _12VPowerOn();
	SensorInit();
	SernsorGetData(sensor_data);
	_12VPowerOff();

    	len += sprintf(SendData+len,"%02X",STRUCT_VERSION);
	//�ȰѴ������ĵ�ַƴ��ȥ
	for(uint8_t i=0;i<sizeof(sensor_data)/sizeof(sensor_data[0]);i++)
	{
//		printf("sensor_data[%u].index:%u\r\n",i,sensor_data[i].index);
//		printf("sensor_data[%u].add:%u\r\n",i,sensor_data[i].add);
//		printf("sensor_data[%u].count:%u\r\n",i,sensor_data[i].count);
//		printf("sensor_data[%u].data[0]:%x\r\n",i,sensor_data[i].data[0]); 
//		printf("sensor_data[%u].data[1]:%x\r\n",i,sensor_data[i].data[1]);
		if(i>1)
			len += sprintf(SendData+len,"%02x",sensor_data[i].add);
	}
	
	//��ƴ�ϵ���
	len += sprintf(SendData+len,"%02x",Battery);
	//Ȼ��ƴ�Ӵ���������
	for(uint8_t i=0;i<sizeof(sensor_data)/sizeof(sensor_data[0]);i++)
	{
		for(uint8_t j=0;j<sensor_data[i].count;j++)
		{
			len += sprintf(SendData+len,"%04x",sensor_data[i].data[j]);
		}
	}
    printf("data=%s\r\n",SendData);
    
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle==&huart1)				//��������ӣ����ڵ���
    {
		//ResendToSim();
		//ResendToGps();
	}
	else if(UartHandle==&hlpuart1)		//sim800ģ��
	{
		SimCheckReply();
	}
	else if(UartHandle==&huart2)		//GPSģ��
	{
		GpsCheckReply();
	}
	else if(UartHandle==&huart5)		//Rs485ģ��
	{
		Rs485CheckReply();
	}
}
