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

extern UART_HandleTypeDef hlpuart1;	//GPRS
extern UART_HandleTypeDef huart1;	//����
extern UART_HandleTypeDef huart2;	//GPS
extern UART_HandleTypeDef huart5;	//485

extern char SimReChar[1];

void UserMain(void)
{
    double Latitude,Longitude;
    uint8_t ret;
    uint32_t timeout=3000;
    
    printf("���Sim800ģ���GPSģ��\r\n\r\n");
    
    if(GpsInit()==0)
        printf("GPSģ�飺����error\r\n\r\n");
        
    LedOpen();//��ʾ��ʼ��λ
    ret=GpsGetLocation(&Longitude,&Latitude,&timeout);
    
    if(ret==1)
    {
        printf("GPSģ�飺OK\r\n\r\n");
    }
    else
    {
        if(ret==2)
        {
            printf("GPSģ�飺��GPSģ�飿����GPSģ����󣿣���\r\n\r\n");
        }
        else
        {
            printf("GPSģ�飺����OK\r\n\r\n");
        }
    }
    GpsPowerOff();
    SimPowerOn();
	HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)SimReChar,1);//�򿪴��ڽ����ж�,��ʱ�жϻ���յ�һ���ַ�
	if(SimExecuteCmd(AT_CMD_CLOSE_ECHO)!=AT_OK)
    {
		printf("Sim800ģ�飺����error\r\n\r\n");
	}
    else
	{
        printf("Sim800ģ�飺����OK\r\n\r\n");
	}
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
