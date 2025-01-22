/***************************************************************************//**
  @file     uart.c
  @brief    driver uart
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <resources/timers/SysTick.h>
#include <stdio.h>
#include <string.h>
//#include "board.h"
#include "gpio.h"
#include "uart.h"
#include "hardware.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MEASURE_IRQ_TIME 1

#define UART_INTERRUPT PORTNUM2PIN(PD,3)

#define UART3_TX PORTNUM2PIN(PC,17)
#define UART3_RX PORTNUM2PIN(PC,16)
#define UART0_TX PORTNUM2PIN(PB,17)
#define UART0_RX PORTNUM2PIN(PB,16)
/*********MAX defines**********/
#define MAX_BUFFER 254
/*******************************/

/*******General purpose defines*****/

/*******************************/

/**************Typedef and enum***************/

/****************************************/


/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
UART_Type* uarts[UART_CANT_IDS]={
							UART0,
							UART1,
							UART2,
							UART3,
							UART4
							};

static bool isRXMessage[UART_CANT_IDS]={false,false,false,false,false};

static uint8_t uartTX_data[UART_CANT_IDS][MAX_BUFFER];
static uint8_t uartRX_data[UART_CANT_IDS][MAX_BUFFER];

static uint16_t pointerBufferTail[UART_CANT_IDS]= {0,0,0,0,0};
static uint16_t cant_Bytes_RX[UART_CANT_IDS]={0,0,0,0,0};
static uint16_t cant_Bytes_TX[UART_CANT_IDS]={0,0,0,0,0};
static uint16_t arrCount[UART_CANT_IDS] = {0,0,0,0,0};
static uint8_t systickTicks;

static bool flagEndTX[UART_CANT_IDS]={1,1,1,1,1};
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
bool uartInit (uint8_t id, uart_cfg_t config)
{

	uint32_t clock;
	uint16_t sbr,brfa;
	uint8_t temp;

	switch(id)
	{
		case 0:
		{
			SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; //enable the clock gating for UART3
			NVIC_EnableIRQ(UART0_RX_TX_IRQn);
			//TX
			PORTB->PCR[PIN2NUM(UART0_TX)] = 0; //Clear
			PORTB->PCR[PIN2NUM(UART0_TX)] |= (((uint32_t)0b011)<<PORT_PCR_MUX_SHIFT);  // 011 for uart porpuse pin.

			//RX
			PORTB->PCR[PIN2NUM(UART0_RX)] = 0; //Clear
			PORTB->PCR[PIN2NUM(UART0_RX)] |= (((uint32_t)0b011)<<PORT_PCR_MUX_SHIFT);  // 011 for uart porpuse pin.
		}break;
		case 1:
		{
			SIM->SCGC4 |= SIM_SCGC4_UART1_MASK; //enable the clock gating for UART3
			NVIC_EnableIRQ(UART1_RX_TX_IRQn);
		}break;
		case 2:
		{
			SIM->SCGC4 |= SIM_SCGC4_UART2_MASK; //enable the clock gating for UART3
			NVIC_EnableIRQ(UART2_RX_TX_IRQn);
		}break;
		case 3:
		{
			SIM->SCGC4 |= SIM_SCGC4_UART3_MASK; //enable the clock gating for UART3
			NVIC_EnableIRQ(UART3_RX_TX_IRQn);

			//TX
			PORTC->PCR[PIN2NUM(UART3_TX)] = 0; //Clear
			PORTC->PCR[PIN2NUM(UART3_TX)] |= (((uint32_t)0b011)<<PORT_PCR_MUX_SHIFT);  // 011 for uart porpuse pin.

			//RX
			PORTC->PCR[PIN2NUM(UART3_RX)] = 0; //Clear
			PORTC->PCR[PIN2NUM(UART3_RX)] |= (((uint32_t)0b011)<<PORT_PCR_MUX_SHIFT);  // 011 for uart porpuse pin.

		}break;
		case 4:
		{
			SIM->SCGC1 |=SIM_SCGC1_UART4_MASK;
			NVIC_EnableIRQ(UART4_RX_TX_IRQn);
		}break;
		case 5:
		{
			SIM->SCGC1 |=SIM_SCGC1_UART5_MASK;
			NVIC_EnableIRQ(UART5_RX_TX_IRQn);
		}break;
		default:
		{
			return false;
		}break;
	}
	//Habliitar pin para medir interrupcion
	gpioMode(UART_INTERRUPT,OUTPUT);

	systickTicks = (config.systickTicks==0) ? 1 : config.systickTicks;
	clock = ((id == 0) || (id == 1))?(__CORE_CLOCK__):(__CORE_CLOCK__>> 1);
	config.baudrate = ((config.baudrate == 0)?(9600):((config.baudrate > 0x1FFF)?(9600):(config.baudrate)));
	sbr = clock/(config.baudrate << 4);
	brfa = (clock << 1)/ config.baudrate - (sbr<<5);


	uarts[id]->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK|UART_C2_RIE_MASK ); // disable while change settings.
	temp = uarts[id]->BDH & ~(UART_BDH_SBR(0x1F)); // safe the first 3 bits of the register.
	uarts[id]->C1 = 0; //clear
	uarts[id]->C1 |= config.parity << UART_C1_PE_SHIFT|config.parityType <<UART_C1_PT_SHIFT |UART_C1_M_MASK;

	uarts[id]->BDH = temp|UART_BDH_SBR(sbr >> 8);
	uarts[id]->BDL = UART_BDL_SBR(sbr);

	uarts[id]->PFIFO = 0b10001000;
	//UART3->PFIFO |= 0b01100110;
	temp = uarts[id]->C4 & ~(UART_C4_BRFA(0x1F));
	uarts[id]->C4 = temp | UART_C4_BRFA(brfa);

	uarts[id]->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK |UART_C2_RIE_MASK );

	return true;
}
uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant)
{
	if(cant_Bytes_TX[id] + cant > 254)
	{
		return 1; //Overflow de data
	}
	cant_Bytes_TX[id] += cant;
	bool bufferPointerChange = false;
	//arrCount[id] = pointerBufferTail[id]; MAL
	for (int i =0 ;i<cant ; i++)
		{
		uint16_t aux = (pointerBufferTail[id]+i);
			if(aux < MAX_BUFFER)
			{
				uartTX_data[id][aux] = msg[i];
			}
			else
			{
				uartTX_data[id][aux-MAX_BUFFER] = msg[i];
				bufferPointerChange = true;
			}

		}
	if(bufferPointerChange)
	{
		pointerBufferTail[id] += cant - MAX_BUFFER;
	}
	else
	{
		pointerBufferTail[id] += cant;
	}

	//msgToBeSend[id] = msg;
	uarts[id]->C2|=UART_C2_TIE_MASK;
	flagEndTX[id]=false;
	return 0;
}
bool uartIsTxMsgComplete(uint8_t id)
{
	return flagEndTX[id];
}
bool uartIsRxMsg(uint8_t id)
{
	return isRXMessage[id];
}
uint8_t uartGetRxMsgLength(uint8_t id)
{
	return cant_Bytes_RX[id];
}
uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant)
{
	cant = (cant <= cant_Bytes_RX[id])?cant:cant_Bytes_RX[id];
	memcpy(msg,uartRX_data[id],cant);
	cant_Bytes_RX[id] -= cant;
	if(cant_Bytes_RX[id] == 0)
	{
		isRXMessage[id] = false;
	}
	else
	{
		for(int i =0; i <cant_Bytes_RX[id]; i++)
		{
			uartRX_data[id][i] = uartRX_data[id][i+cant];
		}
	}
	return cant;
}
void uartDiscardMsg(uint8_t id)
{
	cant_Bytes_RX[id] = 0;
	isRXMessage[id] = false;
}

void UARTX_RX_TX_IRQHandler(uint8_t id)
{
#ifdef MEASURE_IRQ_TIME
	gpioWrite(UART_INTERRUPT,HIGH);
#endif

		static uint8_t status;
		status = uarts[id]->S1;
		if(status & UART_S1_RDRF_MASK)
		{
			uartRX_data[id][cant_Bytes_RX[id]++] = uarts[id]->D;
			isRXMessage[id] = true;
		}
		else if (status & UART_S1_TDRE_MASK)
		{
			if(arrCount[id] >= MAX_BUFFER)
			{
				arrCount[id]=0;
			}
			if(arrCount[id] < MAX_BUFFER)
			{
				uarts[id]->D = (uint8_t)uartTX_data[id][arrCount[id]++];
			}
			else
			{
				uarts[id]->D = (uint8_t)uartTX_data[id][arrCount[id]-MAX_BUFFER];
				arrCount[id]++;
			}

			cant_Bytes_TX[id]--;
			if(cant_Bytes_TX[id] == 0xFFFF)
			{
				cant_Bytes_TX[id] = 0;
			}
			if(!cant_Bytes_TX[id]) //Esto no va a andar por el buffer ciruclar, mirar despues
			{

				flagEndTX[id]= true;
				arrCount[id] = pointerBufferTail[id];
				uarts[id]->C2 &= ~(uint8_t)UART_C2_TIE_MASK;
			}
		}

#ifdef MEASURE_IRQ_TIME
		gpioWrite(UART_INTERRUPT,LOW);
#endif
}

__ISR__ UART0_RX_TX_IRQHandler (void){UARTX_RX_TX_IRQHandler(0);}
__ISR__ UART1_RX_TX_IRQHandler (void){UARTX_RX_TX_IRQHandler(1);}
__ISR__ UART2_RX_TX_IRQHandler (void){UARTX_RX_TX_IRQHandler(2);}
__ISR__ UART3_RX_TX_IRQHandler (void){UARTX_RX_TX_IRQHandler(3);}
__ISR__ UART4_RX_TX_IRQHandler (void){UARTX_RX_TX_IRQHandler(4);}
__ISR__ UART5_RX_TX_IRQHandler (void){UARTX_RX_TX_IRQHandler(5);}

/*******************************************************************************
 ******************************************************************************/

