#ifndef __BOOTLOADER_CONFIG_H
#define __BOOTLOADER_CONFIG_H

#ifdef __cplusplus
	extern "C" {
#endif

#define APP_AREA_A			0x30				//A������,��Ӧ�ַ���0
#define APP_AREA_B			0x31				//B������,��Ӧ�ַ���1

#define CANNOTRUN			0x30				//��������,��Ӧ�ַ���0
#define RUNNABLE			0x31				//������,��Ӧ�ַ���1

#define STREAM_ADDR			0x0801ffe0			//�������洢��ַ
#define STREAM_SIZE			0x20				//������ռ�ô洢�ռ�
#define DEVICE_ID_ADDR		0x0801ffc0			//ID�洢��ַ
#define DEVICE_ID_SIZE		0x20				//IDռ�ô洢�ռ�

#define SERVER_PORT_ADDR	0x0801ffb0			//�������˿ڴ洢��ַ
#define SERVER_PORT_SIZE	0x10				//���ַ���ʽ����������˿�
#define SERVER_ADDR			0x0801ff90			//��������ַ
#define SERVER_SIZE			0x20				//��������ַ��󳤶�

#define URL					0x0801feb0			//����ʹ�õ���ַ
#define URL_SIZE			170					//��ַ����ռ�ô洢�ռ�,170
#define APP_AREA_ADD		0x0801fea0			//��������ı����ĵ�ַ��A��Ϊ'0',B��Ϊ'1'
#define AREA_A_RUN			0x0801fea4			//A�����Ƿ��������,'1'������
#define AREA_B_RUN			0x0801fea8			//B�����Ƿ��������,'1'������
#define	APP_AREA_SIZE		0x10				//��������ı���ռ�õĴ洢�ռ�
#define	VER_A				0x0801fe88			//A���汾��
#define	VER_B				0x0801fe70			//B���汾��
#define VER_SIZE			0x18

#define SAMPLE_PERIOD_ADDR	0x0801fe50			//�������ڴ洢��ַ
#define SAMPLE_PERIOD_SIZE	0x10				//��������ռ�ô洢�ռ�
#define	RECIEVE_SEQ			0x0801fe40			//���յ��������,�������п���
#define RECIEVE_SEQ_ACK		0x0801fe44			//��ACK�н��յ���seq,��ΪRECIEVE_SEQ�ı���
#define RECIEVE_SEQ_SIZE	0x10
#define	SEND_SEQ			0x0801fe30			//���͵��������,�������п���,�������������,��ֵ���ܲ����ϸ���������
#define SEND_SEQ_SIZE		0x10
//#define	SEND_SEQ_TOTAL		0x0801fe20			//���͵��ܴ���,��СΪSEND_SEQ_SIZE
//#define	SEND_SEQ_SUCCEED	0x0801fe10			//���ͺ�ɹ��յ��ظ��Ĵ���,��СΪSEND_SEQ_SIZE

//#define	BOOT_TOTAL			0x0801fe00			//��д���ܴ���
//#define BOOT_CMD_TOTAL		0x0801fe04			//����յ������������д������
//#define REBOOT_TOTAL		0x0801fe08			//����дʧ�ܶ�������д������
//#define BOOT_SUCCEED		0x0801fdf0 			//��д�ɹ�������

//#define INIT_SIMCOM_FAIL	0x0801fde0			//��ʼ��simʧ������
//#define SIMCOM_CMD_DO_ERR	0x0801fde4			//SIMCOM����ִ�г���
//#define SIMCOM_CMD_DO_TMEOUT 0x801fde6			//SIMCOM����ִ�г�ʱ
//#define SIMCOM_CMD_BUSY		0x0801fde8			//SIMCOM����æ
//#define SIMCOM_CMD_SEND_ERR	0x0801fdea			//SIMCOM����ͳ���

//#define CONNECT_TCP_FAIL	0x0801fdd0			//����tcpʧ������
//#define	CONNECT_TCP_NONE	0x0801fdd4			//��tcp����
//#define	CONNECT_TCP_FAILURE	0x0801fdd8			//TCP����ʧ�ܣ��������ӳɹ���������ر�����
//#define	CONNECT_TCP_EXISTED	0x0801fddc			//�Ѵ���tcp����

//#define SEND_TCP_DATA_FAIL	0x0801fdc0			//tcp����ʧ������
//#define SEND_TCP_NONE		0x0801fdc4			//��ǰ��tcp����
//#define SEND_TCP_FAILURE	0x0801fdc8			//���ڹ�TCP���ӣ����Ѿ����ر�
//#define SEND_TCP_TRANS_FAIL	0x0801fdcc			//tcp����ʧ��

//#define GPRSPROGRAM_FAIL	0x0801fdb0			//�������ݹ���ʧ������
//#define DOWNLOADDATA_ERR	0x0801fdb4			//�������ݴ����HEX�ļ��������
//#define DOWNLOAD_TIMEOUT	0x0801fdb8			//�������ݳ�ʱ

//#define URL_FAIL			0x0801fda0			//URL��������
//#define URL_ERROR			0x0801fda4			//URLû���ҵ�'/'
//#define URL_EMPTY			0x0801fda8			//URLΪ��

//#define MEMORY_OVERFLOW		0x0801fd90			//�ڴ����,�ڷ�������ʱ�п��ܳ��˴���		
//#define SHOWFLASH			0x0801fd80			//���ڿ����Ƿ�������FLASH�е�����
//#define UPDATE_RETRY		0x0801fd70			//��������

#define CTRL_NAME			0x0801fd30			//���п��Ƶ�����
#define	CTRL_NAME_SIZE		0x10
#define CTRL_STATE			0x0801fd20			//���п��Ƶ�״̬,0�ɹ�,1ʧ��,2��ֹ�ٴη��ʹ˿�������
#define	CTRL_STATE_SIZE		0x10
#define FLAG_ADD			0x0801fd10			//���ݼĴ����Ĵ���λ�ã����ڼ�¼���ֱ�ʶλ
#define	FLAG_ADD_SIZE		0x10

#define SWITCH_REPLY_ADD 	0x0801fd00			//���ڱ�����λ���ػظ�������������
#define DOWNSTREAM_ADD		0x0801fc80			//���ڱ���ɵ����п�������
#define DOWNSTREAM_SIZE		0x80

#define SEND_TOTAL			0x0801fc70			//���͵�ȫ������
#define SEND_FAIL			0x0801fc60			//����ʧ�ܵĴ���
#define SEND_SUCCEED		0x0801fc50			//���ͳɹ��Ĵ���
#define SEND_RECIEVE_ACK	0x0801fc40			//���ͳɹ������յ��������ظ��Ĵ���

#define DATE_ADD            0X0801FC30          //���ڣ�������ַ

#define RS485_CONNECT_ADD   0x0801fc20          //485���ӱ���
#define RS485_CONNECT_SIZE  0x10
#define SEND_FLASH_ADD      0x0801fc10          
#define RS485_CONNECT_SIZE  0x10
#define SAVE_FLASH_ADD      0x0801fc00          
#define RS485_CONNECT_SIZE  0x10

#define BOOTLOADER_SIZE			0x5000			//bootloderռ�õ�flash�ռ�	//20K
#define AREA_SISE				0xa000			//A��B����ռ�õ�flash�ռ�	//40K
#define ERROR_DATA              BOOTLOADER_SIZE+AREA_SISE*2 //�����¼����
#define	DATA_AREA_SIZE			0x1000			//���ݴ��������С		//4K

#define ApplicationAddressA    	(0x8000000+BOOTLOADER_SIZE)			//�û�����A������ڵ�ַ
#define ApplicationAddressB    	(ApplicationAddressA+AREA_SISE)		//�û�����B������ڵ�ַ

#ifdef __cplusplus
}
#endif

#endif
