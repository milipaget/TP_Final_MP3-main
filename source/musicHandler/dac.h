/***************************************************************************//**
  @file     dac.c
  @brief    DAC
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/
#ifndef DAC_H_
#define DAC_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "MK64F12.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void dacInit (void);

void dacDMAEN(void);

void dacSendData(uint16_t datatosend);


#endif /* DAC_H_ */
