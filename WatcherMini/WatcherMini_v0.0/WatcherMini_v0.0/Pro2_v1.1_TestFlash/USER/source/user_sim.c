/*
 * user_sim.c	simģ�������ļ�
*/

#include "user_sim.h"

extern UART_HandleTypeDef hlpuart1;

char SimReChar[1]={0};								//���յ�һ���ֽ�
char SimReBuff[SIM_RECIEVE_COUNT_MAX]={0};			//���յ�������
volatile uint16_t SimReCount;						//���յ����ֽ���

char SIM_REPLY_USER_DATA[SIM_RECIEVE_COUNT_MAX];	//����ָ���У�����š�GPSλ����Ϣ�ȣ�SIMģ�鷵�ص�����

E_SimState G_SimState       = E_SIM_DISCONNECT;	    //��ʶSIMģ��״̬

uint8_t G_IsSendData=0;

T_AtCmdStruct at_cmd[]=
{
	/*ָ���ַ���		�������յ����ַ���		ÿ�γ�ʱʱ��	���Դ���	����ִ�з��صĳ�ʼ״̬}*/
	{"ATE0\r\n",		"OK",					2000,			10,			AT_NONE},	// �رջ��ԣ�����ִ��10��ÿ�εȴ�1s,��ʼ�ķ���״̬ΪAT_NONE,�ɹ�����AT_OK
	{"AT+CIURC=1\r\n",	"SMS Ready",			120000,			1,			AT_NONE},	// �ȴ�ģ��call ready���ȴ�120s
	{NULL,				"CONNECT OK",			25000,			2,			AT_NONE},	// ���ӷ�����,������������û�ָ��
	{"AT+CIPSEND\r\n",	">",					2000,			1,			AT_NONE},	// ���뷢������״̬
	{NULL,				"SEND OK",				30000,			1,			AT_NONE},	// ͨ���Ѿ����������ӷ������ݣ����ݼ����ȡ���ʱʱ���ɳ�������趨
	{"AT+CIPSHUT\r\n",	"SHUT OK",				5000,			1,			AT_NONE},	// �ر�����
	{"AT+CIPSTATUS\r\n","CONNECT OK",			2000,			1,			AT_NONE},	// ��ѯTCP����״̬
    {"AT+CGCLASS=\"CC\"\r\n","OK",				1000,			2,			AT_NONE},	// ���ù���ģʽΪCC
	{NULL,				"OK",					2000,			1,			AT_NONE},	// ����������
	{"AT+CPIN?\r\n",    "+CPIN: READY",         3000,           5,          AT_NONE},   // ���sim����

	{NULL,				NULL,					0,				0,			AT_NONE}	//�û��Զ������һ��Ҫ�������
};
/*
 * SimIsConnectServer:	��ѯsimģ���Ƿ������ŷ�����
 * ����:				��
 * ����ֵ:				1��ʾ�������ˣ�0��ʾδ����
*/
uint8_t SimIsConnectServer(void)
{
	if(G_SimState==E_SIM_CONNECT)
		return 1;
	else
		return 0;
}
/*
 * SimGetState:	��ȡsimģ�鵱ǰ״̬
 * ����:
 * ����ֵ:
*/
E_SimState SimGetState(void)
{
	return G_SimState;
}
/*
 * SimPowerOn:			��simģ���Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimPowerOn(void)
{
	HAL_GPIO_WritePin(OUT_SIM_Power_ON_GPIO_Port, OUT_SIM_Power_ON_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(OUT_SIM_EN_GPIO_Port, OUT_SIM_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	return 1;
}
/*
 * SimCheckModle:		���ģ���Ƿ�ͨ������
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimCheckModle(void)
{
	at_cmd[AT_CMD_CLOSE_ECHO].RetryCount=5;
	if(SimExecuteCmd(AT_CMD_CLOSE_ECHO)!=AT_OK)
    {
        G_SimState = E_SIM_ERROR;
        DEBUG("ͨѶʧ��\r\n");
		return 0;
	}
    if(SimExecuteCmd(AT_CMD_CARD_INSERT)!=AT_OK)
    {
        DEBUG("SIM��δ����\r\n");
        G_SimState = E_SIM_ERROR;
        return 0;
    }
    return 1;
}
/*
 * SimPowerOff:			�ر�simģ���Դ
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimPowerOff(void)
{
	HAL_GPIO_WritePin(OUT_SIM_EN_GPIO_Port, OUT_SIM_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(OUT_SIM_Power_ON_GPIO_Port, OUT_SIM_Power_ON_Pin, GPIO_PIN_RESET);
	return 1;
}

/*
 * SimSendAtCmd:	����Atָ���Simģ��
 * cmd:				Atָ�����ɼ�AtCmd�Ķ���
 * ����ֵ:			1�ɹ� 0ʧ��
*/
uint8_t SimSendAtCmd(E_AtCmd cmd)
{
    uint32_t starttime=HAL_GetTick();
    //DEBUG("%u\r\n",SimReCount);
    while( SimReCount != 0 && HAL_GetTick()-starttime < 500 );
    HAL_NVIC_DisableIRQ(RNG_LPUART1_IRQn);
	if(HAL_UART_Transmit(&hlpuart1,(uint8_t *)at_cmd[cmd].Command,strlen(at_cmd[cmd].Command),0xffff)!=HAL_OK)
		return 0;
    HAL_NVIC_EnableIRQ(RNG_LPUART1_IRQn);
	return 1;
}

E_AtState SimExecuteCmd(E_AtCmd cmd)
{
    if(cmd==AT_CMD_USER)
	{
		memset(SIM_REPLY_USER_DATA,0,sizeof(SIM_REPLY_USER_DATA));
	}
	for(uint8_t i=0;i<at_cmd[cmd].RetryCount;i++)
	{
		at_cmd[cmd].AtRlplyState=AT_NONE;
		//DEBUG("%s,%u",at_cmd[cmd].Command,strlen(at_cmd[cmd].Command));
        DEBUG("%s",at_cmd[cmd].Command);//,strlen(at_cmd[cmd].Command));
		uint32_t StartTime=HAL_GetTick();
		if(SimSendAtCmd(cmd)!=1)
		{
			DEBUG("Sim Module Error\r\n");
			return AT_ERROR;
		}
		while(at_cmd[cmd].AtRlplyState==AT_NONE && HAL_GetTick()-StartTime<at_cmd[cmd].WaitTime);
		if(at_cmd[cmd].AtRlplyState==AT_OK)
		{
			break;
		}
	}
	if(at_cmd[cmd].AtRlplyState==AT_NONE)
		return AT_FAIL;
    if(cmd==AT_CMD_USER)
	{
		at_cmd[AT_CMD_USER].Command=NULL;
	}
	return at_cmd[cmd].AtRlplyState;
}
/*
 * SimSetBaud:			���ò�����
 * ����:				������
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimSetBaud(uint32_t Baud)
{
	char command[64];
	sprintf(command,"AT+IPR=%u\r\n",Baud);
	//����д���Ǵ���ģ���Ȼ�﷨��û������
	//sprintf(at_cmd[AT_CMD_CONNECT_SERVER].Command,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",ServerIP,ServerPort);
	at_cmd[AT_CMD_SET_BAUD].Command=command;
	if(SimExecuteCmd(AT_CMD_SET_BAUD)==AT_OK)
	{
		return 1;
	}
	return 0;
}
/*
 * SimGetIccid:			��ȡSim���ĵ绰����
 * ����PhoneNum:		�绰����
 * ����ֵ:				�绰����
*/
char * SimGetIccid(char *PhoneNum)
{
    T_AtCmdStruct gps_cmd;
	gps_cmd.Command="AT+CCID\r\n";
	gps_cmd.ExpectReply="OK";
	gps_cmd.WaitTime=3000;
	gps_cmd.RetryCount=2;
	gps_cmd.AtRlplyState=AT_NONE;
	at_cmd[AT_CMD_USER]=gps_cmd;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return NULL;

    if(SIM_REPLY_USER_DATA[0]!='\0')
    {
        for(uint8_t i=0;i<strlen(SIM_REPLY_USER_DATA);i++)
        {
            if(SIM_REPLY_USER_DATA[i]!='\r' && SIM_REPLY_USER_DATA[i+1]=='\r')
            {
                SIM_REPLY_USER_DATA[i+1]='\0';
                break;
            }
        }
        sprintf(PhoneNum,"%s",SIM_REPLY_USER_DATA);
        return PhoneNum;
    }
    else
    {
        return NULL;
    }
}
/*
 * SimGetImei:			��ȡ�豸�ĺ���
 * ����ImeiNum:		ģ���豸��
 * ����ֵ:				�绰����
*/
char * SimGetImei(char *ImeiNum)
{
    T_AtCmdStruct gps_cmd;
	gps_cmd.Command="AT+GSN\r\n";
	gps_cmd.ExpectReply="OK";
	gps_cmd.WaitTime=3000;
	gps_cmd.RetryCount=2;
	gps_cmd.AtRlplyState=AT_NONE;
	at_cmd[AT_CMD_USER]=gps_cmd;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return NULL;
	if(SIM_REPLY_USER_DATA[0]!='\0')
	{
//        DEBUG("���յ�:");
//        DEBUG("%s",SIM_REPLY_USER_DATA);
        for(uint8_t i=0;i<strlen(SIM_REPLY_USER_DATA);i++)
        {
                if(SIM_REPLY_USER_DATA[i]!='\r' && SIM_REPLY_USER_DATA[i+1]=='\r')
                {
                        SIM_REPLY_USER_DATA[i+1]='\0';
                        break;
                }
        }
        sprintf(ImeiNum,"%s",SIM_REPLY_USER_DATA);
        return ImeiNum;
	}
	else
	{
        return NULL;
	}
}

/*
 * SimGetCSQ:		��ѯ�ź�����
 * Csq:		        �ź�����ֵ
 * ����ֵ:			�ź�����ֵ ��λ�� -dbm,0��ʾ��ȡʧ��
*/
uint8_t SimGetCSQ(uint8_t *Csq)
{
    uint32_t csq,ber;
    T_AtCmdStruct gps_cmd;
	gps_cmd.Command="AT+CSQ\r\n";
	gps_cmd.ExpectReply="OK";
	gps_cmd.WaitTime=5000;
	gps_cmd.RetryCount=2;
	gps_cmd.AtRlplyState=AT_NONE;
	at_cmd[AT_CMD_USER]=gps_cmd;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	if(SIM_REPLY_USER_DATA[0]!='\0')
	{
        //
        for(uint8_t i=0;i<strlen(SIM_REPLY_USER_DATA);i++)
        {
            if(SIM_REPLY_USER_DATA[i]!='\r' && SIM_REPLY_USER_DATA[i+1]=='\r')
            {
                    SIM_REPLY_USER_DATA[i+1]='\0';
                    break;
            }
        }
        //DEBUG("%s",SIM_REPLY_USER_DATA);
        *Csq = 99;
        uint8_t ret = sscanf(SIM_REPLY_USER_DATA,"+CSQ:%u,%u",&csq,&ber);
        if(ret != 2)
        {
            return 0;
        }
        else
        {
            *Csq = csq;
            if(csq>1 && csq<32)
                return 110-(csq-2)*2;
            else
                return 0;
        }
	}
	else
	{
        return 0;
	}
}
/*
 * SimCheckCard:		���Sim��ʱ�����
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimSetApn(char * pApn)
{
//    SimSetApn("CMNET");
//    SimSetApn("CMIOT");
//    SimSetApn("UNINET");
//    SimSetApn("3GNET");
    char temp[32]={0};
    //sprintf(temp,"AT+CGDCONT=1,\"IP\",\"%s\"\r\n",pApn);
    sprintf(temp,"AT+CIPCSGP=1,\"%s\"\r\n",pApn);
    //sprintf(temp,"AT+CSTT=\"%s\"\r\n",pApn);
    T_AtCmdStruct gps_cmd;
	gps_cmd.Command=temp;
	gps_cmd.ExpectReply="OK";
	gps_cmd.WaitTime=3000;
	gps_cmd.RetryCount=2;
	gps_cmd.AtRlplyState=AT_NONE;
	at_cmd[AT_CMD_USER]=gps_cmd;
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
    char PhoneNum[32]={0};
    char ImeiNum[32]={0};

	SimReCount=0;
    G_IsSendData=0;
    G_SimState = E_SIM_INIT;

	SimPowerOn();

	if(HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)SimReChar,1)!=HAL_OK)//�򿪴��ڽ����ж�
		return 0;

    HAL_Delay(10);

	if(SimExecuteCmd(AT_CMD_CLOSE_ECHO)!=AT_OK)
		return 0;
    
    if(SimCheckModle()==0)
        return 0;

	if(SimExecuteCmd(AT_CMD_WAIT_INIT)!=AT_OK)
		return 0;
	if(SimExecuteCmd(AT_CMD_SET_CLASS)!=AT_OK)//�����µ�SIMģ�鶼��Ҫ����һ�ι���ģʽ����������PDP DEACT
                                              //ģʽCC�Ǽ���Ŀǰ����SIM���ģ���ͨ���ƶ���������������ʹ��
		return 0;

    SimGetImei(ImeiNum);

    SimGetIccid(PhoneNum);

	if(ImeiNum[0] != '\0')
	{
		if(PhoneNum[0]!='\0')
			DEBUG(";ccid:%s,%s;\r\n",PhoneNum,ImeiNum);
		else
			DEBUG(";ccid:error;\r\n");
	}
	else
	{
        DEBUG(";ccid:error;\r\n");
	}
	return 1;
}

/*
 *	SimcomG_SimState:ͨ��SIMCOMģ�����ӷ�����
 *	ServerIP:			��������ַ��������ip���������������������б��ϵ�ַ��ȷ��
 *	ServerPort:			�������˿ڣ�0~65535
 *	����ֵ��			1�ɹ�|0ʧ��
 */
uint8_t SimConnectServer(char *ServerIP,char *ServerPort)
{
	char command[64];
	sprintf(command,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",ServerIP,ServerPort);
	at_cmd[AT_CMD_CONNECT_SERVER].Command=command;
    if(SimExecuteCmd(AT_CMD_CONNECT_SERVER)==AT_OK)
    {
        G_SimState=E_SIM_CONNECT;
        return 1;
    }
	return 0;
}

/*
 * SimSendData:			�������ݵ������ӵ�IP��ַ
 * data:				Ҫ���͵�����
 * len:                 ���ݳ���
 * ����ֵ:				1�ɹ� 0ʧ��
*/
uint8_t SimSendData(char *data,uint16_t len)
{
	data[len]=0x1A;			//�����ַ�
    data[len+1]=0;
	at_cmd[AT_CMD_SEND_DATA].Command=data;
	at_cmd[AT_CMD_SEND_DATA].AtRlplyState=AT_NONE;
	if(SimExecuteCmd(AT_CMD_START_TRAN)==AT_OK)
	{
		//DEBUG("send:%s",temp);
		HAL_Delay(100);
		if(SimExecuteCmd(AT_CMD_SEND_DATA)==AT_OK)
		{
			G_SimState=E_SIM_CONNECT;
			return 1;
		}
	}
	G_SimState=E_SIM_DISCONNECT;
	return 0;
}
/*
 * SimEntertSleepMode:	Simģ���������ģʽ
 * ����Mode:			1��Ӧ����ģʽ1��2��Ӧ����ģʽ2
 * ����ֵ:				1�ɹ�0ʧ��
*/
uint8_t SimEntertSleepMode(uint8_t Mode)
{
	if(Mode!=1 && Mode!=2)
		return 0;
	T_AtCmdStruct gps_cmd;
	char command[32]={0};
	sprintf(command,"AT+CSCLK=%u\r\n",Mode);
	gps_cmd.Command=command;
	gps_cmd.ExpectReply="OK";
	gps_cmd.WaitTime=2000;
	gps_cmd.RetryCount=2;
	gps_cmd.AtRlplyState=AT_NONE;
	at_cmd[AT_CMD_USER]=gps_cmd;
	if(SimExecuteCmd(AT_CMD_USER)!=AT_OK)
		return 0;
	return 1;
}
/*
 * SimCloseConnect:	    �Ͽ���������
 * ����:      			��
 * ����ֵ:				1�ɹ�0ʧ��
*/
uint8_t SimCloseConnect(void )
{
	if(SimExecuteCmd(AT_CMD_CLOSE_CONNECTION)!=AT_OK)
		return 0;
    G_SimState = E_SIM_DISCONNECT;
	return 1;
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
	DEBUG("%c",SimReChar[0]);

	if ( SimReBuff[SimReCount-2]=='>' && SimReBuff[SimReCount-1]==' ' ) 		// '>'�������ݴ���״̬���������"\r\n"Ϊ��β
	{
		at_cmd[AT_CMD_START_TRAN].AtRlplyState=AT_OK;
        G_IsSendData=1;
		SimReCount = 0;
	}
    else if ( SimReCount<4 && SimReBuff[SimReCount-2]=='\r' && SimReBuff[SimReCount-1]=='\n' )
        SimReCount = 0;//��simģ��ǰ���\r\nȥ����ȥ���հ���
	else if ( SimReCount>=4 && SimReBuff[SimReCount-2]=='\r' && SimReBuff[SimReCount-1]=='\n' )	// �յ�"\r\n"����������ظ�
	{
        if(at_cmd[AT_CMD_USER].Command!=NULL)//���û����ݱ��浽SIM_REPLY_USER_DATA��
		{
            SimReBuff[SimReCount]='\0';
            sprintf(SIM_REPLY_USER_DATA+strlen(SIM_REPLY_USER_DATA),"%s",SimReBuff);
		}
		for (int8_t i=sizeof(at_cmd)/sizeof(at_cmd[0])-1; i>=0; i--)
		{
			if (strstr(SimReBuff,at_cmd[i].ExpectReply)!=NULL)
			{
				at_cmd[i].AtRlplyState=AT_OK;
			}

            if (strstr(SimReBuff,"CONNECT FAIL")!=NULL)
            {
                at_cmd[AT_CMD_CONNECT_SERVER].AtRlplyState=AT_ERROR;
            }
		}
        if (strstr(SimReBuff,"CLOSED")!=NULL)
        {
            G_SimState=E_SIM_DISCONNECT;
        }
		SimReCount = 0;
	}
	else if ( SimReCount>15 && SimReBuff[SimReCount-2]!='\r' && SimReBuff[SimReCount-1]=='\n')	// �������������ݹ���
	{
        SimReBuff[SimReCount]=0;
        SimRxSvMsgCallback(SimReBuff,SimReCount);
        SimReCount = 0;
	}
    HAL_StatusTypeDef ret=HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)SimReChar,1);//���´򿪴��ڽ����ж�
    if(ret!=HAL_OK)//���´򿪴��ڽ����ж�
    {
        DEBUG("hlpuart1����:%x,%x,%x\r\n",ret,HAL_UART_GetError(&hlpuart1),HAL_UART_GetState(&hlpuart1));
        if (HAL_UART_Init(&hlpuart1) != HAL_OK)
        {
            DEBUG("����:ʧ��\r\n");
            while(1);//�������Ź��Ḵλ
        }
        do
        {
            ret = HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)SimReChar,1);
        }while(ret!=HAL_OK);//�������Ź��Ḵλ
        DEBUG("����:�ɹ�\r\n");
    }
}
