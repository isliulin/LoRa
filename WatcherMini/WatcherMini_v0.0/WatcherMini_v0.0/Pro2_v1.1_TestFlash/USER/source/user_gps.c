/*
 * user_gps.c	gpsģ�������ļ�
*/

#include "user_gps.h"

extern UART_HandleTypeDef huart2;

char GpsReChar[1]={0};					//���յ�һ���ֽ�
char GpsReBuff[GPS_RECIEVE_COUNT_MAX]={0};	//���յ�������
char LocationData[GPS_RECIEVE_COUNT_MAX]={0};	//���յ�������
//char GpsTxBuff[GPS_RECIEVE_COUNT_MAX]={0};	//���͵�����
uint16_t GpsReCount=0;					//���յ����ֽ���
uint8_t GpsReData;


/*
 *GpsInit: 		��ȡADCĳͨ���ϵĵ�ѹֵ,��Ե�ѹ�����ڲ���ѹ
 *����:			��		
 *����ֵ:		0ʧ�� 1�ɹ�
*/
uint8_t  GpsInit(void)
{
	//MX_USART2_UART_Init();
	GpsPowerOn();
	HAL_UART_Receive_IT(&huart2,(uint8_t *)GpsReChar,1);//�򿪴��ڽ����ж�
	return 1;
}

/*
 *GpsPowerOff: 	�ر�GPS��Դ
 *����:			��		
 *����ֵ:		0ʧ�� 1�ɹ�
*/
uint8_t  GpsPowerOff(void)
{
	HAL_GPIO_WritePin(OUT_GPS_Power_On_GPIO_Port,OUT_GPS_Power_On_Pin,GPIO_PIN_RESET);
	return 1;
}

/*
 *GpsPowerOn: 	��GPS��Դ
 *����:			��		
 *����ֵ:		0ʧ�� 1�ɹ�
*/
uint8_t  GpsPowerOn(void)
{
    HAL_GPIO_WritePin(OUT_SIM_Power_ON_GPIO_Port, OUT_SIM_Power_ON_Pin, GPIO_PIN_SET);//��չС����ܵ�Դ����
	HAL_GPIO_WritePin(OUT_GPS_Power_On_GPIO_Port,OUT_GPS_Power_On_Pin,GPIO_PIN_SET);
	return 1;
}


/*
 * GpsCheckReply:	���Gpsģ��Ļظ�״̬����Ҫ�����޸�AtCmdStruct�ṹ���AtRlplyState
 * ����:			��
 * ����ֵ:			��
*/
void GpsCheckReply(void)
{
	if(GpsReCount==GPS_RECIEVE_COUNT_MAX)			//�������֮ǰ������
		GpsReCount=0;
	GpsReBuff[GpsReCount++]=GpsReChar[0];		//������յ�������
	//DEBUG("%c",GpsReChar[0]);
	
	if ( GpsReBuff[GpsReCount-2]=='\r' && GpsReBuff[GpsReCount-1]=='\n' )	// �յ�"\r\n"����������ظ�
	{
		char *pt=NULL;
		if (GpsReCount>15)//
		{
			pt = strstr(GpsReBuff,"$GPRMC");//ֻ����GPRMC����Ϣ
			if(pt!=NULL)
			{
				memcpy(LocationData,GpsReBuff,GpsReCount);
			}			
		}
		GpsReCount = 0;
	}
	HAL_StatusTypeDef ret=HAL_UART_Receive_IT(&huart2,(uint8_t *)GpsReChar,1);//���´򿪴��ڽ����ж�
	if(ret!=HAL_OK)//���´򿪴��ڽ����ж�
    {
        DEBUG("������������:%x,%x\r\n",ret,HAL_UART_GetError(&huart2));
        if (HAL_UART_Init(&huart2) != HAL_OK)
        {
            while(1);//�������Ź��Ḵλ
        }
        do
        {
            ret = HAL_UART_Receive_IT(&huart2,(uint8_t *)GpsReChar,1);
        }while(ret!=HAL_OK);//�������Ź��Ḵλ
    }
}
/*
 * SimGpsGetLocation:	��ȡ��γ��(��ʼ��λ)
 * Longitude:			���ȣ������ַ��������ʼ��ַ,����ִ�гɹ��������ᱻ�޸�
 * Latitude:			γ�ȣ������ַ��������ʼ��ַ,����ִ�гɹ��������ᱻ�޸�
 * time:				��λ�õ�ʱ��,��ʱʱ��,����λ�ɹ�������ʵ�ʵĶ�λʱ��
 * ����ֵ:				1�ɹ� 0ʧ�� 2��ʾ��GPS����GPSģ�����
*/
uint8_t GpsGetLocation(double *Longitude,double *Latitude,uint32_t *time)
{
	nmea_msg gpsx;
    uint16_t count=0;
	uint32_t StartTime=HAL_GetTick();
    uint32_t StartTime1=HAL_GetTick();
	while(1)
	{
		if(LocationData[0]!=0)
		{
            StartTime1=HAL_GetTick();
			DEBUG("Get Data:%s",LocationData);
			if(strstr(LocationData,",A,")!=NULL)//	��,A,����ʾ��λ�ɹ�
			{
				DEBUG("count:%u\r\n",count);
                count++;
                if( count>30 )
                    break;
                memset(LocationData,0,sizeof(LocationData));
			}
			else
			{
				memset(LocationData,0,sizeof(LocationData));
				if(HAL_GetTick()-StartTime>*time)//��λ��ʱ
				{
					break;
				}
			}
		}
        else if(HAL_GetTick()-StartTime1>3000)//3����û���յ�GPSģ�����Ϣ�ظ�����Ϊ��λģ�����
		{
			//DEBUG("��GPSģ���GPSģ�����\r\n");
			return 2;
		}
	}
	if(LocationData[0]==0)
	{
		return 0;
	}
	//unsigned char *LocationData="$GPRMC,023543.00,A,2308.28715,N,11322.09875,E,0.195,,240213,,,A*78";
	//unsigned char *LocationData="$GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50";
//	DEBUG("Get Data:%s\r\n",LocationData);
//	GPS_Analysis(&gpsx,(u8*)GpsReBuff);	//�����ַ���
//	NMEA_GPGSV_Analysis(&gpsx,LocationData);	//GPGSV����
//	NMEA_GPGGA_Analysis(&gpsx,LocationData);	//GPGGA���� 	
//	NMEA_GPGSA_Analysis(&gpsx,LocationData);	//GPGSA����
	NMEA_GPRMC_Analysis(&gpsx,(u8*)LocationData);	//GPRMC����
//	NMEA_GPVTG_Analysis(&gpsx,LocationData);	//GPVTG����
	Gps_Msg_Show(gpsx);					//��ʾ��Ϣ
	//NMEA_GPRMC_Analysis����γ������100000��
	if(gpsx.ewhemi=='W')//����
		*Longitude=(double)gpsx.longitude/-100000.0;
	else
		*Longitude=(double)gpsx.longitude/100000.0;
		
	if(gpsx.nshemi =='S')//��γ
		*Latitude=(double)gpsx.latitude/-100000.0;
	else
		*Latitude=(double)gpsx.latitude/100000.0;
	*time=HAL_GetTick()-StartTime;

//	*Latitude=-123.456;
//	*Longitude=123.456;
	return 1;
}
void Gps_Msg_Show(nmea_msg gpsx)
{
 	float tp;		   
	char dtbuf[50];
	char * fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ��� 
	tp=gpsx.longitude;	   
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
 	DEBUG("%s\r\n",dtbuf);	 	   
	tp=gpsx.latitude;	   
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
 	DEBUG("%s\r\n",dtbuf);	 	 
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
 	DEBUG("%s\r\n",dtbuf);	 			   
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���	 
 	DEBUG("%s\r\n",dtbuf);	 				    
	if(gpsx.fixmode<=3)														//��λ״̬
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
		DEBUG("%s\r\n",dtbuf);			   
	}	 	   
	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��������
 	DEBUG("%s\r\n",dtbuf);	    
	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�������
 	DEBUG("%s\r\n",dtbuf);		 
	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
	DEBUG("%s\r\n",dtbuf);		    
	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
  	DEBUG("%s\r\n",dtbuf);		  
}

//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������							  
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
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
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
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
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p,*p1,dx;
	u8 len,i,j,slx=0;
	u8 posx;   	 
	p=buf;
	p1=(u8*)strstr((const char *)p,"$GPGSV");
	len=p1[7]-'0';								//�õ�GPGSV������
	posx=NMEA_Comma_Pos(p1,3); 					//�õ��ɼ���������
	if(posx!=0XFF)gpsx->svnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(u8*)strstr((const char *)p,"$GPGSV");  
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
//����GPGGA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"$GPGGA");
	posx=NMEA_Comma_Pos(p1,6);								//�õ�GPS״̬
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);								//�õ����ڶ�λ��������
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);								//�õ����θ߶�
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);  
}
//����GPGSA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx; 
	u8 i;   
	p1=(u8*)strstr((const char *)buf,"$GPGSA");
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
//����GPRMC��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;	   
	float rs;  
	p1=(u8*)strstr((const char *)buf,"GPRMC");//"$GPRMC",������&��GPRMC�ֿ������,��ֻ�ж�GPRMC.
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
//����GPVTG��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"$GPVTG");							 
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
void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
{
	NMEA_GPGSV_Analysis(gpsx,buf);	//GPGSV����
	NMEA_GPGGA_Analysis(gpsx,buf);	//GPGGA���� 	
	NMEA_GPGSA_Analysis(gpsx,buf);	//GPGSA����
	NMEA_GPRMC_Analysis(gpsx,buf);	//GPRMC����
	NMEA_GPVTG_Analysis(gpsx,buf);	//GPVTG����
}

//GPSУ��ͼ���
//buf:���ݻ������׵�ַ
//len:���ݳ���
//cka,ckb:����У����.
void Ublox_CheckSum(u8 *buf,u16 len,u8* cka,u8*ckb)
{
	u16 i;
	*cka=0;*ckb=0;
	for(i=0;i<len;i++)
	{
		*cka=*cka+buf[i];
		*ckb=*ckb+*cka;
	}
}

