/*
 * user_sim.c	simģ�������ļ�
*/

#include "user_server.h"
#include "user_sim.h"
#include "user_flash_L072.h"

char G_A_Ack[SIM_RECIEVE_COUNT_MAX]           ={0};   // ���������������ACK��Ϣ
char G_A_Downstream[SIM_RECIEVE_COUNT_MAX]    ={0};   // ������������������п�������
char G_Arg[SIM_RECIEVE_COUNT_MAX]             ={0};   // ���п����еĲ�������

/*
 * SimRxSvMsgCallback Simģ����յ������������ݽ��ڴ˴��������ڴ����ж��б�����
*/
void SimRxSvMsgCallback(char *RxMsg,uint16_t Len)
{
    //DEBUG("RxMsg:%s\r\n",RxMsg);
    char *pt=strstr(RxMsg, "$ack@");
    if( pt!=NULL )
    {
        memcpy(G_A_Ack,pt,RxMsg+Len-pt);
    }
    else if(*RxMsg=='C')//������Ϣ
    {
        memcpy(G_A_Downstream,RxMsg,Len);
    }
    else
    {
        DEBUG("ʲô��\r\n");
    }
}

/*
 * ServerAck:	�������ظ�Ack
 * ����:	    ������Ϣ�ַ���
 * ����ֵ:	    0��ʾ�ظ��ɹ���1��ʱ��2�ظ���ʽ����3�ظ���seq�ȷ��͵�seq��(����Ӧ�ûظ����ͳ�ȥ��seq)
*/
uint8_t ServerAck(uint32_t *SensorSeq)
{
	//$ack@17081TEST001:$E=nbi_monitor,$seq=1
    uint32_t starttime=HAL_GetTick();
    uint8_t ret;
    while(HAL_GetTick()-starttime<5000 && G_A_Ack[0]==0);//5��ȴ���������
    if(G_A_Ack[0]==0)
    {
        DEBUG("ACK:none\r\n");
        return 1;
    }
    else
    {
        uint32_t seq_recive=0;
        DEBUG("ACK:%s\r\n",G_A_Ack);
        sprintf(G_Arg,"$ack@%s:$E=%s,$seq=",DeviceID,Stream);
        if ( strstr(G_A_Ack,G_Arg) == NULL)
        {
            DEBUG("ACK Data Format ERROR\r\n");
            ret = 2;	//���ݸ�ʽ����
        }
        else
        {
            ret = sscanf(G_A_Ack,"%*[^,],$seq=%u",&seq_recive);
            //ret����ת���ɹ��ĸ������п����ǲ���ת���ɹ���ת���ɹ��Ĳ���ֵ���ı䣬���ɹ���ֵδ���ı�
            //-1��ʾת����ʽ���󣬱����һ����������ΪNULL,0��ʾ���в�����û��ת���ɹ���������ָ�������Щ����cmd,arg,seq_char�ȣ�����ָ�����Ĳ���
            if (ret != 1)
            {
                DEBUG("ACK Data Format ERROR\r\n");
                ret = 2;	//���ݸ�ʽ����
            }
            else
            {
                if(*SensorSeq == seq_recive )
                {
                    DEBUG("ACK OK\r\n");
                    ret = 0;	//�ظ��ɹ�
                }
                else
                {
                    DEBUG("seq_recive:%u\r\nseq_send:%u\r\n",seq_recive,*SensorSeq);
                    *SensorSeq=(*SensorSeq>seq_recive)?*SensorSeq:seq_recive;
                    DEBUG("resend \r\n");	//�ٷ���һ�����ݣ����ͻ��˵�seq���������seq��Ӧ����
                    ret = 3;
                }
            }
        }
        memset(G_A_Ack,0,sizeof(G_A_Ack));
    }
    
    return ret;
}

/*
 * IsControl:	���������Ƿ��п�������
 * ����:		�豸ID������������һ�ο�����������к�,�������к�Ϊ���µ�
 * ����ֵ:		-1��ʾ�޿�������
*/
int8_t ServerDowmsteam(uint32_t *CtrlSeq)
{
	/*state=0	ִ�гɹ�
	*state=1	ִ��ʧ�ܣ�����ʧ����bootloader������־
	*state=2	������ִ��(�л��汾)
	*state=3	�����������
	*state=4	��֧������
	*state=5	�ϴ��Ѿ�ִ�й�������
	*state=6	��������
	*/
	//C17021Y2EP017:seq=1,n=SP,a=60
	//C17081TEST001:seq=1,n=IP,a=47.90.33.7|8033
	//C1703185FK030:seq=14,n=UD,a=http://down.nongbotech.cn/WatcherStandarApplication.hex|PR02_V3_170930
    //C1703185FK030:seq=14,n=UD,a=http://down.nongbotech.cn/WatcherStandarApplication.hex|PR02_V3_170930
	//char *service_ctrl = "C17021Y2EP017:seq=1,n=SP,a=60";
	//char *service_ctrl = "C123456789:seq=2,n=GL,a=0";
	//DEBUG("CTRL ServerReply:%x\r\n",ServerReply);
    if(G_A_Downstream[0]==0)
        return -1;
    char *pt = G_A_Downstream;
    int8_t ret = 0;
    char seq_char[11] = { 0 };
    char *arg_char=G_Arg;	//ʹ��SimReBuff���ڴ�ռ䣬��ʡ�ڴ�
    char cmd_char[3] = { 0 };	//�����������2���ַ�
    uint32_t seq_recive = 0;
    DEBUG("ctrl:%s\r\n", G_A_Downstream);
    SetFlag(DOWNLINK_CONFIRM);//���п��Ʊ�־λ
    if (*pt != 'C' || strstr(pt, DeviceID) == NULL || strstr(pt, ",a=") == NULL || strstr(pt, ",n=") == NULL || strstr(pt, ":seq=") == NULL)
    {
        DEBUG("Data Format ERROR\r\n");
        cmd_char[0] = 'X';
        cmd_char[1] = 'X';
        FlashWrite16(CTRL_NAME, (uint16_t *)cmd_char, 1);
        return 6;	//���ݸ�ʽ����
    }
    ret = sscanf(pt, "%*[^:]:seq=%10[^,],n=%2[^,],a=%200s", seq_char, cmd_char, arg_char);
    //ret����ת���ɹ��ĸ������п����ǲ���ת���ɹ���ת���ɹ��Ĳ���ֵ���ı䣬���ɹ���ֵδ���ı�
    //-1��ʾת����ʽ���󣬱����һ����������ΪNULL,0��ʾ���в�����û��ת���ɹ���������ָ�������Щ����cmd,arg,seq_char�ȣ�����ָ�����Ĳ���

    DEBUG("cmd_char:%s\r\narg_char:%s\r\nseq_char:%s\r\n", cmd_char, arg_char, seq_char);
    if (ret != -1 && ret != 0)
        ret = sscanf(seq_char, "%u", &seq_recive);//-1����ת����0xffffffff
    DEBUG("seq_downstream_recive:%u\r\n", seq_recive);
    if (seq_recive == 0 || cmd_char[0] == 0 || arg_char[0] == 0 || ret == 0 || ret == -1)
    {
        DEBUG("Data Format ERROR:NONE\r\n");
        return 6;
    }
    if (*CtrlSeq == seq_recive)
    {
        DEBUG("Already executed\r\n");
        return 5;
    }
    else
    {
        *CtrlSeq = seq_recive;
        DEBUG("Execute Command\r\n");						//�յ���seq�뱾�صĲ���ͬ��ִ������
        FlashWrite32(RECIEVE_SEQ, &seq_recive, 1);			//������ܵ���seq
        FlashWrite16(CTRL_NAME, (uint16_t *)cmd_char, 1);	//������ܵ��Ŀ�������
        SetFlag(DOWNLINK_CONFIRM);							//���п��ƻظ���־
    }
    pt = cmd_char;
    /********************Զ������*********************/
    if (strncmp(pt, "UD", 2) == 0)
    {
        //arg="http://down.nongbotech.cn/Application.hex|pro2_v3_170930";
        //char url[URL_SIZE + 1] = { 0 };
        char *url=G_A_Ack;
        char stream[STREAM_SIZE + 1] = { 0 };
        pt = arg_char;
        ret = sscanf(pt, "%98[^|]|%32s", url, stream);
        if (url[0] == 0 || stream[0] == 0 || ret == 0 || ret == -1)
        {
            DEBUG("arg error\r\n");
            return 3;
        }
        //���ø��±�־λ�������´�����ʱ����bootloader����
        SetFlag(UPDATE_RESET);
        FlashWrite16(URL, (uint16_t *)url, strlen(url) / 2 + 1);
        FlashWrite16(STREAM_ADDR, (uint16_t *)stream, strlen(stream) / 2 + 1);
        SetFlag(SOFT_RESET);
        return 0;//ִ�гɹ�
    }
    /********************Զ������*********************/

    /*********************�޸�IP**********************/
    if (strncmp(pt, "IP", 2) == 0)//�޸�IP
    {
        char ip[32] = { 0 };
        char port[16] = { 0 };
        //arg=47.90.33.7|65535
        pt = arg_char;
        ret = sscanf(pt, "%31[^|]|%16s", ip, port);
        if (ip[0] == 0 || port[0] == 0 || ret == 0 || ret == -1)
        {
            DEBUG("arg error\r\n");
            return 3;
        }
        DEBUG("Change IP:\r\nip:%s\r\nport:%s\r\n", ip, port);
        FlashWrite16(SERVER_ADDR, (uint16_t *)ip, strlen(ip) / 2 + 1);
        FlashWrite16(SERVER_PORT_ADDR, (uint16_t *)port, strlen(port) / 2 + 1);
        SetFlag(SOFT_RESET);	//��λһ��
        return 0;//ִ�гɹ�
    }
    /*********************�޸�IP**********************/

    /******************�޸Ĳ�������**********************/
    if (strncmp(pt, "SP", 2) == 0)
    {
        char time_char[11] = { 0 };
        uint32_t time = 0;
        //arg=30;
        pt = arg_char;
        ret = sscanf(pt, "%10s", time_char);
        if (time_char[0] == 0 || ret == 0 || ret == -1)
        {
            DEBUG("arg error\r\n");
            return 3;
        }
        ret = sscanf(time_char, "%u", &time);
        if (time == 0 || ret == 0 || ret == -1)
        {
            DEBUG("arg error\r\n");
            return 3;
        }
        DEBUG("New Sample time��%u\r\n", time);
        FlashWrite32(SAMPLE_PERIOD_ADDR, &time, 1);
        SetFlag(SOFT_RESET);	//��λһ��
        return 0;//ִ�гɹ�
    }
    /******************�޸Ĳ�������**********************/

    /******************�л��汾**********************/
    if (strncmp(pt, "SV", 2) == 0)
    {
        if (FlashRead16(APP_AREA_ADD) == APP_AREA_A && FlashRead16(AREA_B_RUN) == 0x31)		//��ǰ������A����ͬʱB����������
            FlashWrite16(APP_AREA_ADD, (uint16_t *)"1", 1);	//����B��
        else if (FlashRead16(APP_AREA_ADD) == APP_AREA_B && FlashRead16(AREA_A_RUN) == 0x31)	//��ǰ������B����ͬʱA����������
            FlashWrite16(APP_AREA_ADD, (uint16_t *)"0", 1);	//��ת��A��	
        else {
            DEBUG("Can't Switch Version \r\n");
            return 2;	//������ִ��
        }
        SetFlag(SOFT_RESET);	//��λһ��
        return 0;//ִ�гɹ�
    }
    /******************�л��汾**********************/

    /*********************��ȡλ����Ϣ**********************/
    if (strncmp(pt, "GL", 2) == 0)//GetLocation
    {
        SetFlag(GET_LOCATION);
        SetFlag(SOFT_RESET);
        DEBUG("Get Location\r\n");
        return 0;//ִ�гɹ�
    }
    /*********************��ȡλ����Ϣ**********************/

    return 4;//��֧�ֵ�ָ��
}
