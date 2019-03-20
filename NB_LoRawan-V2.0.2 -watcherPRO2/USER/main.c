/*
**************************************************************************************************************
*	@file	main.c
*	@author Ysheng
*	@version V1.1
*	@date    2017/2/23
*	@brief	NBI_LoRaWAN���ܴ���: add OTAA
***************************************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <math.h>
#include "stm32l0xx_hal.h"
#include "usart.h"
#include "rtc-board.h"
#include "timer.h"
#include "delay.h"
#include "board.h"
#include "user-app.h"
#include "LoRa-cad.h"

#define TEST_MULTIPLE_DATA											 1

#define VREFINT_CAL_ADDR                   							((uint16_t*) ((uint32_t)0x1FF80078U)) /* Internal voltage reference, address of parameter VREFINT_CAL: VrefInt ADC raw data acquired at temperature 30 DegC (tolerance: +-5 DegC), Vref+ = 3.0 V (tolerance: +-10 mV). */
#define VREFINT_CAL_VREF                   							((uint32_t) 3U)                    /* Analog voltage reference (Vref+) value with which temperature sensor has been calibrated in production (tolerance: +-10 mV) (unit: mV). */
#define VDD_APPLI                      		 						((uint32_t) 1220U)    /* Value of analog voltage supply Vdda (unit: mV) */
#define VFULL														((uint32_t) 4095U)


#define LC4                { 472100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 472300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 472500000, { ( ( DR_6 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 472700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 472900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!***********************************���м���************************************/

#if  OVER_THE_AIR_ACTIVATION

extern uint8_t DevEui[8];
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

extern TimerEvent_t JoinReqTimer;
extern volatile bool IsNetworkJoined;
extern bool JoinReq_flag;

#endif

LoRaMacRxInfo *loramac_rx_info;
mac_evt_t loramac_evt;

/*
* LoRa CAD�����쳣��������,��ֹ�������Ŵ���CAD DETECT mode 8Sʱ�������л�ΪCAD����
*/
TimerEvent_t CadTimer;
void OncadTimerEvent( void )
{  
	if( LoRapp_SenSor_States.Rx_States == RFWRITE )
	LoRapp_SenSor_States.Rx_States = RFREADY;
	DEBUG(2,"%s  LoRapp_SenSor_States = %d \r\n",__func__, LoRapp_SenSor_States.Rx_States );
	TimerStop( &CadTimer );
}

/*
* LoRa����ʱ����������������ݴ���ʱ��: 8S���յȴ���RADIO�л�ΪRFREADY״̬
*/
void OnsleepTimerEvent( void )
{  
	SleepTimerEvent = true;
	TimerStop( &SleepTimer );
	if(LoRapp_SenSor_States.Rx_States == RADIO) ///�ȴ�RADIO״̬һ��ʱ���,�ָ�λ����
	LoRapp_SenSor_States.Rx_States = RFREADY;
	DEBUG(2,"%s\r\n",__func__);
}

void app_mac_cb (mac_evt_t evt, void *msg)
{
    switch(evt){
    case MAC_STA_TXDONE:                
    case MAC_STA_RXDONE:
    case MAC_STA_RXTIMEOUT:
    case MAC_STA_ACK_RECEIVED:
    case MAC_STA_ACK_UNRECEIVED:
    case MAC_STA_CMD_JOINACCEPT:         
    case MAC_STA_CMD_RECEIVED:
         loramac_rx_info = msg;   ///mac�����������Ϣ��rssi �˿ڵ�
         loramac_evt = evt;
         
         break;
    }
}


/*!***********************************�ָ���************************************/

extern UART_HandleTypeDef 			    UartHandle;
extern RTC_HandleTypeDef 				RtcHandle;
extern SPI_HandleTypeDef            	SPI1_Handler;  


bool test_rtc_state = false;

bool first_time = false;

bool test_cad = false;

#define Temperature 								0x00	// Lux�Ĵ������ֽڵĵ�ַ
#define Humidity					             	0x01


#define Hdc1080_WRITE_ADDR							0x80	// Hdc1080����ַ: TI�ṩ7Bit ��ַ�����λBit ȱʡΪ0 ��1000000 =  0x80

/* Register addresses */
#define Configuration                   			0x02
#define HDC_Manufacturer_ID							0xFE
#define HDC_Device_ID 								0xFF
#define HDC1080_EXP									16

#define Manufacturer_ID_value 						0x5449
#define Device_ID_value 							0x1050

#define ADC_MODE									0

#ifndef SUCCESS
#define SUCCESS                        				1
#endif

#ifndef FAIL
#define FAIL                            			0
#endif


/*******************************************************************************
  * @��������	main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��

	�汾˵����
	��1����V2.0.2��MCU---stm32L0�����ݲɼ��豸 PRO II;

  �Ż����ܣ�
	��1���� ʵ��LORAWAN������ͨ�š�
	��2���� RTCͣ�����ѻ��ơ�
	��3���� ����CAD���ƣ�ֻʹ��5·�ŵ�����ʱ�ϱ�����������
	��4���� ABP/OTAAģʽ���߱���ʵ��ֻʹ��ABP mode��
	��5���� NwkSKey��AppSKey�����ڲ��̶������Žӿڸ��Ĳ�����DEVID��datarate��ʱ�ڶ��������FLASH���Ĳ�����
	��6���� lower-power run mode
	��7���� ���Ż��͹��Ļ���
	��8���� BQ24195��ʹ��������ã�������ô�������
	��9���� �ر�RX
				  LoRaMacSetReceiveDelay1( 200000 );
				  LoRaMacSetReceiveDelay2( 200000 );
	��10��: �ð汾��GPS��ʱ����Ϊ30S���ڲ��Զ�λ���ڲ�����ʱ�䲻�Ƿ��ӣ������汾��Ҫ����
	��11��������Ĭ��=max ͬʱ����ADR�Զ�����ʹ��
    ��12����V2.0.1���Ż��͹������ߴ���
	
  *****************************************************************************/
/* variable functions ---------------------------------------------------------*/	

int main(void)
{	
   BoardInitMcu(  );	
   DEBUG(2,"TIME : %s  DATE : %s\r\n",__TIME__, __DATE__);
	
   user_app_init(app_mac_cb);
	
   LoRaMacChannelAdd( 3, ( ChannelParams_t )LC4 );
   LoRaMacChannelAdd( 4, ( ChannelParams_t )LC5 );
   
   TimerInit( &ReportTimer, OnReportTimerEvent );
   TimerInit( &CadTimer, OncadTimerEvent ); 
   TimerInit( &SleepTimer, OnsleepTimerEvent ); 
   TimerInit( &CSMATimer, OnCsmaTimerEvent ); 
   TimerInit( &GPSTimer, OnGpsTimerEvent );
	
   ReportTimerEvent = true;
   LoRapp_SenSor_States.loramac_evt_flag = 0;
   LoRaCad.cad_all_time = 0;
    
   RF_Send_Data.AT_PORT = randr( 1, 0xDF );

   RF_Send_Data.Get_sensor = true;

   RF_Send_Data.Send_Buf = (uint8_t *)malloc(sizeof(uint8_t)*64); ///ʹ��ָ���������ַ�ռ䣬��������HardFault_Handler����
   samples.sockets = (uint8_t *)malloc(sizeof(uint8_t)*8); ///ʹ��ָ���������ַ�ռ䣬��������HardFault_Handler����     

   DEBUG(3, "Battery = %d\r\n",CheckBattery( ));
   
   /********************ֻ���ڳ�����֤������***********************/
   char *data = "deve";
   RF_Send_Data.RX_LEN = 5;
   RF_Send_Data.TX_Len = 5;
   do
   {								
      user_app_send(UNCONFIRMED, (uint8_t *)data, RF_Send_Data.TX_Len,3);
      HAL_Delay(3000);
   }
   while(!LoRapp_SenSor_States.loramac_evt_flag);
   
    memset(RF_Send_Data.Send_Buf, 0, RF_Send_Data.RX_LEN);
    RF_Send_Data.RX_LEN = 0;
    __disable_irq();
    LoRapp_SenSor_States.loramac_evt_flag = 0;
    __enable_irq();
   /**************************************************************/
    
   /***********����GPS***********/	
//   GPS_Init(  );
//   Gps_Set(  );		

//   GpsSendAgainTime(  );
   
   /***********����GPS***********/   
 	 
   while (1)
   {		
//	if(!Set_Gps_Ack.GPS_DONE)
//	{
//		Get_Gps_Position( );
//	}
//	else
	{
		User_Send_Api(  );	
	}		
   }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{ 
	DEBUG(2,"error\r\n");
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

