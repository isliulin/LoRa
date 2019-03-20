/*
 * user_flash_L072.c	L072оƬFlahs��������
*/

#include "user_flash_L072.h"

uint32_t FLASH_BUF [ FLASH_PAGE_SIZE/4 ]={0};

/*
 *	FlashReadPage:		��ȡ1ҳ����
 *	����PageAddr��		ҳ��ַ
 *	����pBuffer��		�����ȡ�������ݵ�ָ��
 *	����ֵ��			1�ɹ� 0ʧ��	
 */
uint8_t FlashReadPage(uint32_t PageAddr, uint32_t *pBuffer)
{
	uint32_t Address = 0;
	if(PageAddr<FLASH_BASE||PageAddr+FLASH_PAGE_SIZE>FLASH_BASE+FLASH_SIZE)
		return 0;
	if(PageAddr%FLASH_PAGE_SIZE!=0)
		return 0;
	Address = PageAddr;
	while (Address < PageAddr+FLASH_PAGE_SIZE)
	{
		*pBuffer = *(__IO uint32_t *)Address;
		Address = Address + 4;
		pBuffer++;
	}
	return 1;
}

/*
 *	FlashWritePage:		д1ҳ����
 *	����PageAddr��		ҳ��ַ
 *	����pBuffer��		����д��Flash�е�����ָ��
 *	����ֵ��			1�ɹ� 0ʧ��	
 */
uint8_t FlashWritePage( uint32_t PageAddr, uint32_t *pPageBuffer)
{
	uint32_t Address = 0, PAGEError =0;
	if(PageAddr<FLASH_BASE||PageAddr+FLASH_PAGE_SIZE>FLASH_BASE+FLASH_SIZE)
		return 0;
	
	if(PageAddr%FLASH_PAGE_SIZE!=0)
		return 0;
	//����
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = PageAddr;
	EraseInitStruct.NbPages     = 1;
//	DEBUG("PageAddr:%x\r\n",PageAddr);
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		DEBUG("ERASE FLASH ERROR: %x\r\n",HAL_FLASH_GetError());
		HAL_FLASH_Lock();
		return 0;
	}
	
	Address=PageAddr;
	while (Address < PageAddr+FLASH_PAGE_SIZE)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, *pPageBuffer) == HAL_OK)
		{
			Address = Address + 4;
			pPageBuffer++;
		}
		else
		{
			DEBUG("Write Flash Error\r\n");
			HAL_FLASH_Lock();
			return 0;
		}
	}
	HAL_FLASH_Lock();
	return 1;
}
/*
 *	FlashWrite32:		д4�ֽ�(32λ)����,д�����ݲ��ܿ�ҳ(Ŀǰ�������ò�����ҳд����)
 *	����WriteAddr��		��������Flash�еĵ�ַ
 *	pBuffer:			����д��Flash�е�����ָ��
 *	NumToWrite			���ݳ���(С��ҳ��С/4)
 *	����ֵ��			1�ɹ� 0ʧ��			
 */
uint8_t FlashWrite32( uint32_t WriteAddr, uint32_t * pBuffer, uint16_t NumToWrite )
{
	uint32_t PageAdd;	//WriteAddr��Ӧ��ҳ��ַ
	uint32_t OffSet;	//ҳ��ַ��ƫ��  PageAdd+OffSet=WriteAddr
	uint16_t i=0;
	//��ȡWriteAddr��Ӧ��ҳ��ַ
	if(WriteAddr<FLASH_BASE||WriteAddr+NumToWrite*4>FLASH_BASE+FLASH_SIZE)
		return 0;
		
	if(WriteAddr%4!=0)
		return 0;
		
	if(NumToWrite>FLASH_PAGE_SIZE/4)
		return 0;
	PageAdd=WriteAddr & 0xffffff00;		//L072��ҳ��СΪ128(0x80),����λ���㼴��
	OffSet=WriteAddr & 0x000000ff;
	if(OffSet/FLASH_PAGE_SIZE>0)//
	{
		PageAdd+=FLASH_PAGE_SIZE;
		OffSet-=FLASH_PAGE_SIZE;
	}
	if(PageAdd+FLASH_PAGE_SIZE < WriteAddr+NumToWrite*4)//���ݿ�ҳ
		return 0;
	if(FlashReadPage(PageAdd,FLASH_BUF)!=1)	//�ȶ�һҳ�����������
		return 0;
	//�޸ľ�����
	
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_BUF[OffSet/4+i]=*pBuffer;
		pBuffer++;
	}
	return FlashWritePage(PageAdd,FLASH_BUF);	//���������ݡ�д��������
}
/*
 *	FlashWrite16:		д2�ֽ�(16λ)����,д�����ݲ��ܿ�ҳ(Ŀǰ�������ò�����ҳд����)
 *	����WriteAddr��		��������Flash�еĵ�ַ
 *	pBuffer:			����д��Flash�е�����ָ��
 *	NumToWrite			���ݳ���(С��ҳ��С)
 *	����ֵ��			1�ɹ� 0ʧ��			
 */
uint8_t FlashWrite16( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite )
{
	uint32_t PageAdd;	//WriteAddr��Ӧ��ҳ��ַ
	uint32_t OffSet;	//ҳ��ַ��ƫ��  PageAdd+OffSet=WriteAddr
//	uint16_t i;
	
	//��ȡWriteAddr��Ӧ��ҳ��ַ
	if(WriteAddr<FLASH_BASE||WriteAddr+NumToWrite*2>FLASH_SIZE+FLASH_BASE)
		return 0;
		
	if(WriteAddr%2!=0)
		return 0;
		
	if(NumToWrite>FLASH_PAGE_SIZE/4*2)
		return 0;
	
	PageAdd=WriteAddr & 0xffffff00;
	OffSet=WriteAddr & 0x000000ff;
	if(OffSet/FLASH_PAGE_SIZE>0)//
	{
		PageAdd+=FLASH_PAGE_SIZE;
		OffSet-=FLASH_PAGE_SIZE;
	}
	if(PageAdd+FLASH_PAGE_SIZE < WriteAddr+NumToWrite*2)//���ݿ�ҳ
		return 0;
	if(FlashReadPage(PageAdd,FLASH_BUF)!=1)	//�ȶ�һҳ�����������
		return 0;
	//�޸ľ�����
	memcpy(FLASH_BUF+OffSet/4,pBuffer,NumToWrite*2);
//	for(i=0;i<NumToWrite/2;i++)
//	{
//		uint32_t temp1,temp2;
//		//��16λ����ƴ��Ϊ32λ
//		temp1=*pBuffer;
//		temp2=*(pBuffer+1);
//		FLASH_BUF[OffSet/4+i]=(temp2<<16|temp1);
//		pBuffer+=2;
//	}
//	for(uint8_t j=0;j<NumToWrite%2;j++)
//	{
//		FLASH_BUF[OffSet/4+i]=FLASH_BUF[OffSet/4+i] & 0xffff0000;	//��պ�2�ֽ�
//		FLASH_BUF[OffSet/4+i]=FLASH_BUF[OffSet/4+i] | *pBuffer;
//	}
	if(FlashWritePage(PageAdd,FLASH_BUF)!=1)	//���������ݡ�д��������
		return 0;
	return 1;
}
/*
 *	FlashRead32:		��ȡ4�ֽ�(32λ)����
 *	����ReadAddr��		��������Flash�еĵ�ַ
 *	����ֵ��			���ض�ȡ��������		
 */
uint32_t FlashRead32(uint32_t ReadAddr )
{
	if(ReadAddr<FLASH_BASE||ReadAddr>FLASH_BASE+FLASH_SIZE)
		return 0;
	return (uint32_t)(*(__IO uint32_t *)ReadAddr);
}
/*
 *	FlashRead16:		��ȡ2�ֽ�(16λ)����
 *	����ReadAddr��		��������Flash�еĵ�ַ
 *	����ֵ��			���ض�ȡ��������		
 */
uint16_t FlashRead16(uint32_t ReadAddr )
{
	if(ReadAddr<FLASH_BASE||ReadAddr>FLASH_BASE+FLASH_SIZE)
		return 0;
	return (uint16_t)(*(__IO uint16_t *)ReadAddr);
}
/*
 *	FlashRead8:		��ȡ1�ֽ�(8λ)����
 *	����ReadAddr��	��������Flash�еĵ�ַ
 *	����ֵ��		���ض�ȡ��������		
 */
uint8_t FlashRead8(uint32_t ReadAddr )
{
	if(ReadAddr<FLASH_BASE||ReadAddr>FLASH_BASE+FLASH_SIZE)
		return 0;
	return (uint8_t)(*(__IO uint8_t *)ReadAddr);
}
/*
 *	FlashReadChar:	��ȡ�ֽ�(8λ)����,ͨ�����ڶ�ȡ������Flash���ַ���
 *	����ReadAddr��	��������Flash�еĵ�ַ
 *	����pBuffer��	�����ȡ�������ݵ�ָ��
 *	����NumToRead��	��ȡ�ĳ���
 *	����ֵ��		ʵ�ʶ�ȡ�����ַ�����,��ȡ���󷵻�0
 */
uint16_t FlashReadChar(uint32_t ReadAddr,char* pBuffer,uint16_t NumToRead)
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		*pBuffer=FlashRead8(ReadAddr);
		if(*pBuffer=='\0')
			return i;
		pBuffer++;
		ReadAddr++;
	}
	return 0;
}
/*
 *	FlashRead16More:	��ȡ���2�ֽ�(16λ)����
 *	����ReadAddr��	��������Flash�еĵ�ַ
 *	����pBuffer��	�����ȡ�������ݵ�ָ��
 *	����NumToRead��	��ȡ�ĳ���
 *	����ֵ��		1�ɹ� 0ʧ��
 */
uint16_t FlashRead16More(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead) 
{
	if(ReadAddr<FLASH_BASE||ReadAddr+NumToRead*2>FLASH_BASE+FLASH_SIZE)
		return 0;
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=FlashRead16(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
	return 1;
}

/*
 *	SetFlag:			�޸�FLAG_ADD��ַ��ֵ��������ر�־λ
 *	����flag��			ֻ��ΪFlagType����
 *	����ֵ��			��	
 */
void SetFlag(FlagType flag)
{
	uint32_t temp = (0x000000001U<<(uint8_t)flag);
	temp=FLAG|temp;
	//DEBUG("Set Flag before:%x\r\n",FLAG);
	FlashWrite32(FLAG_ADD,&temp,1);
	DEBUG("Flag after Set:%x\r\n",FLAG);
}
/*
 *	CleanFlag:			�޸�FLAG_ADD��ַ��ֵ�������ر�־λ
 *	����flag��			ֻ��ΪFlagType����
 *	����ֵ��			��	
 */
void CleanFlag(FlagType flag)
{
	uint32_t temp = (0x000000001U<<(uint8_t)flag);
	temp=FLAG&(~temp);
	//DEBUG("Clean Flag before:%x\r\n",FLAG);
	FlashWrite32(FLAG_ADD,&temp,1);
	DEBUG("Flag after Clean:%x\r\n",FLAG);
}
/*
 *	CheckFlag:			��ѯ��ر�־λ�Ƿ񱻱�־
 *	����flag��			ֻ��ΪFlagType����
 *	����ֵ��			����־����>0��ֵ��δ��־����0
 */
uint8_t CheckFlag(FlagType flag)
{
	uint32_t temp = (0x000000001U<<(uint8_t)flag);
	return (FLAG&temp)>0?1:0;
}

/*
 *	ReadSerialNumber:	��flash��ȡ���к�
 * 	device_id:			�豸ID
 *	stream:				������
 *	server_add:			������IP��ַ
 *	server_port:		�������˿�
 *	����ֵ��				��ȡ�ɹ��򷵻�0�����򷵻�1
 */
uint8_t ReadSerialNumber(char *device_id,char *stream,char *server_addr,char *server_port)
{
	FlashReadChar(SERVER_ADDR,server_addr,SERVER_SIZE);  
	FlashReadChar(SERVER_PORT_ADDR,server_port,SERVER_PORT_SIZE);  
	FlashReadChar(DEVICE_ID_ADDR,device_id,DEVICE_ID_SIZE);
	FlashReadChar(STREAM_ADDR,stream,STREAM_SIZE);
    
    DEBUG("Device:%s\r\n",DeviceID);
	DEBUG("Stream:%s\r\n",Stream);
	DEBUG("ServerAdd:%s\r\n",ServerAdd);
	DEBUG("ServerPort:%s\r\n",ServerPort);
    
    if(DeviceID[0]==0)
    {
        DEBUG("���豸��\r\n");
        return 1;
	}
    if(Stream[0]==0)
    {
        DEBUG("��������\r\n");
        return 1;
	}
    if(ServerAdd[0]==0)
    {
        DEBUG("�޷�������ַ\r\n");
        return 1;
	}
    if(ServerPort[0]==0)
    {
        DEBUG("�޷������˿�\r\n");
        return 1;
	}
    return 0;
}
