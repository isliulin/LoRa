/*
**************************************************************************************************************
*	@file	parse_hex_file.c
*	@author 
*	@version 
*	@date    
*	@brief	����hex�����ļ�������д��FLASH
***************************************************************************************************************
*/
#include "parse_hex_file.h"

// FLASH PAGE�Ĵ�С
#define PAGE_SIZE		FLASH_PAGE_SIZE
#define PAGE_SIZE_MASK	(PAGE_SIZE-1)

#define RECORD_MARK		':'	// �����е���ʼ��ʶ
#define RECTYP_DATA		0	// ��¼����--����
#define RECTYP_EOF		1	// ��¼����--�ļ�����
#define RECTYP_ESAR		2	// ��¼����--Extended Segment Address Record
#define RECTYP_SSAR		3	// ��¼����--Start Segment Address Record
#define RECTYP_ELAR		4	// ��¼����--Extended Linear Address Record
#define RECTYP_SLAR		5	// ��¼����--Start Linear Address Record

#define DATATYP_8BIT 	1
#define DATATYP_16BIT	2
#define DATATYP_32BIT	4

// ��ȡ����ʼ�ַ�':'ƫ����Ϊoffset������
#define GET_DIGITAL(p_mark, offset)	(*((p_mark)+(offset))-'0')

/*
 *	InitHexObjFile:	��ʼ��Hex�ļ��ṹ��
 *	p��				HexObjFile�ṹ��
 *	����ֵ��		��	
 */
extern void InitHexObjFile(HexObjFile_t *p)
{
	p->empty_addr = 0;
	p->status = IAP_CONTINUE;

//	/* Unlock the Flash Bank1 Program Erase controller */
//	FLASH_UnlockBank1();
}

/*
 *	CvtHexAsciiToNumber:	��ʼ��Hex�ļ��ṹ��
 *	str��					�ַ�����ַ
 *	type:					ת�������ͣ�DATATYP_8BITΪһ���ֽڣ���Ӧ�����ַ�����"10"����������������
 *	data:					ת���������
 *	����ֵ��				0--ת���ɹ�  -1--ת��ʧ��	
 */
static int8_t CvtHexAsciiToNumber(char *str, uint32_t type, uint32_t *data)
{
	uint8_t i;

	if (type!=DATATYP_8BIT && type!=DATATYP_16BIT && type!=DATATYP_32BIT )
	{
		return -1;
	}

	*data = 0;
	for (i=0; i<type; i++)
	{
		char ch1 = *(str++);
		char ch2 = *(str++);
		(*data) <<= 8;			 

		if (ch1>='0' && ch1<='9')
		{
			(*data) |= (ch1-'0')<<4;
		}
		else if (ch1>='a' && ch1<='f')
		{
			(*data) |= (ch1-'a'+10)<<4;
		}
		else if (ch1>='A' && ch1<='F')
		{
			(*data) |= (ch1-'A'+10)<<4;
		}
		else
		{
			return -1;
		}

		if (ch2>='0' && ch2<='9')
		{
			(*data) |= (ch2-'0');
		}
		else if (ch2>='a' && ch2<='f')
		{
			(*data) |= (ch2-'a'+10);
		}
		else if (ch2>='A' && ch2<='F')
		{
			(*data) |= (ch2-'A'+10);
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

/*
 *	ParseHexObjFile:	����Hex�ļ������أ��������̲�Ҫ������������Ϣ��������ʧ��
 *	p_hex��				HexObjFile�ṹ��
 *	str:				�ַ����У���'\0'����'\n'��β
 *	����ֵ��			0--�ɹ� -1--ʧ��				
 */
extern int8_t ParseHexObjFile(HexObjFile_t *p_hex, char *str)
{
	char ch;
	volatile uint16_t APP_AREA;
	uint32_t rectyp, len, data, offset,start_addr;
//	uint32_t end_addr, page_start_addr;
	// Ѱ����ʼ�ַ�':'
    //printf("%s",str);
	for (ch=*str; ch!='\n'&&ch!='\0'; ch=*(++str))
	{
		if (ch == RECORD_MARK)
		{
			if(p_hex->Start!=1)//IAPû�п�ʼ
			{
				if(strncmp(str,":02000004",9)==0)//�ҵ��̼���ͷ
				{
					printf("Start IAP\r\n");
					p_hex->Start=1;
					break;
				}
			}
			break;
		}
	}
	
	if(p_hex->Start!=1)//IAPû�п�ʼ
	{
//		printf("HTTP HEADER\r\n");
		p_hex->status = IAP_UNKNOWN_CMD;
		return IAP_UNKNOWN_CMD;
	}

	// �ҵ���ʼ�ַ�����ʼ����

	if (CvtHexAsciiToNumber(str+7, DATATYP_8BIT, &rectyp)==0) 	// ��¼���ͺϷ�
	{
		switch (rectyp)
		{
			case RECTYP_ELAR:	// ��չ���Ե�ַ��¼
				
				APP_AREA=FlashRead16(APP_AREA_ADD);
				//printf("APP_AREA:%x,%x\r\n",APP_AREA,APP_AREA_ADD);

				if (CvtHexAsciiToNumber(str+9, DATATYP_16BIT, &p_hex->linear_base_addr) == 0)
				{
                    p_hex->linear_base_addr <<= 16;	 			//0x0800_0000
                    p_hex->linear_base_addr = p_hex->linear_base_addr + 0;
                    p_hex->vct_offset = 0;
                    p_hex->status = IAP_CONTINUE;
				}
                printf("p_hex->linear_base_addr:%x\r\n",p_hex->linear_base_addr);   //0xa000
                printf("p_hex->vct_offset:%x\r\n",p_hex->vct_offset);               //0xa000
				break;
	
	
			case RECTYP_DATA:	// ���ݼ�¼
                //printf("���ݼ�¼\r\n");
                if(strlen(str)>=HEX_DATA_LEN)
                {
                    if (CvtHexAsciiToNumber(str+3, DATATYP_16BIT, &offset)==0 && CvtHexAsciiToNumber(str+1, DATATYP_8BIT, &len)==0)
                    {
                        HAL_StatusTypeDef FLASHStatus =HAL_OK;
                        //offset�Ǵ�5000(BOOTLOADER_SIZE)��ʼ
                        start_addr = p_hex->linear_base_addr + offset;	// ��д����ʼ��ַ,0xa000+0x5XXX
                        uint32_t byte_cnt;
                        uint32_t little_endian_data = 0;
                        for (byte_cnt=0; (byte_cnt<len)&&(FLASHStatus==HAL_OK); byte_cnt+=4)
                        {
                            if (CvtHexAsciiToNumber(str+9+(byte_cnt<<1), DATATYP_32BIT, &data) != 0)
                            {
                                break;
                            }
                            // �ֽ���ת��
                            little_endian_data = data>>24 & 0xFF;
                            little_endian_data |= (data>>16 & 0xFF) << 8;
                            little_endian_data |= (data>>8  & 0xFF) << 16;
                            little_endian_data |= (data     & 0xFF) << 24;
                            if((little_endian_data&0xfff00000)==0x08000000)//�ж�������ĵ�ַ�ض�Ϊ80��ͷ,//�ж�������ĵ�ַ�ض�С�ڷ���������ַ��ͬʱ���ڷ����Ļ���ַ
                            {
                                if( ((little_endian_data+ p_hex->vct_offset) < (p_hex->linear_base_addr+AREA_SISE+BOOTLOADER_SIZE)) && ((little_endian_data + p_hex->vct_offset) > p_hex->linear_base_addr+BOOTLOADER_SIZE) )
                                {
                                    //printf("l:%x\r\n",little_endian_data);
                                    little_endian_data = little_endian_data + p_hex->vct_offset;
                                    //printf("l:%x\r\n",little_endian_data);
                                }
                            }
                            //printf("%x:%x\r\n",start_addr+byte_cnt,little_endian_data);
                            FLASHStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start_addr+byte_cnt, little_endian_data);
                            //HAL_Delay(1);
                            if(FLASHStatus!=HAL_OK)
                            {
                                printf("��д����:%x,%x\r\n",p_hex->linear_base_addr,offset);
                                printf("��д����:%x,%x,%x\r\n",start_addr+byte_cnt,FLASHStatus,HAL_FLASH_GetError());
                                p_hex->status = IAP_FAILURE;
                            }
                        }
                        if (byte_cnt == len)
                        {
                            p_hex->status = IAP_CONTINUE;
                        }
                    }
                    else
                    {
                        printf("����ת��ʧ��\r\n");
                        p_hex->status = IAP_FAILURE;
                    }
                }
                else
                {
                    printf("���Զϲ�����\r\n");
                    p_hex->status = IAP_CONTINUE;
                }
				break;

			case RECTYP_EOF:
				printf("End Of Update\r\n");
				APP_AREA=FlashRead16(APP_AREA_ADD);
//				printf("APP_AREA:%x,%x\r\n",APP_AREA,APP_AREA_ADD);
				if(APP_AREA==(uint16_t)APP_AREA_A)				//��ǰ�û�������A������,����дB��,B����д��ɣ��ѵ�ǰ���������޸�ΪB��
				{
					//printf("Program B:finish\r\n");
					FlashWrite16(APP_AREA_ADD,(uint16_t *)"1",1);	//��ת��B��
					FlashWrite16(AREA_B_RUN,(uint16_t *)"1",1);		//B��������
				}
				else if(APP_AREA==(uint16_t)APP_AREA_B)		//��ǰ�û�������B�����У�����дA��,A����д��ɣ��ѵ�ǰ���������޸�ΪA��
				{
					//printf("Program A:finish\r\n");
					FlashWrite16(APP_AREA_ADD,(uint16_t *)"0",1);	//��ת��A��
					FlashWrite16(AREA_A_RUN,(uint16_t *)"1",1);		//A��������
				}
				else	//A��B�������ڣ������ǵ�һ���ϵ磬Ҳ�����Ǹ����ݱ��ƻ�
				{
					//printf("Program A:finish(first)\r\n");
					FlashWrite16(APP_AREA_ADD,(uint16_t *)"0",1);	//��ת��A��
					FlashWrite16(AREA_A_RUN,(uint16_t *)"1",1);		//A��������
				}

				p_hex->status = IAP_SUCCESS;
				break;
			

//			case RECTYP_ESAR:
//				p_hex->status = IAP_FAILURE;
//				break;
//			case RECTYP_SSAR:
//				p_hex->status = IAP_FAILURE;
//				break;
//			case RECTYP_SLAR:
//				p_hex->status = IAP_CONTINUE;
//				break;

			default:
                p_hex->status = IAP_CONTINUE;
				break;
		}
	}

	return p_hex->status;
}

/*
 *	ShowFlash:			��Flash�еļ�������ʾ����
 *	������				��
 *	����ֵ��			��				
 */
void ShowFlash()
{
	printf("Application current area(A:0x30 B:0x31):%x\r\n",FlashRead16(APP_AREA_ADD));//��ǰ��������
	printf("BOOT_TOTAL:%d\r\n",FlashRead32(BOOT_TOTAL));
	printf("BOOT_CMD_TOTAL:%d\r\n",FlashRead32(BOOT_CMD_TOTAL));
	printf("REBOOT_TOTAL:%d\r\n",FlashRead32(REBOOT_TOTAL));
	printf("BOOT_SUCCEED:%d\r\n\r\n",FlashRead32(BOOT_SUCCEED));

	printf("SEND_SEQ_TOTAL:%d\r\n",FlashRead32(SEND_SEQ_TOTAL));
	printf("SEND_SEQ_SUCCEED:%d\r\n",FlashRead32(SEND_SEQ_SUCCEED));
	printf("RECIEVE_SEQ:%d\r\n",FlashRead32(RECIEVE_SEQ));
	printf("RECIEVE_SEQ_ACK:%d\r\n\r\n",FlashRead32(RECIEVE_SEQ_ACK));

	printf("INIT_SIMCOM_FAIL:%d\r\n",FlashRead32(INIT_SIMCOM_FAIL));
	printf("SIMCOM_CMD_DO_ERR:%d\r\n",FlashRead16(SIMCOM_CMD_DO_ERR));
	printf("SIMCOM_CMD_DO_TMEOUT:%d\r\n",FlashRead16(SIMCOM_CMD_DO_TMEOUT));
	printf("SIMCOM_CMD_BUSY:%d\r\n",FlashRead16(SIMCOM_CMD_BUSY));
	printf("SIMCOM_CMD_SEND_ERR:%d\r\n\r\n",FlashRead16(SIMCOM_CMD_SEND_ERR));

	printf("CONNECT_TCP_FAIL:%d\r\n",FlashRead32(CONNECT_TCP_FAIL));
	printf("CONNECT_TCP_NONE:%d\r\n",FlashRead32(CONNECT_TCP_NONE));
	printf("CONNECT_TCP_FAILURE:%d\r\n",FlashRead32(CONNECT_TCP_FAILURE));
	printf("CONNECT_TCP_EXISTED:%d\r\n\r\n",FlashRead32(CONNECT_TCP_EXISTED));

	printf("SEND_TCP_DATA_FAIL:%d\r\n",FlashRead32(SEND_TCP_DATA_FAIL));
	printf("SEND_TCP_NONE:%d\r\n",FlashRead32(SEND_TCP_NONE));
	printf("SEND_TCP_FAILURE:%d\r\n",FlashRead32(SEND_TCP_FAILURE));
	printf("SEND_TCP_TRANS_FAIL:%d\r\n\r\n",FlashRead32(SEND_TCP_TRANS_FAIL));

	printf("GPRSPROGRAM_FAIL:%d\r\n",FlashRead32(GPRSPROGRAM_FAIL));
	printf("DOWNLOADDATA_ERR:%d\r\n",FlashRead32(DOWNLOADDATA_ERR));
	printf("DOWNLOAD_TIMEOUT:%d\r\n\r\n",FlashRead32(DOWNLOAD_TIMEOUT));

	printf("URL_FAIL:%d\r\n",FlashRead32(URL_FAIL));
	printf("URL_ERROR:%d\r\n",FlashRead32(URL_ERROR));
	printf("URL_EMPTY:%d\r\n",FlashRead32(URL_EMPTY));
	printf("MEMORY_OVERFLOW:%d\r\n\r\n",FlashRead32(MEMORY_OVERFLOW));
    while(1);
}

