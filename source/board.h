/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"


//Para probar con switch de la placa definir DEBUG
//#define DEBUG 1

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

/*#define MEASURE_IRQ_TIME 1*/

/***************Pins for Interruptions*********************/
#define SEGMENT_E PORTNUM2PIN(PC,0)
#define SEGMENT_D PORTNUM2PIN(PC,9)
#define SEGMENT_C PORTNUM2PIN(PC,8)
#define SEGMENT_B PORTNUM2PIN(PC,1)
#define SEGMENT_A PORTNUM2PIN(PB,19)

#define MEASURE_IRQ_FIR PORTNUM2PIN(PB,9)
#define MEASURE_IRQ_FTM3 PORTNUM2PIN(PD,2)

#define SEL_0 	  PORTNUM2PIN(PB,2)
#define SEL_1 	  PORTNUM2PIN(PB,3)
#define STATUS_0  PORTNUM2PIN(PB,10)
#define STATUS_1  PORTNUM2PIN(PB,11)
/****************************************************/

/***************Pins for Magnetic Card****************/
#define PIN_DATA      PORTNUM2PIN(PD,1)     //AZUL
#define PIN_CLOCK     PORTNUM2PIN(PD,3)     //VERDE
#define PIN_ENABLE    PORTNUM2PIN(PD,2)     //AMARILLO
/****************************************************/

/***************Pins for Encoder*********************/
#define PIN_ENCODER_A 	PORTNUM2PIN(PD,0)
#define PIN_ENCODER_B 	PORTNUM2PIN(PC,4)

#define PIN_ENCODER_C 	PORTNUM2PIN(PC,3)//


#define ENCODER_ACTIVE 	LOW
/****************************************************/

/****************************************************/
// On Board User LEDs
#define PIN_LED_RED     PORTNUM2PIN(PB,22)
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26)
#define PIN_LED_BLUE    PORTNUM2PIN(PB, 21)

#define LED_ACTIVE      LOW

// On Board User Switches
#define PIN_SW2         PORTNUM2PIN(PC, 6)
#define PIN_SW3         PORTNUM2PIN(PA, 4)
#define SW_ACTIVE       LOW
/****************************************************/
#define SD_PIN			PORTNUM2PIN(PE, 6)
/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
