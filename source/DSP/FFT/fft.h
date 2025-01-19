/***************************************************************************//**
  @file     fft.h
  @brief    Fast Fourier Transform Cooley Tukey algorithm
  @author   TEAM OREO
 ******************************************************************************/
#ifndef _FFT_H_
#define _FFT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "arm_const_structs.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SPECTRUM_GROUPS 8
#define FFTSIZE 4096

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void fft_Init();

/*	
 *  @brief Solve the FFT of a digital complex signal in overwriting vector out.
 *
 *	@param input: array with all float32_t values of the signal
 *  @param output: array where the spectrum goes if conserveInput is true
 *  @param samples: amount
 *
 */
void fftGroups(q15_t* input, q15_t* output, uint16_t samples, bool ifft, uint8_t volume);
/*******************************************************************************
 ******************************************************************************/

#endif // _FFT_H_
