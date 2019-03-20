/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f2xx_hal.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

#include "board.h"
#include "etimer.h"
#include "autostart.h"
#include "sys/mt.h"
#include "sim7600.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/*!***********************************???D?��??************************************/

#if  OVER_THE_AIR_ACTIVATION

extern uint8_t DevEui[8];
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

extern TimerEvent_t JoinReqTimer;
extern volatile bool IsNetworkJoined;
extern bool JoinReq_flag;

#endif

LoRaMacRxInfo *loramac_rx_info;
mac_evt_t loramac_evt;


void app_mac_cb (mac_evt_t evt, void *msg)
{
    switch(evt){
    case MAC_STA_TXDONE:                
    case MAC_STA_RXDONE:
    case MAC_STA_RXTIMEOUT:
    case MAC_STA_ACK_RECEIVED:
    case MAC_STA_ACK_UNRECEIVED:
    case MAC_STA_CMD_JOINACCEPT:         
    case MAC_STA_CMD_RECEIVED:
         loramac_rx_info = msg;   
         loramac_evt = evt;
         
         break;
    }
}


/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

extern TimerEvent_t SleepTimer;

void OnSleepTimerTimerEvent( void )
{
	DEBUG(2,"%s\r\n",__func__);

	TimerStop( &SleepTimer );	
	TimerSetValue( &SleepTimer, 5000 );
	TimerStart( &SleepTimer );
}

PROCESS(Netsend_process,"Netsend_process");
PROCESS(NetReceive_process,"NetReceive_process");
PROCESS(NetProtect_process,"NetProtect_process");
AUTOSTART_PROCESSES(&NetProtect_process,&Netsend_process); //

static process_event_t LoRaReceiveDone;

void Netsend_post(void)
{
    process_post(&Netsend_process,LoRaReceiveDone,NULL);
}

/*
*RFTXDONE�����÷���������ȼ�
*������    ��
*����ֵ��  ��
*/
void RFTXDONE(void)
{
	process_poll(&Netsend_process); ///���ý������ȼ�����Ӧ�������
}

PROCESS_THREAD(NetReceive_process,ev,data)
{
	static struct etimer et;
	PROCESS_BEGIN();
	
	USR_UsrLog("Contiki System SX1278Receive Process..."); 
	etimer_set(&et,CLOCK_SECOND*40);
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		DEBUG(2,"hello world\r\n");
        
        ///���շ��������п����������������Ҫ����A��ģʽ�±�������������ʱ�������У��������������ȴ������в�����
        ///Ҫ����������֡��Ϊ����A/C����Ϊ�������ж�����ʹ��
        ///LoRaMacDevAddr == �����豸ID��
        ///user_app_send ��������ע�⣺CLASS A����A����ֻҪ1Sʱ�䣬��Ҫ�������ʱ������س���node����ʧ��
        
        ///�㲥���ݴ���LoRaMacDevAddrΪ�����ַ��ֻ���C��
		etimer_reset(&et);
	}
	
	PROCESS_END();
}

extern bool Send_Ack;

/*
*Function:NetProtect Process�������쳣��������
*Input Paragramed:
*Output Paragramed:
*Remarks:
*Note: 4Gģ�鲻��Ҫ������������ƣ�Ӳ����������Ч/��̫��ģ�鹦�ܴ�����
*/
PROCESS_THREAD(NetProtect_process,ev,data) 
{
    static struct etimer timer;
    
    static uint32_t SimcomTimer = 0;
	
	PROCESS_BEGIN();
	
	USR_UsrLog("Contiki System NetProtect Process..."); 
    	
	etimer_set(&timer,CLOCK_SECOND*1);
    while(1)
	{	
		PROCESS_YIELD();
		
		if(ev == PROCESS_EVENT_TIMER) ///�����쳣��������
		{	 
            if(InternetMode == LAN) ///��̫�����
            {
                DEBUG(2,"----InternetMode----LAN\r\n");    
                do_tcp_client(  );  ///��̫���������� 
            }
            else if(InternetMode == SIMCOM) ///4Gģʽ���
            {
                DEBUG(2,"----InternetMode----SIMCOM\r\n");   
                ///���类�Ͽ����ٴγ�ʼ������ REPLY_NONE
                if(commandid_reply == REPLY_CIPCLOSE || commandid_reply == REPLY_ERROR || commandid_reply == REPLY_NONE) ///�������Ͽ�/�ϵ������������������
                {
                    DEBUG(2,"SimcomConnectServer\r\n"); 
                    SimcomConnectServer( );
                }
                uint8_t dhcpret = check_DHCP_state(SOCK_DHCP);
                if(DHCP_RET_UPDATE == dhcpret)
                    InternetMode = LAN;    
            }                

            etimer_reset(&timer);
        }        
	}
	PROCESS_END();
}

/*
*Function:Netsend Process: 4G/��̫�����ͺ���
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
PROCESS_THREAD(Netsend_process,ev,data)
{
	static struct etimer timer;
    static uint8_t  buf[250] = {0};
    static uint8_t  len = 0;

	PROCESS_BEGIN();
	
	USR_UsrLog("Contiki System Netsend Process..."); 
    LoRaReceiveDone = process_alloc_event();
        	
	while(1)
	{	
		PROCESS_YIELD();

        if(ev == LoRaReceiveDone)///�첽����
        {  
            DEBUG(2,"----reply111----%d %d\r\n",Send_Ack,Net_Buffers.Receive);
            
            /**********************4G���緢������***************************/
            if(InternetMode == SIMCOM)
            {
                if(commandid_reply == REPLY_CIPOPEN)
                {
                   ///����LORa���ݣ����з�������첽�źŴ���ģʽ                                            
                    DEBUG(2,"----reply0123----%d\r\n",commandid_reply); 
                    SpliceSend(buf, len);
                    SimcomSendData((char *)buf,len);
                    memset(buf, 0, strlen((char *)buf));
                    HAL_Delay(100);
                }
            }
            /**********************��̫����������***************************/
            else if(InternetMode == LAN)
            {
                uint16_t SR_Data = getSn_SR(SOCK_TCPC);
                
                if(SOCK_ESTABLISHED == SR_Data)
                {
                    SpliceSend(buf, len);
                                        
                    send(SOCK_TCPC,(uint8_t *)buf,strlen((char *)buf));
                    DEBUG(2," send....����%s\r\n",buf);
                    memset(buf, 0, strlen((char *)buf));   
                }                    
            }
            
            if(Net_Buffers.Receive)  ///�������·����ݣ�4G����̫��
            {
               HAL_Delay(1000); ///end nodeʱ��ƥ�䣺������Ҫ����
               Net_Buffers.Receive = false;
               ///LoRaMacDevAddr == �����豸ID��
//               user_app_send(UNCONFIRMED, "helloworld", strlen("helloworld"), 2);�������ݴ���
            }
            else if(!Net_Buffers.Receive && Send_Ack)///�Ƿ���������
            {
                HAL_Delay(1000); ///end nodeʱ��ƥ��
                Send_Ack = false;               
                DEBUG(2,"----reply222----%d\r\n",commandid_reply);
                if(LoRapp_SenSor_States.loramac_evt_flag)
                {
                    user_app_send(UNCONFIRMED, "helloworld", strlen("helloworld"), 2);                                   
                    DEBUG(2,"----reply333----%d\r\n",commandid_reply);
                    LoRapp_SenSor_States.loramac_evt_flag = 0;                                        
                }           
            }
        }    
	}
	PROCESS_END();
}

/*******************************************************************************
  * @��������	main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��

	�汾˵����
	��1����V1.0.1��MCU---stm32F02������WG;

	�Ż����ܣ�
	��1���� ʵ��LORAWAN������ͨ�š�
	��2���� SPI2 --- TX  SPI3 --- RX
    ��3���� �շ����ݸ�ʽ��״̬�����ֿ���������ڹ����շ��쳣����
    ��4���� �����ŵ��滮��TX��Ӧnode RX1 = RX2+200e3
    ��5���� ��̫��IP��server_ip

	
  *****************************************************************************/
/* variable functions ---------------------------------------------------------*/	

/* USER CODE END 0 */

int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* USER CODE BEGIN Init */

    BoardInitMcu(  );

    /* USER CODE END Init */

    /* USER CODE BEGIN SysInit */

    user_app_init(app_mac_cb);

    LoRaMacTestRxWindowsOn( false ); ///�رս��մ���

    LoRaMacChannelAddFun(  );

    Channel = 6; ///��ȡ�ŵ�ID 4Gģ���ȡ

    LoRaCad.Iq_Invert = true;  ///ʹ�ܽڵ��ͨ��

    ReportTimerEvent = true;
    LoRapp_SenSor_States.loramac_evt_flag = 1;

    LoRapp_SenSor_States.AT_PORT = randr( 1, 0xDF );
    Net_Buffers.SensorBuf = (uint8_t *)malloc(sizeof(uint8_t)*64); ///ʹ��ָ���������ַ�ռ䣬��������HardFault_Handler����
    Net_Buffers.Device_Id = (char *)malloc(sizeof(char)*16); 

    /* USER CODE END SysInit */

    /* Initialize interrupts */
    MX_NVIC_Init();

    /* USER CODE BEGIN 2 */
      
    clock_init();

    TimerInit( &ReportTimer, OnReportTimerEvent );

    LoRaMacSetDeviceClass( CLASS_C );

    Systick_Init(100);

    reset_w5500();                     /* W5500Ӳ����λ */  
    printf("verson: %02x\r\n", IINCHIP_READ(VERSIONR)); ///verson==4 ��SPI���� 0x003900������0x0039�����ȡʧ��

    SimcomPower(OPEN);
  /*******************************4G����̫��ѡ��Ĭ����̫��ģʽ*****************************/
  
    DEBUG(2,"-----Init Ethernet-----\r\n");
    set_w5500_mac();
    init_dhcp_client();	

    uint32_t timeover = HAL_GetTick();
    do_dhcp_ip( ); ///��Ҫ��ʱ���ƣ��л�Ϊ4G��ʼ��.10S��ʱ����

    DEBUG(2,"W5500 Init Complete! %d\r\n",HAL_GetTick()-timeover);
    timeover = 0;
    
    if(INITSIMDONE == InitSimcom(  ))
    {
        DEBUG(2,"-----Init Simcom Done-----\r\n");
        SimcomExecuteCmd(cmds[CTCPKA]); ///����������
        
        SimcomExecuteCmd(cmds[CIPMODE]);
        
        SimcomOpenNet(  );
    
        SimcomConnectServer(  );
    }
    else
    {
        DEBUG(2,"-----Init Simcom Fail-----\r\n");
    }	
    
    ///03022820TEST0001
    memcpy(Net_Buffers.Device_Id, "03022820TEST0002", 16); ///�豸ID
    Net_Buffers.Versions = VERSIONS;     ///�汾��
    Net_Buffers.Type = TYPE;  
    Net_Buffers.Command  = COMMANDS;      ///����ָ��

    DEBUG(2,"Net_Buffers.Device_Id: %s\r\n",Net_Buffers.Device_Id);
        
    process_init();
    process_start(&etimer_process,NULL); ///�Զ�����������߳�
    autostart_start(autostart_processes);
      
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
     do
    {
    }while(process_run() > 0);
    }
    /* USER CODE END 3 */

}

/** NVIC Configuration
*/
void MX_NVIC_Init(void)
{
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
 
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 8, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
