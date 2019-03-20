/*
**************************************************************************************************************
*	@file	sx1276-board.c
*	@author Ysheng
*	@version 
*	@date    
*	@brief	
***************************************************************************************************************
*/
#include "board.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;


/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    SX1276Init,
    SX1276GetStatus,
    SX1276GetTxStatus,
    SX1276SetModem,
	SX1276TxSetModem,
    SX1276SetChannel,
	SX1276TxSetChannel,
    SX1276IsChannelFree,
    SX1276Random,
	SX1276TxRandom,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
	SX1276TxSetSleep,
    SX1276SetStby,
	SX1276TxSetStby,	
    SX1276SetRx,
    SX1276StartCad,
    SX1276ReadRssi,
    SX1276Write,
	SX1276TxWrite,
    SX1276Read,
	SX1276TxRead,
    SX1276WriteBuffer,
	SX1276TxWriteBuffer,
    SX1276ReadBuffer,
	SX1276TxReadBuffer,
    SX1276SetMaxPayloadLength,
	SX1276TxSetMaxPayloadLength
};


/**
  * @brief SX1276 I/O��ʼ��
  * @param �жϳ�ʼ������Դ����λ���ų�ʼ��
  * @retval None
  */
void SX1276IoInit( void )
{
  SX1276EXTI_Init( );			///SX1276: DIO�����ⲿ�ж�
  SX1276GPIO_Init( );		///SX1276: RESET��λ���� 
}

uint8_t SX1276GetPaSelect( uint32_t channel )
{
    if( channel < RF_MID_BAND_THRESH )
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    else
    {
        return RF_PACONFIG_PASELECT_RFO;
    }
}

/**
  * @brief  ��Ƶ����ģʽѡ��
  * @param  ע���ģ�鲻���д˹��ܣ�Ӳ����Ӳ��ֱ������ʹ��
  * @retval None
  */
void SX1276SetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;

        if( status == false )
        {
            SX1276AntSwInit( );
        }
        else
        {
            SX1276AntSwDeInit( );
        }
    }
}

void SX1276AntSwInit( void )
{
    /* RF switch save power mode */
    SX1276.RxTx = 2;
}

void SX1276AntSwDeInit( void )
{
    /* RF switch save power mode */
    SX1276.RxTx = 2;
}

void SX1276SetAntSw( uint8_t rxTx )
{
    if( SX1276.RxTx == rxTx )
    {
        return;
    }

    SX1276.RxTx = rxTx;

    if( rxTx != 0 ) // 1: TX, 0: RX
    {
		/** TX mdoe */
//         GpioWrite( &AntSwitchRxTx1, 1 );
//         GpioWrite( &AntSwitchRxTx2, 0 );
    }
    else
    {
		/** RX mdoe */
//         GpioWrite( &AntSwitchRxTx1, 0 );
//         GpioWrite( &AntSwitchRxTx2, 1 );
    }
}

bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supportted
    return true;
}
