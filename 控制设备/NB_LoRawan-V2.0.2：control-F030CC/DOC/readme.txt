
Ӳ��IO�ӿ�ͼ������ʾ��
SX1278���Žӿ�˵����DIO4 DIO5����ȡ����

		SX1278					stm32F030CC
							 _ _ _ _ _ _ _ _ _ _ _ 
SPI     NSS	  --------	PA4 |					  |
		SCK	  --------	PA5 |    				  |
		MISO  --------	PA6 |					  |
		MOSI  --------	PA7 |					  |
						    |					  |
EXti	DIO0  --------	PB1 |                     |
		DIO1  --------	PB2 |					  |
		DIO2  --------	PB10|					  |
		DIO3  --------	PB11|					  |
							|					  |
CPIO	RESET --------	PB13|					  |
		DEBUG_LED ----- PA14|					  |
							|					  |
							|					  |
USART1	RX ------------	PB7	|					  |
		TX ------------	PB6	|					  |
							|					  |
USART5  RX ------------ PB4 |					  |
		TX ------------ PB3 |					  |
							|_ _ _ _ _ _ _ _ _ _ _|	



1��ʹ���ڲ�ʱ��HSI����ʹ���ⲿʱ��ԭ��Ӳ��������ʱ�ϵ�RTC��ʼ��ʧ��

2�����ܷ��棺
ADRĬ�Ͽ�����û�йر�

�쳣ԭ��
timer.c��TimerIrqHandler�������

���ݣ�100pf

SX1276Write( 0x4d, 0x87 );///0x87�� Set Pmax to +20dBm for PA_HPͬʱ����PABOOST���ø�PA

McpsIndication.RxData


CLASS C�������⣺�豸���ڴ��ڽ���״̬���׳���LoRa״̬�쳣����˱���ĳЩʱ������ϱ����������������߽���״̬�л�

