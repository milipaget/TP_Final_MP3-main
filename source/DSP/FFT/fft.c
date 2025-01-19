
/*****************************************************************************
  @file     fft.c
  @brief    Fast Fourier Transform thanks ARM
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fft.h"
#include <arm_math.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/* Length of the overall data in the test */
#define TESTLENGTH 4096

/* Block size for the underlying processing */
#define BLOCKSIZE TESTLENGTH

/* Total number of blocks to run */
#define NUMBLOCKS (TESTLENGTH/BLOCKSIZE)

/* Number of 2nd order Biquad stages per filter */
#define NUMSTAGES 2

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

/* ------------------------------------------------------------------
* Global variables for FFT Bin Example
* ------------------------------------------------------------------- */
arm_rfft_fast_instance_f32 fftInst[5];
uint16_t sizes[5] = {256, 512, 1024, 2048, 4096};

/*******************************************************************************
 *******************************************************************************
 *                      GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void fft_Init()
{
	for (uint8_t i = 0; i < 5; i++)
	{
		arm_rfft_fast_init_f32(&fftInst[i], sizes[i]);
	}
}

void fftGroups(q15_t* inputQ15, q15_t* outputQ15, uint16_t samples, bool ifftFlag, uint8_t volumen)
{
	int8_t n = -1;

	// me adapto a la cantidad de samples recibida (entre 256 y 4096)
	for (uint8_t i = 4; i >= 0; i--)
	{
		if (samples >= sizes[i])
		{
			n = i;
			break;
		}
	}

	float32_t complexOutput[2*4096], magnitudeOutput[4096];
	float32_t inputF32[4096], outputF32[SPECTRUM_GROUPS];

	arm_q15_to_float(inputQ15, inputF32, sizes[n]);	// amplitud entre (-1, 1)

	arm_rfft_fast_f32(&fftInst[n], inputF32, complexOutput, ifftFlag);
	arm_cmplx_mag_f32(complexOutput, magnitudeOutput, sizes[n]/2);
	magnitudeOutput[0] /= 2;		// para compensar que no considero los pares de frecuencias

	//uint16_t groupSize = sizes[n]/(2*SPECTRUM_GROUPS);  // linear
	uint16_t groupSize = sizes[n]/4;					  // log

	// arma los grupos de mayor tamaño a mayores frecuencias, es decir, la division es logaritmica
	for (int i = SPECTRUM_GROUPS - 1; i >= 0; i--)	// cada grupo
	//for (int i = 0; i < SPECTRUM_GROUPS; i++)
	{
		outputF32[i] = 0;

//		if (i != 0)
			groupSize /= 2;

		for (int j = 0; j < groupSize; j++)	// cada elemento del grupo
		{
//			if (i != 0)
				outputF32[i] += ((float) volumen / 15.0) * magnitudeOutput[groupSize + j] / groupSize;
//			else
//				outputF32[i] += magnitudeOutput[j] / groupSize;
		}

		/*for (int j = 0; j < groupSize; j++)	// cada elemento del grupo
		{
			outputF32[i] += 8 * magnitudeOutput[i*groupSize + j];
		}*/
	}

	//normalizo para indicar la cantidad de LEDs
	uint8_t levels[8] = {3, 6, 10, 18, 32, 56, 100, 178};
	for (uint8_t i = 0; i < SPECTRUM_GROUPS; i++)
	{
		outputQ15[i] = 0;
		for (uint8_t j = 8; j > 0; j--)
		{
			if (outputF32[i] >= levels[j-1])
			{
				outputQ15[i] = j;
				break;
			}
		}
//		outputQ15[i] = outputF32[i] / 6;	// este se ajusta empiricamente
	}
}

/*******************************************************************************
 *******************************************************************************
 *                      LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/
