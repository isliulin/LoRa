
	
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
TimerSetValue( &MacStateCheckTimer, 10 );  //10ms
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

3: TimerSetValue( &MacStateCheckTimer, 1 ); //1ms����ֽ������⣺MCUʱ�ӹ��ͣ�����������Ҫ��Ӧ���ʱ�䣬�ȴ�һ��ʱ������ɣ����߸���Ϊ10ms

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

PROCESS_YIELD_UNTIL(RF_Tx_Done == s_tRFResult || RF_Tx_Timeout == s_tRFResult);
process_post(&NetworkProcess, NETWORK_EVENT_TX_RF_PACKAGE, NULL);          if (NETWORK_EVENT_TX_RF_PACKAGE == ev) /* post by Comm2Trm-Process */
process_poll(&NetworkProcess);


�����

1��OnRadioRxDone

RxWindow1Delay = ReceiveDelay1 - RADIO_WAKEUP_TIME;
RxWindow2Delay = ReceiveDelay2 - RADIO_WAKEUP_TIME;

���ý��մ���ʱ�䣺 OnRadioTxDone( void )----> TimerSetValue( &RxWindowTimer1, RxWindow1Delay ); TimerStart( &RxWindowTimer1 );

2���ŵ���
/*!
 * Current channel index
 */
uint8_t Channel;

����Ĭ��datarate���������ADR/����Ӧ����������Զ������ź�ǿ�ȸ���datarate

sync word��ֻ���FSK��Ч

RegSyncConfig��RegSyncValue1�Ĵ�����������   REG_SYNCCONFIG

RxTimeoutTimer --- SX1276OnTimeoutIrq


if(LoRaCad.Iq_Invert) ///???

3��P2PͨѶ���ã�
3.1������IQ��ʹ�ܣ�LoRaCad.Iq_Invert

3.2�����մ������ã���ΪRX1 RX2����ģʽѡ��
	 RX2 : ��ʹ��RX2��ֱ��LoRaMacSetDeviceClass( CLASS_C );������������գ�����Ҫע�ⳤ�ڴ���RX mode ������쳣����Ҫ����ʱ�л�״̬��
	 RX1 ����ʹ��RX1����ҪOnRxWindow1TimerEvent()������ 
	 RxWindowSetup( (Channels[Channel].Frequency), datarate, bandwidth, symbTimeout, false )��Ϊ��������ģʽ
	 RxWindowSetup( (Channels[Channel].Frequency), datarate, bandwidth, symbTimeout, true )
	 
	 ͬʱ���ģ�
	 OnRadioRxDone ��if( LoRaMacDeviceClass != CLASS_C )  Radio.Sleep( );ע�͵�����ģ��ֻ����һ�κ������ߣ����ٽ�������

���մ�������һ��ԭ�� SX1276OnDio0Irq ----->     if( SX1276.Settings.Fsk.RxContinuous == false )
                        {
                            SX1276.Settings.State = RF_IDLE; ��־λ����
                            TimerStart( &RxTimeoutSyncWord );
                        }
3.3��DOWN_LINK��������Ϊ0�� ����MIC�������������ú����ٽ��մ���������
	 UpLinkCounter
	 
	
3.4����������

downLinkCounterĬ��Ϊ0
sequenceCounterPrev = ( uint16_t )downLinkCounter; ///�����ϴν��յ�������֡��
sequenceCounterDiff = ( sequenceCounter - sequenceCounterPrev );  ///��¼������

3.5���رս��մ���
IsRxWindowsEnabled = false;
OnRadioTxDone�н�Radio.Sleep( );ע�͵�����RX1������

3.6����Э��Ķ����ٳ���������RX2��������
ʹ��IQ��LoRaCad.Iq_Invert
IsRxWindowsEnabled = false;
����RX2�ŵ�#define RX_WND_2_CHANNEL                                  { 472100000, DR_1 }  ///505.3  471300000

3.7��while( curMulticastParams != NULL )��ȡ����curMulticastParams=NULLԭ��MulticastChannelsָ��û�����ڴ棬���¿��Խ����
MulticastChannels = (MulticastParams_t *)malloc(sizeof(MulticastChannels));����Ϊȫ�ַ�����ظ���ʼ������


3.8��OnRadioRxDone  ����fCtrl->Bits.FOptsLen�쳣ԭ�򣺶���ն��豸ͬʱ�������ݣ����ݼ�������ײ����������OnRadioRxError�������ʽ��
 OnRadioRxError����֡���ݷ�����ײʱ����֣� Radio.Standby( ) --->  OnRxWindow2TimerEvent
���ý��ա�����ģʽ����������ΪRadio.Standby( ); or Radio.Sleep( );������Ч


3.9���ô���汾�����ݴ�����ԭ��
3.9.1�����͡�����ǰ���볤�Ȳ�һ��
Radio.SetTxConfig( MODEM_LORA, txPower, 0, 0, datarate, 1, 20, false, true, 0, 0, false, 3e3 ); ///ǰ��������Ϊ20

3.9.2��class Aģʽ�½��մ������������ݳ���ԭ��rx1��ͬ��������RX2�ſ��Խ��մ������������ݣ���˴�С���ز��ܼ���

3.10��CLASS Aģʽ����С��������ʧ��ԭ��ʱ��Ƶ�ʴ��ڲ��쵼�½���ʧ�ܣ������ʽ������
#define RECEIVE_DELAY2                              2200
#define MAX_RX_WINDOW                               3200
SX1276SetRx ---- ����Ϊ SX1276SetOpMode( RFLR_OPMODE_RECEIVER );����1.2S��������       
if( rxContinuous == true )
{
    SX1276SetOpMode( RFLR_OPMODE_RECEIVER );
}
else
{
    SX1276SetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
}



#pragma pack(2)

.\Objects\LoRaWAN_Pro.axf: Error: L6218E: Undefined symbol User_send (referred from gps.o).

LORA_MAX_NB_CHANNELS

    MaxRxWindow = MAX_RX_WINDOW;
    ReceiveDelay1 = RECEIVE_DELAY1;
    ReceiveDelay2 = RECEIVE_DELAY2;
    JoinAcceptDelay1 = JOIN_ACCEPT_DELAY1;
    JoinAcceptDelay2 = JOIN_ACCEPT_DELAY2;

RxWindow2Delay


            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr ) & 0xFF;
            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr >> 8 ) & 0xFF;
            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr >> 16 ) & 0xFF;
            LoRaMacBuffer[pktHeaderLen++] = ( LoRaMacDevAddr >> 24 ) & 0xFF;
            
LoRapp_SenSor_States.loramac_evt_flag = 1;�жϵ���            
            

OnMacStateCheckTimerEvent���״̬�����


OnRxWindow1TimerEvent(  );

OnAckTimeoutTimerEvent


AckTimeoutRetry = true;
LoRaMacState &= ~MAC_ACK_REQ;



LoRaMacState &= ~MAC_TX_RUNNING;

NodeAckRequested = false;
McpsConfirm.AckReceived = false;
McpsConfirm.NbRetries = AckTimeoutRetriesCounter;
if( IsUpLinkCounterFixed == false )
{
UpLinkCounter++;
}


AckTimeoutRetry = false;
NodeAckRequested = false;
if( IsUpLinkCounterFixed == false )
{
    UpLinkCounter++;
}
McpsConfirm.NbRetries = AckTimeoutRetriesCounter;



.\Objects\LoRaWAN_Pro.axf: Error: L6218E: Undefined symbol InitSI7021 (referred from si7021.o).