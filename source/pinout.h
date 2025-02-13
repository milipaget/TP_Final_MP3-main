/*******************************************************************************
   @file    pinout.h
   @brief   Archivo que contiene las constantes de los pines utilizados
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#ifndef _PINOUT_H_
#define _PINOUT_H_

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//Pines utilizados para el encoder
#define PORT_ENCODER_RCHA PORTNUM2PIN(PC, 8)
#define PORT_ENCODER_RCHB PORTNUM2PIN(PB, 9)
#define PORT_ENCODER_BUTTON PORTNUM2PIN(PB, 19)

//Pines utilizados para los botones
#define ON_OFF_BUTTON_PIN PORTNUM2PIN(PC,6)
#define NEXT_BUTTON_PIN PORTNUM2PIN(PC,7)
#define PREV_BUTTON_PIN PORTNUM2PIN(PC,0)
#define CHANGE_FILTER_BUTTON_PIN PORTNUM2PIN(PC,9)

//Pines utilizados para la matriz de LEDs
//#define DIN_PIN PORTNUM2PIN(PB, 9) // GPIO to transfer leds data


#define PIN_LED_RED     PORTNUM2PIN(PB,22)
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26)
#define PIN_LED_BLUE    PORTNUM2PIN(PB, 21)

#define LED_ACTIVE      LOW

/*******************************************************************************
 ******************************************************************************/

#endif // _PINOUT_H_
