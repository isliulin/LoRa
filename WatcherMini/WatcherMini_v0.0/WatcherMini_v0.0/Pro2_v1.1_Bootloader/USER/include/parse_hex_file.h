/*
**************************************************************************************************************
*	@file	parse_hex_file.h
*	@author 
*	@version 
*	@date    
*	@brief	����hex�����ļ�������д��FLASH
***************************************************************************************************************
*/
#ifndef __PARSE_HEX_FILE
#define __PARSE_HEX_FILE

#include "stm32l0xx_hal.h"
#include "bootloader_config.h"
#include "user_flash_L072.h"
#include <string.h>
#include <stdio.h>
#ifdef __cplusplus
	extern "C" {
#endif


// IAP״̬
#define IAP_FAILURE	   -1	// IAPʧ�ܣ����������������ش���
#define IAP_CONTINUE	0	// IAP������
#define IAP_SUCCESS		1	// IAP�ɹ�����
#define IAP_UNKNOWN_CMD	2	// ��IAP����
#define IAP_TIMEOUT		3	//��ʱʧ��
//#define IAP_DOWN_ERROR	4	//�������ݴ���

// hex�ļ�����ṹ��
#define HEX_DATA_LEN    45 //һ������45�ַ�(����\r\n)
typedef struct
{
	uint32_t linear_base_addr;	// Linear Base Address
	uint32_t empty_addr;		// Ŀǰ�Ѿ���������ߵ�ַ
	int32_t  status;			// IAP״̬
	uint16_t vct_offset;		//�ж�������Ķ�̬ƫ��(a��b����һ��)
	uint8_t Start;
}
HexObjFile_t;	

/*
 *	InitHexObjFile:	��ʼ��Hex�ļ��ṹ��
 *	p��				HexObjFile�ṹ��
 *	����ֵ��		��	
 */
extern void InitHexObjFile(HexObjFile_t *p);

/*
 *	ParseHexObjFile:	����Hex�ļ�������
 *	p_hex��				HexObjFile�ṹ��
 *	str:				�ַ����У���'\0'����'\n'��β
 *	����ֵ��			0--�ɹ� -1--ʧ��				
 */
extern int8_t ParseHexObjFile(HexObjFile_t *p_hex, char *str);

/*
 *	ShowFlash:			��Flash�еļ�������ʾ����
 *	������				��
 *	����ֵ��			��				
 */
extern void ShowFlash(void);
/*
 *	GetURL:				��Flash�е�URL��ȡ������ͬʱ����ز������ú�
 *	data:				����http�ļ�������
 *	server:				URL�еķ�������ַ
 *	����ֵ��			1--�ɹ� -1--ʧ��				
 */
extern int8_t GetURL(char *data,char *server);
#ifdef __cplusplus
}
#endif

#endif
