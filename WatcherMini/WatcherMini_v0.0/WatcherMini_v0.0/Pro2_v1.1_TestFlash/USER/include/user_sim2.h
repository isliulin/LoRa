/*
 * user_sim.h	simģ������ͷ�ļ�
*/

#ifndef __USER_SIM2_H__
#define __USER_SIM2_H__

#include "user_config.h"

#define SIM_RECIEVE_COUNT_MAX	128

typedef enum AT_CMD
{
//	AT_CMD_CLOSE_ECHO 			=0,				// �رջ���	  
//	AT_CMD_WAIT_INIT,							// �ȴ�CALLREADY״̬��call ready����ģ��׼������
//	AT_CMD_CONNECT_SERVER,						// ���ӷ�����
//	AT_CMD_START_TRAN,							// �������ݴ���״̬
//	AT_CMD_SEND_DATA,							// ��ʼ��������
//	AT_CMD_CLOSE_CONNECTION,					// �رշ���������
//	AT_CMD_CHECK_CONNECT,						// ��ѯ����״̬
//	AT_CMD_SET_CLASS,							// ���ù���ģʽ
//	AT_CMD_SET_BAUD,							// ���ò�����
//	AT_CMD_CARD_INSERT,                         // ���SIM���Ƿ��в���
    
	AT_CMD_USER									//�û��Զ������һ��Ҫ�������
}E_AtCmd;

typedef enum AT_STATE	//ATָ���ִ�з���״̬
{
	AT_NONE=0,	    //�޷���
	AT_OK,		    //�ظ��ɹ�
	AT_ERROR,	    //�ظ�����
	AT_FAIL		    //ִ��ʧ��(��ʱʧ��)
}E_AtState;

typedef struct AT_CMD_STRUCT
{
	char *Command;				//ָ���ַ���
	char *ExpectReply;			//�������յ����ַ���
	uint32_t WaitTime;			//��ʱʱ��
	uint16_t RetryCount;		//���Դ���
	E_AtState AtRlplyState;		//����ִ�з��ص�״̬
}T_AtCmdStruct;

typedef enum            	    //SIMģ��״̬
{
    E_SIM_INIT      =(0x01U<<0),	// ��ʼ��
	E_SIM_CONNECT	=(0x01U<<1),	// ����������
	E_SIM_DISCONNECT=(0x01U<<2),	// δ����������
	E_SIM_ERROR	    =(0x01U<<3)		// ģ��Ӳ���������ͨѶʧ�ܣ�SIM��δ���룩
}E_SimState;

uint8_t SimInit(void);

uint8_t SimSetBaud(uint32_t Baud);

uint8_t SimCheckModle(void);

E_SimState SimGetState(void);

uint8_t SimCloseConnect(void);

uint8_t SimPowerOn(void);

uint8_t SimPowerOff(void);

uint8_t SimSendAtCmd(E_AtCmd cmd);

void SimCheckReply(void);

char * SimGetImei(char *ImeiNum);

char * SimGetIccid(char *PhoneNum);

uint8_t SimConnectServer(char *ServerIP,char *ServerPort);

E_AtState SimExecuteCmd(E_AtCmd cmd);

uint8_t SimSendData(char *data,uint16_t len);

uint8_t SimEntertSleepMode(uint8_t Mode);

uint8_t SimIsConnectServer(void);

uint8_t SimQSendInit(void);

uint8_t SimQSendData(char *data,uint16_t len);

uint8_t SimReConnect(uint32_t TimeOut);

uint8_t SimConnectHeartbeat(void);

uint8_t SimGetCSQ(uint8_t *Csq);

/*
 * SimRxServerMsgCallback,�������������ݴ�����
 *
 * �˺���ֻ�ڴ˴����壬���û�ʵ�־��崦��
*/
void SimRxSvMsgCallback(char *RxMsg,uint16_t len);

#endif /* __USER_SIM2_H__ */
