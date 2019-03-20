/*
 * user_sim.c	simģ�������ļ�
*/

#include "user_sim.h"
#include <string.h>
#include "bootloader_config.h"
#include "user_flash_L072.h"

#include "iap_protocol.h"

extern UART_HandleTypeDef hlpuart1;

char SimReChar[1]={0};								//���յ�һ���ֽ�
char SimReBuff[SIM_RECIEVE_COUNT_MAX]={0};			//���յ�������
uint16_t SimReCount=0;								//���յ����ֽ���

volatile uint8_t G_IsGetData=0;

AtCmdStruct at_cmd[]=
{
	/*ָ���ַ���				�������յ����ַ���		ÿ�γ�ʱʱ��	���Դ���	����ִ�з��صĳ�ʼ״̬}*/
	{"ATE0\r\n",			"OK",					1000,			10,			AT_NONE},	//�رջ��ԣ�����ִ��10��ÿ�εȴ�1s,��ʼ�ķ���״̬ΪAT_NONE,�ɹ�����AT_OK
	{"AT+CIURC=1\r\n",		"SMS Ready",			120000,			1,			AT_NONE},	//�ȴ�ģ��call ready���ȴ�120s
	{NULL,					"CONNECT OK",			20000,			1,			AT_NONE},	//���ӷ�����,������������û�ָ��
	{"AT+CIPSEND\r\n",		">",					2000,			1,			AT_NONE},	//���뷢������״̬
	{NULL,					"SEND OK",				30000,			1,			AT_NONE},	//ͨ���Ѿ����������ӷ������ݣ����ݼ����ȡ���ʱʱ���ɳ�������趨
	{"AT+CIPSHUT\r\n",		"SHUT OK",				2000,			1,			AT_NONE},	//�ر�����
	{"AT+CIPSTATUS\r\n",	"CONNECT OK",			2000,			1,			AT_NONE},	//��ѯTCP����״̬
	{"AT+CGCLASS=\"CC\"\r\n","OK",					2000,			1,			AT_NONE},	//���ù���ģʽΪCC
    
	{NULL,				NULL,					0,				0,			AT_NONE}	//�û��Զ������һ��Ҫ�������
};

/*
 * SimPowerOn:			��simģ���Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimPowerOn(void)
{
	HAL_GPIO_WritePin(Out_SIM_Power_ON_GPIO_Port, Out_SIM_Power_ON_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Out_SIM_EN_GPIO_Port, Out_SIM_EN_Pin, GPIO_PIN_SET);
	return 1;
}

/*
 * SimPowerOff:			�ر�simģ���Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimPowerOff(void)
{
	HAL_GPIO_WritePin(Out_SIM_EN_GPIO_Port, Out_SIM_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Out_SIM_Power_ON_GPIO_Port, Out_SIM_Power_ON_Pin, GPIO_PIN_RESET);
	return 1;
}

/*
 * SimSendAtCmd:	����Atָ���Simģ��
 * cmd:				Atָ�����ɼ�AtCmd�Ķ���
 * ����ֵ:			1�ɹ� 0ʧ��
*/
uint8_t SimSendAtCmd(AtCmd cmd)
{
    HAL_NVIC_DisableIRQ(RNG_LPUART1_IRQn);
	if(HAL_UART_Transmit(&hlpuart1,(uint8_t *)at_cmd[cmd].Command,strlen(at_cmd[cmd].Command),0xffff)!=HAL_OK)
    {
        HAL_NVIC_EnableIRQ(RNG_LPUART1_IRQn);
        printf("����ʧ��\r\n");
		return 0;
    }
    HAL_NVIC_EnableIRQ(RNG_LPUART1_IRQn);
    return 1;
}

AtState SimExecuteCmd(AtCmd cmd)
{
	at_cmd[cmd].AtRlplyState=AT_NONE;
	for(uint8_t i=0;i<at_cmd[cmd].RetryCount;i++)
	{
		printf("%s",at_cmd[cmd].Command);//,strlen(at_cmd[cmd].Command));
		uint32_t StartTime=HAL_GetTick();
		if(SimSendAtCmd(cmd)!=1)
			return AT_ERROR;
		while(at_cmd[cmd].AtRlplyState==AT_NONE && HAL_GetTick()-StartTime<at_cmd[cmd].WaitTime);
		if(at_cmd[cmd].AtRlplyState==AT_OK)
			break;
	}
	if(at_cmd[cmd].AtRlplyState==AT_NONE)
		return AT_FAIL;
	HAL_Delay(100);//��ʱ��ֹģ�鷴Ӧ������
    if(cmd==AT_CMD_USER)
    {
        at_cmd[AT_CMD_USER].Command=NULL;
	}
    return at_cmd[cmd].AtRlplyState;
}

uint8_t SimHttpInit(void)
{
	at_cmd[AT_CMD_USER].Command="AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n";//���ó��أ���������
	at_cmd[AT_CMD_USER].ExpectReply="OK";
	at_cmd[AT_CMD_USER].WaitTime=1000;
	at_cmd[AT_CMD_USER].RetryCount=1;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	
//	at_cmd[AT_CMD_USER].Command="AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n";//����APN�������ñ�ʾʹ��Ĭ�ϵ�APN
//	at_cmd[AT_CMD_USER].ExpectReply="OK";
//	at_cmd[AT_CMD_USER].WaitTime=1000;
//	at_cmd[AT_CMD_USER].RetryCount=1;
//	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
//		return 0;
	
	at_cmd[AT_CMD_USER].Command="AT+SAPBR=1,1\r\n";//����GPRS�����뼤��
	at_cmd[AT_CMD_USER].ExpectReply="OK";
	at_cmd[AT_CMD_USER].WaitTime=30000;
	at_cmd[AT_CMD_USER].RetryCount=1;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;

	at_cmd[AT_CMD_USER].Command="AT+HTTPINIT\r\n";
	at_cmd[AT_CMD_USER].ExpectReply="OK";
	at_cmd[AT_CMD_USER].WaitTime=1000;
	at_cmd[AT_CMD_USER].RetryCount=1;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	
//	at_cmd[AT_CMD_USER].Command="AT+HTTPPARA=\"CID\",1\r\n";	//�ɲ����ã�������֮���п��ܵ����豸����ʧ��
//	at_cmd[AT_CMD_USER].ExpectReply="OK";
//	at_cmd[AT_CMD_USER].WaitTime=1000;
//	at_cmd[AT_CMD_USER].RetryCount=1;
//	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
//		return 0;
	return 1;
}

uint8_t SimHttpSetUrl(char *url)
{
	char command[128]={0};
	sprintf(command,"AT+HTTPPARA=\"URL\",\"%s\"\r\n",url);
	at_cmd[AT_CMD_USER].Command=command;
	at_cmd[AT_CMD_USER].ExpectReply="OK";
	at_cmd[AT_CMD_USER].WaitTime=1000;
	at_cmd[AT_CMD_USER].RetryCount=1;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	return 1;
}

uint8_t SimHttpSetBreak(uint32_t BreakPoint)
{
	char command[64]={0};
	sprintf(command,"AT+HTTPPARA=\"BREAK\",%u\r\n",BreakPoint);
	at_cmd[AT_CMD_USER].Command=command;
	at_cmd[AT_CMD_USER].ExpectReply="OK";
	at_cmd[AT_CMD_USER].WaitTime=1000;
	at_cmd[AT_CMD_USER].RetryCount=1;
    G_IsGetData=0;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	return 1;
}

uint8_t SimHttpSetBreakEnd(uint32_t BreakEnd)
{
	char command[64]={0};
	sprintf(command,"AT+HTTPPARA=\"BREAKEND\",%u\r\n",BreakEnd);
	at_cmd[AT_CMD_USER].Command=command;
	at_cmd[AT_CMD_USER].ExpectReply="OK";
	at_cmd[AT_CMD_USER].WaitTime=1000;
	at_cmd[AT_CMD_USER].RetryCount=1;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	return 1;
}

uint8_t SimHttpGet(void)
{
	at_cmd[AT_CMD_USER].Command="AT+HTTPACTION=0\r\n";
	at_cmd[AT_CMD_USER].ExpectReply="+HTTPACTION:";
	at_cmd[AT_CMD_USER].WaitTime=60000;
	at_cmd[AT_CMD_USER].RetryCount=1;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	return 1;
}

uint8_t SimHttpReadData(void)
{
	at_cmd[AT_CMD_USER].Command="AT+HTTPREAD\r\n";
	at_cmd[AT_CMD_USER].ExpectReply="+HTTPREAD:";
	at_cmd[AT_CMD_USER].WaitTime=30000;
	at_cmd[AT_CMD_USER].RetryCount=1;
	G_IsGetData=1;                                  //��ʼ������������
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;

	return 1;
}
/*
 * SimInit:				��ʼ��simģ�飬ģ�����
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimInit(void)
{
    G_IsGetData=0;

	if(SimPowerOn()==0)
		return 0;
	if(HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)SimReChar,1)!=HAL_OK)//�򿪴��ڽ����ж�
		return 0;
	if(SimExecuteCmd(AT_CMD_CLOSE_ECHO)!=AT_OK)
		return 0;
	if(SimExecuteCmd(AT_CMD_WAIT_INIT)!=AT_OK)
		return 0;
	if(SimExecuteCmd(AT_CMD_SET_CLASS)!=AT_OK)
		return 0;
    if(SimHttpInit()!=1)
        return 0;
    return 1;
}

/*
 * SimConnectServer:	��ʼ��simģ��
 * ServerIP:			������IP��ַ
 * ServerPort:			�������˿�
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimConnectServer(char *ServerIP,char *ServerPort)
{
	char command[64];
	sprintf(command,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",ServerIP,ServerPort);
	//����д���Ǵ���ģ���Ȼ�﷨��û������
	//sprintf(at_cmd[AT_CMD_CONNECT_SERVER].Command,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",ServerIP,ServerPort);
	at_cmd[AT_CMD_CONNECT_SERVER].Command=command;
	for(uint8_t i=0;i<2;i++)//����2�Σ�ʧ��������
		if(SimExecuteCmd(AT_CMD_CONNECT_SERVER)==AT_OK)
			return 1;
	return 0;
}

/*
 * SimSendData:			�������ݵ������ӵ�IP��ַ
 * data:				Ҫ���͵�����
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimSendData(char *data)
{
	char temp[256]={0};
	uint8_t len;
	len=sprintf(temp,"%s",data);
	temp[len]=0x1A;			//�����ַ�
	at_cmd[AT_CMD_SEND_DATA].Command=temp;
	at_cmd[AT_CMD_SEND_DATA].AtRlplyState=AT_NONE;
	if(SimExecuteCmd(AT_CMD_START_TRAN)==AT_OK)
	{
		//printf("send:%s",temp);
		if(SimExecuteCmd(AT_CMD_SEND_DATA)==AT_OK)
			return 1;
	}
	return 0;
}

/*
 * SimCheckReply:	���Simģ��Ļظ�״̬����Ҫ�����޸�AtCmdStruct�ṹ���AtRlplyState
 * ����:			��
 * ����ֵ:			��
*/
void SimCheckReply(void)
{
	if(SimReCount==SIM_RECIEVE_COUNT_MAX)			//�������֮ǰ������
		SimReCount=0;
	SimReBuff[SimReCount++]=SimReChar[0];		//������յ�������
	//printf("%c",SimReChar[0]);
	
	if ( SimReBuff[SimReCount-1]=='>' ) 		// '>'�������ݴ���״̬���������"\r\n"Ϊ��β
	{
		for (int8_t i=sizeof(at_cmd)/sizeof(at_cmd[0])-1; i>=0; i--)
		{
			if (*at_cmd[i].ExpectReply=='>')
			{
				at_cmd[i].AtRlplyState=AT_OK;
			}
		}
		SimReCount = 0;
	}
    else if ( SimReCount<4 && SimReBuff[SimReCount-2]=='\r' && SimReBuff[SimReCount-1]=='\n' )
        SimReCount = 0;//��simģ��ǰ���\r\nȥ����ȥ���հ���
	else if ( SimReCount>=4 && SimReBuff[SimReCount-2]=='\r' && SimReBuff[SimReCount-1]=='\n' )	// �յ�"\r\n"����������ظ�
	{
        for (int8_t i=sizeof(at_cmd)/sizeof(at_cmd[0])-1; i>=0; i--)
        {
            if (strstr(SimReBuff,at_cmd[i].ExpectReply)!=NULL)
            {
                at_cmd[i].AtRlplyState=AT_OK;
            }
            if (strstr(SimReBuff,"ERROR")!=NULL)
            {
                at_cmd[i].AtRlplyState=AT_ERROR;
            }
        }
        if(G_IsGetData==1)//���յ���������
        {
            if(SimReBuff[0]==':')//��ͷ��:�ģ���Ϊ��hex�ļ�
            {
                HandleIAPData(SimReBuff,SimReCount);
            }
        }
		SimReCount = 0;
	}
	HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)SimReChar,1);//���´򿪴��ڽ����ж�
}

void Lpuart1Send(char *p, uint16_t len)
{	
	HAL_Delay(10);
	HAL_UART_Transmit(&hlpuart1,(uint8_t *)p,len,0xffff);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle==&hlpuart1)		//sim800ģ��
	{
		SimCheckReply();
	}
}
