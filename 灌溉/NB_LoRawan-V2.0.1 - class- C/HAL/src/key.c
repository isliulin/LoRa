
#include "key.h"
#include "rtc-board.h"
#include "stm32l0xx_hal.h"
#include "delay.h"
#include "debug.h"


void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();               		 //����GPIOBʱ��

	GPIO_Initure.Pin=KEY_A_PIN|KEY_B_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      			//�����ش���
	GPIO_Initure.Pull=GPIO_PULLDOWN;
	HAL_GPIO_Init(KEY_PORT,&GPIO_Initure);
	
		//�ж���4-PC15
	HAL_NVIC_SetPriority(EXTI4_15_IRQn,4,0);       //��ռ���ȼ�Ϊ0�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);             //ʹ���ж���9

}

void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();               		 //����GPIOBʱ��

	GPIO_Initure.Pin=GPIO_PIN_15;  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;      			//�����ش���
	GPIO_Initure.Pull=GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);	//PB12��1
}

/*
 * ��ʼ����������
*/
void StandbyInit(void)
{
	/* ����IOӲ����ʼ���ṹ����� */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* ʹ��(����)KEY���Ŷ�ӦIO�˿�ʱ�� */  
	PWR_KEY_RCC_CLK_ENABLE(); 

	/* ����KEY2 GPIO:�ж�ģʽ���½��ش��� */
	GPIO_InitStruct.Pin = WAKEUP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // �ر�ע������Ҫʹ���ж�ģʽ,�����������ش���
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(WAKEUP_PORT, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(PWR_KEY_EXTI_IRQn, 1, 0);	//�˴����ж����ȼ����ܱ�ϵͳʱ�ӵ����ȼ��ߣ����򰴼��������ò�����
	HAL_NVIC_EnableIRQ(PWR_KEY_EXTI_IRQn);			//�ж�ʹ��
}


/**
  * ��������ģʽ(�ػ�)
  */
void StandbyEnterMode(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	Standy_Io_Mode(  );
		
  __HAL_RCC_BACKUPRESET_FORCE();      //��λ��������
  HAL_PWR_EnableBkUpAccess();         //���������ʹ��  
	
	Radio.Sleep( );  ///LoRa������״̬
	
	LoRaPower_Disable(  );
	
	GPS_Disable(  );
		
	__HAL_RTC_WRITEPROTECTION_DISABLE(&RtcHandle);//�ر�RTCд����
	
	
	//�ر�RTC����жϣ�������RTCʵ�����
	__HAL_RTC_WAKEUPTIMER_DISABLE_IT(&RtcHandle,RTC_IT_WUT);
	__HAL_RTC_TIMESTAMP_DISABLE_IT(&RtcHandle,RTC_IT_TS);
	__HAL_RTC_ALARM_DISABLE_IT(&RtcHandle,RTC_IT_ALRA|RTC_IT_ALRB);
	
	//���RTC����жϱ�־λ
	__HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
	__HAL_RTC_TIMESTAMP_CLEAR_FLAG(&RtcHandle,RTC_FLAG_TSF); 
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RtcHandle,RTC_FLAG_WUTF);
	
	/* ���û���Դ:��������PA0 ����PA0����ˮ���������ϣ�Ϊ�������⣬������� */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

//	/* ������л��ѱ�־λ */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  __HAL_RCC_BACKUPRESET_RELEASE();                    //��������λ����
  __HAL_RTC_WRITEPROTECTION_ENABLE(&RtcHandle);     //ʹ��RTCд����

	/* ʹ�ܻ������ţ�PC13��Ϊϵͳ�������� */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);

	/* �������ģʽ */
	HAL_PWR_EnterSTANDBYMode();
	
	
}

/**
  * ��������: ���ڼ�ⰴ���Ƿ񱻳�ʱ�䰴��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����1 ����������ʱ�䰴��  0 ������û�б���ʱ�䰴��
  */
uint8_t StandbyCheckPwrkey(void)
{			
	uint8_t downCnt =0;																//��¼���µĴ���
	uint8_t upCnt =0;																//��¼�ɿ��Ĵ���			
 
	while(1)																		//��ѭ������return����
	{	
		delay_ms(10);																//�ӳ�һ��ʱ���ټ��
		if(HAL_GPIO_ReadPin(WAKEUP_PORT,WAKEUP_PIN) == KEY_ON)			//��⵽���°���
		{		
			downCnt++;																//��¼���´���
			upCnt=0;																//��������ͷż�¼
			if(downCnt>=100)														//����ʱ���㹻
			{
				DEBUG(2,"������Դ��ť \r\n");
				while(HAL_GPIO_ReadPin(WAKEUP_PORT,WAKEUP_PIN) == KEY_ON); ///�ȴ������ͷ�
				return 1; 															//��⵽������ʱ�䳤����
			}
		}
		else 
		{
			upCnt++; 																//��¼�ͷŴ���
			if(upCnt>5)																//������⵽�ͷų���5��
			{
				DEBUG(2,"����ʱ�䲻��\r\n");		
				while(HAL_GPIO_ReadPin(WAKEUP_PORT,WAKEUP_PIN) == KEY_ON); ///�ȴ������ͷ�
				return 0;															//����ʱ��̫�̣����ǰ�����������
			}
		}
	}
}

/*
*�豸����״̬�ж�
*/
void WorkStatusJudgment(void)
{
	 /* ���ϵͳ�Ƿ��ǴӴ���ģʽ������ */ 	
	if(StandbyCheckPwrkey())  ///��IO��Ϣ���޴����ж�
	{
		printf("����\r\n");		
	
			/* ���������־λ */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		__HAL_RCC_CLEAR_RESET_FLAGS();
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);	//PB12��1
		
	}
	else
	{
        DEBUG(3,"ϵͳ��������\r\n");
        StandbyEnterMode( );
	}

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
  {
    DEBUG(3,"\n�����λ������\n");
		__HAL_RCC_CLEAR_RESET_FLAGS();
  }
}


void Standy_Io_Mode(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
	
	HAL_UART_MspDeInit(&huart1);
	
	GPIO_InitStructure.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9|GPIO_PIN_10,GPIO_PIN_SET);	//PB12��1

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
//  __HAL_RCC_GPIOC_CLK_DISABLE();
//  __HAL_RCC_GPIOH_CLK_DISABLE();

}
