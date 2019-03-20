#include "user_main.h"
#include "user_sim.h"
#include "user_led.h"
#include "user_adc.h"
#include "user_flash_L072.h"
#include "user_sensor_pro2.h"
#include "user_gps.h"
#include "user_server.h"
#include "user_battery.h"
#include "user_spi_flash.h"
#include "time.h"

#define         DEVICEID            0xc2201300

extern UART_HandleTypeDef hlpuart1;	//GPRS
extern UART_HandleTypeDef huart1;	//����
//extern UART_HandleTypeDef huart2;	//GPS
//extern UART_HandleTypeDef huart5;	//485
extern RTC_HandleTypeDef hrtc;
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;     //ι�����л�LEDʱ��

char Uart1ReChar[1]={0};
char Uart1ReBuff[128]={0};
uint8_t Uart1ReCount=0;

char TimeNow[20]                    ={0};   //�����ʱ�䵱ǰʱ��
char SendData[127+1]	            ={0};	//��󳤶�Ϊ127���ַ�����һ�������ͽ�����0x0A
char ServerAdd[SERVER_SIZE]	        ={0};	//��������ַ
char ServerPort[SERVER_PORT_SIZE]   ={0};	//�˿ڵ�ַ
char DeviceID[DEVICE_ID_SIZE]	    ={0};	//�豸ID
char Stream[STREAM_SIZE]		    ={0};	//������

uint32_t    G_htim2TimeOut;                 //��ʱ��2�ĳ�ʱʱ��
uint32_t    G_SendAddr;                     //ָ���Ѿ������˵����ݵ�����ַ
uint32_t    G_SaveAddr;                     //δ���͵����ݱ�����SPIflash�еĵ�ַ

T_SensorData sensor_data[2]={0};			//���������նȡ�������ʪ�ȼ���5�����������ݣ������8������

/*
 * ��ȡ��ǰϵͳRTCʱ�䣬
*/
void RtcGetTime(char *str)
{
    RTC_DateTypeDef Date;
    RTC_TimeTypeDef Time;
	HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN);//�涨��ʽ
    sprintf(str,"20%02d-%02d-%02d %02d:%02d:%02d",\
    Date.Year,Date.Month,Date.Date, Time.Hours, Time.Minutes, Time.Seconds);
	DEBUG("����ʱ��:%s\r\n",str);
}


/*
 * ��������
*/
void SaveSensorData(uint8_t *Bat)
{    
    G_SaveAddr = FlashRead32(SAVE_FLASH_ADD);
//    G_SaveAddr = 8200;
    if(G_SaveAddr==0xFFFFFFFF)
    {
        G_SaveAddr = 0;
    }
    uint32_t temp =sizeof(sensor_data)+sizeof(TimeNow)+1;//Ҫд����ֽ���
    if(G_SaveAddr+temp>SPI_FLASH_SIZE||G_SaveAddr==0)
    {
        DEBUG("������ҳ\r\n");
        SpiFlashSectorErase(0*4096);
        G_SaveAddr = 0;
    }
    uint32_t a= G_SaveAddr / SPI_FLASH_SECTOR_SIZE;
    
    uint32_t b= (G_SaveAddr+temp) / SPI_FLASH_SECTOR_SIZE;
    
    DEBUG("a= %d, b=%d,G_SaveAddr=%d, %d\r\n", a,b,G_SaveAddr,(G_SaveAddr+temp));
    if(a<b)//���ݿ�ҳ
    {
        SpiFlashSectorErase(b*4096);
        DEBUG("���ݿ�ҳ\r\n");
    }
    
    DEBUG("11G_SaveAddr:%u len : %d \r\n",G_SaveAddr, sizeof(sensor_data));
    
    for(uint8_t i = 0; i < 2; i++)
    {
        for(uint8_t j = 0; j < 2; j++)
        {
            DEBUG("0x%04x ",sensor_data[i].data[j]);
        }
    }
    DEBUG("\r\n");
    
    uint32_t DeviceID = 0;
    DeviceID = SpiFlashReadDeviceID( );    
    printf("DeviceID: 0x%08x\r\n", DeviceID);
    if(DeviceID == DEVICEID)
    {       
        SpiFlashBufferWrite((uint8_t*)sensor_data,G_SaveAddr,sizeof(sensor_data));
        G_SaveAddr +=(sizeof(sensor_data));
        DEBUG("22G_SaveAddr:%u\r\n",G_SaveAddr);
        SpiFlashBufferWrite(Bat,G_SaveAddr,1);
        G_SaveAddr +=1;
        
        DEBUG("33G_SaveAddr:%u\r\n",G_SaveAddr);
        DEBUG("---time:%s\r\n",TimeNow);
        SpiFlashBufferWrite((uint8_t*)TimeNow,G_SaveAddr,sizeof(TimeNow));
        G_SaveAddr +=(sizeof(TimeNow));
        DEBUG("44G_SaveAddr:%u\r\n",G_SaveAddr);
        FlashWrite32(SAVE_FLASH_ADD,&G_SaveAddr,1);
    }
}
/*
 * ������δ���͵����ݣ�����1�����򷵻�0
*/
uint8_t ReadOldData(uint8_t *Bat)
{
    G_SendAddr = FlashRead32(SEND_FLASH_ADD);
    
    if(G_SendAddr==0xFFFFFFFF)
    {
        G_SendAddr = 0;
    }
//    G_SendAddr = 8200;
    if(G_SaveAddr!=G_SendAddr)
    {
        DEBUG("111G_SendAddr:%u\r\n",G_SendAddr);
        uint32_t temp =sizeof(sensor_data)+sizeof(TimeNow)+1;//Ҫ�������ֽ�����������д�����ͬ
        if(G_SendAddr+temp>SPI_FLASH_SIZE)
        {
            G_SaveAddr = 0;
        }
        
        DEBUG("��������Ҫ���͵�������: len : %d ",sizeof(sensor_data));
        SpiFlashBufferRead((uint8_t*)sensor_data,G_SendAddr,sizeof(sensor_data));
        
        for(uint8_t i = 0; i < 2; i++)
        {
            for(uint8_t j = 0; j < 2; j++)
            {
                DEBUG("0x%04x ",sensor_data[i].data[j]);
            }
        }
        DEBUG("\r\n");
        G_SendAddr += sizeof(sensor_data);
        
        DEBUG("222G_SendAddr:%d\r\n",G_SendAddr);
        SpiFlashBufferRead(Bat,G_SendAddr,1);
        DEBUG("Bat:%d\r\n",*Bat);
        G_SendAddr += 1;
        DEBUG("333G_SendAddr:%d\r\n",G_SendAddr);
        SpiFlashBufferRead((uint8_t*)TimeNow,G_SendAddr,sizeof(TimeNow));
        DEBUG("TimeNow:%s\r\n",TimeNow);
        G_SendAddr += sizeof(TimeNow);
        DEBUG("444G_SendAddr:%d\r\n",G_SendAddr);
        //FlashWrite32(SEND_FLASH_ADD,&G_SendAddr,1);//��Ҫ�ڷ��ͳɹ�������²Ű�ֵ����
        return 1;
    }
    else
    {
        return 0;
    }
}


static uint32_t simsevercount = 0;
void UserMain(void)
{
	uint8_t Battery;
	uint16_t len=0;

	uint32_t seq_send;
    uint32_t seq_ctrl;
	int8_t ret;
        
    //HAL_UART_Receive_IT(&huart1,(uint8_t *)Uart1ReChar,1);//���¿���uart1���ж�ʹ��
    
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
    {
        DEBUG("��ʱ��2����ʧ��\r\n");	            //����ι��
        HAL_NVIC_SystemReset();	                //ֱ�������λ
    }
    G_htim2TimeOut=0;
	RtcGetTime(TimeNow);
	if(ReadSerialNumber(DeviceID,Stream,ServerAdd,ServerPort)!=0)
        HAL_NVIC_SystemReset();

    if(BatInit()==0)
        HAL_NVIC_SystemReset();
    else
    {   
        BatEnableCharge();
        HAL_Delay(1000);                        //���ݳ��,��ʱ��ȡ��ص����ǲ�׼ȷ��
        BatCheck(&Battery);
        if(Battery==0)
        {
            LedChangeState(E_LED_ERROR);
            HAL_Delay(3000);                    //����˸
            return;                             //��������
        }
    }

    //�����������ʼ������ȡ���������ݹ���,LED����
    LedChangeState(E_LED_INIT);
    
    SensorInit();
	SernsorGetData(sensor_data);
    
//    DEBUG("--111�¶�: %d, ʪ��: %d\r\n",sensor_data[1].data[0],sensor_data[1].data[1]);
    //�������ݵ��ⲿflash
    SaveSensorData(&Battery);
//    DEBUG("--222�¶�: %d, ʪ��: %d\r\n",sensor_data[1].data[0],sensor_data[1].data[1]);
    //��������
    LedChangeState(E_LED_DISCONNECT);
        
//    while(1);

 #if 1
    
	if(SimInit()==0)
    {
		DEBUG("Sim Init fail\r\n");
        LedChangeState(E_LED_ERROR);    //����3�������
        SimPowerOff();					//�رյ�Դ��ʵ����Ҫ5��
		HAL_Delay(3000);
		return;
	}
    else
		DEBUG("Sim Init OK\r\n");

    simsevercount=FlashRead32(START_COUNT_ADD);
    
    simsevercount ++;
    
    FlashWrite32(START_COUNT_ADD,&simsevercount,1);
    
    printf("----- simsevercount : %d\r\n",simsevercount);
    
//    if(simsevercount>=10)
    {
        if(SimConnectServer(ServerAdd,ServerPort)==0)
        {
            DEBUG("Connect Server fail\r\n");
        }
        else
        {
            DEBUG("Connect Server OK\r\n");
            LedChangeState(E_LED_CONNECT);
            
            DEBUG("--333�¶�: %d, ʪ��: %d\r\n",sensor_data[1].data[0],sensor_data[1].data[1]);

            while(ReadOldData(&Battery))
            {
                DEBUG("--444�¶�: %d, ʪ��: %d\r\n",sensor_data[1].data[0],sensor_data[1].data[1]);

                //�豸ID����������6��485������(mini���ޣ�����ȫΪ0)����ص���
                len=0;
    //            DEBUG("Battery : %d\r\n",Battery);
                len += sprintf(SendData+len,"D%s,stream=%s,data=%02x000000000000%02x",DeviceID,Stream,STRUCT_VERSION,Battery);
    //            DEBUG("SendData : %s\r\n",SendData);

                //Ȼ��ƴ�Ӵ���������
                for(uint8_t i=0;i<2;i++)
                {
    //                DEBUG("sensor_data[i].count : %d\r\n",sensor_data[i].count);
                    for(uint8_t j=0;j<2;j++)
                    {
    //                    DEBUG("line : %d\r\n",__LINE__);
                        len += sprintf(SendData+len,"%04x",sensor_data[i].data[j]);
                        DEBUG("senddata : %d, 0x%04x\r\n",i, sensor_data[i].data[j]);
                    }
                }
                
                DEBUG("��GPSģ��\r\n");
                //len += sprintf(SendData+len,"|null|%s",TimeNow);
                len += sprintf(SendData+len,"|null");

                seq_send=FlashRead32(SEND_SEQ);
                if(seq_send==0)	//������0
                    seq_send=1;
                len += sprintf(SendData+len,",seq=%u\n",seq_send);
                if(CheckFlag(DOWNLINK_CONFIRM))		//������������Ҫȷ�ϣ���ȷ����Ϣƴ���ں���
                {
                    char name[CTRL_NAME_SIZE]={0};
                    FlashRead16More(CTRL_NAME,(uint16_t *)name,CTRL_NAME_SIZE/2+1);
                    seq_ctrl=FlashRead32(RECIEVE_SEQ)  ;

                    len += sprintf(SendData+len,"n=%s,seq=%u,state=%u\n",name,seq_ctrl,FlashRead32(CTRL_STATE));//seq�յ�ʲô�ͻ�ʲô
                    if(FlashRead16(CTRL_STATE)==1)	//����ִ��ʧ��(Ŀǰֻ�и�������Ż�ִ��ʧ��)����Ҫ�ѱ����seq�޸�һ��,��ֹ�������ط�ָ��ʱ��ִ�С�
                    {
                        seq_ctrl +=1;
                        FlashWrite32(RECIEVE_SEQ,&seq_ctrl,1);
                    }
                    //DEBUG("Reply:%s",SendData);
                }
                DEBUG("%s\r\n",SendData);
            
                for(uint8_t i=0;i<2;i++)
                {
                    if(SimSendData(SendData,len)==0)
                    {
                        DEBUG("Send data Fail\r\n");
                    }
                    else
                    {
                        DEBUG("Send data OK\r\n");
                        FlashWrite32(SEND_FLASH_ADD,&G_SendAddr,1);//��Ҫ�ڷ��ͳɹ�������²Ű�ֵ����
                        ret=ServerAck(&seq_send);
                        //Flash����Ӧ���ڽ��շ���������֮����ΪдFlashʱ��������__HAL_LOCK()�������·��������ݵĶ���
                        uint32_t temp=seq_send+1;
                        FlashWrite32(SEND_SEQ,&temp,1);     //���͵�seq����1
                        if(CheckFlag(DOWNLINK_CONFIRM))		//������������Ҫȷ��
                            CleanFlag(DOWNLINK_CONFIRM);	//���ȷ�ϱ�־
                        if(ret==0)//�������ظ��ɹ�
                        {
                            break;
                        }
                        else if(ret==3)
                        {
                            char *ch=strstr(SendData,",seq=");
                            seq_send++;
                            sprintf(ch,",seq=%u\n",seq_send);
                            DEBUG("SendData:%s",SendData);
                        }
                    }
                }
                ret=ServerDowmsteam(&seq_ctrl);
                if(ret!=-1)
                {
                    uint32_t a=ret;
                    FlashWrite32(CTRL_STATE,&a,1);
                }
                memset(SendData,0,sizeof(SendData));
                memset(TimeNow,0,sizeof(TimeNow));
            }
            LedChangeState(E_LED_SENDOK);                   //���ݷ������
            SimPowerOff();
            HAL_Delay(1000);
        }
    }
	if(FlashRead32(SAMPLE_PERIOD_ADDR)==0||FlashRead32(SAMPLE_PERIOD_ADDR)==0xffffffff)
	{
		uint32_t time=300;//Ĭ��300�룬���Ϸ��͹��̴��20��
		FlashWrite32(SAMPLE_PERIOD_ADDR,&time,1);
	}
	if(CheckFlag(SOFT_RESET))
	{
		CleanFlag(SOFT_RESET);
		DEBUG("��λ\r\n");
		HAL_Delay(2000);
		HAL_NVIC_SystemReset();
	}
    
#endif    
/**************���������ݻ�ȡ����*************/
}

/*
 *ResendToSim:�����ڽ��յ�������ת����SIMģ��
*/
//void ResendToSim(void)
//{
//	Uart1ReBuff[Uart1ReCount++]=Uart1ReChar[0];		//������յ�������
//	//DEBUG("%c",Uart1ReChar[0]);

//	if ( Uart1ReCount>=4 && Uart1ReBuff[Uart1ReCount-2]!='\r' && Uart1ReBuff[Uart1ReCount-1]=='\n' )	// �յ�"\r\n"����������ظ�
//	{
//        Uart1ReBuff[Uart1ReCount]=0;
//        SimRxSvMsgCallback(Uart1ReBuff,Uart1ReCount);
//		Uart1ReCount = 0;
//	}
//	HAL_UART_Receive_IT(&huart1,(uint8_t *)Uart1ReChar,1);//���¿���uart1���ж�ʹ��
//}


//extern char *service_ctrl;
//extern uint32_t ServerReply;
//extern char service_ack[64];

//void CheckServerReply(void)
//{
//    Uart1ReBuff[Uart1ReCount++]=Uart1ReChar[0];		//������յ�������
//	//DEBUG("%c",Uart1ReChar[0]);
//
//    if ( Uart1ReCount>=4 && Uart1ReBuff[Uart1ReCount-2]=='\r' && Uart1ReBuff[Uart1ReCount-1]=='\n' )
//    {
//        DEBUG("%s",Uart1ReBuff);
//        char *pt;
//        if (Uart1ReCount>15 && *Uart1ReBuff == 'C')//�յ����������п���
//        {
//            pt = strstr(Uart1ReBuff,"$ack@");
//            if(pt!=NULL)
//            {
//                memcpy(service_ctrl,Uart1ReBuff,pt-Uart1ReBuff);
//                ServerReply |=SERVER_REPLY_CTRL;
//            }
//        }
//        if(Uart1ReCount>=16 && (pt = strstr(Uart1ReBuff,"$ack@"))!=NULL )//�������ظ�ack
//        {
//            memcpy(service_ack,pt,Uart1ReBuff+Uart1ReCount-pt);
//            ServerReply |= SERVER_REPLY_ACK;
//        }
//    }
//	HAL_UART_Receive_IT(&huart1,(uint8_t *)Uart1ReChar,1);//���¿���uart1���ж�ʹ��
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(&htim2 == htim)//50msһ��
	{
        G_htim2TimeOut++;
        if(G_htim2TimeOut==20)//1sιһ��
        {
            HAL_IWDG_Refresh(&hiwdg);
            G_htim2TimeOut=0;
        }
        LedDisplay(50);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle==&huart1)				//��������ӣ����ڵ���
    {
		//ResendToSim();
		//ResendToGps();
        //CheckServerReply();//Ҫ��9600
	}
	else if(UartHandle==&hlpuart1)		//sim800ģ��
	{
		SimCheckReply();
	}
//	else if(UartHandle==&huart2)		//GPSģ��
//	{
//		GpsCheckReply();
//	}
//	else if(UartHandle==&huart5)		//Rs485ģ��
//	{
//		Rs485CheckReply();
//	}
}



