/*******************************************************************************
   @file    drv_BOTONES.h
   @brief   Driver que inicializa y maneja el uso de los botones
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
********************************************************************************/

#ifndef _DRV_BOTONES_H_
#define _DRV_BOTONES_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "MK64F12.h"
#include "../../pinout.h"
#include "../../gpio.h"
#include "../timers/timer.h"
#include "../eventos/eventQueue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Esta función se encarga de inicializar todos los botones conectados
 *        encargados del funcionamiento del reproductor.
 * @return void
 */
void initButtons(void);

/**
 * @brief Esta función se encarga de hacer el manejo periódico de los botones.
 * @return void
 */
void buttonMgmtISR(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_BOTONES_H_
