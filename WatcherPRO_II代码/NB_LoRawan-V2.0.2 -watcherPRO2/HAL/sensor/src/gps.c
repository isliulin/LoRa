#include "gps.h" 					   
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h" 
#include "gpio.h" 
#include "stm32l0xx_hal.h"


#define MTK_COULD				"$PMTK103*30\r\n"
#define MTK_HOST       			"$PMTK101*32\r\n"

#define MTK_POS_FIX				"$PMTK220,1000*1F\r\n" //  $PMTK220,3000*1D
#define MTK_GLL					"$PMTK314,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0*28\r\n"

TimerEvent_t GPSTimer;

void OnGpsTimerEvent( void )
{  
	if(!Set_Gps_Ack.GPS_DONE)
	{
		DEBUG(3,"OnGpsTimerEvent\r\n");
		Location_Led(  );
		TimerStop( &GPSTimer );
		TimerSetValue( &GPSTimer, 500000 ); 
		TimerStart( &GPSTimer );
	}
	else
	{
		TimerStop( &GPSTimer );
		HAL_GPIO_WritePin(LORA_LED, LORA_LED_PIN, GPIO_PIN_RESET);
	}
}

nmea_msg gpsx; 											//GPS��Ϣ

void GPS_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��

	GPIO_Initure.Pin=GPS_IO_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPS_IO,&GPIO_Initure);
}

void GPS_Enable(void)
{
	HAL_GPIO_WritePin(GPS_IO,GPS_IO_PIN,GPIO_PIN_SET);
}

void GPS_Disable(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��

	GPIO_Initure.Pin=GPS_IO_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;  //�������
	GPIO_Initure.Pull=GPIO_NOPULL;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPS_IO,&GPIO_Initure);

	HAL_GPIO_WritePin(GPS_IO,GPS_IO_PIN,GPIO_PIN_SET);
}


/*
*Gps_Set: ����GPS
*���أ��ɹ���1  ʧ�ܣ�0
*/
uint8_t Gps_Set(void)
{
	GPS_Enable( );	
    uint32_t GPS_TIME = 0;
	
	do
	{		
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		HAL_UART_Transmit(&huart2, (uint8_t *)MTK_COULD, sizeof(MTK_COULD), 0xFFFF);
		HAL_NVIC_EnableIRQ(USART2_IRQn);		
		HAL_Delay(1000);
        GPS_TIME ++;
		DEBUG(2,"line = %d\r\n",__LINE__);
	}
	while(!Set_Gps_Ack.START && ( GPS_TIME <= 3 ));
    
    Set_Gps_Ack.Get_PATION_TIME = HAL_GetTick();
	
	if(!Set_Gps_Ack.START && ( GPS_TIME > 3 ))
	{
        if(LoRapp_SenSor_States.Hardware_Exist_GPS)
        {
            RF_Send_Data.GPS = 0x30; ///GPSģ���쳣
            DEBUG(2,"Hardware_Exist_GPS ERROR\r\n");
        }
        
		return 0;
	}	
	if(Set_Gps_Ack.START)
	{
		do
		{
			HAL_NVIC_DisableIRQ(USART2_IRQn);
			HAL_UART_Transmit(&huart2, (uint8_t *)MTK_GLL, sizeof(MTK_GLL), 0xFFFF);
			HAL_NVIC_EnableIRQ(USART2_IRQn);		
			HAL_Delay(200);	
			DEBUG(3,"line = %d\r\n",__LINE__);
		}while(!Set_Gps_Ack.GPGLL);
		
		do
		{
			HAL_NVIC_DisableIRQ(USART2_IRQn);
			HAL_UART_Transmit(&huart2, (uint8_t *)MTK_POS_FIX, sizeof(MTK_POS_FIX), 0xFFFF);
			HAL_NVIC_EnableIRQ(USART2_IRQn);	
			HAL_Delay(200);	
			DEBUG(3,"line = %d\r\n",__LINE__);
		}while(!Set_Gps_Ack.POS_FIX);
		
		PowerDisble_Led(  );		
		TimerStop( &GPSTimer );
		TimerSetValue( &GPSTimer, 500000 );  ///GPS״̬��
		TimerStart( &GPSTimer );
		return 1;
	}
	
	return 0;
}

/*
*Get_Gps_Position: ��ȡGPS����,������
*���أ��ɹ���1  ʧ�ܣ�0
*/
void Get_Gps_Position(void)
{	
	if((((LoRaMacGetUpLinkCounter( ) % (LoRapp_SenSor_States.Work_Time)) == 0) || Set_Gps_Ack.Get_PATION_Again) && !Set_Gps_Ack.START)  ///����Ԥ��GPS�ϱ�ʱ�䣬����ʹ��GPSģ��
	{
		DEBUG(2,"line = %d time = %d\r\n",__LINE__, Set_Gps_Ack.GPS_OVER_TIME);
		Set_Gps_Ack.GPS_DONE = false;
		Set_Gps_Ack.START = false;
		Set_Gps_Ack.POS_FIX = false;
		Set_Gps_Ack.GPGLL	= false;      
        Set_Gps_Ack.Get_PATION_Again = false;
		GPS_Init(  );
		Gps_Set(  );	
        Set_Gps_Ack.GPS_OVER_TIME = HAL_GetTick( );
	}			
 
     if(Set_Gps_Ack.Get_PATION && Set_Gps_Ack.START) ///�ж��Ƿ���GPS����ȡGPS��Ϣ  
     {	        
        Set_Gps_Ack.Get_PATION = false; ///�ر�GPSλ����Ϣ
        ReportTimerEvent = true;
        NMEA_GPGLL_Analysis(&gpsx, (uint8_t *)Set_Gps_Ack.GLL); ///��γ��  "$GPGLL,2233.1773,N,11356.7148,E,094100.210,A,A*5E\r\n"
        DEBUG(3,"11---%.5f %1c, %.5f %1c\r\n", (double)gpsx.latitude,gpsx.nshemi, (double)gpsx.longitude,gpsx.ewhemi);
        DEBUG(2,"22---%.5f %1c, %.5f %1c\r\n", (double)gpsx.latitude/100000,gpsx.nshemi, (double)gpsx.longitude/100000,gpsx.ewhemi);
     
        RF_Send_Data.Send_Buf[RF_Send_Data.RX_LEN] = 'J';
        uint32_t temp[4] = {gpsx.latitude,gpsx.nshemi,gpsx.longitude,gpsx.ewhemi};
        uint8_t i;
        for(i = 0 ,RF_Send_Data.RX_LEN = 1; RF_Send_Data.RX_LEN <= 10; i++)
        {
            if(i%2==1)
            {
                RF_Send_Data.Send_Buf[RF_Send_Data.RX_LEN++] = (temp[i])&0xff;	///��γ�ַ�
            }					
            else
            {
                RF_Send_Data.Send_Buf[RF_Send_Data.RX_LEN++] = (temp[i] >> 24)&0xff;
                RF_Send_Data.Send_Buf[RF_Send_Data.RX_LEN++] = (temp[i] >> 16)&0xff;
                RF_Send_Data.Send_Buf[RF_Send_Data.RX_LEN++] = (temp[i] >> 8)&0xff;
                RF_Send_Data.Send_Buf[RF_Send_Data.RX_LEN++] = (temp[i])&0xff;			
            }						
        }	
        do
        {								
             User_send(CONFIRMED, RF_Send_Data.Send_Buf);
        }
        while(!LoRapp_SenSor_States.loramac_evt_flag);
        __disable_irq();
        LoRapp_SenSor_States.loramac_evt_flag = 0;
        __enable_irq();
        
        ReportTimerEvent = false;  
        RF_Send_Data.Get_sensor = false;
        Set_Gps_Ack.START = false;
        
        TimerStop( &ReportTimer );
        TimerSetValue( &ReportTimer, 10000000 + randr( -1000000, 1000000 ) );
        TimerStart( &ReportTimer );	
        
        RF_Send_Data.GPS = 0x10;		///��λ�ɹ�
        Set_Gps_Ack.GPS_DONE = true; ///GPS������ɱ��	 
        system_time = HAL_GetTick( ); ///����GPSռ��ʱ��
        gpsx.gpssta = 1;
     } 
     else if(((HAL_GetTick( ) - Set_Gps_Ack.GPS_OVER_TIME) > 300000) && (Set_Gps_Ack.START && !Set_Gps_Ack.Get_PATION ))  ///GPS 5�����ڶ�λʧ�ܣ�Ĭ��GPS�쳣���ٶ�λ 300000
     {
        DEBUG(2,"GPS_TIME22 : %d\r\n",HAL_GetTick( ) - Set_Gps_Ack.GPS_OVER_TIME);
         
        GPS_Disable(  ); ///�ر�GPS
         
        Set_Gps_Ack.START = false;

        memset(Set_Gps_Ack.GLL, 0, strlen(Set_Gps_Ack.GLL));
        RF_Send_Data.GPS = 0x20; ///��λʧ��
        Set_Gps_Ack.GPS_DONE = true; ///GPS������ɱ��	 
        RF_Send_Data.Get_sensor = true;
        ReportTimerEvent = true;
        system_time = HAL_GetTick( ); ///����GPSռ��ʱ�� 
        gpsx.gpssta = 0;
     }
     
     else if(!Set_Gps_Ack.GPS_DONE && !Set_Gps_Ack.START)  ///������GPS����Ҫ����ֻ����һ������
     {
        GPS_Disable(  ); ///�ر�GPS

        memset(Set_Gps_Ack.GLL, 0, strlen(Set_Gps_Ack.GLL));
         
         if(LoRapp_SenSor_States.Hardware_Exist_GPS)
         {
            RF_Send_Data.GPS = 0x30; ///GPSģ���쳣
            DEBUG(2,"Hardware_Exist_GPS ERROR\r\n");
         }
         else
        RF_Send_Data.GPS = 0x00; ///GPSû����
         
        Set_Gps_Ack.GPS_DONE = true; ///GPS������ɱ��	
        RF_Send_Data.Get_sensor = true;
        ReportTimerEvent = true;
        TimerStop( &GPSTimer );
        HAL_GPIO_WritePin(LORA_LED, LORA_LED_PIN, GPIO_PIN_RESET); 
        system_time = HAL_GetTick( ); ///����GPSռ��ʱ��	 
     }
}

void Location_Led(void)
{
	HAL_GPIO_TogglePin(LORA_LED,LORA_LED_PIN);
}


const uint32_t BAUD_id[9]={4800,9600,19200,38400,57600,115200,230400,460800,921600};//ģ��֧�ֲ���������
//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������							  
uint8_t NMEA_Comma_Pos(uint8_t *buf,uint8_t cx)
{	 		    
	uint8_t *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n����
//����ֵ:m^n�η�.
uint32_t NMEA_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}
//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ
int NMEA_Str2num(uint8_t *buf,uint8_t*dx)
{
	uint8_t *p=buf;
	uint32_t ires=0,fres=0;
	uint8_t ilen=0,flen=0,i;
	uint8_t mask=0;
	int res;
	while(1) //�õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
		if(*p==','||(*p=='*'))break;//����������
		if(*p=='.'){mask|=0X01;p++;}//����С������
		else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//ȥ������
	for(i=0;i<ilen;i++)	//�õ�������������
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//���ȡ5λС��
	*dx=flen;	 		//С����λ��
	for(i=0;i<flen;i++)	//�õ�С����������
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 
//����GPGSV��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p,*p1,dx;
	uint8_t len,i,j,slx=0;
	uint8_t posx;   	 
	p=buf;
	p1=(uint8_t*)strstr((const char *)p,"$GPGSV");
	len=p1[7]-'0';								//�õ�GPGSV������
	posx=NMEA_Comma_Pos(p1,3); 					//�õ��ɼ���������
	if(posx!=0XFF)gpsx->svnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(uint8_t*)strstr((const char *)p,"$GPGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);	//�õ����Ǳ��
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);//�õ��������� 
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);//�õ����Ƿ�λ��
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	//�õ����������
			else break;
			slx++;	   
		}   
 		p=p1+1;//�л�����һ��GPGSV��Ϣ
	}   
}
//����BDGSV��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_BDGSV_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p,*p1,dx;
	uint8_t len,i,j,slx=0;
	uint8_t posx;   	 
	p=buf;
	p1=(uint8_t*)strstr((const char *)p,"$BDGSV");
	len=p1[7]-'0';								//�õ�BDGSV������
	posx=NMEA_Comma_Pos(p1,3); 					//�õ��ɼ�������������
	if(posx!=0XFF)gpsx->beidou_svnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(uint8_t*)strstr((const char *)p,"$BDGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->beidou_slmsg[slx].beidou_num=NMEA_Str2num(p1+posx,&dx);	//�õ����Ǳ��
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->beidou_slmsg[slx].beidou_eledeg=NMEA_Str2num(p1+posx,&dx);//�õ��������� 
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->beidou_slmsg[slx].beidou_azideg=NMEA_Str2num(p1+posx,&dx);//�õ����Ƿ�λ��
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->beidou_slmsg[slx].beidou_sn=NMEA_Str2num(p1+posx,&dx);	//�õ����������
			else break;
			slx++;	   
		}   
 		p=p1+1;//�л�����һ��BDGSV��Ϣ
	}   
}
//����GNGGA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNGGA_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;    
	p1=(uint8_t*)strstr((const char *)buf,"$GNGGA");
	posx=NMEA_Comma_Pos(p1,6);								//�õ�GPS״̬
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);								//�õ����ڶ�λ��������
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);								//�õ����θ߶�
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);  
}
//����GNGSA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNGSA_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx; 
	uint8_t i;   
	p1=(uint8_t*)strstr((const char *)buf,"$GNGSA");
	posx=NMEA_Comma_Pos(p1,2);								//�õ���λ����
	if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);	
	for(i=0;i<12;i++)										//�õ���λ���Ǳ��
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}				  
	posx=NMEA_Comma_Pos(p1,15);								//�õ�PDOPλ�þ�������
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,16);								//�õ�HDOPλ�þ�������
	if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,17);								//�õ�VDOPλ�þ�������
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}
//����GNRMC��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNRMC_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;     
	uint32_t temp;	   
	float rs;  
	p1=(uint8_t*)strstr((const char *)buf,"$GNRMC");//"$GNRMC",������&��GNRMC�ֿ������,��ֻ�ж�GPRMC.
	posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=NMEA_Comma_Pos(p1,3);								//�õ�γ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p1,4);								//��γ���Ǳ�γ 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//�õ�����
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p1,6);								//������������
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,9);								//�õ�UTC����
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//�õ�UTC����
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
}

//����GPGLL��Ϣ   $GPGLL,2233.1493,N,11356.6989,E,072246.000,A,A*5D
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGLL_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;     
	uint32_t temp;	   
	float rs;  
	p1=(uint8_t*)strstr((const char *)buf,"$GPGLL");//"$GPGLL",������&��GNRMC�ֿ������,��ֻ�ж�GPGLL.
	
	posx=NMEA_Comma_Pos(p1,1);								//�õ�γ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p1,2);								//��γ���Ǳ�γ 
	
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);	
	
 	posx=NMEA_Comma_Pos(p1,3);								//�õ�����
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p1,4);								//������������
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		

	posx=NMEA_Comma_Pos(p1,5);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}		
}

//����GNVTG��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNVTG_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;    
	p1=(uint8_t*)strstr((const char *)buf,"$GNVTG");							 
	posx=NMEA_Comma_Pos(p1,7);								//�õ���������
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//ȷ������1000��
	}
}  
//��ȡNMEA-0183��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void GPS_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	NMEA_GPGSV_Analysis(gpsx,buf);	//GPGSV����
	NMEA_BDGSV_Analysis(gpsx,buf);	//BDGSV����
	NMEA_GNGGA_Analysis(gpsx,buf);	//GNGGA���� 	
	NMEA_GNGSA_Analysis(gpsx,buf);	//GPNSA����
	NMEA_GNRMC_Analysis(gpsx,buf);	//GPNMC����
	NMEA_GNVTG_Analysis(gpsx,buf);	//GPNTG����
}
