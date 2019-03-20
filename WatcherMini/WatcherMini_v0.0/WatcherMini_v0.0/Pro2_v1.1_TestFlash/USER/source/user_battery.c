/**
  ******************************************************************************
  * �ļ�����: 
  * ��    ��: 
  * ��    ��: 
  * ��д����: 
  * ��    ��: 
  ******************************************************************************
  * ˵����
  * оƬ��������ѹΪ3-16.5V
  * ����Ϊ4.2V
  * �س��Ϊ����4.1V
  ******************************************************************************
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "user_battery.h"
#include "user_adc.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/*
 * IO�ڳ�ʼ��
*/
uint8_t BatInit(void)
{
   if(AdcInit()!=1)
   {
        DEBUG("AdcInit Fail\r\n");
        return 0;
   }
   return 1;
}

/*
 * ��ʼ���
*/
void BatEnableCharge(void)
{
    HAL_GPIO_WritePin(OUT_CH_CE_GPIO_Port,OUT_CH_CE_Pin,GPIO_PIN_RESET);
}

/*
 * �رճ��
*/
void BatDisableCharge(void)
{
    HAL_GPIO_WritePin(OUT_CH_CE_GPIO_Port,OUT_CH_CE_Pin,GPIO_PIN_SET);;
}

/*
 * �����״̬
 * ����ֵ��  
 *      0 -- ���ڳ��
 *      1 -- δ���
 *      2 -- ������
 *      3 -- δ��������
*/
uint8_t BatCheck(uint8_t *bat)
{
    uint8_t s1 = (uint8_t)HAL_GPIO_ReadPin(IN_CH_STAT1_GPIO_Port,IN_CH_STAT1_Pin);
    uint8_t s2 = (uint8_t)HAL_GPIO_ReadPin(IN_CH_STAT2_GPIO_Port,IN_CH_STAT2_Pin);
    uint8_t PG = (uint8_t)HAL_GPIO_ReadPin(IN_CH_PG_GPIO_Port,IN_CH_PG_Pin);
    float bat_v,charge_v;
    
    *bat = BatGetBattery(&bat_v);
    DEBUG("��ص�ѹ��%f\r\n",bat_v);
    DEBUG("��ص�����%u\r\n",*bat);
    
    BatGetChargePower(&charge_v);
    DEBUG("����ѹ��%f\r\n",charge_v);
    if(charge_v<3)//����ѹС��3V
    {
        DEBUG("δ��������\r\n");
        return 3;
    }
    
    if(PG==1)
    {
        DEBUG("�������\r\n");//��������ѹС�ڵ�ص�ѹ
        //return 4;
    }
    switch( (s2<<1) | s1 )
    {
        case 0x01:
            DEBUG("������\r\n");
            return 0;
        case 0x02:
            DEBUG("���ڳ��\r\n");
            return 2;
        default:
            DEBUG("δ���\r\n");
            return 1;
    }
}

/*
 *AdcGetBattery: 		��ȡ��ص�ѹ
 *Battery:				��ص�ѹ
 *����ֵ:				��ص�����0-100%
 */
uint8_t BatGetBattery(float *Battery_V)
{
	*Battery_V=AdcGetVoltage(ADC_CHANNEL_1)*2*1.0;//���Լ���
    uint8_t bat;
    if(*Battery_V<3.6)
    {
        bat=0;
	}
    else
    {
        bat=(int32_t)((*Battery_V-3.6)/(4.18-3.6)*100);//��·�����2M��2M����߳���ѹΪ4.2V
	}
    if(bat>100)
        bat=100;
    return bat;
}

/*
 *AdcGetChargePower: 		��ȡ����ѹ
 *Battery:				��ص�����0-100%
 *����ֵ:				��ص�����0-100%
 */
float BatGetChargePower(float *ChargePower)
{
	*ChargePower=(AdcGetVoltage(ADC_CHANNEL_0)*1.0312)*222/22;//��·�����220K��2M
	return *ChargePower;
}
