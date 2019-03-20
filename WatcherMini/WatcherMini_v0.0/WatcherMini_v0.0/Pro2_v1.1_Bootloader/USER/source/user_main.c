
#include "user_main.h"
#include "user_flash_L072.h"
#include "bootloader_config.h"
#include "iap_protocol.h"
#include "parse_hex_file.h"
#include "user_sim.h"
#include <stdio.h>
#include <string.h>

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
/*
 * δ������жϣ�����ʹ��
 * index==strlen(str)+1ʱ����ʾ��ĩβ����
*/
void InsertChar(char *str,uint8_t index,char *ch)
{
    uint16_t i;
    uint16_t Oldlen=strlen(str);
    uint16_t Newlen=strlen(str)+strlen(ch);
    if(index>strlen(str)+1)
        return;
    for(i=0;i<Oldlen-index+1;i++)
    {
        str[Newlen-1-i]=str[Oldlen-i-1];
    }
    for(i=0;i<strlen(ch);i++)
    {
        str[index-1+i]=ch[i];
    }
    str[Newlen+1]='\0';
    //printf("������ַ���:%s \n\n",str);
}


void CheckUpgrade()
{
	char url[128];
	//char server[50];
	int32_t	returnvalue,succeed=0;
	uint32_t time,update_retry=0;
	update_retry=FlashRead32(UPDATE_RETRY);
	if ( CheckFlag(UPDATE_RESET) )	// �յ���������
	{	
        HAL_GPIO_WritePin(Out_LED_GPIO_Port,Out_LED_Pin, GPIO_PIN_SET);
		CleanFlag(UPDATE_RESET);
		printf("Start Update...%d\r\n",update_retry);
        uint32_t a=1;//Ĭ�ϸ���ʧ��
        FlashWrite32(CTRL_STATE,&a,1);
		FlashIncrease32(BOOT_TOTAL);						//�ܵ���д��
		if(update_retry==0)									//���յ���������									
			FlashIncrease32(BOOT_CMD_TOTAL);
        //FlashWrite16(URL, (uint16_t *)"http://down.nongbotech.cn/WatcherStandarApplication.hex", strlen("http://down.nongbotech.cn/WatcherStandarApplication.hex") / 2 + 1);
		FlashReadChar(URL,url,100);
        uint16_t APP_AREA=FlashRead16(APP_AREA_ADD);
        uint8_t len = strlen(url);
        if(APP_AREA==APP_AREA_B)				//��ǰ�û�������B������,�������ļ�A
        {
            InsertChar(url,len-4+1,"A");
        }
        else if(APP_AREA==APP_AREA_A)		//��ǰ�û�������A������,�������ļ�B
        {
            InsertChar(url,len-4+1,"B");
        }
        else	//A��B�������ڣ������ǵ�һ���ϵ磬Ҳ�����Ǹ�flash���ݱ��ƻ�,�������ļ�A
        {
            InsertChar(url,len-4+1,"A");
        }
        printf("url:%s\r\n",url);
        //GetURL(data,server);
		if (strlen(url)>0)
		{
			InitGprsIAP();
			time=HAL_GetTick();
			if(SimInit()==1){
				time=HAL_GetTick()-time;
				printf("InitSimcom succeed time:%d\r\n",time);
				time=HAL_GetTick();
				//if(SimHttpSetUrl("http://down.nongbotech.cn/WatcherStandarApplication.hex")==1){
                if(SimHttpSetUrl(url)==1){
					time=HAL_GetTick()-time;
					printf("ConnectTcpServer succeed time:%d\r\n",time);
					//�Ƚ����ҳ������Ȼ������¼����¼��ʱ��Ͳ����ٲ�����
					HAL_FLASH_Unlock();	//����
					if(FlashRead16(APP_AREA_ADD)==APP_AREA_B)				//��ǰ�û�������B������,����дA��
					{
						FLASH_EraseInitTypeDef EraseInitStruct;
						uint32_t PAGEError =0;
						printf("Program A\r\n");
						EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
						EraseInitStruct.PageAddress = ApplicationAddressA;
						EraseInitStruct.NbPages     = AREA_SISE/FLASH_PAGE_SIZE;
						if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError)!=HAL_OK)
						{
							printf("����ʧ��");
							return;
						}
						else{
							printf("�����ɹ�\r\n");
						}
						FlashWrite16(AREA_A_RUN,(uint16_t *)"0",1);//��ʱA����������(��ʱ̫��,���½��ճ�����)���˺������Flash����
					}
					else if(FlashRead16(APP_AREA_ADD)==APP_AREA_A)		//��ǰ�û�������A�����У�����дB��
					{
						FLASH_EraseInitTypeDef EraseInitStruct;
						uint32_t PAGEError =0;
						printf("Program B\r\n");
						EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
						EraseInitStruct.PageAddress = ApplicationAddressB;
						EraseInitStruct.NbPages     = AREA_SISE/FLASH_PAGE_SIZE;
						if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError)!=HAL_OK)
						{
							printf("����ʧ��");
							return;
						}
						else{
							printf("�����ɹ�\r\n");
						}
						FlashWrite16(AREA_B_RUN,(uint16_t *)"0",1);//��ʱB����������(��ʱ̫��,���½��ճ�����)���˺������Flash����
					}
					else	//A��B�������ڣ������ǵ�һ���ϵ磬Ҳ�����Ǹ�flash���ݱ��ƻ�
					{
						FLASH_EraseInitTypeDef EraseInitStruct;
						uint32_t PAGEError =0;
						printf("first running��area data lost��Program A\r\n");
						EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
						EraseInitStruct.PageAddress = ApplicationAddressA;
						EraseInitStruct.NbPages     = AREA_SISE/FLASH_PAGE_SIZE;
						if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError)!=HAL_OK)
						{
							printf("����ʧ��");
							return;
						}
						else{
							printf("�����ɹ�\r\n");
						}
						FlashWrite16(AREA_B_RUN,(uint16_t *)"0",1);//��ʱA����������(��ʱ̫��,���½��ճ�����)���˺������Flash����
					}
					HAL_FLASH_Lock();	//����
					
                    printf("Updating...\r\n");
                    time=HAL_GetTick();
                    returnvalue=UpdateProgramFromGprs();
                    //returnvalue=IAP_SUCCESS;
                    if(returnvalue==IAP_SUCCESS){
                        uint32_t a=0;               //���³ɹ�
                        SetFlag(UPDATE_SUCCEED);	//���³ɹ����λ���������û������лظ�ȷ����Ϣ
                        FlashWrite32(UPDATE_RETRY,&a,1);
                        FlashWrite32(CTRL_STATE,&a,1);
                        FlashIncrease32(BOOT_SUCCEED);
                        SimPowerOff();			//�ر�simģ���Դ���û�����������
                        HAL_Delay(3000);
                        return ;
                        //SoftReset();
                    }else{												//ʧ�ܵĻ��ں����������������Ϣ
                        FlashIncrease32(GPRSPROGRAM_FAIL);
                        if(returnvalue == IAP_FAILURE){					//�������ݴ���
                            FlashIncrease32(DOWNLOADDATA_ERR);
                        }else if(returnvalue == IAP_TIMEOUT){			//���س�ʱ
                            FlashIncrease32(DOWNLOAD_TIMEOUT);
                        }else;
                    }
				}
			}
			else{															//ʧ�ܵĻ��ں����������������Ϣ
                printf("Simģ���ʼ��ʧ��\r\n");
			}
			SimPowerOff();
			if(succeed!=1 && update_retry<3)		//���²��ɹ����߳�������3��
			{
				printf("Update fail\r\n");
				FlashIncrease32(UPDATE_RETRY);
				FlashIncrease32(REBOOT_TOTAL);
				SetFlag(UPDATE_RESET);		        //���ø���������־
				HAL_Delay(500);						//�ȴ������������
                //�˴���ֱ�Ӹ�λ��������ת���û�
				//HAL_NVIC_SystemReset();				//���¸�λ
                
			}else if(update_retry!=0)				//���³ɹ�����ʧ��3��,��������
			{
				uint32_t value=0;
				FlashWrite32(UPDATE_RETRY,&value,1);
			}
		}
		SetFlag(UPDATE_FAIL);						//����ʧ�ܱ��λ
	}
}

//void UserMain(void)
//{
//    uint32_t temp=0;
//    uint32_t time=0;
////    while(1)
////    {
////        time= HAL_GetTick();
////        FlashWrite32(APP_AREA_ADD,(uint32_t *)"0",1);
////        FlashWrite16(AREA_A_RUN,(uint16_t *)"1",1);
////        time=HAL_GetTick()-time;
////        printf("%u,%u\r\n",temp,time);
////        temp++;
////    }
//    
//	if(FlashRead32(FLAG_ADD)==0xffffffff)
//	{
//		uint32_t temp=0;
//		FlashWrite32(FLAG_ADD,&temp,1);
//	}
//	if(FlashRead32(APP_AREA_ADD)==0)
//	{
//		FlashWrite32(APP_AREA_ADD,(uint32_t *)"0",1);
//		FlashWrite16(AREA_A_RUN,(uint16_t *)"1",1);
//	}
//    CheckUpgrade();
//	if(FlashRead16(APP_AREA_ADD)==APP_AREA_B  && FlashRead16(AREA_B_RUN)== RUNNABLE)			//���е�ַΪB��������B����������
//	{
//		printf("jump to user app area��B:%x\r\n",ApplicationAddressB);
//		HAL_Delay(500);//�ȴ������������
//		/* Jump to user application */
//		JumpAddress = *(__IO uint32_t*) (ApplicationAddressB + 4);			//��λλ��
//		Jump_To_Application = (pFunction) JumpAddress;
//		/* Initialize user application's Stack Pointer */
//		__set_MSP(*(__IO uint32_t*) ApplicationAddressB);					//ջ��ָ���λ��
//		Jump_To_Application();
//	}
//	else if(FlashRead16(APP_AREA_ADD)==APP_AREA_A && FlashRead16(AREA_A_RUN)== RUNNABLE)
//	{
//		printf("jump to user app area��A:%x\r\n",ApplicationAddressA);
//		HAL_Delay(500);
//		/* Jump to user application */
//		JumpAddress = *(__IO uint32_t*) (ApplicationAddressA + 4);
//		Jump_To_Application = (pFunction) JumpAddress;
//		/* Initialize user application's Stack Pointer */
//		__set_MSP(*(__IO uint32_t*) ApplicationAddressA);
//		Jump_To_Application();
//	}
//    else
//    {
//        printf("Can not Jump To Application\r\n");
//        printf("FlashRead16(APP_AREA_ADD):%x\r\n",FlashRead16(APP_AREA_ADD));
//        printf("FlashRead16(AREA_A_RUN):%x\r\n",FlashRead16(AREA_A_RUN));
//        printf("FlashRead16(AREA_B_RUN):%x\r\n",FlashRead16(AREA_B_RUN));
//        HAL_Delay(10);
//        HAL_NVIC_SystemReset();
//    }
//}

void UserMain(void)
{
    //printf("Bootloader\r\n");
	if(FlashRead32(FLAG_ADD)==0xffffffff)
	{
		uint32_t temp=0;
		FlashWrite32(FLAG_ADD,&temp,1);
        HAL_Delay(10);
	}
    
	if(FlashRead32(APP_AREA_ADD)!=APP_AREA_B && FlashRead32(APP_AREA_ADD)!=APP_AREA_A)
	{
		if(FlashWrite32(APP_AREA_ADD,(uint32_t *)"0",1)!=1)//д��A��
            printf("FlashWrite32(APP_AREA_ADD) error\r\n");
        HAL_Delay(10);
	}
    
    if( FlashRead16(AREA_A_RUN)!= RUNNABLE && FlashRead16(AREA_B_RUN)!= RUNNABLE )
    {
        if(FlashWrite16(AREA_A_RUN,(uint16_t *)"1",1)!=1)
            printf("FlashWrite16(AREA_A_RUN) error\r\n");
        HAL_Delay(10);
    }
    
    CheckUpgrade();
	if(FlashRead16(APP_AREA_ADD)==APP_AREA_B  && FlashRead16(AREA_B_RUN)== RUNNABLE)			//���е�ַΪB��������B����������
	{
		printf("jump to user app area��B:%x\r\n",ApplicationAddressB);
		HAL_Delay(50);//�ȴ������������
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (ApplicationAddressB + 4);			//��λλ��
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) ApplicationAddressB);					//ջ��ָ���λ��
		Jump_To_Application();
	}
	else if(FlashRead16(APP_AREA_ADD)==APP_AREA_A && FlashRead16(AREA_A_RUN)== RUNNABLE)
	{
		printf("jump to user app area��A:%x\r\n",ApplicationAddressA);
		HAL_Delay(50);
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (ApplicationAddressA + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) ApplicationAddressA);
		Jump_To_Application();
	}
	else
    {
		printf("boot:error;\r\n");
        printf("FlashRead16(APP_AREA_ADD):%x\r\n",FlashRead16(APP_AREA_ADD));
        printf("FlashRead16(AREA_A_RUN):%x\r\n",FlashRead16(AREA_A_RUN));
        printf("FlashRead16(AREA_B_RUN):%x\r\n",FlashRead16(AREA_B_RUN));
        HAL_Delay(100);
        HAL_NVIC_SystemReset();
    }
}
