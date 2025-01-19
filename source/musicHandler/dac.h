/***************************************************************************//**
  @file     dac.h
  @brief    Driver for Digital to Analog Conversion
  @author   TEAM OREO
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
