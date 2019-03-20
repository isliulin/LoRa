#ifndef __FLASH_H_
#define __FLASH_H_
#include "stm32l0xx_hal.h"


/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/************************** STM32 �ڲ� FLASH ���� *****************************/
#define STM32_FLASH_SIZE        192  // ��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN        1    // stm32оƬ����FLASH д��ʹ��(0������;1��ʹ��)

#define FLASH_ALINE    4

#if FLASH_ALINE == 2
  #define FLASH_TYPE uint16_t //�ֽ�
#else 
  #define FLASH_TYPE uint32_t
#endif

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void STMFLASH_Read_AllWord( uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead );
uint16_t STMFLASH_ReadHalfWord ( uint16_t faddr );
uint32_t STMFLASH_ReadAllWord ( uint32_t faddr );
HAL_StatusTypeDef STMFLASH_Write( uint32_t WriteAddr, FLASH_TYPE * pBuffer, uint32_t NumToWrite );		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read( uint32_t ReadAddr, uint16_t * pBuffer, uint32_t NumToRead );   	//��ָ����ַ��ʼ����ָ�����ȵ�����
HAL_StatusTypeDef STMFLASH_WriteStruct(uint32_t WriteAddr, void * pBuffer, uint16_t NumToWrite);
void STMFLASH_ReadStruct(uint32_t WriteAddr, void * pBuffer, uint16_t NumToWrite);
extern HAL_StatusTypeDef STMFLASH_writeString(uint32_t WriteAddr, void * pBuffer, uint16_t NumToWrite);
void STMFLASH_ReadString(uint32_t ReadAddr, char* pBuffer, uint16_t NumToRead);
#endif

