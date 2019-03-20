#ifndef __TCP_DEMO_H
#define __TCP_DEMO_H

#include <stdbool.h>
#include "types.h"

/*Socket �˿�ѡ�񣬿ɰ��Լ���ϰ�߶���*/
#define SOCK_TCPS             0
#define SOCK_HUMTEM			  0
#define SOCK_PING			  0
#define SOCK_TCPC             1
#define SOCK_UDPS             2
#define SOCK_WEIBO      	  2
#define SOCK_DHCP             3
#define SOCK_HTTPS            4
#define SOCK_DNS              5
#define SOCK_SMTP             6
#define SOCK_NTP              7
//#define NETBIOS_SOCK    6 //��netbios.c�Ѷ���

extern bool socket_connet;
extern uint32_t Socketopencounter;

extern uint8 server_ip[4];				// ����Զ�̷�����IP��ַ
extern uint16 server_port;

void Systick_Init (uint8_t SYSCLK);
void Delay_ms( uint32_t time_ms );
void Delay_us( uint32_t time_us );

void do_dhcp_ip(void);
void do_tcp_server(void);//TCP Server�ػ���ʾ����
void do_tcp_client(void);//TCP Clinet�ػ���ʾ����
#endif 

