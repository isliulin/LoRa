/*
**************************************************************************************************************
*	@file	LoRa-cad.c
*	@author Jason<Jason_531@163.com>
*	@version 
*	@date    
*	@brief	Ӧ�ò�ͷ�ļ�������MAC��
***************************************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "LoRa-cad.h"

/******************************24·ѡ��TX1---23**************************************/
#define LC4                { 471100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 471300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 471500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 471700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 478100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }  

#define LC9                { 478300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC10               { 478500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC11               { 478900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC12               { 479100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC13               { 479300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC14               { 479500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC15               { 479700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC16               { 485900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

#define LC17               { 486100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC18               { 486300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC19               { 486500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC20               { 486700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC21               { 486900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC22               { 487100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC23               { 487300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/************************����CADģʽ����**************************/
LoRaCad_t LoRaCad = {true, false, false, false, 0, 0, {0}, 0, 0, 0, 0, 0, 0, {0}, {0}};

TimerEvent_t CadTimer;

/*
 * LoRa_Cad_Init:	 CAD��ʼ��
 * ����:				   ��
 * ����ֵ:				 ��
*/
void LoRa_Cad_Init(void)
{
	Radio.Standby();
	Radio.StartCad( );  // Set the device into CAD mode
}

float SymbolTime(void)
{
	LoRaCad.symbolTime = 0;
	uint8_t LORA_SPREADING_FACTOR = 0;
	
	if(LoRapp_SenSor_States.default_datarate == 0)
		LORA_SPREADING_FACTOR = 12;
	else if(LoRapp_SenSor_States.default_datarate == 1)
		LORA_SPREADING_FACTOR = 11;
	else if(LoRapp_SenSor_States.default_datarate == 2)
		LORA_SPREADING_FACTOR = 10;
	else if(LoRapp_SenSor_States.default_datarate == 3)
		LORA_SPREADING_FACTOR = 9;
	else if(LoRapp_SenSor_States.default_datarate == 4)
		LORA_SPREADING_FACTOR = 8;
	else 
		LORA_SPREADING_FACTOR = 7;
	
	 LoRaCad.symbolTime = (( pow( (float)2, (float)LORA_SPREADING_FACTOR ) ) + 32 ) / 125000;  // SF7 and BW = 125 KHz
	 LoRaCad.symbolTime = LoRaCad.symbolTime * 1000000;  // symbol Time is in us
	 DEBUG(3,"LORA_SPREADING_FACTOR = %d symbolTime = %lf\r\n",LORA_SPREADING_FACTOR,LoRaCad.symbolTime);
	 return LoRaCad.symbolTime;
}


extern void OnadTimerEvent( void );

/*
 * LoRa_Cad_Mode:	 ��������ǰ�ŵ����
 * ����:			 ��
 * ����ֵ:	         ��
*/
void LoRa_Cad_Mode(void)
{	
	Send_time = 0;
					
	Channel = Get_Flash_Datas.channels = randr( 0, 4 ); ///��ȡ�ŵ�ID: flash��ȡ
	
	LoRaCad.Iq_Invert = true;  ///ʹ�ܽڵ��ͨ��
	
	/*********************************�����ŵ���æ���ٴμ���������ʱ����*******************************/
	if(LoRaCad.Cad_Detect == true)
	{
		DEBUG(3,"int cad again\r\n");
		
		LoRaCad.Cad_Detect = false;
	}									
	
	LoRaCad.Channel_Num = 0;
		
	while(!LoRaCad.Cad_Detect && LoRaCad.Channel_Num<5)	
	{		
		LoRaCad.Cad_Done = false;

		Radio.Standby();
		OnRxWindow1TimerEvent( ); ///���ý���ģʽΪ�ڵ�����ģʽ
		
		LoRa_Cad_Init( ); ///ע�⣺������ɻ�ȡ������Ҫ�����ٳ�ʼ��Rx�� ���򲻽�������
						
		uint32_t symbolTime = SymbolTime();	
		
	    delay_us( 240 ); 

		delay_us( symbolTime + 240 ); 

		LoRaCad.Rssi[LoRaCad.Channel_Num]  = Radio.Rssi(MODEM_LORA); ///��¼�ź�ǿ��
		LoRaCad.Channel_Scan[LoRaCad.Channel_Num] = Channel;		///��¼�ŵ�ID
					
		LoRaCad.Rssi[LoRaCad.Channel_Num] = ~(LoRaCad.Rssi[LoRaCad.Channel_Num] - 1); ///����ת����
	
		DEBUG(3,"symbolTime = %d,rssi[0] = %d Channel = %d\r\n", symbolTime,LoRaCad.Rssi[LoRaCad.Channel_Num],LoRaCad.Channel_Scan[LoRaCad.Channel_Num]);
		
		LoRaCad.Channel_Num++;
		Channel++;
		
		if(Channel==5)
			Channel = 0;
		
		uint32_t t = HAL_GetTick( );//Send_time;

		while(LoRaCad.Cad_Done != true && (HAL_GetTick( ) - t < 50));	///50ms  
	}
			
	/********************RTCʱ��ȡ�����㣺�͹��Ļ��ѵ�ǰ��֧��MS������ֹʱ�����ƫ��***********************/
	LoRaCad.Iq_Invert = false;
	LoRaCad.Cad_Mode = false;
	Radio.Sleep();
}


extern TimerEvent_t SleepTimer;
extern volatile bool SleepTimerEvent;
extern void OnsleepTimerEvent( void );

/*
 * LoRa_Detect_Mode:	CAD����������CAD��RX mode�л� 
 * ����:				��
 * ����ֵ:				��
*/
void LoRa_Detect_Mode(void)
{
	int16_t rssi[2];
			
	///��������ģʽ
	if(!LoRaCad.Cad_Detect)
	{
		LoRaCad.Cad_Done = false;
		LoRaCad.Iq_Invert = false;
		
		LoRa_Cad_Init( ); ///ע�⣺������ɻ�ȡ������Ҫ�����ٳ�ʼ��Rx�� ���򲻽�������
						
		uint32_t symbolTime = SymbolTime();	
		
		delay_us( 240 ); 

		delay_us( (symbolTime + 240)/2 ); 
		rssi[0] = Radio.Rssi(MODEM_LORA); ///��¼�ź�ǿ��
		
		delay_us( (symbolTime + 240)/2 ); 
		rssi[1] = Radio.Rssi(MODEM_LORA); ///��¼�ź�ǿ��
					
		uint32_t t = Send_time;

		while((LoRaCad.Cad_Done != true) && (Send_time - t <50));	///50ms  
	 
		DEBUG(3,"get rssi[0] = %d rssi[1] = %d\r\n", rssi[0], rssi[1]);

		///�˲��������Ƶsleep����Ҫ�������ݰ���һ���ݰ����ѣ�һ���ݰ��������ݣ�ͬʱcad detect����ʱSX1278������
		if(LoRaCad.Cad_Detect == false) 
		{
			DEBUG(3,"LoRaCad.Cad_Detect != Cad_Detect %d\r\n", 15*( symbolTime + 480));
			Radio.Sleep();
			__WFI(); ///���Բ���MCU����ģʽ
			SleepTimerEvent = false;
			TimerStop( &SleepTimer );
			TimerSetValue( &SleepTimer, 15*( symbolTime + 480)/1000 );
			TimerStart( &SleepTimer );
			while(!SleepTimerEvent);
			SleepTimerEvent = false;
		}else if(LoRaCad.Cad_Detect) ///CAD���������ݣ��л�Ϊ����ģʽ8S��û���յ����ݣ�8S���ٴ��л�ΪCAD MODE
		{	
			TimerStop( &CadTimer );
			TimerSetValue( &CadTimer, 8000 );
			TimerStart( &CadTimer );
		}
	}
}

uint8_t Get_max(int8_t m,int16_t array[])
{
	uint8_t channel_scan[8] = {0};
	uint8_t channel_new[8] = {0};

	int max,min;
	int8_t t = 0;
	
	max=min=array[0];  
	for(int8_t i=1;i<m;i++)  
	{    
		if(max<array[i]) 
		{
			max=array[i];
		}			
		else if(min>array[i]) 
		{			
			min=array[i]; 
		}
	}	
	
	for(int8_t j=0;j<m;j++)  
	{	
		if(max == array[j])
		{
			channel_scan[t] = j; ///��¼��ͬRSSIʱ���ŵ�ID�������±�
			channel_new[t]= LoRaCad.Channel_Scan[channel_scan[t]];	///����RSSI�¶�ȡ�����ŵ�ID		
			t++;
		}
	}

	uint8_t ID = randr( 0, t-1 ); 
	Channel = channel_new[ID]; 		
	return Channel;
}

uint8_t Bublesort(uint8_t a[],uint8_t n)
{
 int i,j,k;
	
 for(j=0;j<n-1;j++)   /* ð�ݷ�����n�� */
 {
	for(i=0;i<n-j-1;i++)  /* ֵ�Ƚϴ��Ԫ�س���ȥ��ֻ��ʣ�µ�Ԫ�����ֵ�ٳ���ȥ */
	{
		 if(a[i]>a[i+1])  /* ���ֵ������ */
		 {
				k=a[i];
				a[i]=a[i+1];
				a[i+1]=k;
		 }
	}
 }
 return *a;
}

/*
*LoRaMacChannelAddFun�������豸Ƶ��
*������                ��
*����ֵ��              ��
*/
void LoRaMacChannelAddFun( void )
{
	LoRaMacChannelAdd( 3, ( ChannelParams_t )LC4  );
	LoRaMacChannelAdd( 4, ( ChannelParams_t )LC5  );
	LoRaMacChannelAdd( 5, ( ChannelParams_t )LC6  );
	LoRaMacChannelAdd( 6, ( ChannelParams_t )LC7  );
	LoRaMacChannelAdd( 7, ( ChannelParams_t )LC8  );
    LoRaMacChannelAdd( 8, ( ChannelParams_t )LC9  );
	LoRaMacChannelAdd( 9, ( ChannelParams_t )LC10 );
	LoRaMacChannelAdd( 10,( ChannelParams_t )LC11 );
	LoRaMacChannelAdd( 11,( ChannelParams_t )LC12 );
	LoRaMacChannelAdd( 12,( ChannelParams_t )LC13 );
    LoRaMacChannelAdd( 13,( ChannelParams_t )LC14 );
	LoRaMacChannelAdd( 14,( ChannelParams_t )LC15 );
	LoRaMacChannelAdd( 15,( ChannelParams_t )LC16 );
	LoRaMacChannelAdd( 16,( ChannelParams_t )LC17 );
	LoRaMacChannelAdd( 17,( ChannelParams_t )LC18 );
    LoRaMacChannelAdd( 18,( ChannelParams_t )LC19 );
	LoRaMacChannelAdd( 19,( ChannelParams_t )LC20 );
	LoRaMacChannelAdd( 20,( ChannelParams_t )LC21 );
	LoRaMacChannelAdd( 21,( ChannelParams_t )LC22 );
	LoRaMacChannelAdd( 22,( ChannelParams_t )LC23 );
}

/*
 *	CalcCRC16:	����CRC16У��ֵ
 *	data:		����ָ��
 *	len:		���ݳ���
 *	����ֵ��	16λ��CRCУ��ֵ
 */
uint16_t CalcCRC16(uint8_t *data, uint8_t len)
{
	uint16_t result = 0xffff;
	uint8_t i, j;

	for (i=0; i<len; i++)
	{
		result ^= data[i];
		for (j=0; j<8; j++)
		{
			if ( result&0x01 )
			{
					result >>= 1;
					result ^= 0xa001;
			}
			else
			{
					result >>= 1;
			}
		}
	}
    
//  command.data[command.data_len-2] = crc_val&0xff;		//CRC��λ	
//	command.data[command.data_len-1] = crc_val>>8;			//CRC��λ
    
    DEBUG(2, "result : %02x\r\n",result);

	return result;
}
