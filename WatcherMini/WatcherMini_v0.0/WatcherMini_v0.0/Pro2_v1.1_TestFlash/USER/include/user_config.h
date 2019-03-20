/*
 * user_config.h	�����ļ�
*/
/**
  ******************************************************************************
  * @file    user_config.h
  * @author  LWY
  * @version V1.0
  * @date    18/01/24
  * @brief   ������ģ�飬����һЩ����
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "stm32l0xx_hal.h"
#include <string.h>
#include "bootloader_config.h"

/* �궨�� --------------------------------------------------------------------*/
/*
__LINE__����Դ�����в��뵱ǰԴ�����кţ�
__FILE__����Դ�ļ��в��뵱ǰԴ�ļ�����
__DATE__����Դ�ļ��в��뵱ǰ�ı�������
__TIME__����Դ�ļ��в��뵱ǰ����ʱ�䣻
__STDC__����Ҫ������ϸ���ѭANSI C��׼ʱ�ñ�ʶ����ֵΪ1��
__cplusplus������дC++����ʱ�ñ�ʶ�������塣
*/

#define __DEBUG__

#ifdef __DEBUG__
    /*�����ض���*/
    #ifdef __GNUC__
      #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
    #else
      #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
    #endif
    //#define DEBUG(format,...) printf("%s,%05d\r\n"format, __FILE__,__LINE__, ##__VA_ARGS__)
    #define DEBUG(format,...) printf(format,##__VA_ARGS__)
#else  
    #define DEBUG(format,...)  do{}while(0)
#endif  

#define STRUCT_VERSION	3			        //���������ݸ�ʽ

extern char ServerAdd[SERVER_SIZE];         // ������IP��ַ
extern char ServerPort[SERVER_PORT_SIZE];   // �������˿�
extern char DeviceID[DEVICE_ID_SIZE];       // �豸ID
extern char Stream[STREAM_SIZE];            // ������

/*���÷�����*/
#define RESERVE_IP1         "60.205.184.237"
#define RESERVE_PORT1       "1111"

#define RESERVE_IP2         "39.108.76.246"
#define RESERVE_PORT2       "1111"

#endif /* __USER_CONFIG_H__ */
