/*
**************************************************************************************************************
*	@file		control.c
*	@author Ysheng
*	@version V.0.1
*	@date    2017/8/9
*	@brief �����豸�ļ�
***************************************************************************************************************
*/
#include <math.h>
#include "control.h"
#include "stm32f0xx_hal.h"
#include "delay.h"
#include "debug.h"
#include "user-app.h"

Control_State Control_States = {true, {01,0,0,0}, {0}, 0};

////////////////////////////////////////////////////////// 
// 
//���ܣ�16����תΪ10����
// 
//���룺uint8_t *hex         ��ת����16��������
//      uint8_t length       16�������ݳ���
// 
//?�����
// 
//���ؽ��:int  rslt        ת�����10��������
// 
//˼·:16����ÿ���ַ�λ����ʾ��10���Ʒ�Χ��0 ~255,
//      ����8λ(<<8)�ȼ���256 
// 
///////////////////////////////////////////////////////// 
uint8_t HextoDec(uint8_t *hex, uint8_t length) 
{ 
    int i; 
    uint8_t rslt = 0;
    for(i=0; i<length; i++) 
    { 
        rslt += (uint8_t)(hex[i])<<(8*(length-1-i)); 
                                                         
    }
		delay_ms(2);
		DEBUG(2,"rslt = %d\r\n",rslt);
    return rslt; 
}


/*���ܣ�		��ʼ����ȡ�ֶ�/�Զ�ģʽ
*���ز����� ��
*/
void Get_Work_ModeInit( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();               		 //����GPIOBʱ��

	GPIO_InitStruct.Pin=GPIO_PIN_13;  
	GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING;      			//�����ش���
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
	
		//�ж���4-PC15
	HAL_NVIC_SetPriority(EXTI4_15_IRQn,7,0);       //��ռ���ȼ�Ϊ0�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);             //ʹ���ж���9
}


/*���ܣ�		��ʼ���̵���
*���ز����� ��
*/
void Control_Relay_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;
 
	/* Peripheral clock enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Pin=GPIO_PIN_1; 
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_InitStruct.Pull=GPIO_PULLUP;          //����
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;     //����
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);
}

/*���ܣ�		����STM8��Դ
*���ز����� ��
*/
void Enable_Stm8_Power( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();               		 //����GPIOBʱ��

	GPIO_InitStruct.Pin=GPIO_PIN_12;  
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;      			//����
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);	
}

/*���ܣ�		��ѯ�ֶ����Զ������л�����
*���ز����� ��
*/
void Check_Key_Mode( void )
{
	///������ѯ�������ܣ���ֹMCU�ϵ�û��ѯ���ж�״̬�����²�����
	if( (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)) == KEY_ON ) ///�ֶ�ģʽ
	{
		delay_ms(20);
		
		if( (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)) == KEY_ON ) ///�ֶ�ģʽ
		{
			Control_States.Auto_Mode = false;
		}  
	}
	else
	{
		delay_ms(20);
		
		if( (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)) == KEY_OFF ) ///�Զ�ģʽ
		{
			Control_States.Auto_Mode = true;
		}
	}	
}

/*���ܣ�		���������Ϣ
*��������� RF96���յ������ݾ�ת��Ϊ10����
*���ز����� ��
*/
void Control_Relay(uint8_t *rfdata)
{
	 Control_States.retry_conter = 0;
	 memset(Control_States.send_buf, 0, 4);
	 DEBUG(2,"tx_data000 = %s\r\n",Control_States.control_buf);
	 Control_States.control_buf[0] = 01;
	 Control_States.control_buf[1] = 00;
//	 Control_States.control_buf[2] = HextoDec(&rfdata[0],1);
//	 Control_States.control_buf[3] = HextoDec(&rfdata[1],1);	
	 Control_States.control_buf[2] = rfdata[0];
	 Control_States.control_buf[3] = rfdata[1];	
	
	if(Control_States.control_buf[2] != 0) ///��ִ�в�ѯ����
	{
		do
		{			
			HAL_UART_Transmit(&huart5, Control_States.control_buf, 4, 0xFFFF);
			DEBUG(2,"tx_data555 = %s\r\n",Control_States.control_buf);
			delay_ms(50);
			
			///�ȴ�USART5�������ݴ������״̬
			if(UART_RX_DATA.USART_TX_STATE)
			{
			 for(uint8_t i = 0; i < 4; i++)
			 DEBUG(2,"%02d",UART_RX_DATA.USART_RX_BUF[i]);
			 DEBUG(2,"\r\n");

			 UART_RX_DATA.USART_TX_STATE = false;
			 memcpy(Control_States.send_buf, UART_RX_DATA.USART_RX_BUF, UART_RX_DATA.USART_RX_Len); ///USART5���ջ���copy
			 memset(UART_RX_DATA.USART_RX_BUF, 0, UART_RX_DATA.USART_RX_Len);
			 UART_RX_DATA.USART_RX_Len = 0;
			}			 		 
		Control_States.retry_conter++;
		}while((0 != strcmp((char *)Control_States.control_buf,(char *)Control_States.send_buf)) && Control_States.retry_conter<=2);
	}
	else  ///ִ�в�ѯ����
	{
		HAL_UART_Transmit(&huart5, Control_States.control_buf, 4, 0xFFFF);
		DEBUG(2,"tx_data666 = %s\r\n",Control_States.control_buf);
		delay_ms(50);
		
		///�ȴ�USART5�������ݴ������״̬
		if(UART_RX_DATA.USART_TX_STATE)
		{
		 for(uint8_t i = 0; i < 4; i++)
		 DEBUG(2,"%02d",UART_RX_DATA.USART_RX_BUF[i]);
		 DEBUG(2,"\r\n");
		 UART_RX_DATA.USART_TX_STATE = false;
		 memcpy(Control_States.send_buf, UART_RX_DATA.USART_RX_BUF, UART_RX_DATA.USART_RX_Len);
		 memset(UART_RX_DATA.USART_RX_BUF, 0, UART_RX_DATA.USART_RX_Len);
		 UART_RX_DATA.USART_RX_Len = 0;
		}			 
	}
}
