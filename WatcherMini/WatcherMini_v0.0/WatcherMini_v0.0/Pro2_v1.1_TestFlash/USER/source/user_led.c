/*
 * led.c	LED�����ļ�
*/

#include "user_led.h"

E_LedState  G_Ledstate;          //led��״̬
uint16_t    G_TimeOut=0;           //��ʱ
/*
 * InitLed:				��ʼ��LED
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
void InitLed(void)
{
	//MX_GPIO_Init();�ѳ�ʼ��
	
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	HAL_GPIO_WritePin(GPIOA,LED_Pin, GPIO_PIN_RESET);
//	GPIO_InitStruct.Pin |= LED_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
 * LedOpen:		��Led
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
void LedOpen(void)
{
	//led�޵�Դ���أ�ֱ����VDD���ӣ��͵�ƽ��
	HAL_GPIO_WritePin(OUT_LED_GPIO_Port,OUT_LED_Pin, GPIO_PIN_SET);
}

/*
 * LedClose:			�ر�Led
 * ����:				��
 * ����ֵ:				1�ɹ� 0ʧ��
*/
void LedClose(void)
{
	//led�޵�Դ���أ�ֱ����VDD���ӣ��ߵ�ƽ�ر�
	HAL_GPIO_WritePin(OUT_LED_GPIO_Port,OUT_LED_Pin, GPIO_PIN_RESET);
}

/*
 * LedChangeState:		�л�LED��˸��ʽ
 * State			    LED��˸��ʽ
 * ����ֵ:				��
*/
void LedChangeState(E_LedState State)
{
    G_Ledstate = State;
}

/*
 * �������ڶ�ʱ���ж�ʹ��
 * BaseTime Ϊ��ʱ��������,��λ�Ǻ���,��С������50ms
*/
void LedDisplay(uint16_t BaseTime)
{
     G_TimeOut++;
    if( G_Ledstate == E_LED_LOCATION )
    {
        if( (3000/BaseTime) == G_TimeOut)       //3���л�һ�Σ�����
        {
            HAL_GPIO_WritePin(OUT_LED_GPIO_Port,OUT_LED_Pin, GPIO_PIN_SET);
            G_TimeOut = 0;
        }
    }
    else if( G_Ledstate == E_LED_CONNECT )
    {
        if( (1000/BaseTime) == G_TimeOut)       //1���л�һ�Σ�����
        {
            HAL_GPIO_TogglePin(OUT_LED_GPIO_Port,OUT_LED_Pin);
            G_TimeOut=0;
        }
    }
    else if( G_Ledstate == E_LED_DISCONNECT )   //200ms�л�һ�Σ�����
    {
        if( (200/BaseTime) ==G_TimeOut)
        {
            HAL_GPIO_TogglePin(OUT_LED_GPIO_Port,OUT_LED_Pin);
            G_TimeOut=0;
        }
    }
    else if( G_Ledstate == E_LED_ERROR )        //�쳣,����1�룬����200ms
    {
        if( BaseTime*G_TimeOut<1000)
        {
            LedOpen();
        }
        else
        {
            LedClose();
            if(BaseTime*G_TimeOut>1200)
                G_TimeOut=0;
        }
    }
    else if( G_Ledstate == E_LED_SENDOK )       //�������   
    {
        if( (50/BaseTime) ==G_TimeOut)
        {
            HAL_GPIO_TogglePin(OUT_LED_GPIO_Port,OUT_LED_Pin);
            G_TimeOut=0;
        }
    }
    else if( G_Ledstate == E_LED_INIT )         //��ʼ��    
    {
        LedOpen();//����
        G_TimeOut=0;
    }
}
