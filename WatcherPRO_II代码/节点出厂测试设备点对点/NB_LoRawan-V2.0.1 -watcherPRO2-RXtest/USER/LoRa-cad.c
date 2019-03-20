/*
**************************************************************************************************************
*	@file	LoRa-cad.c
*	@author Ysheng
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
	
	if(RF_Send_Data.default_datarate == 0)
		LORA_SPREADING_FACTOR = 12;
	else if(RF_Send_Data.default_datarate == 1)
		LORA_SPREADING_FACTOR = 11;
	else if(RF_Send_Data.default_datarate == 2)
		LORA_SPREADING_FACTOR = 10;
	else if(RF_Send_Data.default_datarate == 3)
		LORA_SPREADING_FACTOR = 9;
	else if(RF_Send_Data.default_datarate == 4)
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
 * ����:				   ��
 * ����ֵ:				 ��
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
 * ����:				      ��
 * ����ֵ:				    ��
*/
void LoRa_Detect_Mode(void)
{
	int16_t rssi[2];
			
	///��������ģʽ
	if(!LoRaCad.Cad_Detect && LoRapp_SenSor_States.Rx_States == RFWRITE )
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
			DEBUG(3,"Rx_States222 = %d\r\n",LoRapp_SenSor_States.Rx_States);			
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
