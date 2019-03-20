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
#include "adc.h"
#include "gpio.h"
#include "power.h"
#include "debug.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/*
 * Bq24195Init:			��ʼ�������ԴоƬ
 * ����:				��
 * ����ֵ:				��
*/
void Bq24195Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE(); 
    __HAL_RCC_GPIOB_CLK_ENABLE();               		 //����GPIOBʱ��
   
    GPIO_InitStruct.Pin = Out_CH_CE_Pin_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(Out_CH_CE_Pin_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = In_CH_INT_Pin_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(In_CH_INT_Pin_GPIO_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = IN_CH_STATE1_Pin|IN_CH_STATE2_Pin|IN_CH_PG_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(IN_CH_STATE1_GPIO_Port, &GPIO_InitStruct);
}


/*
 * ��ʼ���
*/
void BatEnableCharge(void)
{
    HAL_GPIO_WritePin(Out_CH_CE_Pin_GPIO_Port,Out_CH_CE_Pin_Pin,GPIO_PIN_RESET);
}

/*
 * �رճ��
*/
void BatDisableCharge(void)
{
    HAL_GPIO_WritePin(Out_CH_CE_Pin_GPIO_Port,Out_CH_CE_Pin_Pin,GPIO_PIN_SET);;
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
    uint8_t s1 = (uint8_t)HAL_GPIO_ReadPin(IN_CH_STATE1_GPIO_Port,IN_CH_STATE1_Pin);
    uint8_t s2 = (uint8_t)HAL_GPIO_ReadPin(IN_CH_STATE2_GPIO_Port,IN_CH_STATE2_Pin);
    uint8_t PG = (uint8_t)HAL_GPIO_ReadPin(IN_CH_PG_GPIO_Port,IN_CH_PG_Pin);
       
    if (CheckBattery()<=3)					//��ص�����
    {
        DEBUG(2,"battery extremely low %dmV;enter standby\r\n",RF_Send_Data.Battery*6+3600);
        IntoLowPower( ); 					//��ص����ر�ͣ�ֱ�����ý�������
    }

    float bat_v,charge_v;
    
//    *bat = BatGetBattery(&bat_v);
//    DEBUG("��ص�ѹ��%f\r\n",bat_v);
//    DEBUG("��ص�����%u\r\n",*bat);
//    
//    BatGetChargePower(&charge_v);
//    DEBUG("����ѹ��%f\r\n",charge_v);
    if(charge_v<3)//����ѹС��3V
    {
        DEBUG(2,"δ��������\r\n");
        return 3;
    }
    
    if(PG==1)
    {
        DEBUG(2,"������ߣ���������ѹС�ڵ�ص�ѹ\r\n");
        //return 4;
    }
    switch( (s2<<1) | s1 )
    {
        case 0x01:
            DEBUG(2,"���ڳ��\r\n");
            return 0;
        case 0x02:
            DEBUG(2,"������\r\n");
            return 2;
        default:
            DEBUG(2,"δ���\r\n");
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
//	*Battery_V=AdcGetVoltage(ADC_CHANNEL_1)*2*1.0312;//���Լ���
//    uint8_t bat;
//    if(*Battery_V<3.6)
//    {
//        bat=0;
//	}
//    else
//    {
//        bat=(int32_t)((*Battery_V-3.6)/(4.18-3.6)*100);//��·�����2M��2M����߳���ѹΪ4.2V
//	}
//    if(bat>100)
//        bat=100;
//    return bat;
}

/*
 *AdcGetChargePower: 		��ȡ����ѹ
 *Battery:				��ص�����0-100%
 *����ֵ:				��ص�����0-100%
 */
float BatGetChargePower(float *ChargePower)
{
//	*ChargePower=(AdcGetVoltage(ADC_CHANNEL_0)*1.0312)*222/22;//��·�����220K��2M
//	return *ChargePower;
}
