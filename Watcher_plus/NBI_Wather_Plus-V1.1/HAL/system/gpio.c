/*
**************************************************************************************************************
*	@file	gpio.c
*	@author Jason_531@163.com
*	@version 
*	@date    
*	@brief	GPIO
***************************************************************************************************************
*/
#include "board.h"

/**
  * @brief GPIO��ʼ��
  * @param SX1276 RESET���ų�ʼ��---PB0  
  * @retval None
  */
void SX1276GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��

	GPIO_Initure.Pin=LORA_REST_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_NOPULL; //GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(LORA_IO,&GPIO_Initure);

	HAL_GPIO_WritePin(LORA_IO,LORA_REST_PIN,GPIO_PIN_SET);	//PB0��1 
}

/**
  * @brief GPIO��ʼ��
  * @param  LoRa��Դ����  HOST_2_Lora_DFU_EN���ų�ʼ��---PB15 
  * @retval None
  */
void LoRaPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_Initure.Pin=LORA_POWER_ON; //PB12  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_NOPULL;//GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(LORA_IO,&GPIO_Initure);
	
	GPIO_Initure.Pin=LORA_LED_PIN;     //PA15 
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_NOPULL;//GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	
	HAL_GPIO_Init(LORA_LED,&GPIO_Initure);

	HAL_GPIO_WritePin(LORA_LED,LORA_LED_PIN,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(LORA_IO,LORA_POWER_ON,GPIO_PIN_SET);	
}

/**
  * @brief �ⲿ�жϳ�ʼ��
  * @param huart: DIO0--PB1   DIO1--PB2   DIO2--PB10   DIO3--PB11   
  * @retval None
  */
void SX1276EXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();               		 //����GPIOBʱ��

	GPIO_Initure.Pin=LORA_DIO0|LORA_DIO1|LORA_DIO2;  
	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      			//�����ش���
	GPIO_Initure.Pull=GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
			
	//�ж���2-PC2
	HAL_NVIC_SetPriority(EXTI0_1_IRQn,5,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);             //ʹ���ж���2
	
	//�ж���3-PC3
	HAL_NVIC_SetPriority(EXTI2_3_IRQn,6,0);       //��ռ���ȼ�Ϊ3�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);             //ʹ���ж���3
	
	//�ж���4-PC15
	HAL_NVIC_SetPriority(EXTI4_15_IRQn,7,0);       //��ռ���ȼ�Ϊ0�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);             //ʹ���ж���9
}

void Rs485Pin_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();               		 //����GPIOBʱ��

    GPIO_InitStruct.Pin = Out_12V_ON_Pin_Pin|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|POWER_485IC_Pin;   ///pin3 12V                      
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = Out_485_DE_Pin_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(Out_485_DE_Pin_GPIO_Port, &GPIO_InitStruct);
}

void _rs485_openpin(int index)
{
	uint16_t pin = GPIO_PIN_4;
	for(int i = 0 ; i < 6 ; i ++)
	{			
		HAL_GPIO_WritePin(GPIOB,pin << i,GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(GPIOB,pin << index ,GPIO_PIN_SET);
}

void _rs485_clsoepin(void)
{
	uint16_t pin = GPIO_PIN_4;
	for(int i = 0 ; i < 6 ; i ++)
	{			
        HAL_GPIO_WritePin(GPIOB,pin << i,GPIO_PIN_RESET);
    }
}

void GpioWrite( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState )
{
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin,  PinState);
}

void GpioToggle( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin )
{
  HAL_GPIO_TogglePin(GPIOx,  GPIO_Pin);   
}

uint32_t GpioRead( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin )
{
	return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}
