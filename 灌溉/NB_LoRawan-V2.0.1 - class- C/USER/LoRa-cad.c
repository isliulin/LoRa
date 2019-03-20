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
LoRaCad_t LoRaCad = {true, false, false, false, 0, 0, {0}, 0, 0, 0, 0, 0, 0, {0}, {0}, 0};


TimerEvent_t CSMATimer;
volatile bool CSMATimerEvent = false;

void OnCsmaTimerEvent( void )
{  
	CSMATimerEvent = true;
	TimerStop( &CSMATimer );
}

/*
* LoRa CAD�����󴥷�����
* LoRa CAD�����쳣��������,��ֹ�������Ŵ���CAD DETECT mode 8Sʱ�������л�ΪCAD����
*/
TimerEvent_t CadTimer;
void OnCadUnusualTimerEvent( void )
{  
	if( LoRapp_SenSor_States.Tx_States == RFWRITE )  ///CAD�����ȴ�״̬�£�״̬�л�����ֹCAD�쳣����
	{
		LoRapp_SenSor_States.Tx_States = RFREADY;
		DEBUG(2,"%s  LoRapp_SenSor_States = %d \r\n",__func__, LoRapp_SenSor_States.Tx_States );
	}
	TimerStop( &CadTimer );
}

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

///RF_Send_Data.default_datarate
float SymbolTime(uint8_t Datarate)
{
	LoRaCad.symbolTime = 0;
	uint8_t LORA_SPREADING_FACTOR = 0;
	switch(Datarate)
	{
		case 0:  //12 -- 51
		LORA_SPREADING_FACTOR = 12;
		break;
		case 1:  //11 -- 51
		LORA_SPREADING_FACTOR = 11;
		break;
		case 2:  //10 -- 51
		LORA_SPREADING_FACTOR = 10;
		break;
		case 3:  //9 --- 115
		LORA_SPREADING_FACTOR = 9;
		break;
		case 4:  //8 --- 222
		LORA_SPREADING_FACTOR = 8;
		break;
		case 5:  //7 --- 222
		LORA_SPREADING_FACTOR = 7;
		break;
		default: break;
	}
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
	LoRaCadtime = 0;
					
	Channel = Get_Flash_Datas.channels = randr( 5, 7 ); ///��ȡ�ŵ�ID: flash��ȡ
	
	LoRaCad.Iq_Invert = true;  ///ʹ�ܽڵ��ͨ��
	
	/*********************************�����ŵ���æ���ٴμ���������ʱ����*******************************/
	if(LoRaCad.Cad_Detect == true)
	{
		if(LoRapp_SenSor_States.Tx_States != RFWRITE)  ///�ų���CAD��������ģʽ
		{
		 DEBUG(2,"int cad again\r\n");
		
		 LoRaCad.Cad_Detect = false;
		}
	}	

#if 1
	LoRaCad.TimeOnAir = Radio.TimeOnAir( MODEM_LORA, (RF_Send_Data.TX_Len + 13) ); 
	
	DEBUG(2,"TimeOnAir = %d\r\n",LoRaCad.TimeOnAir);
	int32_t data[8] = {0.5*LoRaCad.TimeOnAir,1*LoRaCad.TimeOnAir,1.5*LoRaCad.TimeOnAir,2*LoRaCad.TimeOnAir,2.5*LoRaCad.TimeOnAir,
					   3*LoRaCad.TimeOnAir,3.5*LoRaCad.TimeOnAir,4*LoRaCad.TimeOnAir}; ///�������ʱ�����飬��ʱ��Ϊ���٣���Ҫ��Բ�ͬSF���в�ͬ���� paload + total time on air

	Get_Flash_Datas.sync_time = randr( 0, 3 ); ///�ǹ̶�ģʽ�£����ʱ϶								
	
	/********************sync_timeʱ�䷶Χ�ж�*************************/										
	if(Get_Flash_Datas.sync_time == 0)
	{
		LoRaCad.randtime1 = -LoRaCad.TimeOnAir;
		LoRaCad.randtime2	= 0;			
	}
	else
	{
		LoRaCad.randtime1 = data[Get_Flash_Datas.sync_time-1];	
		LoRaCad.randtime2	= data[Get_Flash_Datas.sync_time];
		DEBUG(2,"sync_time = %d\r\n",Get_Flash_Datas.sync_time);				
	}	
	
	Radio.Standby();
	OnRxWindow1TimerEvent( ); ///���ý���ģʽΪ�ڵ�����ģʽ
	
	LoRa_Cad_Init( ); ///ע�⣺������ɻ�ȡ������Ҫ�����ٳ�ʼ��Rx�� ���򲻽�������
	TimerStop( &CSMATimer );
	TimerSetValue( &CSMATimer, LoRaCad.TimeOnAir + randr(LoRaCad.randtime1, LoRaCad.randtime2)); //+ randr(-1.5*TimeOnAir, 0)
	TimerStart( &CSMATimer );	

	LoRaCad.cad_all_time += LoRaCad.TimeOnAir + randr(LoRaCad.randtime1, LoRaCad.randtime2); ///��������CAD����ʱ��
		
	DEBUG(2,"GET UID LoRaCad.cad_all_time = %d LoRaCad.TimeOnAir = %d tt = %d\r\n",LoRaCad.cad_all_time, LoRaCad.TimeOnAir, LoRaCad.TimeOnAir + randr(LoRaCad.randtime1, LoRaCad.randtime2));
	while(!CSMATimerEvent);

#endif	
	
	LoRaCad.Channel_Num = 0;
		
	while(!LoRaCad.Cad_Detect && LoRaCad.Channel_Num<3)	
	{		
		LoRaCad.Cad_Done = false;

		Radio.Standby();
		OnRxWindow1TimerEvent( ); ///���ý���ģʽΪ�ڵ�����ģʽ
		
		LoRa_Cad_Init( ); ///ע�⣺������ɻ�ȡ������Ҫ�����ٳ�ʼ��Rx�� ���򲻽�������
						
		uint32_t symbolTime = SymbolTime(RF_Send_Data.default_datarate);	
		
		delay_us( 240 ); 

		delay_us( symbolTime + 240 ); 

		LoRaCad.Rssi[LoRaCad.Channel_Num]  = Radio.Rssi(MODEM_LORA); ///��¼�ź�ǿ��
		LoRaCad.Channel_Scan[LoRaCad.Channel_Num] = Channel;		///��¼�ŵ�ID
					
		LoRaCad.Rssi[LoRaCad.Channel_Num] = ~(LoRaCad.Rssi[LoRaCad.Channel_Num] - 1); ///����ת����
	
		DEBUG(2,"symbolTime = %d,rssi[0] = %d Channel = %d\r\n", symbolTime,LoRaCad.Rssi[LoRaCad.Channel_Num],LoRaCad.Channel_Scan[LoRaCad.Channel_Num]);
		
		LoRaCad.Channel_Num++;
		Channel++;
		
		if(Channel==8)
			Channel = 5;
		
		uint32_t t = LoRaCadtime;

		while(LoRaCad.Cad_Done != true && (LoRaCadtime - t < 50));	///50ms  
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
	if( !LoRaCad.Cad_Detect && (LoRapp_SenSor_States.Tx_States == RFWRITE) )
	{
		LoRaCad.Cad_Done = false;
		LoRaCad.Iq_Invert = false;
		
		LoRa_Cad_Init( ); ///ע�⣺������ɻ�ȡ������Ҫ�����ٳ�ʼ��Rx�� ���򲻽�������
						
		uint32_t symbolTime = SymbolTime(Rx2Channel.Datarate);	
		
		delay_us( 240 ); 

		delay_us( (symbolTime + 240)/2 ); 
		rssi[0] = Radio.Rssi(MODEM_LORA); ///��¼�ź�ǿ��
		
		delay_us( (symbolTime + 240)/2 ); 
		rssi[1] = Radio.Rssi(MODEM_LORA); ///��¼�ź�ǿ��
					
		uint32_t t = LoRaCadtime;

		while((LoRaCad.Cad_Done != true) && (LoRaCadtime - t <50));	///50ms  
	 
		DEBUG(3,"get rssi[0] = %d rssi[1] = %d\r\n", rssi[0], rssi[1]);

		///�˲��������Ƶsleep����Ҫ�������ݰ���һ���ݰ����ѣ�һ���ݰ��������ݣ�ͬʱcad detect����ʱSX1278������
		if(LoRaCad.Cad_Detect == false) 
		{
			DEBUG(3,"LoRaCad.Cad_Detect != Cad_Detect %d\r\n", 15*( symbolTime + 480));
			
			SleepTimerEvent = false;
			TimerStop( &SleepTimer );
			TimerSetValue( &SleepTimer, 15 *( symbolTime + 480) ); 
			TimerStart( &SleepTimer );
			
			LoRapp_SenSor_States.WKUP_State = true;
			Into_Low_Power(  );	
			while(!SleepTimerEvent);
			
			LoRapp_SenSor_States.WKUP_State = false;
		}else if(LoRaCad.Cad_Detect) ///CAD���������ݣ��Զ��л�Ϊ����ģʽ10S��û���յ����ݣ�10S���ٴ��л�ΪCAD MODE����ֹ�����
		{	
			DEBUG(3,"Rx_States222 = %d\r\n",LoRapp_SenSor_States.Tx_States);
			TimerStop( &CadTimer );
			TimerSetValue( &CadTimer, 10000000 );
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
