/*
**************************************************************************************************************
*	@file	user-app.h
*	@author Ysheng
*	@version 
*	@date    
*	@brief	Ӧ�ò�ͷ�ļ�������MAC��
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_APP_H
#define __USER_APP_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include <stdio.h>
#include <stdint.h>

#define AT_MODULE_PORT							(10)    ///FPort

#define AESKEY_LEN                  (16)

///128KB==0X1F400    										0x0800000 ---- 0x0801F400
#define DEV_ADDR                        0x0801F3E8        ////UID 0x0801F3EC ---- 0x0801F3FC
#define DEV_ADDR_SIZE                   0x12

#define Rx2DATARATE_ADDR								0x0801F3E0  		///����RX2���� 0x0801F3E0 ---- 0x0801F3E1

#define RX2FREQ_ADDR     							  0x0801F3DC     ///����RX2 FREQ
#define RX2FREQ_ADDR_SIZE								0x02

#define LORAMAC_DEFAULT_DATARATE        0x0801F3D8       ///DR_1��Ĭ�Ͽ��� SF

#define SET_ADR_ADDR                    0x0801F3D4	    ///ADR


/*!
 * When set to 1 the application uses the Over-the-Air activation procedure
 * When set to 0 the application uses the Personalization activation procedure
 */
#define OVER_THE_AIR_ACTIVATION                     0

/*!
 * Application IEEE EUI (big endian)
 */
#define LORAWAN_APPLICATION_EUI                     { 0x12,0x34,0x56,0x78,0x90,0xAB,0xCD,0xEF }


/*!
 * AES encryption/decryption cipher application key
 */
#define LORAWAN_APPLICATION_KEY                     { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }

// report status in period: �����ڵ�ǰ״̬
typedef enum{
    SYS_STA_IDLE,     ///����  
    SYS_STA_TX,
    SYS_STA_WAIT,
}sys_sta_t;

extern volatile bool ReportTimerEvent;

extern TimerEvent_t ReportTimer;

extern TimerEvent_t SleepTimer;
extern volatile bool SleepTimerEvent;
extern void OnsleepTimerEvent( void );

extern uint32_t LoRaHeartTimer;

extern TimerEvent_t HEARTimer;
void OnHearTimerEvent( void );

typedef enum{
	IDLE,
	BUSY,
	DONE,
}LoRaStatus_t;

typedef enum{
	CONFIRMED,
	UNCONFIRMED
}LoRaFrameType_t;

typedef enum{
	INVALID,
	RECEIVED,
	UNRECEIVED,
}LoRaTxAckReceived_t;

typedef enum{
    LORAMAC_NWKSKEY=0,
    LORAMAC_APPSKEY=1,
    LORAMAC_APPKEY=2,
}LoRaMacKey_t;

typedef struct{
    int16_t rssi;
    uint8_t snr;
    uint8_t win;
    uint8_t port;
    uint16_t size;
    uint8_t buf[256];
}LoRaMacRxInfo;

typedef enum{
    MAC_STA_TXDONE,
    MAC_STA_RXDONE,
    MAC_STA_RXTIMEOUT,
    MAC_STA_ACK_RECEIVED,
    MAC_STA_ACK_UNRECEIVED,
    MAC_STA_CMD_JOINACCEPT,
    MAC_STA_CMD_RECEIVED,
}mac_evt_t;

typedef enum Rx_State{
	RFWRITE = 0, ///�ȴ�״̬
	RFBATHEART,    ///����������
	RADIO,       ///����������״̬��һ֡���ݻ�����������������ݣ������ٴν�������
	RFACK,     ///��������״̬
	WATERAUTOPEN,        ///�����豸�ϱ�
	WATERAUTCLOSE,			 ///�ر��豸
	WATERMANOPEN, ///ˮ���ֶ���
	WATERMANCLOSE, ///ˮ���ֶ���
	RFREADY,     ///׼��״̬,�л�λ�ȴ�
}Tx_State_t;

extern Tx_State_t SaveTxState;

typedef enum Water_Control{
	WRITE = 0,
	OPEN,
	CLOSE,
}Water_Control_t;


/*
 *	Ӧ�ò�״̬��־λ
 */
typedef struct{
	bool 							WKUP_State; ///�͹��ı�־
	Water_Control_t 	Water_Control_State;  ///ˮ��״̬
	uint8_t 					loramac_evt_flag;          ///lora����״̬λ
	Tx_State_t 				Tx_States;     ///��Ƶ����״̬
	uint32_t          Work_Time;  ///��¼����ʱ�䣬������GPS�������ʱ��
	uint32_t          WaterMOCounter;
	uint32_t          WaterMCounter;
}LoRapp_State;

/*
*RF���Ͱ�����
*/
typedef struct{
	uint16_t  RX_LEN ;
	uint16_t  Len;	///RF_BUF�����±�
	uint16_t 	TX_Len;	///�������ݳ���
	uint8_t   AT_PORT; ///FPort
	uint8_t 	RF_BUF[20];
	uint8_t 	Send_Buf[20];
	uint8_t 	Error_count;   ///RF����ERROR����
	uint8_t		ADR_Datarate;   ///��ȡ��ADRʱSF����
  int8_t    default_datarate;  ///Ĭ��datarate
	bool 			RF_Send;			///RF���ݷ�����ɱ��
	bool		  Send_again;	///RF����ʧ��ʱ,��ֹ���»�ȡ����
}RF_Send;


extern RF_Send RF_Send_Data;


/*****************��ȡflash��ȡADR״̬********************/

typedef struct{
    bool LoRaMacSetAdrOnState;
    char ReadSetAdar_Addr;
    uint8_t datarate;
		int16_t stop_time;   ///����ʱ��Ĭ��24S
	  uint8_t min_datarate;
	  uint8_t max_datarate;
		uint8_t channels;
		uint8_t sync_time;
}Get_Flash_Data;

extern Get_Flash_Data Get_Flash_Datas;  ///��ȡflash����

extern LoRapp_State LoRapp_SenSor_States;

typedef void (*mac_callback_t) (mac_evt_t mac_evt, void *msg);

int PowerXY(int x, int y);

int Convert16To10(int number);

uint32_t Read_DecNumber(char *str);

void String_Conversion(char *str, uint8_t *src, uint8_t len);

uint8_t Read_Flash_Data(void);

void user_app_init(mac_callback_t mac);

int user_app_send( LoRaFrameType_t frametype, uint8_t *buf, int size, int retry);

uint32_t app_get_devaddr(void);

void Into_Low_Power(void);

void Send_Led(void);

void Rx_Led(void);

void OnReportTimerEvent( void );

void SX1278_Send(uint8_t LoRaFrameTypes,uint8_t *send_buf);

void User_send(uint8_t LoRaFrameTypes,uint8_t *Send_Buf);

void Irrigate_Control(void);


#endif /* __USER_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
