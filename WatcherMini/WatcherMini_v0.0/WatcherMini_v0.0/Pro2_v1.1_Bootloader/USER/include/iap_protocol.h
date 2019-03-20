/*
**************************************************************************************************************
*	@file	iap_protocol.h
*	@author 
*	@version 
*	@date    
*	@brief	��������In-Application-ProgrammingЭ��
***************************************************************************************************************
*/
#ifndef __IAP_PROTOCOL_H
#define __IAP_PROTOCOL_H

#include "stm32l0xx_hal.h"
#include "parse_hex_file.h"
#include "bootloader_config.h"
#include <string.h>

#ifdef __cplusplus
	extern "C"{
#endif


/*
 *	InitGprsIAP:	��ʼ��GPRS���߱��
 *	������			��
 *	����ֵ��		��	
 */
extern void InitGprsIAP(void);

/*
 *	UpdateProgramFromGprs:	ͨ��GPRS������������
 *	������					��
 *	����ֵ��				��	
 */
extern int32_t UpdateProgramFromGprs(void);


/*
 *	HandleIAPData:	����IAP����
 *	p��				�������ָ��
 *	len:			���ݳ���
 *	����ֵ��		��
 */
extern void HandleIAPData(char *p, uint32_t len);

/*
 *	GetIAPStatus:	��ȡIAP״̬
 *	����ֵ��		IAP״̬
 *					IAP_FAILURE    --IAPʧ��
 *					IAP_CONTINUE   --IAP������
 *					IAP_SUCCESS	   --IAP�ɹ�����
 * 					IAP_UNKNOWN_CMD--��IAP����
 */
extern int32_t GetIAPStatus(void);
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
