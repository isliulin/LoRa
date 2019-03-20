/*
 * user_sim.h	simģ������ͷ�ļ�
*/

#ifndef __USER_SIM_H__
#define __USER_SIM_H__

#include "stm32l0xx_hal.h"

#define SIM_RECIEVE_COUNT_MAX	256

typedef enum AT_CMD
{
	AT_CMD_CLOSE_ECHO 			=0,				// �رջ���	  
	AT_CMD_WAIT_INIT,							// �ȴ�CALLREADY״̬��call ready����ģ��׼������
	AT_CMD_CONNECT_SERVER,						// ���ӷ�����
	AT_CMD_START_TRAN,							// �������ݴ���״̬
	AT_CMD_SEND_DATA,							// ��ʼ��������
	AT_CMD_CLOSE_CONNECTION,					// �رշ���������
	AT_CMD_CHECK_CONNECT,						// ��ѯ����״̬
	AT_CMD_SET_CLASS,							// ���ù���ģʽΪ
    
	AT_CMD_USER									//�û��Զ������һ��Ҫ�������
}AtCmd;

typedef enum AT_STATE	//ATָ���ִ�з���״̬
{
	AT_NONE,	//�޷���
	AT_OK,		//ִ�гɹ�
	AT_ERROR,	//ִ�д���
	AT_FAIL		//ִ��ʧ��(��ʱʧ��)
}AtState;

typedef struct AT_CMD_STRUCT
{
	char *Command;				//ָ���ַ���
	char *ExpectReply;			//�������յ����ַ���
	uint32_t WaitTime;			//��ʱʱ��
	uint16_t RetryCount;		//���Դ���
	AtState AtRlplyState;		//����ִ�з��ص�״̬
}AtCmdStruct;

/*
 * SimInit:				��ʼ��simģ��
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimInit(void);

/*
 * SimPowerOn:			��simģ���Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimPowerOn(void);

/*
 * SimPowerOff:			�ر�simģ���Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimPowerOff(void);

/*
 * SimSendAtCmd:	����Atָ���Simģ��
 * cmd:				Atָ�����ɼ�AtCmd�Ķ���
 * ����ֵ:			1�ɹ� 0ʧ��
*/
uint8_t SimSendAtCmd(AtCmd cmd);

/*
 * SimCheckReply:	���Simģ��Ļظ�״̬����Ҫ�����޸�AtCmdStruct�ṹ���AtRlplyState
 * ����:			��
 * ����ֵ:			��
*/
void SimCheckReply(void);

/*
 * SimConnectServer:	��ʼ��simģ��
 * ServerIP:			������IP��ַ
 * ServerPort:			�������˿�
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimConnectServer(char *ServerIP,char *ServerPort);

/*
 * SimExecuteCmd:		ִ�����úõ�ATָ��
 * cmd:					���úõ�ATָ��
 * ����ֵ:				�ɹ�����AT_OK�������AtState����
*/
AtState SimExecuteCmd(AtCmd cmd);
/*
 * SimSendData:			�������ݵ������ӵ�IP��ַ
 * data:				Ҫ���͵�����
 * ServerPort:			�������˿�
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimSendData(char *data);

/*
 * SimGetIccid:			��ȡSim���ĵ绰����
 * ����PhoneNum:		�绰����
 * ����ֵ:				�绰����
*/
char * SimGetIccid(char *PhoneNum);

uint8_t SimHttpInit(void);
uint8_t SimHttpSetUrl(char *url);
uint8_t SimHttpSetBreak(uint32_t BreakPoint);
uint8_t SimHttpSetBreakEnd(uint32_t BreakEnd);
uint8_t SimHttpGet(void);
uint8_t SimHttpReadData(void);


void Lpuart1Send(char *p, uint16_t len);
#endif /* __USER_SIM_H__ */
