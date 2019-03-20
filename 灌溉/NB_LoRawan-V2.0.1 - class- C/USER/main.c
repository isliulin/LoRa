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
#include "board.h"


#define TEST_MULTIPLE_DATA															 1


/*!***********************************���м���************************************/

#if ( OVER_THE_AIR_ACTIVATION == 1 ) 

extern uint8_t DevEui[8];
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

extern TimerEvent_t JoinReqTimer;
extern volatile bool IsNetworkJoined;
extern bool JoinReq_flag;

#endif

#define 	TEST_WATER				0x00
#define 	TEST_WATER_PRESSURE     0x00
#define 	TEST_WATER_FLOW			0x00
#define     LORA_MODE				0x04

#define LC4                { 472100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 472300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 472500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 472700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 472900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }


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

extern UART_HandleTypeDef 			UartHandle;
extern RTC_HandleTypeDef 			RtcHandle;
extern SPI_HandleTypeDef            SPI1_Handler;  


bool test_rtc_state = false;

/*******************************************************************************
  * @��������		main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��

	�汾˵����
	��1�� ��V2.0.1��MCU---stm32L0����Ҫ���ڿ����豸����;
	��2�� ��work on LOWER-RUN �������⣺MCUʱ�ӹ���SPI��дƵ�������test_lora_pingpong.c���ܲ���lora��
				 ʹ��LoRaWANʱ���������⣬����Ҫ��������

  �Ż����ܣ�
	��1�� ��ʵ��LORAWAN������ͨ�š�
	��2�� ��RTCͣ�����ѻ��ơ�
	��3�� ������CAD���ƣ�ֻʹ��3·�ŵ��������������ݺ��ж��ŵ�Ȼ��ֱ�ӷ������ݣ����ݸ�ʽҪ��������������ԽСԽ�á�
	��4�� ��ABP/OTAAģʽ���߱���ʵ��ֻʹ��ABP mode��
	��5�� ��NwkSKey��AppSKey�����ڲ��̶������Žӿڸ��Ĳ�����DEVID��datarate��ʱ�ڶ��������FLASH���Ĳ�����
	��6�� ��lower-power run mode;
	��7�� �����Ż��͹��Ļ���;
	��8�� ������CAD���ѹ��ܣ���Ӧ�ٶ��Ż�������������CADʱ�䲻ʹ�����������ʱ��ֱ���·��Ϳ��Ի���;
	��9�� ������CAD mode����ʱ1/15ʱ������CAD������ʱ���������mode;
	��10�����������ϱ�����ʱ���豸����CAD mode�ȴ��´η������ݻ��߽�������.
	��11������Ҫ����ϱ��������ڡ�����ˮ��ִ�б�������(**************)
	��12����GPS����ʧ�ܣ�ģ��������
	��13��������RX2����
	��14��  �·����ݣ���������֡+  ������+����ʱ��  /�أ��ر�
	        �ϱ����ݣ�����+ˮѹ+ˮ����+������ʱ��
	��15��������ֶ�ģʽ�������ϱ�����
	��16�����������ѳ��ִ��ڴ�ӡ0��������ԭ��
	��17���������ϱ�ʱ������CAD mode ��ֹ���п�����Ӧ�����Ȳ���������ģʽ��ˮ����ҪRADIO+PAYLOAD ��ˮ����PAYLOADһ֡����
	        ��Ҫ��Ϻ�̨���ԣ�(1)��������RADIO����ʱ���Ƿ�������������������
		                      (2)��Ч�����·�4S��û���յ�Ӧ�����ٴ��·�����ֹ����ʧЧ 
                              (3)�ر�������õ�֡���ݣ�����ʱ�·��������ڹ涨4S��ûӦ�����ٴ��·���Ĭ�ϵ�һ��Ϊ����				
	��18���������������1��/h		
	��19�����������Թر�USART��2.5ma(max) BUG�������RTCʱ�䶨ʱ��׼�����ò��ر�USART
					���ر�USART: 3ma(max)�����ر�USART���Ļ��1ma����
   	       
  *****************************************************************************/
/* variable functions ---------------------------------------------------------*/	

int main(void)
{	
	BoardInitMcu();	
	
	/********************�ϵ�ģʽѡ�񣺴������߿���*********************/
//	WorkStatusJudgment( );
	
	DEBUG(2,"hello world NBI LoRaWAN\r\n");

	/******************��flash��ȡ���ò���*****************/
	Read_Flash_Data(  );

	LoRaPower_Enable(  );
	user_app_init(app_mac_cb);
	
	LoRaMacSetRx2Channel( Rx2Channel );  ///����RX2����
	
	LoRaMacChannelAdd( 3, ( ChannelParams_t )LC4 );
	LoRaMacChannelAdd( 4, ( ChannelParams_t )LC5 );
	LoRaMacChannelAdd( 5, ( ChannelParams_t )LC6 );
	LoRaMacChannelAdd( 6, ( ChannelParams_t )LC7 );
	LoRaMacChannelAdd( 7, ( ChannelParams_t )LC8 );
   
	TimerInit( &ReportTimer, OnReportTimerEvent );
	TimerInit( &CadTimer, OnCadUnusualTimerEvent ); 
	TimerInit( &SleepTimer, OnsleepTimerEvent ); 
	TimerInit( &CSMATimer, OnCsmaTimerEvent ); 
	TimerInit( &HEARTimer, OnHearTimerEvent ); 
   
	LoRapp_SenSor_States.loramac_evt_flag = 0;
	LoRaCad.cad_all_time = 0;
	
	/******************���ٳ�ʼ��*****************/
	RF_Send_Data.AT_PORT = randr( 0, 0xDF );
	LoRaMacSetDeviceClass( CLASS_C );
	 	 
	LoRapp_SenSor_States.Tx_States = RFWRITE;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
		 
	///ʹ��GPS
//	Gps_Set(  ); 
    Set_Gps_Ack.GPS_DONE = true;
	/************GPS��λʱLoRa����************/
	Radio.Sleep();
	
	/*****************��������ʱ��*****************/
	TimerStop( &HEARTimer );
	TimerSetValue( &HEARTimer, 60000000 ); ///1min 60000000
	TimerStart( &HEARTimer );
	
//#ifdef	TEST_WATER
//	Ctrl_12V_PowerOn( );
//#else 
//	Ctrl_12V_PowerOff( );
//#endif

	while (1)
	{	

//#ifdef	TEST_WATER_PRESSURE   	
//		SamplingData( WaterSensorsData.temp );
//		delay_ms(1000);
//#endif
		
//#ifdef 	TEST_WATER_FLOW
//		WaterSensorsData.pulsecount = 0;
//		delay_ms(1000);
//		DEBUG(2, "WaterSensorsData.pulsecount = %d\r\n",WaterSensorsData.pulsecount);
//		WaterSensorsData.pulsecount = 0;
//#endif	
		
//#ifdef 	LORA_MODE	
		
		if(!Set_Gps_Ack.GPS_DONE)
		{    
			Get_Gps_Position(  ); //����GPS��Ϣ		
		}
		else  /***************GPS��λ��ɺ���ִ�п���ģʽ****************/
		{		 
			Irrigate_Control(  );
		}
//#endif	
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

