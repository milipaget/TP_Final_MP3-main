/***************************************************************************/ 
/**
 * archivo está diseñado para controlar los temporizadores FTM (Flexible Timer Module) 
 * del microcontrolador K64, permitiendo configurar y manejar distintos modos de operación.
 */
 
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "FTM.h"         // Librería FTM para control del temporizador
#include    "../../gpio.h"             // Librería GPIO para manejar pines
#include "MK64F12.h"          // Definiciones específicas del microcontrolador K64
#include <stdlib.h>           // Funciones estándar de C
#include <stdint.h>           // Tipos de datos estándar
#include <stdbool.h>          // Soporte para valores booleanos

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

static void (*FTM_Interruption)(void) = NULL;   // Puntero a función de interrupción

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void FTM_Init(void)
{
	// Habilitar relojes para los módulos FTM
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;

	// Habilitar interrupciones para cada FTM
	NVIC_EnableIRQ(FTM0_IRQn);
	NVIC_EnableIRQ(FTM1_IRQn);
	NVIC_EnableIRQ(FTM2_IRQn);
	NVIC_EnableIRQ(FTM3_IRQn);
}

// Funciones para configurar el temporizador FTM

void FTM_SetPrescaler(FTM_t ftm, FTM_Prescal_t data)
{
	// Configura el prescaler para dividir la frecuencia del reloj del temporizador
	ftm->SC = (ftm->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(data);
}

void FTM_SetModulus(FTM_t ftm, FTMData_t data)
{
	// Configura el valor máximo del contador del temporizador
	ftm->CNTIN = 0X00;
	ftm->CNT = 0X00;
	ftm->MOD = FTM_MOD_MOD(data);
}

FTMData_t FTM_GetModulus(FTM_t ftm)
{
	// Obtiene el valor máximo del contador
	return ftm->MOD & FTM_MOD_MOD_MASK;
}

void FTM_StartClock(FTM_t ftm)
{
	// Inicia el contador del temporizador
	ftm->SC |= FTM_SC_CLKS(0x01);
}

void FTM_StopClock(FTM_t ftm)
{
	// Detiene el contador del temporizador
	ftm->SC &= ~FTM_SC_CLKS(0x01);
	ftm->CNT = 0;
}

void FTM_SetOverflowMode(FTM_t ftm, bool mode)
{
	// Configura el modo de interrupción por desbordamiento
	ftm->SC = (ftm->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(mode);
}

bool FTM_IsOverflowPending(FTM_t ftm)
{
	// Verifica si está pendiente una interrupción por desbordamiento
	return ftm->SC & FTM_SC_TOF_MASK;
}

void FTM_ClearOverflowFlag(FTM_t ftm)
{
	// Limpia la bandera de desbordamiento
	ftm->SC &= ~FTM_SC_TOF_MASK;
}

void FTM_SetWorkingMode(FTM_t ftm, FTMChannel_t channel, FTMMode_t mode)
{
	// Configura el modo de trabajo del canal (PWM, captura, etc.)
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) |
								  (FTM_CnSC_MSB((mode >> 1) & 0X01) | FTM_CnSC_MSA((mode >> 0) & 0X01));

	if (mode == FTM_mPulseWidthModulation)
	{
		// Si es PWM, actualiza la configuración
		ftm->PWMLOAD = (FTM_PWMLOAD_LDOK_MASK | (0x01 << channel) | 0x200);
	}
}

FTMMode_t FTM_GetWorkingMode(FTM_t ftm, FTMChannel_t channel)
{
	// Obtiene el modo de trabajo actual del canal
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) >> FTM_CnSC_MSA_SHIFT;
}

void FTM_SetInputCaptureEdge(FTM_t ftm, FTMChannel_t channel, FTMEdge_t edge)
{
	// Configura el flanco de captura de entrada (subida o bajada)
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
								  (FTM_CnSC_ELSB((edge >> 1) & 0X01) | FTM_CnSC_ELSA((edge >> 0) & 0X01));
}

FTMEdge_t FTM_GetInputCaptureEdge(FTM_t ftm, FTMChannel_t channel)
{
	// Obtiene el flanco de captura de entrada configurado
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetOutputCompareEffect(FTM_t ftm, FTMChannel_t channel, FTMEffect_t effect)
{
	// Configura el efecto de comparación de salida
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
								  (FTM_CnSC_ELSB((effect >> 1) & 0X01) | FTM_CnSC_ELSA((effect >> 0) & 0X01));
}

FTMEffect_t FTM_GetOutputCompareEffect(FTM_t ftm, FTMChannel_t channel)
{
	// Obtiene el efecto de comparación de salida configurado
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetPulseWidthModulationLogic(FTM_t ftm, FTMChannel_t channel, FTMLogic_t logic)
{
	// Configura la lógica de PWM en el canal
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
								  (FTM_CnSC_ELSB((logic >> 1) & 0X01) | FTM_CnSC_ELSA((logic >> 0) & 0X01));
}

FTMLogic_t FTM_GetPulseWidthModulationLogic(FTM_t ftm, FTMChannel_t channel)
{
	// Obtiene la lógica PWM configurada en el canal
	return (ftm->CONTROLS[channel].CnSC & (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) >> FTM_CnSC_ELSA_SHIFT;
}

void FTM_SetCounter(FTM_t ftm, FTMChannel_t channel, FTMData_t data)
{
	// Configura el valor del contador para el canal
	ftm->CONTROLS[channel].CnV = FTM_CnV_VAL(data);
}

FTMData_t FTM_GetCounter(FTM_t ftm, FTMChannel_t channel)
{
	// Obtiene el valor del contador para el canal
	return ftm->CONTROLS[channel].CnV & FTM_CnV_VAL_MASK;
}

void FTM_SetInterruptMode(FTM_t ftm, FTMChannel_t channel, bool mode)
{
	// Habilita o deshabilita la interrupción del canal
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(mode);
}

bool FTM_IsInterruptPending(FTM_t ftm, FTMChannel_t channel)
{
	// Verifica si hay una interrupción pendiente en el canal
	return ftm->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK;
}

void FTM_ClearInterruptFlag(FTM_t ftm, FTMChannel_t channel)
{
	// Limpia la bandera de interrupción del canal
	ftm->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}

void FTM_DmaMode(FTM_t ftm, FTMChannel_t channel, bool dma_mode)
{
	// Configura el canal para usar DMA o no
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_DMA_MASK)) |
								  (FTM_CnSC_DMA(dma_mode));
}

void FTM_SetInputCaptureChannelSource(FTM_t ftm, FTM_InputCaptureSource_t source)
{
	// Configura la fuente de captura de entrada para el canal
	switch ((uint32_t)ftm)
	{
	case (uint32_t)FTM1:
		SIM->SOPT4 = (SIM->SOPT4 & ~SIM_SOPT4_FTM1CH0SRC_MASK) | SIM_SOPT4_FTM1CH0SRC(source);
		break;

	case (uint32_t)FTM2:
		SIM->SOPT4 = (SIM->SOPT4 & ~SIM_SOPT4_FTM2CH0SRC_MASK) | SIM_SOPT4_FTM2CH0SRC(source);
		break;
	}
}

int FTM_GetInputCaptureChannelSource(FTM_t ftm, FTM_InputCaptureSource_t source)
{
	// Obtiene la fuente de captura de entrada configurada para el canal
	switch ((uint32_t)ftm)
	{
	case (uint32_t)FTM1:
		return (SIM->SOPT4 & SIM_SOPT4_FTM1CH0SRC_MASK) >> SIM_SOPT4_FTM1CH0SRC_SHIFT;
		break;

	case (uint32_t)FTM2:
		return (SIM->SOPT4 & SIM_SOPT4_FTM2CH0SRC_MASK) >> SIM_SOPT4_FTM2CH0SRC_SHIFT;
		break;
	default:
		return 0; //Esto lo ponemos para q no joda pero si se rompe algo ver esto :) besos
		break;
	}
}

// Configura la función de interrupción para el FTM
void FTM_SetInterruptionCallback(void (*interruption)(void))
{
	FTM_Interruption = interruption;
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void FTM0_IRQHandler(void)
{
	// Rutina de interrupción para FTM0
	if (FTM_Interruption != NULL)
	{
		FTM_Interruption();  // Ejecuta la función de interrupción registrada
	}
	return;
}
