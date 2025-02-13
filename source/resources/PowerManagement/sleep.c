
/*****************************************************************************
  @file     sleep.c
  @brief    sleep
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "sleep.h"
#include "hardware.h"
#include "core_cm4.h"
#include "../../gpio.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MY_PORT PC
#define MY_PIN	3
#define MY_LLWU_PIN PORTNUM2PIN(MY_PORT,MY_PIN)
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static bool	setUpDone = false;
/*******************************************************************************
 *******************************************************************************
 *                      GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/*
 * Configure a GPIO pin that will be used to wake from LLS mode.
 * Configure pin as digital input (Kinetis device shown)
 * Configure LLWU module pin PORTE1 (LLWU_P0) as a valid wake-up source.
 */
void setUpSleep()
{

	/* Enable Port C3 to be a digital pin. */
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	/*
	* clear flag if there | set pin functionality -GPIO |
	* falling edge interrupt enable |pull enable |pullup enable
	*/
	PORTC->PCR[MY_PIN]  =	(PORT_PCR_ISF_MASK | PORT_PCR_MUX(01) | PORT_PCR_IRQC(0x0A)|
							PORT_PCR_PE_MASK | PORT_PCR_PS_MASK); //

	/* Set the LLWU pin enable bits to enable the PORTC3 input
	* to be a wake-up source.
	* WUPE0 is used in this case since it is associated with PTE1.
	* This information is in the Chip Configuration chapter of the Reference Manual.
	* 0b10: External input pin enabled with falling edge detection
	*/

	LLWU->PE2 = LLWU_PE2_WUPE7(0b10); //falling edge detection
	NVIC_EnableIRQ(LLWU_IRQn);
	setUpDone = true;
}

void enteringLLS(void)
{

	if(setUpDone)
	{

		SMC->PMPROT = SMC_PMPROT_ALLS_MASK;

		SMC->PMCTRL = SMC_PMCTRL_STOPM(0b011);

		unsigned int dummyread = SMC->PMCTRL;

		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

		__WFI();

	}

}
/*******************************************************************************
 *******************************************************************************
 *                      LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void LLW_IRQHandler(void){

	/* after exiting LLS (can be in LLWU interrupt service Routine)
	 * clear the wake-up flag in the LLWU-write one to clear the flag
	 */
	if (LLWU->F1 & LLWU_F1_WUF7_MASK) {
		  LLWU->F1 |= LLWU_F1_WUF7_MASK;
	}
	//Start everything that we turn off (clock, clock gating, PLL, etc...)
	hw_Init();
}
/*******************************************************************************
 ******************************************************************************/
