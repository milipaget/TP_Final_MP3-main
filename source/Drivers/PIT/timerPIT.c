/***************************************************************************//**
  @file     timerPIT.c
  @brief    Driver for Periodic Interrupt Timer module
  @author   TEAM OREO
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
/*#include "timerPIT.h"
#include "board.h"*/


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/*#define PIT_CANT_TIMERS 4



/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
/*PIT_Type* pits = PIT ;
static pit_config my_configs[PIT_CANT_TIMERS];
static pittimer_callback pitCallbacks[PIT_CANT_TIMERS];


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/*void timerPITinit(pit_config config, pittimer_callback pitcallback )
{
	//Clock gating
	 SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	//enable interrupts
	// NVIC_EnableIRQ(PIT0_IRQn);
//	 NVIC_EnableIRQ(PIT1_IRQn);
//	 NVIC_EnableIRQ(PIT2_IRQn);
//	 NVIC_EnableIRQ(PIT3_IRQn);

	// Module Control Register
	 PIT->MCR = 0x00;	//Enable

	//Save configs
	my_configs[config.id] = config;

	//Load callback
	pitCallbacks[config.id] = pitcallback;
	//Timer Control Register TCTRL
	 // enable Timer interrupts
	 PIT->CHANNEL[config.id].TCTRL = 0x00;

	 //Timer Load Value Register (PIT_LDVALn)
	 // These registers select the timeout period for the timer interrupts.
	 //Bus clock 50MHz --> 20ns
	 PIT->CHANNEL[config.id].LDVAL = config.LDVAL_;
}

void setTimerPITLDVAL(uint8_t id,uint32_t LDVAL)
{
	PIT->CHANNEL[id].LDVAL = LDVAL;
}
uint32_t timerPITgetCurrentVal(pit_id id)
{
	//Current Timer Value Registe
	return PIT->CHANNEL[id].CVAL;
}

void timerPITStop(pit_id id) {
	PIT->CHANNEL[id].TCTRL &= ~PIT_TCTRL_TEN_MASK;		// Stop the timer
}
void timerPITStart(pit_id id) {
	PIT->CHANNEL[id].TCTRL |= PIT_TCTRL_TEN_MASK;		// Start the timer
}
__ISR__ PIT0_IRQHandler(void)
{
	//Clear interrupt flag PIT_TFLG
	pits->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	//sets to 1 at the end of the timer period.
	//Writing 1 to this flag clears it. Writing 0 has no effect.
	//(*pitCallbacks[0])();
}

__ISR__ PIT1_IRQHandler(void)
{
	//Clear interrupt flag PIT_TFLG
	pits->CHANNEL[1].TFLG = PIT_TFLG_TIF(1);
	//sets to 1 at the end of the timer period.
	//Writing 1 to this flag clears it. Writing 0 has no effect.
	(*pitCallbacks[1])();
}

__ISR__ PIT2_IRQHandler(void)
{
	//Clear interrupt flag PIT_TFLG
	pits->CHANNEL[2].TFLG = PIT_TFLG_TIF(1);
	//sets to 1 at the end of the timer period.
	//Writing 1 to this flag clears it. Writing 0 has no effect.
	(*pitCallbacks[2])();
}

__ISR__ PIT3_IRQHandler(void)
{
	//Clear interrupt flag PIT_TFLG
	pits->CHANNEL[3].TFLG = PIT_TFLG_TIF(1);
	//sets to 1 at the end of the timer period.
	//Writing 1 to this flag clears it. Writing 0 has no effect.
	(*pitCallbacks[3])();
}
*/
