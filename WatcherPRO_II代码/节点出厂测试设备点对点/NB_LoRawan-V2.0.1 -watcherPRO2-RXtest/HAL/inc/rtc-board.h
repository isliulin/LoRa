/*
**************************************************************************************************************
*	@file	rtc-board.h
*	@author Ysheng
*	@version 
*	@date    
*	@brief	RTCʱ�������ļ�
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_BOARD_H__
#define __RTC_BOARD_H__

#include <stdio.h>
#include <stdint.h>
#include "timer.h"
#include "board.h"

extern RTC_HandleTypeDef RtcHandle;
/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#endif

/*!
 * \brief Initializes the RTC timer
 *
 * \remark The timer is based on the RTC
 */
void RTC_Init( void );

/*!
 * \brief Start the RTC timer
 *
 * \remark The timer is based on the RTC Alarm running at 32.768KHz
 *
 * \param[IN] timeout Duration of the Timer
 */
void RtcSetTimeout( uint32_t timeout );

/*!
 * \brief Adjust the value of the timeout to handle wakeup time from Alarm and GPIO irq
 *
 * \param[IN] timeout Duration of the Timer without compensation for wakeup time
 * \retval new value for the Timeout with compensations
 */
TimerTime_t RtcGetAdjustedTimeoutValue( uint32_t timeout );

/*!
 * \brief Get the RTC timer value
 *
 * \retval RTC Timer value
 */
TimerTime_t RtcGetTimerValue( void );

/*!
 * \brief Get the RTC timer elapsed time since the last Alarm was set
 *
 * \retval RTC Elapsed time since the last alarm
 */
TimerTime_t RtcGetElapsedAlarmTime( void );

/*!
 * \brief Compute the timeout time of a future event in time
 *
 * \param[IN] futureEventInTime Value in time
 * \retval time Time between now and the futureEventInTime
 */
TimerTime_t RtcComputeFutureEventTime( TimerTime_t futureEventInTime );

/*!
 * \brief Compute the elapsed time since a fix event in time
 *
 * \param[IN] eventInTime Value in time
 * \retval elapsed Time since the eventInTime
 */
TimerTime_t RtcComputeElapsedTime( TimerTime_t eventInTime );

/*!
 * \brief This function blocks the MCU from going into Low Power mode
 *
 * \param [IN] status [true: Enable, false: Disable
 */
void BlockLowPowerDuringTask ( bool status );

/*!
 * \brief Sets the MCU into low power STOP mode
 */
void RtcEnterLowPowerStopMode( void );

/*!
 * \brief Restore the MCU to its normal operation mode
 */
void RtcRecoverMcuStatus( void );

#endif // __RTC_BOARD_H__
