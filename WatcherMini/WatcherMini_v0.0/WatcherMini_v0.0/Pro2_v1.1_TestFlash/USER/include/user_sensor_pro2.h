/*
 * user_sensor_pro2.h	pro2�豸�Ĵ���������ͷ�ļ�������I2C��������RS485������
*/

#ifndef __USER_SENSOR_PRO2_H__
#define __USER_SENSOR_PRO2_H__

#include "user_config.h"


typedef struct SENSORDATA					//����������
{
	uint8_t index;				//����������λ�ñ�ʶ��0���� 1������ʪ�� 2 Socket0�Ĵ��������ݣ�3 Socket1�Ĵ��������ݣ��Դ����ƣ�
	uint8_t add;				//��������ַ(��485�������ĵ�ַ�����ա�������ʪ�ȵĿ��Բ��ø�ֵ)
	uint8_t count;				//����������������
	uint16_t data[2];			//���2������
}T_SensorData;

/******************������ʪ�ȴ�����******************/
/* Register addresses */
#define Hdc1080_ADDR			0x80	//�豸I2C��ַ
#define HDC_TEMPERATURE_ADD		0x00
#define HDC_HUMIDITY_ADD		0x01
#define HDC_CONFIG_ADD          0x02
#define HDC_MANUFACTURE_ID_ADD	0xFE
#define HDC_DEVICE_ID_ADD 		0xFF
#define Manufacturer_ID_value 	0x5449
#define Device_ID_value 		0x1050
/*
 *	SensorHdc1080ReadData��	��ȡ������ʪ��
 *	������					measure��������ʪ�����ݣ�[0]Ϊ�¶ȣ�1Ϊʪ��
 *  ����ֵ:					1�ɹ� 0ʧ��
 */
uint8_t SensorHdc1080ReadData(int16_t *measure);

/******************������ʪ�ȴ�����******************/

/********************���նȴ�����********************/
//#define MAX44009_ADDR		0x94	// MAX44009���豸I2C��ַ����ַ�Žӵ�
#define MAX44009_ADDR		0x96	// MAX44009���豸I2C��ַ����ַ�Ž�VDD
#define LUX_HIGHT_ADDR		0x03	// Lux�Ĵ������ֽڵĵ�ַ
#define LUX_LOW_ADDR		0x04	// Lux�Ĵ������ֽڵĵ�ַ

/*
 *	SensorMax44009ReadData����ȡ���ն�
 *	������					lux��������ն�
 *  ����ֵ:					0ʧ��,�ɹ����ع��ն�,�����һ����ֻ�Ƿ�����ʱ��������
 */
uint32_t SensorMax44009ReadData(uint32_t *lux);
/********************���նȴ�����********************/


/*
 *	SensorInit��			��������ʼ��
 *	������					��
 *  ����ֵ:					1�ɹ� 0ʧ��
 */
uint8_t SensorInit(void);
/*
 *	SernsorGetData��		��ȡ���д���������,�������ݶ�������sensor_data������
 *	����pt_sensor_data��	ָ���ȡ�������ݵĳ�ʼ��ַ
 *  ����ֵ:					0ʧ�� �ɹ�����1,����sensor_data�ĵ�ַ���浽������
 */
uint8_t SernsorGetData(T_SensorData  *pt_sensor_data);

#endif /* __USER_SENSOR_PRO2_H__ */

