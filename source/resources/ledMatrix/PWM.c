/***************************************************************************/ /**
   @file     +Nombre del archivo (ej: template.c)+
   @brief    +Descripcion del archivo+
   @author   +Nombre del autor (ej: Salvador Allende)+
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "DMA.h"
#include "FTM.h"
#include "../../gpio.h"
#include "MK64F12.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum
{
	PORT_mAnalog,
	PORT_mGPIO,
	PORT_mAlt2,
	PORT_mAlt3,
	PORT_mAlt4,
	PORT_mAlt5,
	PORT_mAlt6,
	PORT_mAlt7,
} PORTMux_t;

typedef enum
{
	PORT_eDisabled = 0x00,
	PORT_eDMARising = 0x01,
	PORT_eDMAFalling = 0x02,
	PORT_eDMAEither = 0x03,
	PORT_eInterruptDisasserted = 0x08,
	PORT_eInterruptRising = 0x09,
	PORT_eInterruptFalling = 0x0A,
	PORT_eInterruptEither = 0x0B,
	PORT_eInterruptAsserted = 0x0C,
} PORTEvent_t;
/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
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

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

uint16_t ticksPerPeriod = 100;      // Ticks por período del PWM (determina la frecuencia).
uint16_t *waveform = 0;            // Puntero a la forma de onda personalizada.
uint32_t waveform_lenght = 0;      // Longitud de la forma de onda.
uint32_t waveform_offset = 0;      // Desplazamiento entre puntos de la forma de onda.


// Inicializa el PWM
void PWM_Init(void)
{
	DMA_Init();  // Inicializa el módulo DMA.
	FTM_Init();  // Inicializa el módulo FTM.

	// Configura el pin de salida del PWM (en este caso, PORTC pin 1).
	PORTC->PCR[1] = PORT_PCR_MUX(PORT_mAlt4) | PORT_PCR_DSE(true) | PORT_PCR_IRQC(PORT_eDisabled);

	//  Set FTM configuration
	FTM_SetPrescaler(FTM0, FTM_PSC_x1);
	FTM_SetInterruptMode(FTM0, FTM_CH_0, false); // Enable interrupts

	//	Set FTM as PWM mode
	FTM_SetWorkingMode(FTM0, FTM_CH_0, FTM_mPulseWidthModulation);		 // MSA  / B
	FTM_SetPulseWidthModulationLogic(FTM0, FTM_CH_0, FTM_lAssertedHigh); // ELSA / B

	//  Set PWWM Modulus and initial Duty
	FTM_SetModulus(FTM0, ticksPerPeriod);
	FTM_SetCounter(FTM0, FTM_CH_0, ticksPerPeriod / 2);

	//  Enable FTM0-CH0 DMA Request

	// FTM_StartClock(FTM0);
}

// Cambia el duty cycle del PWM
void PWM_SetDC(float DC)
{
	FTM_SetCounter(FTM0, FTM_CH_0, ticksPerPeriod * (DC / 100.0f));
}

// Devuelve el duty cycle actual como porcentaje
float PWM_GetDC()
{
	return ((float)FTM_GetCounter(FTM0, FTM_CH_0)) / ((float)ticksPerPeriod);
}

// Cambia el número de ticks por período, afectando la frecuencia del PWM
// Además ajusta el DC para que se mantenga igual
void PWM_SetTickPerPeriod(uint16_t tPerPeriod)
{
	FTM_StopClock(FTM0);		// Detiene el temporizador antes de cambiar configuraciones.
	float DC = PWM_GetDC();		// Guarda el duty cycle actual como porcentaje.

	if (ticksPerPeriod > tPerPeriod)
	{
		ticksPerPeriod = tPerPeriod;
		PWM_SetDC(DC);			// Recalcula el valor del duty cycle.
		FTM_SetModulus(FTM0, ticksPerPeriod);
	}
	else
	{
		ticksPerPeriod = tPerPeriod;
		FTM_SetModulus(FTM0, ticksPerPeriod);
		PWM_SetDC(DC);
	}
	FTM_StartClock(FTM0);
}

// Devuelve el período actual.
uint16_t PWM_GetTickPerPeriod()
{
	return ticksPerPeriod;	// Devuelve el período actual (en ticks).
}

/*
Usa el DMA para cargar valores desde un buffer (waveform) al registro de comparación del FTM.
Genera formas de onda personalizadas en el PWM.
Llama a un callback al final de cada transmisión de forma de onda.
*/
void PWM_GenWaveform(uint16_t *waveform_pointer, uint32_t wave_length, uint32_t waveTable_offset, void (*callback)(void))
{
	waveform = waveform_pointer;		// Guarda el puntero a la forma de onda.
	waveform_lenght = wave_length;
	waveform_offset = waveTable_offset;

	FTM_StopClock(FTM0);				// Detiene el FTM para configurarlo.
	FTM_DmaMode(FTM0, FTM_CH_0, true);	// Activa el modo DMA en el FTM.
	FTM_SetInterruptMode(FTM0, FTM_CH_0, true);	// Habilira interrupciones


	// Configura el DMA para transferir los valores de la forma de onda al registro CnV.
	DMA_SetSourceModulo(DMA_CH0, 0);
	DMA_SetDestModulo(DMA_CH0, 0);

	DMA_SetSourceAddr(DMA_CH0, (uint32_t)waveform);
	DMA_SetDestAddr(DMA_CH0, (uint32_t) & (FTM0->CONTROLS[FTM_CH_0].CnV));

	DMA_SetSourceAddrOffset(DMA_CH0, waveform_offset * 2);
	DMA_SetDestAddrOffset(DMA_CH0, 0);

	DMA_SetSourceLastAddrOffset(DMA_CH0, -2 * (int32_t)(waveform_lenght - waveform_offset));
	DMA_SetDestLastAddrOffset(DMA_CH0, 0);

	DMA_SetSourceTransfSize(DMA_CH0, DMA_TransSize_16Bit);
	DMA_SetDestTransfSize(DMA_CH0, DMA_TransSize_16Bit);

	DMA_SetMinorLoopTransCount(DMA_CH0, 2);

	DMA_SetCurrMajorLoopCount(DMA_CH0, waveform_lenght / waveform_offset - 1);
	DMA_SetStartMajorLoopCount(DMA_CH0, waveform_lenght / waveform_offset - 1);

	DMA_SetEnableRequest(DMA_CH0, true);

	DMAMUX_ConfigChannel(DMA_CH0, true, false, kDmaRequestMux0FTM0Channel0);
	DMA_SetChannelInterrupt(DMA_CH0, true, callback);
	DMA_StartTransfer(DMA_CH0);

	FTM_ClearInterruptFlag(FTM0, FTM_CH_0);
	FTM_ClearOverflowFlag(FTM0);
	FTM_StartClock(FTM0);		// Inicia el temporizador.
}

// Ajusta el desplazamiento entre puntos de la forma de onda generada.
void PWM_SetWaveformOffset(uint32_t waveTable_offset)
{
	waveform_offset = waveTable_offset;		// Actualiza el offset de la forma de onda.
	FTM_StopClock(FTM0);					// Detiene el FTM para modificar la configuración.

	uint32_t newCount = ((int32_t)DMA_GetSourceAddr(DMA_CH0) - (int32_t)waveform) / (2 * waveform_offset);
	if (newCount > 109)
		waveform = waveform;
	if (newCount < 0)
		waveform = waveform;
	DMA_SetSourceAddr(DMA_CH0, (uint32_t)waveform + newCount * waveform_offset * 2);
	DMA_SetSourceAddrOffset(DMA_CH0, waveform_offset * 2);
	DMA_SetCurrMajorLoopCount(DMA_CH0, waveform_lenght / waveform_offset - 1 - newCount);
	DMA_SetStartMajorLoopCount(DMA_CH0, waveform_lenght / waveform_offset - 1);
	DMA_SetSourceLastAddrOffset(DMA_CH0, -2 * (int32_t)(waveform_lenght - waveform_offset));
	FTM_StartClock(FTM0);
	// DMA_StartTransfer(DMA_CH0);
}
uint32_t PWM_GetWaveformOffset();
/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
