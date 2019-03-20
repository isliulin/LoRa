/*
**************************************************************************************************************
*	@file	user-app.c
*	@author Ysheng
*	@version 
*	@date    
*	@brief	Э����÷ֲ㷽ʽ��mac app��������
***************************************************************************************************************
*/
#include <math.h>
#include "user-app.h"
#include "board.h"
#include "LoRaMac.h"
#include "LoRa-cad.h"
#include "LoRaMac-api-v3.h"


#define APP_DATA_SIZE                                   (43)
#define APP_TX_DUTYCYCLE                                (6000000)     // 5min
#define APP_TX_DUTYCYCLE_RND                            (100000)   // ms


/*!
 * Join requests trials duty cycle.
 */
#define OVER_THE_AIR_ACTIVATION_DUTYCYCLE           	10000000 // 10 [s] value in ms

mac_callback_t mac_callback_g;

volatile bool IsNetworkJoined = false;

bool JoinReq_flag = true;

/*!
 * Defines the join request timer
 */
TimerEvent_t JoinReqTimer;

uint8_t DevEui[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8_t DEV[4] = {
    0
};

static uint8_t NwkSKey[] = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};
static uint8_t AppSKey[] = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

static uint32_t DevAddr;
static LoRaMacCallbacks_t LoRaMacCallbacks;
LoRaMacRxInfo RxInfo;

/************************��Ƶ���ʹ�����״̬************************/
LoRapp_State LoRapp_SenSor_States = {true, WRITE, 0, RFWRITE, 0 , 0 ,0};

/************************����RF�ְ����Ͳ���************************/
RF_Send RF_Send_Data = {0, 0, 0, 0, {0}, 0, 5, false, false}; 

/************************��ȡflash����������Ƶ����**************************/
Get_Flash_Data Get_Flash_Datas = {false, 0, 2, 285, 0, 2, 0, 0}; //285


/****************************OTAA����******************************
*********��1��read flash or get buy AT commed.
*********/

uint8_t AppEui[16] = {0};  
uint8_t AppKey[32] = {0};

sys_sta_t sys_sta = SYS_STA_IDLE;

TimerEvent_t SleepTimer;
volatile bool SleepTimerEvent = false;

uint32_t LoRaHeartTimer = 0;
Tx_State_t SaveTxState; ///�������ƣ�״̬����

/*
* LoRa����ʱ����������������ݴ���ʱ��: 8S���յȴ���RADIO�л�ΪRFREADY״̬
*/
void OnsleepTimerEvent( void )
{  
	if(LoRapp_SenSor_States.WKUP_State) ///���ߺ��ٴγ�ʼ��UARTʱ��
	{
	  __HAL_RCC_SYSCFG_CLK_ENABLE();
	  __HAL_RCC_PWR_CLK_ENABLE();

	  delay_init(4);
	  MX_USART1_UART_Init( );  
	}
	SleepTimerEvent = true;
	TimerStop( &SleepTimer );
	if(LoRapp_SenSor_States.Tx_States == RADIO) ///�ȴ�RADIO״̬һ��ʱ���,�ָ�λ����
	LoRapp_SenSor_States.Tx_States = RFREADY;
	DEBUG(3,"%s\r\n",__func__);
}

TimerEvent_t HEARTimer;
void OnHearTimerEvent( void )
{  
	DEBUG(2,"%s\r\n",__func__);
	LoRaHeartTimer++;
	TimerStop( &HEARTimer );
	TimerSetValue( &HEARTimer, 60000000 ); ///1min
	TimerStart( &HEARTimer );
}

/*!
 * \brief Function executed on JoinReq Timeout event
 */
void OnJoinReqTimerEvent( void )
{
    TimerStop( &JoinReqTimer );
    JoinReq_flag = true;
    DEBUG(2,"OnJoinReqTimerEvent \r\n");
}

LoRaFrameType_t LoRaFrameType;

/*!
 * \brief Function to be executed on MAC layer event
 */
void OnMacEvent( LoRaMacEventFlags_t *flags, LoRaMacEventInfo_t *info )///MAC�㷢�͡�����״̬�жϡ����ݴ���
{
	switch( info->Status )
	{
		case LORAMAC_EVENT_INFO_STATUS_OK:
			 break;
		case LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT:;
			 break;
		case LORAMAC_EVENT_INFO_STATUS_ERROR:
			 break;
		default:
			 break;
	}

	if( flags->Bits.JoinAccept == 1 )
	{       
		DEBUG(2,"join done\r\n");
		TimerStop( &JoinReqTimer );
		IsNetworkJoined = true;
//				Rx_Led( );
		mac_callback_g(MAC_STA_CMD_JOINACCEPT, NULL);
	}  
  
	if( info->TxAckReceived == true )
	{ /// McpsConfirm.AckReceived = true;���ջص�����
		if(mac_callback_g!=NULL)
		{
			mac_callback_g(MAC_STA_ACK_RECEIVED, &RxInfo); ///�൱�ڵ���app_lm_cb����
			DEBUG(2,"ACK Received\r\n");
		}
	}else if((flags->Bits.Rx != 1) && (flags->Bits.JoinAccept != 1) && (LoRaFrameType == CONFIRMED))
	{
		DEBUG(2,"=====NO ACK REPLY=====\r\n");     
		mac_callback_g(MAC_STA_ACK_UNRECEIVED, NULL);        
	}
	
	 if( flags->Bits.Rx == 1 )
	{  ///���յ����ݣ�������Ϣ����RxData: ����Ӧ������Ĭ�ϲ���ӡ������Ϣ
		RxInfo.size = info->RxBufferSize;
		memcpy(RxInfo.buf, info->RxBuffer, RxInfo.size);
		RxInfo.rssi = info->RxRssi;
		RxInfo.snr = info->RxSnr;
		RxInfo.win = flags->Bits.RxSlot+1;
		RxInfo.port = info->RxPort;
//            Rx_Led( );
		DEBUG(2,"win = %d snr = %d rssi = %d size = %d \r\n",RxInfo.win, RxInfo.snr, RxInfo.rssi, RxInfo.size);
		
		if(flags->Bits.RxData == 1)
		{
			if(mac_callback_g!=NULL)
			{
				mac_callback_g(MAC_STA_RXDONE, &RxInfo);
				if( RxInfo.size>0 )
				{
					DEBUG(2,"RxInfo.buf = ");
					for( uint8_t i = 0; i < RxInfo.size; i++ )
					DEBUG(2,"%02x ",RxInfo.buf[i]);
					DEBUG(2,"\r\n");									
				}
			}
//								0x52 0x41 0x44 0x49 0x4f
//								if( strcmp((char *)RxInfo.buf, "RADIO") == 0 )
			if((RxInfo.buf[0] == 0x52) && (RxInfo.buf[1] == 0x41) && (RxInfo.buf[2] == 0x44) && (RxInfo.buf[3] == 0x49) && (RxInfo.buf[4] == 0x4f))
			{
				LoRapp_SenSor_States.Tx_States = RADIO;
				DEBUG(2,"RADIO = %d\r\n",LoRapp_SenSor_States.Tx_States);
			}					
			else if((RxInfo.buf[0] == 0x01) && (RxInfo.buf[1] == 0xb1))
			{
				LoRapp_SenSor_States.Water_Control_State = OPEN;
				LoRapp_SenSor_States.Tx_States = RFACK;
				DEBUG(2,"LoRapp_SenSor_States = %d\r\n",LoRapp_SenSor_States.Tx_States);
			}
			else if((RxInfo.buf[0] == 0x02) && (RxInfo.buf[1] == 0xb1))
			{
				LoRapp_SenSor_States.Water_Control_State = CLOSE;
				LoRapp_SenSor_States.Tx_States = WATERAUTCLOSE;

			}
			else ///���յ���������Ϣ����ص�cad״̬
			{
				LoRapp_SenSor_States.Tx_States = RFWRITE;
				ReportTimerEvent = false;
			}
			memset(RxInfo.buf, 0, strlen((char *)RxInfo.buf));
		}     
	}
	 
	if( flags->Bits.Tx == 1 )
	{
		if(mac_callback_g!=NULL)
		{
			mac_callback_g(MAC_STA_TXDONE, NULL);
			if( flags->Bits.JoinAccept == 1 ) ///�����OTAA��������Ӧ����ֱ�ӷ������ݲ���Ҫ�ȴ�������ȴ�
			{
				LoRapp_SenSor_States.loramac_evt_flag = 0;
			}else
			LoRapp_SenSor_States.loramac_evt_flag = 1;
			
			DEBUG(2,"Done\r\n");
	//					Send_Led( );
		}
	}
}

void user_app_init(mac_callback_t mac)
{
    LoRaMacCallbacks.MacEvent = OnMacEvent; ///MAC�����ݽӿ�
    LoRaMacCallbacks.GetBatteryLevel = BoardGetBatteryLevel;
    LoRaMacInit( &LoRaMacCallbacks );
    
    IsNetworkJoined = false;
    
#if( OVER_THE_AIR_ACTIVATION == 0 )  
    
    DevAddr  = DEV[3];
    DevAddr |= (DEV[2] << 8);
    DevAddr |= (DEV[1] << 16);
    DevAddr |= (DEV[0] << 24);
   	DEBUG(2,"DevAddr : %02x-%02x-%02x-%02x\r\n",DEV[0],DEV[1],DEV[2],DEV[3]);
    
    LoRaMacInitNwkIds( 0x000000, DevAddr, NwkSKey, AppSKey );
    IsNetworkJoined = true;
 
#else
     // Initialize LoRaMac device unique ID : ���м���ʱ��Ϊ�������
    BoardGetUniqueId( DevEui );
    for(uint8_t i = 0; i < 8; i++)
    DEBUG(2,"%02x ", DevEui[i]);
    DEBUG(2,"\r\n");
   
     // Sends a JoinReq Command every OVER_THE_AIR_ACTIVATION_DUTYCYCLE
    // seconds until the network is joined
    TimerInit( &JoinReqTimer, OnJoinReqTimerEvent );
    TimerSetValue( &JoinReqTimer, OVER_THE_AIR_ACTIVATION_DUTYCYCLE );
    
#endif
  
    LoRaMacSetAdrOn( Get_Flash_Datas.LoRaMacSetAdrOnState );
    LoRaMacTestSetDutyCycleOn(false);
    
    mac_callback_g = mac;

}

//uint8_t LORAMAC_MIN_DATARATE = 0;
//uint8_t LORAMAC_MAX_DATARATE = 5;

char String_Buffer[33]; ///��ȡflashд���ַ���

int PowerXY(int x, int y)
{
	if(y == 0)
	return 1 ;
	else
	return x * PowerXY(x, y -1 ) ;
}

/*!
*Convert16To10��16����ת��Ϊ10����
*����ֵ: 		     		  10������ֵ
*/
int Convert16To10(int number)
{
	int r = 0 ;
	int i = 0 ;
	int result = 0 ;
	while(number)
	{
		r = number % 16 ;
		result += r * PowerXY(16, i++) ;
		number /= 16 ;
	}
	return result ;
}

/*!
*Read_DecNumber���ַ����е�����ת��Ϊ10����
*����ֵ: 		     10������ֵ
*/
uint32_t Read_DecNumber(char *str)
{
	uint32_t value;

	if (! str)
	{
		return 0;
	}
	value = 0;
	while ((*str >= '0') && (*str <= '9'))
	{
		value = value*10 + (*str - '0');
		str++;
	}
	return value;
}

/*!
*String_Conversion���ַ���ת��Ϊ16����
*����ֵ: 		    ��
*/
void String_Conversion(char *str, uint8_t *src, uint8_t len)
{
   volatile int i,v;
        
   for(i=0; i<len/2; i++)
   {
    sscanf(str+i*2,"%2X",&v);
    src[i]=(uint8_t)v;
   }
}

/*!
*Read_Flash_Abp_Data����ȡABP��������
*����ֵ: 		      ��
*/
uint8_t Read_Flash_Data(void)
{	
	 STMFLASH_Read(SET_ADR_ADDR,(uint16_t*)String_Buffer,2);                 ///ADR
	 String_Conversion(String_Buffer, (uint8_t *)&Get_Flash_Datas.ReadSetAdar_Addr, 2);
	 if(Get_Flash_Datas.ReadSetAdar_Addr < 1)
	 {
		 Get_Flash_Datas.LoRaMacSetAdrOnState = false;
	 }else
	 Get_Flash_Datas.LoRaMacSetAdrOnState = true;
	 memset(String_Buffer, 33, 0);
	 DEBUG(2,"ADR = %d\r\n",Get_Flash_Datas.LoRaMacSetAdrOnState);
	 
	 STMFLASH_Read(LORAMAC_DEFAULT_DATARATE,(uint16_t*)String_Buffer,2);                 ///DataRate������Ĭ�Ͽ���
	 String_Conversion(String_Buffer, &Get_Flash_Datas.datarate, 2);
	 memset(String_Buffer, 33, 0);
	 
	 RF_Send_Data.ADR_Datarate = RF_Send_Data.default_datarate = Get_Flash_Datas.datarate;
	 
	 ///����
//	 RF_Send_Data.default_datarate = 5;
//	 LORAMAC_MIN_DATARATE = Get_Flash_Datas.datarate;
	 ///����
	 
	 DEBUG(2,"LORAMAC_DEFAULT_DATARATE = %d\r\n",Get_Flash_Datas.datarate);

	 STMFLASH_Read(Rx2DATARATE_ADDR,(uint16_t*)String_Buffer,2);                 ///RX2 datarate
	 String_Conversion(String_Buffer, &Rx2Channel.Datarate, 2);
	 memset(String_Buffer, 33, 0);
	 
	 DEBUG(2,"RX2_datarate = %d\r\n",Rx2Channel.Datarate);
	 
	 uint8_t RX2_Frequency[2] = {0};
	 
	 STMFLASH_Read(RX2FREQ_ADDR,(uint16_t*)String_Buffer,RX2FREQ_ADDR_SIZE);                 ///RX2 Frequency
	 String_Conversion(String_Buffer, RX2_Frequency, 4);
	 memset(String_Buffer, 33, 0);
	 
	 Rx2Channel.Frequency |= (RX2_Frequency[0]<<8);
	 DEBUG(2,"RX2_Freq1 = %x \r\n",Rx2Channel.Frequency); 
	 Rx2Channel.Frequency |= RX2_Frequency[1];
	 DEBUG(2,"RX2_Freq2 = %x \r\n",Rx2Channel.Frequency); 
	 
	 Convert16To10(Rx2Channel.Frequency);

	 Rx2Channel.Frequency *= 100e3;
	 
	 DEBUG(2,"RX2_Freq = %d %x %x \r\n",Rx2Channel.Frequency,RX2_Frequency[0],RX2_Frequency[1]); 
	 

//	LORAMAC_MIN_DATARATE = RF_Send_Data.default_datarate; ///Ĭ��datarate = min dataate
	
	///����	 
//	LORAMAC_MAX_DATARATE = 5;  ///MAX DataRate���̶�SF����ͨ�ţ���СSF��������ADR��������
	///����
	 
	Get_Flash_Datas.sync_time = 0;
	Get_Flash_Datas.channels = 0;
	DEBUG(2,"LORAMAC_MIN_DATARATE = %d LORAMAC_MAX_DATARATE = %d\r\n",LORAMAC_MIN_DATARATE,LORAMAC_MAX_DATARATE);
 
	if( OVER_THE_AIR_ACTIVATION == 0 ) 
	{ 
		uint8_t Devaddr[16] = {0};
	
		STMFLASH_Read(DEV_ADDR,(uint16_t*)String_Buffer,DEV_ADDR_SIZE/2);         ////DEV
    
		String_Conversion(String_Buffer, Devaddr, DEV_ADDR_SIZE);   
		memset(String_Buffer, 33, 0);	
		memcpy(DEV,&Devaddr[4],4);
	
		DEBUG(2,"Devaddr : ");
		for(uint8_t i = 0; i < 8; i++)
		DEBUG(2,"%02x",Devaddr[i]);
		DEBUG(2,"\r\n");
			
		if (strlen((const char*)DEV)==0
            ||strlen((const char*)DEV)>5
            ||Get_Flash_Datas.datarate > 5)
            return FAIL;
		}   
	return SUCCESS;   
}


int user_app_send( LoRaFrameType_t frametype, uint8_t *buf, int size, int retry)
{
	int sendFrameStatus;

	if(size == 0 || buf == 0){
		return -3;
	}

	LoRaFrameType = frametype;
	if(frametype == UNCONFIRMED){
		sendFrameStatus = LoRaMacSendFrame( RF_Send_Data.AT_PORT, buf, size );
	}else{
		if(retry <= 0){
			retry = 3;
		}
		sendFrameStatus = LoRaMacSendConfirmedFrame( RF_Send_Data.AT_PORT, buf, size, retry );
	}

	switch( sendFrameStatus )
	{
		case 1: // LoRaMac is Busy
		return -1;
		case 2:
		case 3: // LENGTH_PORT_ERROR
		case 4: // MAC_CMD_ERROR
		case 5: // NO_FREE_CHANNEL
		case 6:
			return -2;
		default:
			break;
	}
	return 0;
}

uint32_t app_get_devaddr(void)
{
    return DevAddr;
}

/*
 *	Into_Low_Power:	����͹���ģʽ��ͣ��
 *	����ֵ: 		��
 */
void Into_Low_Power(void)
{
	Radio.Sleep();
	BoardDeInitMcu(); ///�ر�ʱ����
    
	 /*****************����ͣ��ģʽ*********************/

	HAL_PWR_DisablePVD( );
	
	SET_BIT( PWR->CR, PWR_CR_CWUF );

	/* Set MCU in ULP (Ultra Low Power) */
	HAL_PWREx_EnableUltraLowPower( );
	
	/*Disable fast wakeUp*/
	HAL_PWREx_DisableFastWakeUp( );

	/* Enter Stop Mode */
	HAL_PWR_EnterSTOPMode( PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI );
}

/*
 *	app_loramacjoinreq:	OTAA��������
 *	����ֵ: 		    		��
 */
void app_loramacjoinreq(void)
{
	while( ( IsNetworkJoined == false ) )
	{
		if( JoinReq_flag == true )
		{
			JoinReq_flag = false;
			
			DEBUG(2,"LoRaMacJoinReq \r\n");
			int  sendFrameStatus = LoRaMacJoinReq( DevEui, AppEui, AppKey );
//					Send_Led( ); ///��ǰPB15��ռ��
			DEBUG(2,"sendFrameStatus = %d\r\n",sendFrameStatus);
			switch( sendFrameStatus )
			{
				case 1: // BUSY
					break;
				case 0: // OK
				case 2: // NO_NETWORK_JOINED
				case 3: // LENGTH_PORT_ERROR
				case 4: // MAC_CMD_ERROR
				case 6: // DEVICE_OFF
				default:
					// Relaunch timer for next trial                
					TimerStart( &JoinReqTimer );                  
					break;
			}
		}
	}   
}

void Send_Led(void)
{
	for(uint8_t i = 0; i < 5; i++)
	{
		delay_ms(50);
		GpioWrite(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
		delay_ms(50);
		GpioWrite(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
	}
}

void Rx_Led(void)
{
	for(uint8_t i = 0; i < 5; i++)
	{
		delay_ms(300);
		GpioWrite(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
		delay_ms(300);
		GpioWrite(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
	}
}

/*******************************���巢������************************************/

uint8_t AppData[APP_DATA_SIZE];

volatile bool ReportTimerEvent = false;

TimerEvent_t ReportTimer;

extern void SetRtcAlarm(uint16_t time);

void OnReportTimerEvent( void )
{
	ReportTimerEvent = true;
	DEBUG(2,"%s\r\n",__func__);
	TimerStop( &ReportTimer );	
}

/*
 *	SX1278_Send:	RF���ͺ��������зְ����͹���
 *	����ֵ: 			��
 */
void SX1278_Send(uint8_t LoRaFrameTypes,uint8_t *send_buf)
{	 	      
	switch(sys_sta)
	{
		case SYS_STA_IDLE:
		if(ReportTimerEvent == true)
		{ 
			sys_sta = SYS_STA_TX; 
			if(RF_Send_Data.Len == 0 && RF_Send_Data.Send_again == false) //��ǰ����Ϊ�գ�ͬʱ���Ƿ���ʧ���»�ȡ���ݣ����»�ȡ���ݷ���
			{				
				memcpy(RF_Send_Data.RF_BUF, send_buf, RF_Send_Data.RX_LEN);   //MAC+PHY=56  MAC = 13  
				RF_Send_Data.Error_count = 0;							
			}	
			DEBUG(2,"START RF_Send_Data.RX_LEN = %d\r\n",RF_Send_Data.RX_LEN);					
		}
		 break;
		 case SYS_STA_TX:		
			 while( RF_Send_Data.RX_LEN > 0)
			{
				if(RF_Send_Data.RX_LEN < RF_Send_Data.TX_Len)
				{
					RF_Send_Data.TX_Len = RF_Send_Data.RX_LEN;			
				}
						
				if(ReportTimerEvent == true)
				{
					ReportTimerEvent = false;
					
					LoRaCad.Cad_Mode = true;
					while(LoRaCad.Cad_Mode || LoRaCad.Cad_Detect)	//10ms��ʱ����
					LoRa_Cad_Mode( );		
									
					Channel = Get_max(3,LoRaCad.Rssi); ///��ȡ�ŵ�ID
					DEBUG(2,"Channel_send = %d\r\n",Channel);

					if( user_app_send(LoRaFrameTypes, &RF_Send_Data.Send_Buf[RF_Send_Data.Len], RF_Send_Data.TX_Len, 1) == 0 )
					{                               							 
						DEBUG(2,"Wait ACK app_send UpLinkCounter = %d\r\n", LoRaMacGetUpLinkCounter( ));
						ReportTimerEvent = false;
						if(RF_Send_Data.TX_Len == RF_Send_Data.RX_LEN)
						{  ///�������ݽ������ж����һ�η��͵������Ƿ�Ϊ���һ���������
							memset(AppData, 0, sizeof(AppData));
							RF_Send_Data.RF_Send = true;						
						}
					}
					else 
					{ ///����ʧ�������ʱ���ͣ�����ʧ��ԭ��datarate�����ı䵼�����ݷ���ʧ��
						ReportTimerEvent = false;
						TimerStop( &ReportTimer );
						TimerSetValue( &ReportTimer, APP_TX_DUTYCYCLE_RND + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND ) );
						TimerStart( &ReportTimer );
						DEBUG(2,"app_send again\r\n");
						
						RF_Send_Data.Error_count ++;
						RF_Send_Data.Send_again = true;
						RF_Send_Data.default_datarate = RF_Send_Data.ADR_Datarate; ///���¿���
						break;
					}
				}
										
				if(LoRapp_SenSor_States.loramac_evt_flag == 1)
				{										
					if(RF_Send_Data.RX_LEN > RF_Send_Data.TX_Len)
					{
						RF_Send_Data.Len += RF_Send_Data.TX_Len;   ///�����±�
						RF_Send_Data.RX_LEN -= RF_Send_Data.TX_Len; ///��ǰ������Len 
					
						if(RF_Send_Data.RX_LEN<=RF_Send_Data.TX_Len)
						{
							RF_Send_Data.TX_Len = RF_Send_Data.RX_LEN;
						}
						
						ReportTimerEvent = false;
						TimerStop( &ReportTimer );
						TimerSetValue( &ReportTimer, APP_TX_DUTYCYCLE_RND + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND ) );
						TimerStart( &ReportTimer );
					}
			
					if(RF_Send_Data.RF_Send == true)						
					{
						RF_Send_Data.RF_Send = false;
						RF_Send_Data.Send_again = false;
						RF_Send_Data.TX_Len = RF_Send_Data.Len = 0;
						RF_Send_Data.RX_LEN = 0;
						sys_sta = SYS_STA_IDLE;
						ReportTimerEvent = false;		
						DEBUG(2, "break reporttimer\r\n");

						break;	
					}
				}						
			}
		 break;
		default : 
		 break;
   }			
}

/*
 *	User_send:	�û��������ݺ���
 *	����ֵ: 		��
 */
void User_send(uint8_t LoRaFrameTypes,uint8_t *Send_Buf)
{
	switch(RF_Send_Data.default_datarate)
	{
		case 0:  //12 -- 51
		case 1:  //11 -- 51
		case 2:  //10 -- 51
		RF_Send_Data.TX_Len = 51;
		SX1278_Send( LoRaFrameTypes, Send_Buf );   
		break;
		case 3:  //9 --- 115
		RF_Send_Data.TX_Len = 115;
		SX1278_Send( LoRaFrameTypes, Send_Buf );   
		break;
		case 4:  //8 --- 222
		case 5:  //7 --- 222
		RF_Send_Data.TX_Len = 222;
		SX1278_Send( LoRaFrameTypes, Send_Buf );   
		break;
		default: break;
	}
}


/*
*Irrigate_Control�����������
*��������
*���أ���
*/
void Irrigate_Control(void)
{
	
#if GPSWORKAGAIN	
	
	if((HAL_GetTick( ) - LoRapp_SenSor_States.Work_Time) > GPSWORKTIME )
	Set_Gps_Ack.GPS_DONE = false;
	
#endif
	
	LoRa_Detect_Mode(  );
	
	if(LoRaHeartTimer >= LORAHEARTTIME ) 
	{
		LoRaHeartTimer = 0;
		DEBUG(2,"LoRaHeartTimer\r\n");
		SaveTxState = LoRapp_SenSor_States.Tx_States; ///����������ǰ״̬
		LoRapp_SenSor_States.Tx_States = RFBATHEART;
	}
		
	switch(LoRapp_SenSor_States.Tx_States)
	{
		case RFBATHEART:
			 OnLoRaHeartDeal(  );
		break;
			
		case RADIO: ///���������������л���rx mode	
			DEBUG(2,"RADIO\r\n");
			SleepTimerEvent = false;
			Radio.Standby();
			OnRxWindow2TimerEvent(  ); ///���ý���ģʽΪ�ڵ�����ģʽ	
		 
			///�������ƣ������ղ��������������ݣ��л���Rxģʽ����Ҫ��Ϻ�̨ʵ��
			TimerStop( &SleepTimer );
			TimerSetValue( &SleepTimer, 6000000 );
			TimerStart( &SleepTimer );	
			while(!SleepTimerEvent);		//˼���£����б����д�������л�ֱ��RX		
		break;
	
		case RFACK: ///����һ֡���ݻ��ѡ�һ֡���ݿ��ƣ����յ��������Ӧ��: "AO"
			 OnLoRaAckDeal(  );
			
		break;
	
		case WATERAUTOPEN: ///�ϱ����������ݣ�1min/��   WaterAutopen  WATERAUTOPEN
			 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
			 OnWaterAutopen(  );
			 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
	
		break;		
		case WATERAUTCLOSE: ///�ϱ����������ݣ�1min/��	
//			 HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_15);
			 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
			 OnWaterAutClose(  );
			 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
		
		break;
		
		case WATERMANOPEN: ///�ֶ��ϱ�"MO"
			 WaterManOpen(  );
		
		break;
		
		case WATERMANCLOSE:  ///�ֶ��ϱ�"MC"  WaterManClose
			 WaterManClose(  );
			
		break;
		
		case RFREADY: ///����״̬�����������������յ��������ݺ��л������գ������յ��ر���Ϣ�ٴν�������
			 LoRaCad.Cad_Detect = false; //��Ҫ��ӣ���ֹ���յ���ͬSF cad���Ѻ�û���յ���ȷSF���ݣ���Ƶ��ʱ�䴦�ڽ���״̬
			 LoRapp_SenSor_States.Tx_States = RFWRITE;
	  break;
		default : 
			DEBUG(3,"Rx_States222 = %d\r\n",LoRapp_SenSor_States.Tx_States);

		break;
  }
}
