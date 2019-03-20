/*
**************************************************************************************************************
*	@file	main.c
*	@author Ysheng
*	@version V1.1
*	@date    2017/12/13
*	@brief	NBI_LoRaWAN���ܴ���: add OTAA
***************************************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <math.h>
#include "stm32l0xx_hal.h"
#include "usart.h"
#include "rtc-board.h"
#include "timerserver.h"
#include "delay.h"
#include "board.h"
#include "user-app.h"
#include "etimer.h"
#include "autostart.h"

//#define LC4                { 137000000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC4                { 472100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 472300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 472500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
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


#ifndef SUCCESS
#define SUCCESS                         1
#endif

#ifndef FAIL
#define FAIL                            0
#endif


bool TXSTATE = false;

PROCESS(SX1278Send_process,"SX1278Send_process");
PROCESS(SX1278Receive_process,"SX1278Rever_process");
PROCESS(Sensor_process,"Sensor_process");
PROCESS(Gps_process,"Gps_process");
AUTOSTART_PROCESSES(&SX1278Receive_process, &Sensor_process, &Gps_process); 
void RFTXDONE(void)
{
	process_poll(&SX1278Send_process);
}

process_event_t start_rx;
void PostPress(void)

{
    process_post(&Sensor_process,start_rx,NULL);
}

PROCESS_THREAD(SX1278Send_process,ev,data)
{
	static struct etimer et;
	PROCESS_BEGIN();
	
	USR_UsrLog("Contiki System SX1278Send Process..."); 
	
	etimer_set(&et,CLOCK_SECOND);
	
	while(1)
	{	
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		
		USR_UsrLog("hello world...");
		
		RF_Send_Data.Send_Buf = "helloworld";
		RF_Send_Data.TX_Len = strlen((char *)RF_Send_Data.Send_Buf);
		DEBUG(2,"Wait ACK app_send UpLinkCounter = %d\r\n", LoRaMacGetUpLinkCounter( ));
		
		Channel = 3; ///��ȡ�ŵ�ID 4Gģ���ȡ
		Radio.Standby( );	
		user_app_send(UNCONFIRMED, RF_Send_Data.Send_Buf, RF_Send_Data.TX_Len, 2);
		
//		process_post(&SX1278Receive_process,start_rx,NULL);
		PROCESS_YIELD_UNTIL(LoRapp_SenSor_States.loramac_evt_flag == 1);
		LoRapp_SenSor_States.loramac_evt_flag = 0;
				
		etimer_set(&et,CLOCK_SECOND*3);		
	}
	PROCESS_END();
}

extern uint32_t timer_time;

extern bool rx_start;

uint8_t rxcounter = 0;

PROCESS_THREAD(SX1278Receive_process,ev,data)
{
	static struct etimer et;
	
	PROCESS_BEGIN();
	
	Radio.Sleep( );
	
	etimer_set(&et,CLOCK_SECOND*30);
	while(1)
	{	
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));		
		DEBUG(3,"SX1278Receive\r\n");
		
        SX1276.Settings.State = RF_IDLE;
        rxcounter = 0;
        Radio.Standby( );
        SX1276SetOpMode( RF_OPMODE_TRANSMITTER ); ///RF_OPMODE_TRANSMITTER
        Radio.Standby( );
        OnRxWindow2TimerEvent( );	
		
		rx_start = false;
			
		etimer_reset(&et);
	}
	PROCESS_END();
}

PROCESS_THREAD(Sensor_process,ev,data)
{
    static struct etimer et;
	PROCESS_BEGIN();
    
    etimer_set(&et,CLOCK_SECOND);
    
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));	
        
        Get_Gps_Position( );

        etimer_reset(&et);
    }
    PROCESS_END();    
}

PROCESS_THREAD(Gps_process,ev,data)
{
    static struct etimer et;
	PROCESS_BEGIN();
    
    etimer_set(&et,CLOCK_SECOND);
    
    Get_Gps_Position( );
    
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));	
        
        if(UART_RX_DATA1.USART_TX_STATE)
        {
            UART_RX_DATA1.USART_TX_STATE = false;
            
            DEBUG(2, "%s\r\n",UART_RX_DATA1.USART_RX_BUF);   
            memset(UART_RX_DATA1.USART_RX_BUF,0,UART_RX_DATA1.USART_RX_Len);
            UART_RX_DATA1.USART_RX_Len = 0;
        }
        SamplingData();
        etimer_reset(&et);
    }
    PROCESS_END();    
}



/*******************************************************************************
  * @��������	main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��

	�汾˵����
	��1����V2.0.1��MCU---stm32L0�����ݲɼ��豸 PRO II;

	�Ż����ܣ�
	��1���� ʵ��LORAWAN������ͨ�š�
	��2���� RTCͣ�����ѻ��ơ�
	��3���� ����CAD���ƣ�ֻʹ��5·�ŵ�����ʱ�ϱ�����������
	��4���� ABP/OTAAģʽ���߱���ʵ��ֻʹ��ABP mode��
	��5���� NwkSKey��AppSKey�����ڲ��̶������Žӿڸ��Ĳ�����DEVID��datarate��ʱ�ڶ��������FLASH���Ĳ�����
	��6���� ����RX2���մ���
	
  *****************************************************************************/
/* variable functions ---------------------------------------------------------*/	

int main(void)
{	
	BoardInitMcu();	
	DEBUG(2,"TIME : %s  DATE : %s\r\n",__TIME__, __DATE__);

	/******************���ٳ�ʼ��*****************/
	Read_Flash_Data(  );
	
	TimerHwInit(  );

	user_app_init(app_mac_cb);
    
//	MX_IWDG_Init(  );
	
//	HAL_IWDG_Refresh(&hiwdg); ///���Ź�ι��

	LoRaMacTestRxWindowsOn( false ); ///�رս��մ���
	
	LoRaMacChannelAdd( 3, ( ChannelParams_t )LC4 );
	LoRaMacChannelAdd( 4, ( ChannelParams_t )LC5 );
	LoRaMacChannelAdd( 5, ( ChannelParams_t )LC6 );
	LoRaMacChannelAdd( 6, ( ChannelParams_t )LC7 );
	LoRaMacChannelAdd( 7, ( ChannelParams_t )LC8 );
	
	Channel = 3; ///��ȡ�ŵ�ID 4Gģ���ȡ
	
	LoRaCad.Iq_Invert = true;  ///ʹ�ܽڵ��ͨ��

	ReportTimerEvent = true;
	LoRapp_SenSor_States.loramac_evt_flag = 0;

	RF_Send_Data.AT_PORT = randr( 0, 0xDF );

	RF_Send_Data.Get_sensor = true;
	RF_Send_Data.Send_Buf = (uint8_t *)malloc(sizeof(uint8_t)*64); ///ʹ��ָ���������ַ�ռ䣬��������HardFault_Handler����
	
	clock_init();

	process_init();
	process_start(&etimer_process,NULL); ///�Զ�����������߳�
	autostart_start(autostart_processes);
	
	TimerInit( &ReportTimer, OnReportTimerEvent );
	
	LoRaMacSetDeviceClass( CLASS_C );
    
    GPS_Init(  );
    Gps_Set(  );	

	while (1)
	{		
		do
		{
		}while(process_run() > 0);	
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


/*--------------------------------------------------------------------------------------------------------
                   									     0ooo											
                   								ooo0     (   )
                								(   )     ) /
                								 \ (     (_/
                								  \_)
----------------------------------------------------------------------------------------------------------*/

