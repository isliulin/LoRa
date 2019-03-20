#include "user_main.h"
#include "user_bq24195.h"
#include "user_sim.h"
#include "user_led.h"
#include "user_adc.h"
#include "user_flash_L072.h"
#include "user_sensor_pro2.h"
#include "user_gps.h"
#include "bootloader_config.h"
#include <string.h>

extern UART_HandleTypeDef hlpuart1;	//GPRS
extern UART_HandleTypeDef huart1;	//����
extern UART_HandleTypeDef huart2;	//GPS
extern UART_HandleTypeDef huart5;	//485

char Uart1ReChar[1]={0};
char Uart1ReBuff[256]={0};
uint8_t Uart1ReCount=0;

#define STRUCT_VERSION	3			//ͨ�����ݰ汾��

void UserMain(void)
{
//	printf("~!@#$%^&*()_+|}{:?><zxcvbnm12345���\r\n");
	
/*************������**************///����ͨ��
//	Bq24195EnableCharge();
//	HAL_Delay(5000);
//	Bq24195DisableCharge();
//	HAL_Delay(5000);
//	Bq24195EnableCharge();
//	for(uint8_t i=0;i<10;i++)
//		AnalysisRegisiter(i);
/*************������**************/

/*************����ת��**************///����ͨ��
//	if(HAL_UART_Receive_IT(&huart1,(uint8_t *)Uart1ReChar,1)!=HAL_OK)//�򿪴��ڽ����ж�,����ת��ָ�simģ��
//		return ;
//	if(SimPowerOn()==0)
//		return ;
//	extern char SimReChar[1];
//	if(HAL_UART_Receive_IT(&hlpuart1,(uint8_t *)SimReChar,1)!=HAL_OK)//�򿪴��ڽ����ж�
//		return ;
/*************����ת��**************/

/****************GPS����***********/	/*����ͨ��*/
//	uint8_t index=1,count=0;
//while(1)
//{
//	double Latitude;	//γ��
//	double Longitude;	//����
//	uint32_t time=300000;//3���ӳ�ʱ
//	char data[50]={0};
////	if(HAL_UART_Receive_IT(&huart1,(uint8_t *)Uart1ReChar,1)!=HAL_OK)//�򿪴��ڽ����ж�,����ת��ָ�GPSģ��
////		return;

//	if(GpsGetLocation(&Longitude,&Latitude,&time)==0)
//	{
//		//��λʧ��
//		sprintf(data,"%u,%u,%f,%f,%u\r\n",index,count,Longitude,Latitude,time);
//		if(SimInit()==0)
//			printf("Sim Init fail\r\n");
//		else
//			printf("Sim Init OK\r\n");
//		if(SimConnectServer("47.90.33.7","8933")==0)
//			printf("Connect Server fail\r\n");
//		else
//			printf("Connect Server OK\r\n");
//		
//		if(SimSendData(data)==0)
//			printf("Send data Fail\r\n");
//		else
//			printf("Send data OK\r\n");
//		GpsPowerOff();
//		HAL_Delay(1000);
////		HAL_NVIC_SystemReset();
//	}
//	else
//	{//��λ�ɹ�
//		sprintf(data,"index:%u\r\ncount:%u\r\nLongitude:%f\r\nLatitude:%f\r\ntime:%u\r\n", index,count,Longitude,Latitude,time);
//		if(SimInit()==0)
//			printf("Sim Init fail\r\n");
//		else
//			printf("Sim Init OK\r\n");
//		if(SimConnectServer("47.90.33.7","8933")==0)
//			printf("Connect Server fail\r\n");
//		else
//			printf("Connect Server OK\r\n");
//		
//		if(SimSendData(data)==0)
//			printf("Send data Fail\r\n");
//		else
//		printf("Send data OK\r\n");
//		SimPowerOff();
//		HAL_UART_AbortReceive_IT(&hlpuart1);
//		HAL_Delay(10000);//��ʱ10���ٴζ�λ�ط�������
//	}
//	count++;
//}
/****************GPS����***********/

/**************ADC��ѹ�ɼ�*************///����ͨ��
//	float ADC0Voltage,ADC1Voltage;
//	char data[64]={0};
//	if(AdcInit()==0)
//		printf("AdcInit Fail\r\n");
//	else
//		//while(1)
//		{
//			ADC0Voltage=AdcGetVoltage(ADC_CHANNEL_0)*2;
//			ADC1Voltage=AdcGetVoltage(ADC_CHANNEL_1)*2;
//			printf("ADC0 Voltage:%f\r\n",ADC0Voltage);
//			printf("ADC1 Voltage:%f\r\n",ADC1Voltage);
//			HAL_Delay(2000);
//		}
/**************ADC��ѹ�ɼ�*************/

/************���������Ӳ���***********///����ͨ��
//	if(SimInit()==0)
//		printf("Sim Init fail\r\n");
//	else
//		printf("Sim Init OK\r\n");
//	if(SimConnectServer("47.90.33.7","8933")==0)
//		printf("Connect Server fail\r\n");
//	else
//		printf("Connect Server OK\r\n");
//	
//	if(SimSendData("~��@#��%����&*��������+|��������~!@#$%^&*()_+|}{:?0123456789.qwertyuiopasdfghjklzxcvbnmTest\r\n")==0)
////	sprintf(data,"ADC0Voltage:%f\r\nADC1Voltage:%f\r\n",ADC0Voltage,ADC1Voltage);
////	printf("%s",data);
////	if(SimSendData(data)==0)
//		printf("Send data Fail\r\n");
//	else
//		printf("Send data OK\r\n");
/************���������Ӳ���***********/

/**************��ŷ�����*************/	//����ͨ��
//	_12VPowerOn();
//	HAL_Delay(1000);
//	SolenoidOpen();
//	HAL_Delay(5000);
//	SolenoidClose();
//	_12VPowerOff();
/**************��ŷ�����*************/
	
/**************Flash��д����*************/	//����ͨ��
//	uint32_t FLASH_BUF [ FLASH_PAGE_SIZE/4 ];
//	for(uint8_t i=0;i<FLASH_PAGE_SIZE/4;i++)
//		FLASH_BUF[i]=i+1;
//	FlashWrite32(0x0801fd10,FLASH_BUF,FLASH_PAGE_SIZE/4);
//	for(uint8_t i=0;i<FLASH_PAGE_SIZE/4;i++)
//		printf("%x:%x\r\n",0x0801fd10+i*4,FlashRead32(0x0801fd10+i*4));
//	uint16_t FLASH_BUF [ FLASH_PAGE_SIZE/2 ];
//	for(uint16_t i=0;i<FLASH_PAGE_SIZE/2;i++)
//		FLASH_BUF[i]=i+1;
//	FlashWrite16(0x08010000,FLASH_BUF,FLASH_PAGE_SIZE/2);
//	for(uint8_t i=0;i<FLASH_PAGE_SIZE/2;i++)
//		printf("%x:%x\r\n",0x08010000+i*2,FlashRead16(0x08010000+i*2));

//SetFlag(SOFT_RESET);//���п��Ʊ�־λ
//CleanFlag(SOFT_RESET);
//SetFlag(UPDATE_RESET);//���п��Ʊ�־λ
//CleanFlag(UPDATE_RESET);
//SetFlag(DOWNLINK_CONFIRM);//���п��Ʊ�־λ
//CleanFlag(DOWNLINK_CONFIRM);
//SetFlag(CLEAN_FLASH);//���п��Ʊ�־λ
//CleanFlag(CLEAN_FLASH);
//SetFlag(GET_LOCATION);//���п��Ʊ�־λ
//CleanFlag(GET_LOCATION);
//SetFlag(UPDATE_SUCCEED);//���п��Ʊ�־λ
//CleanFlag(UPDATE_SUCCEED);
//SetFlag(UPDATE_FAIL);//���п��Ʊ�־λ
//CleanFlag(UPDATE_FAIL);
//SetFlag(END_OF_FLAGTYPE);//���п��Ʊ�־λ
//CleanFlag(END_OF_FLAGTYPE);

/**************Flash��д����*************/

/**************���������ݻ�ȡ����*************/	//����ͨ��
	char SendData[128]	={0};				//256̫������ջ������128�㹻��
	char ServerAdd[SERVER_SIZE]	={0};		//��������ַ
	char ServerPort[SERVER_PORT_SIZE]={0};	//�˿ڵ�ַ
	char DeviceID[DEVICE_ID_SIZE]	={0};	//�豸ID
	char Stream[STREAM_SIZE]		={0};	//������
	int32_t Battery;
	uint16_t len=0;
	SensorData sensor_data[8]={0};			//���������նȡ�������ʪ�ȼ���5�����������ݣ������8������
	double Latitude,Longitude;
	uint32_t seq_send;
	uint32_t seq_ctrl;
	int8_t ret;
    
    printf("�������\r\n");
    
	if(ReadSerialNumber(DeviceID,Stream,ServerAdd,ServerPort)!=0)
    {
        printf("�豸���кŶ�ȡʧ��\r\n");
        LedFlash(10,0);//��ѭ��
    }
	printf("Device:%s\r\n",DeviceID);
	printf("Stream:%s\r\n",Stream);
	printf("ServerAdd:%s\r\n",ServerAdd);
	printf("ServerPort:%s\r\n",ServerPort);
    
	if(AdcInit()==0)
    {
		printf("AdcInit Fail\r\n");
        HAL_NVIC_SystemReset();
    }
    else
    {
		AdcGetBattery(&Battery);
        if(Battery==0)
        {
            printf("����������\r\n");
            return;
        }
	}
    
	for(uint8_t i=0;i<3;i++)
	{
		LedOpen();
		HAL_Delay(200);
		LedClose();
		HAL_Delay(200);
	}
	seq_ctrl=FlashRead32(RECIEVE_SEQ);
	if(CheckFlag(DOWNLINK_CONFIRM))		//������������Ҫȷ�ϣ���ȷ����Ϣƴ���ں���
	{
		char name[CTRL_NAME_SIZE]={0};
		FlashRead16More(CTRL_NAME,(uint16_t *)name,CTRL_NAME_SIZE/2+1);  
		len += sprintf(SendData+len,"n=%s,seq=%u,state=%u\n",name,FlashRead32(RECIEVE_SEQ),FlashRead32(CTRL_STATE));//seq�յ�ʲô�ͻ�ʲô
		if(FlashRead16(CTRL_STATE)==1)	//����ִ��ʧ��(Ŀǰֻ�и�������Ż�ִ��ʧ��)����Ҫ�ѱ����seq�޸�һ��,��ֹ�������ط�ָ��ʱ��ִ�С�
		{
			seq_ctrl +=1;
			FlashWrite32(RECIEVE_SEQ,&seq_ctrl,1);
		}
		printf("Reply:%s",SendData);
		CleanFlag(DOWNLINK_CONFIRM);	//���ȷ�ϱ�־
	}
	
	//D171001TEST123,stream=PRO2_V3_170930,data=030000000000005900001ab7011b01ee,seq=2
	//D171001TEST123,stream=PRO2_V3_170930,data=030000000000005900001ab7011b01ee,seq=2
	len += sprintf(SendData+len,"D%s,stream=%s,data=",DeviceID,Stream);
	
	_12VPowerOn();
	SensorInit();
	SernsorGetData(sensor_data);
	_12VPowerOff();
	
	len += sprintf(SendData+len,"%02X",STRUCT_VERSION);
	//�ȰѴ������ĵ�ַƴ��ȥ
	for(uint8_t i=0;i<sizeof(sensor_data)/sizeof(sensor_data[0]);i++)
	{
//		printf("sensor_data[%u].index:%u\r\n",i,sensor_data[i].index);
//		printf("sensor_data[%u].add:%u\r\n",i,sensor_data[i].add);
//		printf("sensor_data[%u].count:%u\r\n",i,sensor_data[i].count);
//		printf("sensor_data[%u].data[0]:%x\r\n",i,sensor_data[i].data[0]);
//		printf("sensor_data[%u].data[1]:%x\r\n",i,sensor_data[i].data[1]);
		if(i>1)
			len += sprintf(SendData+len,"%02x",sensor_data[i].add);
	}
	
	//��ƴ�ϵ���
	len += sprintf(SendData+len,"%02x",Battery);
	//Ȼ��ƴ�Ӵ���������
	for(uint8_t i=0;i<sizeof(sensor_data)/sizeof(sensor_data[0]);i++)
	{
		for(uint8_t j=0;j<sensor_data[i].count;j++)
		{
			len += sprintf(SendData+len,"%04x",sensor_data[i].data[j]);
		}
	}
	
	//�������Ҫ,�ٰ�λ����Ϣƴ��ȥ
    if(DeviceID[strlen(DeviceID)-1]=='G')//��ʾ��GPSģ��
    {
        printf("��GPSģ��\r\n");
        if(CheckFlag(GET_LOCATION)||__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))//���յ���̨��������ϵ縴λ������ж�λ����
        {
            uint32_t TimeOut=300000;	//5���ӳ�ʱ
            uint8_t ret;
            if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
                __HAL_RCC_CLEAR_RESET_FLAGS();//�����λ��־
        
            if(GpsInit()==0)
                printf("Gps Init fail\r\n");
            else
                printf("Gps Init OK\r\n");	
            LedOpen();//��ʾ��ʼ��λ
            ret=GpsGetLocation(&Longitude,&Latitude,&TimeOut);
            if(ret==1)
            {
                printf("GpsGetLocation succeed:%u\r\n",TimeOut);
                LedClose();//��ʾ��λ�ɹ�
                len += sprintf(SendData+len,"|%.6f_%.6f",Latitude,Longitude);
            }
            else
            {
                if(ret==2)
                {
                    printf("ģ�����\r\n");
                    len += sprintf(SendData+len,"|error");
                }
                else
                {
                    printf("��λʧ��\r\n");
                    len += sprintf(SendData+len,"|0_0");
                }
            }
            GpsPowerOff();
            CleanFlag(GET_LOCATION);	//�����־λ
            
            /*******************************�����ϱ�GPSλ�õ�ʱ����*********************************/
            if(FlashRead16(SP_GPS_ADDR)==0||FlashRead16(SP_GPS_ADDR)==0xffff)
            {
                //uint16_t temp1=60*60;					//һСʱ���˴�����Ϊ��λ
                uint16_t temp1=60*1;					//һ���ӣ��˴�����Ϊ��λ
                FlashWrite16(SP_GPS_ADDR,&temp1,1);
            }
            //SetRtcAlarm(FlashRead16(SP_GPS_ADDR));
            /*******************************�����ϱ�GPSλ�õ�ʱ����*********************************/
        }
        else//����λ
        {
            printf("����λ,������һ�εĶ�λ״̬\r\n");
            len += sprintf(SendData+len,"|keep");//������һ�εĶ�λ״̬
        }
    }
    else
    {
        printf("��GPSģ��\r\n");
        len += sprintf(SendData+len,"|null");
    }
//	Latitude=113.949819;
//	Longitude=-22.550605;
//	len += sprintf(SendData+len,"|%.6f_%.6f",Latitude,Longitude);
	//���seqƴ��ȥ
	seq_send=FlashRead32(SEND_SEQ);
	if(seq_send==0)	//������0
		seq_send=1;
	len += sprintf(SendData+len,",seq=%u\r\n",seq_send);
	
	printf("SendData:%s",SendData);
	if(SimInit()==0)
    {
		printf("Sim Init fail\r\n");
        HAL_NVIC_SystemReset();
	}
    else
	{
        printf("Sim Init OK\r\n");
	}
	//if(SimConnectServer("47.90.33.7","8933")==0)
	if(SimConnectServer(ServerAdd,ServerPort)==0)
    {
		printf("Connect Server fail\r\n");
        HAL_NVIC_SystemReset();
    }
    else
    {
		printf("Connect Server OK\r\n");
	}
    
	for(uint8_t i=0;i<2;i++)
	{
		if(SimSendData(SendData)==0)
			printf("Send data Fail\r\n");
		else
		{
			printf("Send data OK\r\n");
			if(i==0)
			{
				uint32_t temp=seq_send+1;
				FlashWrite32(SEND_SEQ,&temp,1);//���͵�seq����1
			}
			if(IsAck(DeviceID,Stream,&seq_send)==0)//�������ظ��ɹ�
			{
				break;
			}
		}
	}
	
	ret=IsControl(DeviceID,Stream,&seq_ctrl);
	if(ret!=-1)
	{
		uint32_t a=ret;
		FlashWrite32(CTRL_STATE,&a,1);
	}
	
	if(FlashRead32(SAMPLE_PERIOD_ADDR)==0||FlashRead32(SAMPLE_PERIOD_ADDR)==0xffffffff)
	{
//		uint32_t time=300;//Ĭ��300��
		uint32_t time=60;//Ĭ��300��
		FlashWrite32(SAMPLE_PERIOD_ADDR,&time,1);
	}
	
	if(CheckFlag(SOFT_RESET))
	{
		CleanFlag(SOFT_RESET);
		printf("��λ\r\n");
		HAL_Delay(100);
		HAL_NVIC_SystemReset();
	}
	
/**************���������ݻ�ȡ����*************/
}

/*
 *ResendToSim:�����ڽ��յ�������ת����SIMģ��
*/
void ResendToSim(void)
{
	Uart1ReBuff[Uart1ReCount++]=Uart1ReChar[0];		//������յ�������
	printf("%c",Uart1ReChar[0]);
	
	if ( Uart1ReCount>=4 && Uart1ReBuff[Uart1ReCount-2]=='\r' && Uart1ReBuff[Uart1ReCount-1]=='\n' )	// �յ�"\r\n"����������ظ�
	{
		HAL_UART_Transmit(&hlpuart1,(uint8_t *)Uart1ReBuff,Uart1ReCount,0xffff);
		Uart1ReCount = 0;
	}
	HAL_UART_Receive_IT(&huart1,(uint8_t *)Uart1ReChar,1);//���¿���uart1���ж�ʹ��
}

void ResendToGps(void)
{
	Uart1ReBuff[Uart1ReCount++]=Uart1ReChar[0];		//������յ�������
	//printf("%02x",Uart1ReChar[0]);
	
	//if ( Uart1ReCount>=4 && Uart1ReBuff[Uart1ReCount-2]=='\r' && Uart1ReBuff[Uart1ReCount-1]=='\n' )	// �յ�"\r\n"����������ظ�
	{
		HAL_UART_Transmit(&huart5,(uint8_t *)Uart1ReBuff,Uart1ReCount,0xffff);
		Uart1ReCount = 0;
	}
	HAL_UART_Receive_IT(&huart1,(uint8_t *)Uart1ReChar,1);//���¿���uart1���ж�ʹ��
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle==&huart1)				//��������ӣ����ڵ���
    {
		//ResendToSim();
		//ResendToGps();
	}
	else if(UartHandle==&hlpuart1)		//sim800ģ��
	{
		SimCheckReply();
	}
	else if(UartHandle==&huart2)		//GPSģ��
	{
		GpsCheckReply();
	}
	else if(UartHandle==&huart5)		//Rs485ģ��
	{
		Rs485CheckReply();
	}
}



