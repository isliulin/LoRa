#include "flash.h"
#include "string.h"
#include "stdio.h"
#include "NBI_common.h"
#include "debug.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#if STM32_FLASH_SIZE < 256
  #define STM_SECTOR_SIZE  128 //�ֽ�
#else 
  #define STM_SECTOR_SIZE	 2048
#endif


/* ˽�б��� ------------------------------------------------------------------*/
#if STM32_FLASH_WREN	//���ʹ����д 
static FLASH_TYPE STMFLASH_BUF [ STM_SECTOR_SIZE / FLASH_ALINE ];//�����2K�ֽ�
#endif

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ��ȡָ����ַ�İ���(16λ����)
  * �������: faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
  * �� �� ֵ: ����ֵ:��Ӧ����.
  * ˵    ������
  */
FLASH_TYPE memCTOI(char *data) {FLASH_TYPE ppp; memcpy(&ppp,data,FLASH_ALINE); return ppp;}	
HAL_StatusTypeDef STMFLASH_WriteStruct(uint32_t WriteAddr, void * pBuffer, uint16_t NumToWrite)
{
	int i = 0;
	uint32_t numData = 0;
	FLASH_TYPE Buffer[STM_SECTOR_SIZE / FLASH_ALINE];	
	for(i = 0; i < NumToWrite/FLASH_ALINE ;i ++)
	{
		Buffer[i] = memCTOI((char *)pBuffer);//*data;	
		pBuffer = ((char *)pBuffer+FLASH_ALINE);
		numData ++;		
	}	
		
	return STMFLASH_Write(WriteAddr,Buffer,numData);	
}	

void STMFLASH_ReadStruct(uint32_t ReadAddr, void * pBuffer, uint16_t NumToRead)
{
	FLASH_TYPE Buffer[STM_SECTOR_SIZE/FLASH_ALINE] = {0};
	#if FLASH_ALINE == 2
		STMFLASH_Read(ReadAddr,Buffer,NumToRead/FLASH_ALINE); //һ�������ֽ�
	#else
		STMFLASH_Read_AllWord(ReadAddr,Buffer,NumToRead/FLASH_ALINE); //һ�������ֽ�
	#endif
	memcpy(pBuffer,Buffer,NumToRead);
}	

HAL_StatusTypeDef STMFLASH_writeString(uint32_t WriteAddr, void * pBuffer, uint16_t NumToWrite)
{
	int i = 0;
	uint32_t numData = 0;
	FLASH_TYPE Buffer[STM_SECTOR_SIZE / FLASH_ALINE];	
//	DEBUG_ERROR("define\r\n");
//	HAL_Delay(1000);
	for(i = 0; i < NumToWrite/FLASH_ALINE ;i ++)
	{
		Buffer[i] = memCTOI((char *)pBuffer);//*data; 
		pBuffer = ((char *)pBuffer+FLASH_ALINE);
		numData ++; 
//		DEBUG_ERROR("is OK\r\n");
//		HAL_Delay(1000);		
	} 
//	DEBUG_ERROR("had finish\r\n");
//	HAL_Delay(2000);
	return STMFLASH_Write(WriteAddr,Buffer,numData);	
} 

void STMFLASH_ReadString(uint32_t ReadAddr, char* pBuffer, uint16_t NumToRead)
{
	FLASH_TYPE Buffer[STM_SECTOR_SIZE/FLASH_ALINE] = {0};
	#if FLASH_ALINE == 2
		STMFLASH_Read(ReadAddr,Buffer,NumToRead/FLASH_ALINE); //һ�������ֽ�
	#else
		STMFLASH_Read_AllWord(ReadAddr,Buffer,NumToRead/FLASH_ALINE); //һ���ĸ��ֽ�
	#endif
	memcpy(pBuffer,Buffer,NumToRead);

}	
/**
  * ��������: ��ȡָ����ַ�İ���(32λ����)
  * �������: faddr:����ַ(�˵�ַ����Ϊ4�ı���!!)
  * �� �� ֵ: ����ֵ:��Ӧ����.
  * ˵    ������
  */
uint16_t STMFLASH_ReadHalfWord ( uint16_t faddr )
{
	return *(__IO uint16_t*)faddr;
}

uint32_t STMFLASH_ReadAllWord ( uint32_t faddr )
{
	return *(__IO uint32_t*)faddr;
}
#if STM32_FLASH_WREN	//���ʹ����д   
/**
  * ��������: ������д��
  * �������: WriteAddr:��ʼ��ַ
  *           pBuffer:����ָ��
  *           NumToWrite:����(16λ)��
  * �� �� ֵ: ��
  * ˵    ������
  */
HAL_StatusTypeDef STMFLASH_Write_NoCheck ( uint32_t WriteAddr, FLASH_TYPE * pBuffer, uint32_t NumToWrite )   
{ 			 		 
	uint32_t i;	
	
	for(i=0;i<NumToWrite;i++)
	{		
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAMDATA_WORD ,WriteAddr,pBuffer[i]) == HAL_ERROR)
		{
			return HAL_ERROR;
		}
		WriteAddr+=FLASH_ALINE;                                    //��ַ����2.
	}  
	return HAL_OK;
} 

/**
  * ��������: ��ָ����ַ��ʼд��ָ�����ȵ�����
  * �������: WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
  *           pBuffer:����ָ��
  *           NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
  * �� �� ֵ: ��
  * ˵    ������
  */
HAL_StatusTypeDef STMFLASH_Write ( uint32_t WriteAddr, FLASH_TYPE * pBuffer, uint32_t NumToWrite )	
{	
//	uint32_t SECTORError = 0;
	uint32_t secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	uint32_t secremain; //������ʣ���ַ(16λ�ּ���)	   
	uint32_t i;    
	uint32_t secpos;	   //������ַ
	uint32_t offaddr;   //ȥ��0X08000000��ĵ�ַ
	
	if(WriteAddr<FLASH_BASE||(WriteAddr>=(FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return HAL_ERROR;
	
	HAL_FLASH_Unlock();						//����	
	//FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
	offaddr=WriteAddr-FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/FLASH_ALINE;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/FLASH_ALINE-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)
		secremain=NumToWrite;//�����ڸ�������Χ	
	while(1)
	{
		#if FLASH_ALINE == 2		
			STMFLASH_Read(secpos*STM_SECTOR_SIZE+FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
//			printf("read data for half word\r\n");
			HAL_Delay(2000);
		#else
			STMFLASH_Read_AllWord(secpos*STM_SECTOR_SIZE+FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/4);//������������������
		#endif
		for(i=0;i<secremain;i++)//У������
		{
			#if FLASH_ALINE == 2
				if(STMFLASH_BUF[secoff+i]!=0XFFFF)
			#else
				if(STMFLASH_BUF[secoff+i]!=0XFFFFFFFF)
			#endif
				break;
		}
		if(i<secremain)//��Ҫ����
		{
			/* Fill EraseInit structure*/
			uint32_t Sector   = secpos*STM_SECTOR_SIZE+FLASH_BASE;	
			FLASH_EraseInitTypeDef f;
			f.NbPages = (int)(NumToWrite / STM_SECTOR_SIZE) + 1;
			f.PageAddress = Sector;
			f.TypeErase = FLASH_TYPEERASE_PAGES;
			
			uint32_t PageError = 0;
//			DEBUG_ERROR("f.NbPages = %d\r\n",f.NbPages);
//			DEBUG_ERROR("f.PageAddress = %d\r\n",Sector);
			
			if(HAL_FLASHEx_Erase(&f, &PageError) == HAL_ERROR)
			{
				HAL_Delay(500);
				if(HAL_FLASHEx_Erase(&f, &PageError) == HAL_ERROR)
				{
					DEBUG_ERROR("erase error  = %X \r\n",PageError);
					return HAL_ERROR;
				}

			}		
			for(i=0;i<secremain;i++)
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}			
			if(STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/FLASH_ALINE) == HAL_ERROR)
			{
				DEBUG_ERROR("Write_NoCheck error\r\n");
				return HAL_ERROR;
			}
		}
		else
		{
			if(STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain) == HAL_ERROR)
			{
				DEBUG_ERROR("Write_NoCheck error\r\n");
				return HAL_ERROR;
			}
		}
		if(NumToWrite==secremain)
			break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
			pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
			NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/FLASH_ALINE))
				secremain=STM_SECTOR_SIZE/FLASH_ALINE;//��һ����������д����
			else 
				secremain=NumToWrite;//��һ����������д����
		}	 
	}
	
	if(HAL_FLASH_Lock() == HAL_ERROR)
	{
		DEBUG_ERROR("HAL_FLASH_Lock error\r\n");
		return HAL_ERROR;
	}
	return HAL_OK;
}
#endif

/**
  * ��������: ��ָ����ַ��ʼ����ָ�����ȵ�����
  * �������: ReadAddr:��ʼ��ַ
  *           pBuffer:����ָ��
  *           NumToRead:����(16λ)��
  * �� �� ֵ: ��
  * ˵    ������
  */
void STMFLASH_Read ( uint32_t ReadAddr, uint16_t *pBuffer, uint32_t NumToRead )
{
	uint16_t i;
	
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr += 2;//ƫ��2���ֽ�.	
	}
}


void STMFLASH_Read_AllWord( uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead )   	
{
	uint16_t i;
	
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i] = STMFLASH_ReadAllWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr += 4;//ƫ��4���ֽ�.	
	}
}

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

