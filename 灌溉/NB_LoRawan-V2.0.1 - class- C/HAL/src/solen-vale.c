/*
**************************************************************************************************************
*	@file	  solen-vale.c
*	@author Ysheng
*	@version 
*	@date    
*	@brief  ��ŷ�
***************************************************************************************************************
*/

#include <stdint.h>
#include "solen-vale.h"
#include "sensor.h"
#include "user-app.h"

SolenoidState Solenoid_State;	//��ŷ���ǰ״̬

/*
 *solen_vale_init����ŷ���ʼ��
 *������		 ��
 *����ֵ��		 ��
*/
void solen_vale_init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	__HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��	
	
	GPIO_Initure.Pin=BI_PIN;    
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(BI_GPIO_PORT,&GPIO_Initure);
	
	GPIO_Initure.Pin=FI_PIN;    
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(FI_GPIO_PORT,&GPIO_Initure);
	
	HAL_GPIO_WritePin(BI_GPIO_PORT,BI_PIN,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(FI_GPIO_PORT,FI_PIN,GPIO_PIN_RESET);	
	
	Ctrl_12V_PowerOff(  );
	Solenoid_State = SLND_CLOSE;
}

/*
 * Ctrl_12V_PowerInit: ��ʼ��12V��Դ����
 * ������	 		   ��
 * ���ز�����		   ��
*/
void Ctrl_12V_PowerInit(void)
{	
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��	
	GPIO_Initure.Pin=CTRL_12V_POWERON;    
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(CTRL_12V_PORT,&GPIO_Initure);
}

/*
 * PowerOn:		�򿪵�ŷ���Դ����
 * ������		��
 * ���ز�����	��
*/
void Ctrl_12V_PowerOn(void)
{
	//12V��ŷ���Դ����
	HAL_GPIO_WritePin(CTRL_12V_PORT,CTRL_12V_POWERON,GPIO_PIN_SET);	 	
}

/*
 * PowerOn:	  �رյ�ŷ���Դ����
 * ������	  ��
 * ���ز����� ��
*/
void Ctrl_12V_PowerOff(void)
{
	HAL_GPIO_WritePin(CTRL_12V_PORT,CTRL_12V_POWERON,GPIO_PIN_RESET);	 	
}

/*
 * solen_vale_open:	 �򿪵�ŷ�
 * ������	 		 ��
 * ���ز�����		 ��
*/
void solen_vale_open(void)
{
	//100ms������
	HAL_GPIO_WritePin(BI_GPIO_PORT,BI_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FI_GPIO_PORT,FI_PIN,GPIO_PIN_SET);
	HAL_Delay(100);	
	HAL_GPIO_WritePin(BI_GPIO_PORT,BI_PIN,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(FI_GPIO_PORT,FI_PIN,GPIO_PIN_RESET);	
	Solenoid_State = SLND_OPEN;

	WaterSensorsData.pulsecount = 0;
}

/*
 * solen_vale_close: �رյ�ŷ�
 * ������	 		 ��
 * ���ز�����		 ��
*/
void solen_vale_close(void)
{
	//100ms������
	HAL_GPIO_WritePin(FI_GPIO_PORT, FI_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BI_GPIO_PORT, BI_PIN,GPIO_PIN_SET);
	HAL_Delay(100);	
	HAL_GPIO_WritePin(BI_GPIO_PORT, BI_PIN,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(FI_GPIO_PORT, FI_PIN,GPIO_PIN_RESET);	
	Solenoid_State = SLND_CLOSE;
}

/*
 * GetSolenoidState:	��ȡ��ǰ��ŷ�����״̬
 * ������				 			��
 * ����ֵ��				    ��ǰ��ŷ�����״̬
*/
SolenoidState GetSolenoidState(void)
{
	return Solenoid_State;
}

/*
 * SetSolenoidState:	���õ�ǰ��ŷ�����״̬
 * ������				 			��
 * ����ֵ��				    ��ǰ��ŷ�����״̬
*/
SolenoidState SetSolenoidState(SolenoidState state)
{
	Solenoid_State=state;
	return Solenoid_State;
}

/*
 *WaterMCounter��ˮ���ֶ��رմ�������
 *������		 ��
 *����ֵ��		 ��
*/
void WaterMCounter(void)
{
 RF_Send_Data.Send_Buf[2] = LoRapp_SenSor_States.WaterMCounter&0xff;
 RF_Send_Data.Send_Buf[3] = (LoRapp_SenSor_States.WaterMCounter>>8)&0xff;
 RF_Send_Data.Send_Buf[4] = (LoRapp_SenSor_States.WaterMCounter>>16)&0xff;
 RF_Send_Data.Send_Buf[5] = (LoRapp_SenSor_States.WaterMCounter>>24)&0xff;

 if(LoRapp_SenSor_States.WaterMCounter<0xff)
 {
	 RF_Send_Data.RX_LEN = 3;
 }
 else if(LoRapp_SenSor_States.WaterMCounter>0xff && LoRapp_SenSor_States.WaterMCounter<0xffff)
 {
	 RF_Send_Data.RX_LEN = 4;
 }
 else if(LoRapp_SenSor_States.WaterMCounter>0xffff && LoRapp_SenSor_States.WaterMCounter<0xffffff)
 {
	 RF_Send_Data.RX_LEN = 5;
 }
 else 
 {
	 RF_Send_Data.RX_LEN = 6;
 }
}

/*
 *WaterMOCounter��ˮ���ֶ��򿪴�������
 *������		  ��
 *����ֵ��		  ��
*/
void WaterMOCounter(void)
{
 /************************32λ���ݳ���ֻȡʵ�����ݳ��ȷ���,��ʡ����***************************/
 RF_Send_Data.Send_Buf[2] = LoRapp_SenSor_States.WaterMOCounter&0xff;
 RF_Send_Data.Send_Buf[3] = (LoRapp_SenSor_States.WaterMOCounter>>8)&0xff;
 RF_Send_Data.Send_Buf[4] = (LoRapp_SenSor_States.WaterMOCounter>>16)&0xff;
 RF_Send_Data.Send_Buf[5] = (LoRapp_SenSor_States.WaterMOCounter>>24)&0xff;

 if(LoRapp_SenSor_States.WaterMOCounter<0xff)
 {
	 RF_Send_Data.RX_LEN = 3;
 }
 else if(LoRapp_SenSor_States.WaterMOCounter>0xff && LoRapp_SenSor_States.WaterMOCounter<0xffff)
 {
	 RF_Send_Data.RX_LEN = 4;
 }
 else if(LoRapp_SenSor_States.WaterMOCounter>0xffff && LoRapp_SenSor_States.WaterMOCounter<0xffffff)
 {
	 RF_Send_Data.RX_LEN = 5;
 }
 else 
 {
	 RF_Send_Data.RX_LEN = 6;
 }
}


uint8_t GetBattery = 0;

/*
 *OnLoRaHeartDeal��LoRa����������
 *������		   ��
 *����ֵ��		   ��
*/
void OnLoRaHeartDeal(void)
{
	ReportTimerEvent = true;
	GetBattery = CheckBattery(  );

	RF_Send_Data.Send_Buf[0] = GetBattery;
	if (GetBattery<=5)					//��ص�����
	{
		RF_Send_Data.Send_Buf[1] = 'L';
	}
	else
	{
	  RF_Send_Data.Send_Buf[1] = 'H';
	}
					
	RF_Send_Data.RX_LEN = 2;
	do
	{
		/**************************����������ǰ����Ƿ������п��ƴ��ڣ������˳�**************************/
		if(LoRapp_SenSor_States.Tx_States == RFBATHEART) 
		{
			///���ͣ�ִ����ɣ�ԭ���ݷ���
			User_send(UNCONFIRMED, RF_Send_Data.Send_Buf);	
		}
		else ///����ǰ״̬�����ģ�ֱ���˳�
		{
			DEBUG(2, "go break\r\n");
			LoRapp_SenSor_States.loramac_evt_flag = 0;
			ReportTimerEvent = false;
			break;
		}		
	}while(!LoRapp_SenSor_States.loramac_evt_flag);
	
	/*******************���������ɱ�־*********************/
	__disable_irq();
	LoRapp_SenSor_States.loramac_evt_flag = 0;
	ReportTimerEvent = false;
	__enable_irq();
	
	if (GetBattery<=5)					//��ص�����
	{
		DEBUG(2,"battery extremely low;enter standby\r\n");
		StandbyEnterMode( );					//��ص����ر�ͣ�ֱ�����ý�������
	}
	LoRapp_SenSor_States.Tx_States = SaveTxState;
}

/*
 *OnLoRaAckDeal��LoRa ACKӦ����
 *������		 ��
 *����ֵ��		 ��
*/
void OnLoRaAckDeal(void)
{
	TimerStop( &CadTimer ); ///�ر�CAD�쳣����
	ReportTimerEvent = true;

	memset(RF_Send_Data.Send_Buf, 0, strlen((char *)RF_Send_Data.Send_Buf));
	memcpy(RF_Send_Data.Send_Buf, "AO", strlen("AO")); 

	RF_Send_Data.RX_LEN = strlen("AO");
	do
	{
		DEBUG(2,"RF_Send_Data.Send_Buf = %s \r\n",RF_Send_Data.Send_Buf);
		///���ͣ�ִ����ɣ�ԭ���ݷ���
		User_send(UNCONFIRMED, RF_Send_Data.Send_Buf);	
	}while(!LoRapp_SenSor_States.loramac_evt_flag);
	
	/*******************���������ɱ�־*********************/
	__disable_irq();
	LoRapp_SenSor_States.loramac_evt_flag = 0;
	__enable_irq();
	LoRapp_SenSor_States.Tx_States = WATERAUTOPEN;
	ReportTimerEvent = true;
}

/*
 *OnWaterAutopen��ˮ���Զ����ƿ�����
 *������		  ��
 *����ֵ��		  ��
*/
void OnWaterAutopen(void)
{
	if(LoRapp_SenSor_States.Water_Control_State == OPEN) ///����ˮ����־λ
	{
		LoRapp_SenSor_States.Water_Control_State = WRITE;
		
		solen_vale_init(  );
		Ctrl_12V_PowerOn(  );
		solen_vale_open( );			
	}
		
	if(ReportTimerEvent)
	{
		DEBUG(2,"line = %d\r\n",__LINE__);
		//��ȡ�������� ��ȡ��ǰˮѹ�� ˮѹ��WaterSensorsData.temp[1]*1.6Pa/5v
		 SamplingData( WaterSensorsData.temp );
		
		 if(WaterSensorsData.temp[0] < 5) ///��������5%����ر�ˮ���豸�������������״̬
		 {
			solen_vale_close( );
			Ctrl_12V_PowerOff( ); 
			LoRapp_SenSor_States.Tx_States = RFBATHEART;
			
			return;
		 }
		 
		 DEBUG(2, "WaterSensorsData.pulsecount = %d\r\n",WaterSensorsData.pulsecount);
		 memset(RF_Send_Data.Send_Buf, 0, 8);
		 memcpy(RF_Send_Data.Send_Buf, &WaterSensorsData, sizeof(WaterSensorsData));   //MAC+PHY=56  MAC = 13 
		 RF_Send_Data.RX_LEN = 8;
		do
		{								
			///����ǰ�ж��Ƿ���յ��ر��������ǿ���˳�
			if(LoRapp_SenSor_States.Water_Control_State != CLOSE)
			{		
				User_send(UNCONFIRMED, RF_Send_Data.Send_Buf);
			}
			else
			{
				DEBUG(2, "brak\r\n");
				LoRapp_SenSor_States.loramac_evt_flag = 0;
				ReportTimerEvent = false;					
				break;
			}				
		}while(!LoRapp_SenSor_States.loramac_evt_flag);  

		/*******************���������ɱ�־*********************/
		__disable_irq();
		LoRapp_SenSor_States.loramac_evt_flag = 0;
		__enable_irq();
		
		/******************�����������ϱ�***********************/
		ReportTimerEvent = false;
		TimerStop( &ReportTimer );
		TimerSetValue( &ReportTimer, 16000000-LoRaCad.cad_all_time );
		TimerStart( &ReportTimer );	
		LoRaCad.cad_all_time = 0;
	}
}

/*
 *OnWaterAutClose��ˮ���Զ�ģʽ�رղ���
 *������		   ��
 *����ֵ��		   ��
*/
void OnWaterAutClose(void)
{
	if(LoRapp_SenSor_States.Water_Control_State == CLOSE) /// �رձ�־λ "AC"
	{	
		LoRapp_SenSor_States.Water_Control_State = WRITE;
		
		TimerStop( &CadTimer ); ///�ر�CAD�쳣����
	
		///������յ��ر�ָ��
		///�رյ�ŷ�+����ˮ�����������
		solen_vale_init(  );
		Ctrl_12V_PowerOn(  );
		Ctrl_12V_PowerOff( );

		WaterSensorsData.pulsecount = 0; 	
	}
	
	TimerStop( &ReportTimer ); ///�ر�ReportTimer����
	ReportTimerEvent = true;
	///���ͣ�ִ����ɣ�ԭ���ݷ���
	memset(RF_Send_Data.Send_Buf, 0, 6);
	memcpy(RF_Send_Data.Send_Buf, "AC", strlen("AC"));  ///�Զ��ر�
	
	RF_Send_Data.RX_LEN = strlen("AC");
	
	do
	{
		User_send(UNCONFIRMED, RF_Send_Data.Send_Buf);	
	}while(!LoRapp_SenSor_States.loramac_evt_flag);
/*******************���������ɱ�־*********************/
	__disable_irq();
	LoRapp_SenSor_States.loramac_evt_flag = 0;
	__enable_irq();		

	LoRapp_SenSor_States.Tx_States = RFREADY;
}

/*
 *WaterManOpen��ˮ���ֶ�ģʽ�򿪲���
 *������		��
 *����ֵ��		��
*/
void WaterManOpen(void)
{
	 TimerStop( &ReportTimer ); ///�ر�ReportTimer����
	 ReportTimerEvent = true;
	 memset(RF_Send_Data.Send_Buf, 0, strlen((char *)RF_Send_Data.Send_Buf));
   ///���ͣ�ִ����ɣ�ԭ���ݷ���
	 memcpy(RF_Send_Data.Send_Buf, "MO", strlen("MO")); 

	 WaterMOCounter(  );

	do
	{
		User_send(UNCONFIRMED, RF_Send_Data.Send_Buf);	
	}while(!LoRapp_SenSor_States.loramac_evt_flag);
	/*******************���������ɱ�־*********************/
	__disable_irq();
	LoRapp_SenSor_States.loramac_evt_flag = 0;
	__enable_irq();
	
	LoRapp_SenSor_States.Tx_States = RFREADY;
}

/*
 *WaterManClose��ˮ���ֶ�ģ�رղ���
 *������		 ��
 *����ֵ��		 ��
*/
void WaterManClose(void)
{
	TimerStop( &ReportTimer ); ///�ر�ReportTimer����
	ReportTimerEvent = true;
	memset(RF_Send_Data.Send_Buf, 0, strlen((char *)RF_Send_Data.Send_Buf));
	///���ͣ�ִ����ɣ�ԭ���ݷ���
	memcpy(RF_Send_Data.Send_Buf, "MC", strlen("MC")); 

	WaterMCounter(  );
	
	do
	{
		User_send(UNCONFIRMED, RF_Send_Data.Send_Buf);	
	}while(!LoRapp_SenSor_States.loramac_evt_flag);
	/*******************���������ɱ�־*********************/
	__disable_irq();
	LoRapp_SenSor_States.loramac_evt_flag = 0;
	__enable_irq();
	
	LoRapp_SenSor_States.Tx_States = RFREADY;
}
