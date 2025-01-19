/***************************************************************************/ /**
   @file     PWM.h
   @brief    Simple PWM services
   @author   Autor
  ******************************************************************************/

#ifndef _PWM_H_
#define _PWM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include    "..\SDK\startup\hardware.h"
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
 * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 * @return Registration succeed
 */
void PWM_Init(void);

void PWM_SetDC(float DC);
float PWM_GetDC();

void PWM_SetTickPerPeriod(uint16_t);
uint16_t PWM_GetTickPerPeriod();

void PWM_SetWaveformOffset(uint32_t waveTable_offset);
uint32_t PWM_GetWaveformOffset();

/**
 * @brief Start generating waveform from 16-bit duty-cycle array, setting a callback at the end of every wave_lengh
 * 		  data transmission.
 * @param waveform_pointer pointer to waveform that contain duty-cycles with a maximum of tick per period lenght. Every tick has 20ns duration.
 * @param wave_length number of 16-bit elements that waveform_pointer array contain.
 * @param waveTable_offset number of points to jump between every copy. Use 1 if you want to use every point in array.
 * @param callback funtion to call at the end of every transmission. Use 0 if not needed.
 * @return void
 */
void PWM_GenWaveform(uint16_t *waveform_pointer, uint32_t wave_length, uint32_t waveTable_offset, void (*callback)(void));

/*******************************************************************************
 ******************************************************************************/

#endif // _PWM_H_
