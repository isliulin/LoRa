
	
1��Ӳ��IO�ӿ�ͼ��

								stm32L072CBT6
SX1278						 _ _ _ _ _ _ _ _ _ _ _ _ _ _ 
SPI     NSS	  --------	PA4 |					  		|
		SCK	  --------	PA5 |    				  		|
		MISO  --------	PA6 |					 		|
		MOSI  --------	PA7 |					  		|
						    |					  		|
EXti	DIO0  --------	PB1 |                    		|
		DIO1  --------	PB2 |					  		|
		DIO2  --------	PB10|					  		|
		DIO3  --------	PB11|					 		|
		DIO4  --------	NC	|					  		|
		DIO5  --------	NC	|					 		|
							|					  		|
CPIO	RESET --------	PB0 |					  		|
		LoRa_Power ---  PB12|					 		|
							|					 		|
							|					 		|
GPS	(UART2)					|					 		|	
		TX	  --------  PA2	|					  		|	
		RX	  --------  PA3	|					  		|	
GPS_Power_ON  --------  PB7	|					  		|									
							|					  		|
485	(UART5)					|					 		|	
		485_TX	------	PB3	|					  		|	
		485_RX	------	PB4	|					  		|	
		485_DE	------	PB5	|					  		|
		12V_ON	------  PA8	|					  		|	
							|					  		|
							|					 		|	
DEBUG(UART1)				|					  		|
		TX   ---------	PA9	|					  		|
		RX	 ---------  PA10|					  		|
							|					  		|
I2C							|					  		|
		I2C2_SDA ----- PB14	|					  		|
		I2C2_SCL ----- PB13	|					  		|
							|					  		|
��Դ����ʹ��  -------- PB9	|					  		|
							|					  		|
							|					  		|
							|					  		|
							|_ _ _ _ _ _ _ _ _ _ _ _ _ _|	




1: extern volatile uint8_t datarateע�͵���Ĭ��Datarate


3������RTC WKUP
    /**Enable the Alarm A 
    */
	
  RTC_AlarmTypeDef sAlarm;
	
  sAlarm.AlarmTime.Hours = 11;
  sAlarm.AlarmTime.Minutes = 20;
  sAlarm.AlarmTime.Seconds = 30;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }


4: �����ʱ϶���֡���������

δ��ɣ�
1��CAD�����ײ�Э�����
2��FLASH��д����
3��SX1278����IO���ö�ӦӲ���İ�

�����ԣ�
1��RTC�͹��Ļ��ѹ���
2�����л�����Ȳ���


.\Objects\stm32L073_mode.axf: Error: L6218E: Undefined symbol SX1276IoInit (referred from rtc-board.o).

void HAL_MspInit(void)��ȱʧ����RTC����RTC�������ѳ�������


right:
maxN = 123 payloadSize = 115 lenN = 115  fOptsLen = 0
maxN = 123 payloadSize = 5 lenN = 5  fOptsLen = 0
error:
maxN = 59 payloadSize = 117 lenN = 115  fOptsLen = 2
????:?ValidatePayloadLength()??????ADR datarate??????????????,??????

3.8.6:??:LORA_MAX_NB_CHANNELS???8
ScheduleTx( )-----> return SendFrameOnChannel( Channels[Channel] );  
Channel = LORA_MAX_NB_CHANNELS;
Channel = enabledChannels[randr( 0, nbEnabledChannels - 1 )];----->SetNextChannel( TimerTime_t* time )
??????? SetNextChannel( TimerTime_t* time )?????:Channel = Get_Flash_Datas.channels; ///????ID????????

3.8.7:?????????:
????????iqInverted,?????????iqInverted,?????iqInverted,?????????iqInverted?

??RX1??????:????,?? OnRadioRxDone()?????case FRAME_TYPE_DATA_CONFIRMED_UP:
NodeAckRequested = false;
OnRxWindow1TimerEvent(  );

LoRaMacStatus_t Send( LoRaMacHeader_t *macHdr, uint8_t fPort, void *fBuffer, uint16_t fBufferSize )

// Send now
Radio.Send( LoRaMacBuffer, LoRaMacBufferPktLen );

Cad_State = CadDetect

����RTC���Ѳ�����
1��alarmStructure.AlarmDateWeekDay = alarmTimer.CalendarDate.Date;   ///�� alarmStructure.AlarmDateWeekDay = 1ʱ alarmStructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;������ʧ��
	alarmStructure.AlarmMask = RTC_ALARMMASK_NONE; 		///���ѱ�־λ


ReceiveDelay1
RECEIVE_DELAY1

RTC MS��������������⣺�����������������ݣ���������ʱ����Ʋ��õ��£������ʱֻ����us������м������������㹻ʱ���ع�����


38. ����ATָ������POWER��DATARATE����������ʾ��

3.8.1������DATARATE��Χ��
  if( ValueInRange( datarate, LORAMAC_MIN_DATARATE, LORAMAC_MAX_DATARATE ) == false )
  LORAMAC_MIN_DATARATE����С���ټ����SF
  LORAMAC_MAX_DATARATE�������ټ���СSF
  
  LORAMAC_DEFAULT_DATARATE��Ĭ�����п���-----������ȷ��Ӧ������ʱ�����û���յ�Ӧ��������DATARATE�Զ�����
  ����SF�������⣺LORAMAC_MAX_DATARATE һ������Ϊ 5 ���SF�����ù̶�SFʱ LORAMAC_MIN_DATARATE = LORAMAC_DEFAULT_DATARATE�򰴹涨�����SF����
  ��ʹ��ADRʱ��������---���SF ��ѡ�����SF����
  LORAMAC_DEFAULT_DATARATE��Ĭ������Ϊ���SF��
  LORAMAC_DEFAULT_DATARATEд���ַΪ��0x0801F398
 
3.8.2������POWER��Χ��
  if( ValueInRange( txPower, LORAMAC_MAX_TX_POWER, LORAMAC_MIN_TX_POWER ) == false )
  LORAMAC_MAX_TX_POWER�����POWER��Χ
  LORAMAC_MIN_TX_POWER����СPOWER��Χ
  LORAMAC_DEFAULT_TX_POWER��Ĭ��ʹ�õ�POWER
 
  ����power:  
  SendFrameOnChannel ---  ChannelsTxPower = LimitTxPower( ChannelsTxPower ); txPower = TxPowers[ChannelsTxPower];
  static int8_t ChannelsTxPower = LORAMAC_DEFAULT_TX_POWER;

3.8.3: RXWIN:
MaxRxWindow = MAX_RX_WINDOW;

3.8.4���ظ�����������
 retry��if(retry<=0); retry=3;
 
3.8.5: �رս��մ��ڣ�OnRadioTxDone( void )������ɺ�򿪽��մ��ڣ��رմ��ڱ����ڸ÷��ͺ���������TimerStop( &RxWindowTimer1 ); TimerStop( &RxWindowTimer2 );
	   ����Թر���Ӧ�Ĵ���

�ְ��������ݺ󣬵����յ�ADRʱ������ʧ��ԭ�򣺸ú�����datarate���ģ��������ݰ����ȴ���
static bool ValidatePayloadLength( uint8_t lenN, int8_t datarate, uint8_t fOptsLen )
{
    uint16_t maxN = 0;
    uint16_t payloadSize = 0;

    // Get the maximum payload length
    if( RepeaterSupport == true )
    {
        maxN = MaxPayloadOfDatarateRepeater[datarate];
    }
    else
    {
        maxN = MaxPayloadOfDatarate[datarate];
    }

    // Calculate the resulting payload size
    payloadSize = ( lenN + fOptsLen );
		printf("lenN = %d  fOptsLen = %d\r\n",lenN, fOptsLen);

    // Validation of the application payload size
    if( ( payloadSize <= maxN ) && ( payloadSize <= LORAMAC_PHY_MAXPAYLOAD ) )
    {
        return true;
    }
    return false;
}	

right:
maxN = 123 payloadSize = 115 lenN = 115  fOptsLen = 0
maxN = 123 payloadSize = 5 lenN = 5  fOptsLen = 0
error:
maxN = 59 payloadSize = 117 lenN = 115  fOptsLen = 2
���ķ�ʽ����ValidatePayloadLength()�������յ���ADR datarate���ص����ͺ������ٴη������ݣ�����������

3.8.6��Ƶ�Σ�LORA_MAX_NB_CHANNELS����Ϊ8
ScheduleTx( )-----> return SendFrameOnChannel( Channels[Channel] );  
Channel = LORA_MAX_NB_CHANNELS;
Channel = enabledChannels[randr( 0, nbEnabledChannels - 1 )];----->SetNextChannel( TimerTime_t* time )
ʹ�ù̶�Ƶ���� SetNextChannel( TimerTime_t* time )��������ӣ�Channel = Get_Flash_Datas.channels; ///�̶��ŵ�ID����Զ�Ƶ��ͨ��

3.8.7�����ڽڵ��������ܣ�
��������ʹ����iqInverted,��ô���շ�ҲҪʹ��iqInverted�����䷽ʧ��iqInverted,��ô���շ�ҲҪʧ��iqInverted��

����RX1���ڼ���ģʽ��������ʾ��ͬʱOnRadioRxDone()���������case FRAME_TYPE_DATA_CONFIRMED_UP:
NodeAckRequested = false;
OnRxWindow1TimerEvent(  );

LoRaMacStatus_t Send( LoRaMacHeader_t *macHdr, uint8_t fPort, void *fBuffer, uint16_t fBufferSize )

// Send now
Radio.Send( LoRaMacBuffer, LoRaMacBufferPktLen );

Cad_State = CadDetect

BUG�б�
1��MCU����4MHZ���¹���Ƶ�ʴ������⣺

1.1��ϵͳʱ����С����ʱ��Ϊ100us����ʱ������ᵼ��ϵͳ��ʱ������MCU������

1.2������ʹ��ϵͳ��ʱ���������⣬��1MS��Ϊ��С��λ���ɣ�I2Cʹ�õ�US��ʱ���Բ���nop

1.3��OnMacStateCheckTimerEvent����ʧ��ԭ��

OnMacStateCheckTimerEvent

// Trig OnMacCheckTimerEvent call as soon as possible
TimerSetValue( &MacStateCheckTimer, 10000 );  //10ms
TimerStart( &MacStateCheckTimer );
MAC�㶨ʱ���ʱ�����Ϊ10MS��ԭ��ϵͳʱ��Ϊ2MHZ��1MS��ʱ�����ᵼ�½�������ʱ������MAC�����ж�

1.4��RTC�������ز��ȶ���

���1ϵ�г��ֵ����⣬��Ҫ��MCU clock����Ϊ4MHZ����

�͹������⣺
�����豸������MCU����ģʽ��CAD+����IO�رմ���+lower-power run mode


LC( 6 ) + LC( 7 ) + LC( 8 );


SetNextChannel
		|
		|
		|
		|
  Channel = enabledChannels[randr( 0, nbEnabledChannels - 1 )]; ///����ǰ�����ŵ�
  
  �ŵ�����ƹ涨�ŵ�ID�ߣ�( LC( 1 ) + LC( 2 ) + LC( 3 ) );�������ŵ�ҲӦ���ָ�ģʽ1 2 3�����
 Radio.SetTxConfig


�������⣺
1������cad������쳣����timer��ʱƵ�ʹ��ߵ��²�������MCU������

2��spi����ΪDMAģʽ������MCU���

3: TimerSetValue( &MacStateCheckTimer, 1000 ); //1ms����ֽ������⣺MCUʱ�ӹ��ͣ�����������Ҫ��Ӧ���ʱ�䣬�ȴ�һ��ʱ������ɣ����߸���Ϊ10ms

4�������豸��Ҫ����쳣�������ƣ���ֹ��ʱ��ûͨ�ţ��Ͽ�����


SX1276SetTxConfig

5.RTCʱ�������

#define RTC_ALARM_TIME_BASE                             122.07  //122.07 1000 //

RtcHandle.Init.AsynchPrediv = 1;
RtcHandle.Init.SynchPrediv = 1; 
����1S��Ƶ����: 32.768/(AsynchPrediv+1)/(SynchPrediv+1) = 8.192  
				1/8.192 = 0.12207ms = 122.07us
RTC��Ƶ�ϵͣ���MCU������MSIģʽʱRTC���׳������⣬��˽��з�Ƶ�������£�ͬ��ʵ��1S������


#define RTC_ALARM_TIME_BASE                             244.14 /// 122.07  //122.07 1000000 //

RtcHandle.Init.AsynchPrediv = 3; //3; 127
RtcHandle.Init.SynchPrediv = 1; ///3; 255

����1S��Ƶ����: 32.768/(AsynchPrediv+1)/(SynchPrediv+1) = 4.096 
				1/4.096 = 0.24414ms = 244.14us
				
6�����ݸ�ʽ���·�����ˮ����Ҫ��֡���ݣ�һ֡���ݻ���: RF_Send_Data.Send_Buf = "RADIO"; 0x52 0x41 0x44 0x49 0x4f
һ֡��Ч����: open: 0x01 0xb1   close : 0x02 0xb1
									   
   ��Ҫ���ֺû�������֡����Ч����֡�����ڻ�������֡���ݽ��յ����ҽ�����ɣ����˳����߽�����ձ���ȷ������Ч����֡ʱ���ϱ�����
   
   
 7���������̣�
 ���յ���������֡+��Ч����֡ (Ӧ�����������������ڹ涨ʱ����û���յ�Ӧ�����ظ�����)---- > ÿ�����ϱ�һ������ ----- > ���յ�ֹͣ����֡ ----- > ����CAD mode
   
524144494f

���� HardFault_Handler error ���⣺һ�㶼������Խ�����ָ��û����ռ����Ұָ�� 
#include "rs485.h"
#pragma pack(1)
typedef struct rs485Command
{
	uint8_t* data;				//�������Լ���������ָ�룬���ݷ���ʱ�����Ǵ���������
	uint8_t data_len;			//���ݳ���
	uint8_t recive_len;			//�����������ݳ���
	uint16_t recive_timeout;	//���ճ�ʱʱ��
}Rs485Command;
���� HardFault_Handler error ---->  #pragma pack(1) ��Ϊ #pragma pack(2)


8��flash����SF 
 ChannelsDefaultDatarate = ChannelsDatarate = LORAMAC_DEFAULT_DATARATE;
 
�����λ��
__set_FAULTMASK(uint32_t faultMask);

HAL_NVIC_SystemReset( );

��ѯCO2ʱ��Ҫ����ʱ5S
HAL_Delay(device.delay);							//ĳЩ�豸��Ҫ������ʱ			

/*!
 * Current channel index
 */
uint8_t Channel;

����Ĭ��datarate���������ADR/����Ӧ����������Զ������ź�ǿ�ȸ���datarate


