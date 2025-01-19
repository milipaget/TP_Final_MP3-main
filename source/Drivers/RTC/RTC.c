/***************************************************************************//**
  @file     dma.h
  @brief    Driver for Dynamic Memory Access
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "RTC.h"
#include "MK64F12.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SECONDS_PER_DAY 86400
/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
uint32_t tmpTSR = 0;
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void RTCInit(void)
{



	SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;
	tmpTSR = RTC->TSR;
	RTC->SR &= ~(RTC_SR_TCE_MASK);
	RTC->TCR= 0;
	RTC->TSR = tmpTSR; // To clear TOF and TIF flags.
	RTC->TAR = SECONDS_PER_DAY; // seconds per day.
	RTC->CR |= RTC_CR_OSCE_MASK;
	RTC->IER &= ~(RTC_IER_TOIE_MASK|RTC_IER_TIIE_MASK);
	RTC->SR |= (RTC_SR_TCE_MASK);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_EnableIRQ(RTC_IRQn);
}
void RTCSetTime(uint8_t hours,uint8_t mins,uint8_t seconds)
{
	uint32_t time = (uint32_t)(hours * 3600 + mins * 60 + seconds);
	if(time < SECONDS_PER_DAY)
	{
		RTC->SR &= ~(RTC_SR_TCE_MASK);
		RTC->TSR = time;
		RTC->SR |= (RTC_SR_TCE_MASK);
	}

}
void RTCGetRealTime(uint8_t* hours,uint8_t* mins,uint8_t* seconds)
{
	uint32_t time = RTC->TSR;
	*seconds = time%60;
	*mins = (time/60)%60;
	*hours = (time / 3600)%24;
}
void RTCgetRealTimeSTR(char* arr)
{
	uint32_t time = RTC->TSR;
	uint8_t seconds = time%60;
	uint8_t mins = (time/60)%60;
	uint8_t hours = (time / 3600)%24;
	arr[0] = ' ';
	arr[1] = ' ';
	arr[2] = ' ';
	arr[3] = ' ';
	arr[4] = (hours/10)%10 + '0';
	arr[5] = hours%10 + '0';
	arr[6] = ':';
	arr[7] = (mins/10)%10 + '0';
	arr[8] = mins%10 + '0';
	arr[9] = ':';
	arr[10] = (seconds/10)%10 + '0';
	arr[11] = seconds%10 + '0';
	arr[12] = ' ';
	arr[13] = ' ';
	arr[14] = ' ';
	arr[15] = ' ';
}
void RTC_IRQHandler()
{

	RTC->SR &= ~(RTC_SR_TCE_MASK);
	RTC->TSR = 1; // To clear TOF and TIF flags.
	RTC->TAR = SECONDS_PER_DAY; // seconds per day.
	RTC->SR |= (RTC_SR_TCE_MASK);
	NVIC_ClearPendingIRQ(RTC_IRQn);

}
