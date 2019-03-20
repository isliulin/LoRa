/*
 * user_sensor_pro2.c	pro2�豸�Ĵ����������ļ�������I2C��������RS485����������Ҫ�õ�I2C�ʹ���
*/

#include "user_sensor_pro2.h"


extern I2C_HandleTypeDef hi2c2;
extern RTC_HandleTypeDef hrtc;

/*
 *	SensorInit��			��������ʼ��
 *	������					��
 *  ����ֵ:					1�ɹ� 0ʧ��
 */
uint8_t SensorInit(void)
{
	return 1;
}
/*
 *	SernsorGetData��		��ȡ���д���������,�������ݶ�������sensor_data������
 *	����pt_sensor_data��		ָ���ȡ�������ݵĳ�ʼ��ַ
 *  ����ֵ:					0ʧ�� �ɹ�����1,����sensor_data�ĵ�ַ���浽������
 */
uint8_t SernsorGetData(T_SensorData  *pt_sensor_data)
{
	int16_t sh[2]={0};
    //int32_t sh[2]={0};
	uint32_t lux = 0;
    
    memset(pt_sensor_data, 0, 16);
    
	if(SensorMax44009ReadData(&lux)==0)
	{
		DEBUG("��ȡ���ն�ʧ��\r\n");
		pt_sensor_data->count=2;
		pt_sensor_data->data[0]=0;
		pt_sensor_data->data[1]=0;
		//return 0;
	}
	else
	{
		pt_sensor_data->count=2;
		pt_sensor_data->data[0]=(uint16_t)((lux&0xffff0000)>>16);
		pt_sensor_data->data[1]=(uint16_t)(lux&0x0000ffff);
	}
	pt_sensor_data++;
	if(SensorHdc1080ReadData(sh)==0)
	{
		DEBUG("��ȡ��ʪ��ʧ��\r\n");
		pt_sensor_data->count=2;
		pt_sensor_data->data[0]=0;//�¶�
		pt_sensor_data->data[1]=0;//ʪ��
	}
	else
	{
		pt_sensor_data->count=2;
		pt_sensor_data->data[0]=(uint16_t)sh[0];//�¶�
		pt_sensor_data->data[1]=(uint16_t)sh[1];//ʪ��       
	}
	return 1;
}
/******************************************************��ȡ������ʪ��***********************************************/
/*
 *	SensorHdc1080ReadData��	��ȡ������ʪ��
 *	������					measure��������ʪ�����ݣ�[0]Ϊ�¶ȣ�1Ϊʪ��
 *  ����ֵ:					1�ɹ� 0ʧ��
 */
uint8_t SensorHdc1080ReadData(int16_t *measure)
{
	uint8_t Temperature = 0x00;
	uint8_t temdata[2]={0};
	float Temperature_Data, Humidity_Data;
	
	if(HAL_I2C_Master_Transmit(&hi2c2,Hdc1080_ADDR, &Temperature,1, 20)!=HAL_OK)
		return 0;
	HAL_Delay(20);
	if(HAL_I2C_Master_Receive(&hi2c2,Hdc1080_ADDR+1,temdata, 2, 20)!= HAL_OK ) /// hi2c->pBuffPtr++; ///�����ģ�������4Mhz�����׳�������
		return 0;
	Temperature_Data = (temdata[0] << 8 )| temdata[1];
	measure[0] = (((Temperature_Data/ 65536)* 165) - 40)*10;	// �Ŵ�10��������Ҫ����10
	//��������
//	measure[0] = ((((Temperature_Data/ 65536)* 165) - 40)*10)*-1;	// �Ŵ�10��������Ҫ����10
//	DEBUG("Temperature_Data = %02x temdata[1] = %02x \r\n", temdata[0], temdata[1]);
//	DEBUG("Temperature_Data = %02f measure[0] = %d\r\n", Temperature_Data,measure[0]);

//	��ȡʪ��
	uint8_t humdata[2] = {0};
	measure[1] = 0;			//�������
	uint8_t  Humidity	= 0x01;
	if(HAL_I2C_Master_Transmit(&hi2c2,Hdc1080_ADDR, &Humidity,1, 20)!=HAL_OK)
		return 0;
	HAL_Delay(20);
	if(HAL_I2C_Master_Receive(&hi2c2,Hdc1080_ADDR+1,humdata,2, 20) != HAL_OK )
		return 0;
	Humidity_Data = (humdata[0] << 8 )| humdata[1];
	measure[1] = ((Humidity_Data/ 65536)*100)*10;	// �Ŵ�10�������㱣�棬������Ҫ����10
//	DEBUG("Humidity_Data = %02x humdata[1] = %02x \r\n", humdata[0], humdata[1]);
//	DEBUG("Humidity_Data = %02f measure[1] = %d\r\n", Humidity_Data,measure[1]);
	DEBUG("Temperature_Data = %d��C Humidity_Data = %d��RH\r\n", measure[0],measure[1]);
	
	return 1;
}

/******************************************************��ȡ������ʪ��***********************************************/

/******************************************************��ȡ���ն�***********************************************/
/*
 *	SensorMax44009ReadData����ȡ���ն�
 *	������					lux��������ն�
 *  ����ֵ:					0ʧ��,�ɹ����ع��ն�,�����һ����ֻ�Ƿ�����ʱ��������
 */
uint32_t SensorMax44009ReadData(uint32_t *lux)
{
	uint8_t hight,low;
	uint32_t mantissa, exp;
	if(HAL_I2C_Mem_Read(&hi2c2, MAX44009_ADDR, LUX_HIGHT_ADDR, 1, &hight, 1,  1000)!=HAL_OK)
		return 0;
	if(HAL_I2C_Mem_Read(&hi2c2, MAX44009_ADDR, LUX_LOW_ADDR, 1, &low, 1,  1000)!=HAL_OK)
		return 0;

	mantissa = ((hight&0xF)<<4) | (low&0xF);
	exp = (hight&0xF0)>>4;
	if (exp != 0xf)									//����exp������Ч���ж��ˣ���Ϊ�����������mantissaΪ0 exp��Ϊ0�����
	{
		*lux = (mantissa<<exp) * 4.5 *2.032;		// ��Ӧ����0.045������Ŵ���100����������Ҫ����100
	}
	DEBUG("lux:%u\r\n",*lux);
	
	return  *lux;
}
/******************************************************��ȡ���ն�***********************************************/

