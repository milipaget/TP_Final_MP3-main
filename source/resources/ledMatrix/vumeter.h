/***************************************************************************/ /**
   @file    vumeter.h
   @brief   vumeter driver
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#ifndef _VUMETER_H_
#define _VUMETER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include "matrixLED.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAXLEVEL (MATRIX_ROWS)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {b34Hz, b80Hz, b190Hz, b450Hz, b1K1Hz, b2K5Hz, b6KHz, b14K2Hz}bar;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize vumeter, changing led colors and starting vumeter in "on" mode.
 * @return void
 */
void initVumeter();

/**
 * @brief Turn on vumeter starting function
 * @return void
 */
void vumeterOn();

/**
 * @brief Turn off vumeter. It stop following music
 * @return void
 */
void vumeterOff();

/**
 * @brief Change leds light intensity
 * @param bright desired brightness from 1 to 100
 * @return void
 */
void adjustBrightness(uint8_t bright);

/**
 * @brief Change selected band level. It must be from 0 to MAXLEVEL. MAXLEVEL = 8 in this case
 * @param level desired level
 * @return void
 */
void setLevel(uint8_t level);

/**
 * @brief Select bar to start to blink. Level of selected bar can be change using setLevel function.
 * @param barID Bar
 * @return void
 */
void selectBar(bar barID);

/**
 * @brief Select bar and set level. Use it to do the vumeter.
 * @param barID Bar to change
 * @param level desired level
 * @return void
 */
void setLevelVumeter(bar barID, uint8_t level);

/*******************************************************************************
 ******************************************************************************/

#endif // _VUMETER_H_
