/*
**************************************************************************************************************
*	@file	iap_protocol.c
*	@author 
*	@version 
*	@date    
*	@brief	��������In-Application-ProgrammingЭ��
***************************************************************************************************************
*/
#include "iap_protocol.h"
#include "user_sim.h"

// IAP�ṹ��
static HexObjFile_t sg_hex_obj_file = {0};

#define IAP_BUFF_THRESHOLD	10240	// ������ֵ�������ճ��ȴ��ڸ���ֵ���յ����б�ʶ�󣬿�ʼ����
#define IAP_BUFF_LENGTH		(IAP_BUFF_THRESHOLD+64) // ���ջ���������
static char sg_iap_buff[IAP_BUFF_LENGTH] = {0};	 // IAP������
static uint32_t sg_iap_buff_pt = 0;		// ��������ָ�룬����ǰ���յ������ݳ���
static uint32_t sg_iap_total_len = 0;	// �Ѿ�׼���õĻ��������ܳ���
volatile uint32_t sg_iap_buff_ready = 0;	// ��ǰ�Ƿ����Ѿ�׼���õĻ�����


/*
 *	InitGprsIAP:	��ʼ��GPRS���߱��
 *	������			��
 *	����ֵ��		��	
 */

//#ifdef VECT_TAB_SRAM
//  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
//#else
//  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
//#endif
 
extern void InitGprsIAP(void)
{
	InitHexObjFile(&sg_hex_obj_file);

	sg_iap_buff_pt = 0;
	sg_iap_buff_ready = 0;
	sg_iap_buff_ready = 0;
}

/*
 *	UpdateProgramFromGprs:	ͨ��GPRS������������
 *	������					��
 *	����ֵ��				IAP_SUCCESS		--�ɹ�
 *							IAP_FAILURE		--ʧ��
 *							IAP_TIMEOUT		--��ʱ
 */
extern int32_t UpdateProgramFromGprs(void)
{
	uint16_t Count=0;
	uint32_t start_time = HAL_GetTick();
	// �ȴ�IAP����������2min��ʱ
	HAL_FLASH_Unlock();	//����
	while (sg_hex_obj_file.status == IAP_CONTINUE && HAL_GetTick()<start_time+3600000)
	{
		uint32_t total_len, data_pt;
		char *buff;
        sg_iap_buff_pt=0;
        memset(sg_iap_buff,0,sizeof(sg_iap_buff));
        SimHttpSetBreak(Count*IAP_BUFF_THRESHOLD);
        SimHttpSetBreakEnd((Count+1)*IAP_BUFF_THRESHOLD+HEX_DATA_LEN-3);//hex�ļ�һ�����45���ַ�(��\r\n)����42�Ƿ�ֹ���ݶϲ㣬ͬʱҲ��ֹ���һ��
        SimHttpGet();
        if(SimHttpReadData()==0)
        {   
            printf("���ݻ�ȡʧ��,����url�Ƿ���ȷ\r\n");
            return IAP_FAILURE;
        }
		while (sg_iap_buff_ready == 0);//�ȴ����ݻ�ȡ���
        HAL_Delay(100);//�ȴ���������������������ֹ��¼��ʱ���������
		printf("%s",sg_iap_buff);
		// ������׼�����
		sg_iap_buff_ready = 0;
		data_pt   = 0;
		total_len = sg_iap_total_len;
		buff = (char *)sg_iap_buff;
		
//		printf("%d,%d\r\n",Count,total_len);//�ָ�һ�λ���������
		Count++;
		// �����λ�����������
		while (data_pt < total_len)
		{
			// ����һ������
			ParseHexObjFile(&sg_hex_obj_file, buff + data_pt);
			if (sg_hex_obj_file.status == IAP_FAILURE)
			{
			    printf("�������ش���,HEX�ļ�������������ض�ʧ����\r\n");
				return IAP_FAILURE;
			}
			// Ѱ����һ��
			while(buff[data_pt++]!='\n' && data_pt<total_len);
		}			
	}
	HAL_FLASH_Lock();//����
	HAL_Delay(5);
	if(HAL_GetTick()>start_time+300*1000)
	{
		printf("���ӳ�ʱ,����ʧ��,�鿴������ַ�Ƿ���ȷ\r\n");
		return IAP_TIMEOUT;
	}else{
		printf("���½���\r\n");
	}
	return sg_hex_obj_file.status;
}

//�˴�������ŵ�RAM�����У�������û���޸�.sct�ļ���ʵ���ϲ�û����RAM������
#pragma arm section code = "RAMCODE"
/*
 *	HandleIAPData:	����IAP����
 *	p��				�������ָ��
 *	len:			���ݳ���
 *	����ֵ��		��
 */
extern void HandleIAPData(char *p, uint32_t len)
{
	char *iap_buff;
	uint32_t new_length;
	// hex�ļ�һ�����50�ֽ����ң����Բ�����һ�³�������������
	new_length = len + sg_iap_buff_pt;
    
    if(*p!=':')//�����նϲ�����
        return;
    if (new_length > IAP_BUFF_LENGTH)
	{
		return;
	}
    if(sg_iap_buff_ready==1)//��������ݲ�����
    {
        return ;
    }

	// ��������
	iap_buff = (char *)sg_iap_buff;
	memcpy(iap_buff + sg_iap_buff_pt, p, len);
	
	sg_iap_buff_pt = new_length;

	
	// ���յ������ݳ�����ֵ�����߽��յ�������־������λ���������
	if (new_length > IAP_BUFF_THRESHOLD || strncmp(iap_buff+sg_iap_buff_pt-13, ":00000001FF", 11)==0)
	{
//		if(new_length > IAP_BUFF_THRESHOLD)
//		{
//			printf("recieve overflow\r\n");
//		if(strncmp(iap_buff+sg_iap_buff_pt-13, ":00000001FF", 11)==0)
//			printf("recieve all \r\n");
//		}
        //printf("������һ������\r\n");
		sg_iap_total_len=sg_iap_buff_pt;
		sg_iap_buff_pt = 0;
		sg_iap_buff_ready = 1;
	}
}
#pragma arm section


/*
 *	GetIAPStatus:	��ȡIAP״̬
 *	����ֵ��		IAP״̬
 *					IAP_FAILURE    --IAPʧ��
 *					IAP_CONTINUE   --IAP������
 *					IAP_SUCCESS	   --IAP�ɹ�����
 * 					IAP_UNKNOWN_CMD--��IAP����
 */
extern int32_t GetIAPStatus(void)
{
	return sg_hex_obj_file.status;
}
///*
// *	GetURL:				��Flash�е�URL��ȡ������ͬʱ����ز������ú�
// *	data:				����http�ļ�������
// *	server:				URL�еķ�������ַ
// *	����ֵ��			1--�ɹ� -1--ʧ��				
// */
//int8_t GetURL(char *data,char *server)
//{	
//	char url[101]={0},*addr;			   
//	FlashReadChar(URL,(char *)url,100); 
//	if(url[0]!=0)
//	{
//		url[100] = 0;
//		addr = strstr(url+7,"/");//���˵�ǰ���8���ַ�: htttp://,�ҵ�֮��ĵ�һ��'/'
//		if(addr>0){
//			strncpy(server,url+7,addr-url-7);
//			sprintf(data,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",addr,server);	
//			printf("%s",data);
//			return 1;
//		}else{
//			FlashIncrease32(URL_ERROR);
//			printf("url error (can not found'/'):%s\r\n",url);	//url�Ҳ���'/'
//		}	
//	}else{
//		FlashIncrease32(URL_EMPTY);
//		printf(",url error(there isn't url):%s\r\n",url);		//û��url,urlΪ��
//	}
//	FlashIncrease32(URL_FAIL);
//	return -1; 
//}
