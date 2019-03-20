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
#include "stm32f0xx_hal.h"
#include "usart.h"
#include "rtc-board.h"
#include "timer.h"
#include "delay.h"
#include "board.h"
#include "user-app.h"
#include "LoRa-cad.h"


/*!***********************************���м���************************************/

extern uint8_t DevEui[8];
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

extern TimerEvent_t JoinReqTimer;
extern volatile bool IsNetworkJoined;
extern bool JoinReq_flag;

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
extern RTC_HandleTypeDef 						RtcHandle;
extern SPI_HandleTypeDef            SPI1_Handler;  

bool test_rtc_state = false;

uint8_t control_buf[4] = {0};

uint32_t check_data = 0;


/*******************************************************************************
  * @��������		main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��

	�汾˵����
	��1����V2.0.3��MCU---stm32F0����Ҫ���ڿ����豸���ڿ��ƴ���

  �Ż����ܣ�
  ��1����ʵ��LORAWAN������ͨ��;
	��2��������CAD���ƣ�ֻʹ��5·�ŵ������ݸ�ʽҪ��������������ԽСԽ��;
	��3����ABP/OTAAģʽ���߱���ʵ��ֻʹ��ABP mode;
	��4����NwkSKey��AppSKey�����ڲ��̶������Žӿڸ��Ĳ�����DEVID��datarate��ʱ�ڶ��������FLASH���Ĳ���;
	��5����FPort�����������;
	��6�����ֶ����Զ��л�ģʽ�ɼ�ʹ���ж�+��ѯ����;
	��7��������������ɣ��ֶ�ģʽû����010a����ظ�������; stm32 3��IO���������ûʹ��
  *****************************************************************************/
/* variable functions ---------------------------------------------------------*/	
int main(void)
{	
   BoardInitMcu();	
   DEBUG(2,"hello world NBI LoRaWAN\r\n");

   user_app_init(app_mac_cb);
   
   TimerInit( &ReportTimer, OnReportTimerEvent );
   ReportTimerEvent = true;
   LoRapp_SenSor_States.loramac_evt_flag = 0;
	
	 /******************���ٳ�ʼ��*****************/
	 RF_Send_Data.ADR_Datarate = RF_Send_Data.default_datarate = Get_Flash_Datas.datarate = LORAMAC_DEFAULT_DATARATE;
	
	 RF_Send_Data.AT_MODULE_PORT = randr( 0, 0xDF );
	 DEBUG(2, "PORT = %d\r\n",RF_Send_Data.AT_MODULE_PORT);
	
	 LoRaMacSetDeviceClass( CLASS_C );
	
	 RF_Send_Data.Estab_Communt_State = true; ///����״̬
	
	 Enable_Stm8_Power(  ); ///ʹ��stm8_power
	
   Control_Relay_Init(  ); ///stm32��3·�̵�������
	 
	 Get_Work_ModeInit(  ); ///��/�Զ���ȡ��ʼ��
	

  while (1)
  {	
      
#if ( OVER_THE_AIR_ACTIVATION == 1 ) 
		app_loramacjoinreq( );
   
#endif        
	 App_Estab_Communt(  );
	 
	 Check_Key_Mode(  ); ///��鵱ǰ��������ģʽ
		
	 if( RF_Send_Data.Estab_Communt_State==false )
	 {
		 Receive_ConTrol_Data();	
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

