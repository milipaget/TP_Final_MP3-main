/***************************************************************************/ /**
   @file     DMA.c
   @brief    Implementación de funciones para configurar y controlar el módulo DMA.
   @author   Nombre del autor
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "DMA.h"
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
static callback_t callback[16] = {0};
/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// Inicializa el módulo DMA.
void DMA_Init()
{
	/* Enable the clock for the eDMA and the DMAMUX. */
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA1_IRQn);
	/* Enable the DMA interrupts. */
	NVIC_EnableIRQ(DMA1_IRQn);
}

// Inicia la transferencia en el canal especificado.
void DMA_StartTransfer(DMAChannel_t chn)
{
	DMA0->SSRT = chn; // Establece el canal
}

// Configura un canal del DMAMUX (habilita/deshabilita, define fuente y trigger).
void DMAMUX_ConfigChannel(DMAChannel_t chn, bool enable, bool trigger, dma_request_source_t source){
	DMA0->TCD[chn].CSR = 0; // Limpia el registro y lo configura
	DMAMUX0->CHCFG[chn] = DMAMUX_CHCFG_ENBL(enable) + DMAMUX_CHCFG_SOURCE(source) + DMAMUX_CHCFG_TRIG(trigger);
}

// Configura la interrupción de un canal del DMA.
void DMA_SetChannelInterrupt(DMAChannel_t chn, bool mode,callback_t cb)
{
	if (cb != 0)
	{
		callback[chn] = cb;
	}
	DMA0->TCD[chn].CSR = (DMA0->TCD[chn].CSR & ~DMA_CSR_INTMAJOR_MASK) + DMA_CSR_INTMAJOR(mode);
}

// Habilita o deshabilita la solicitud de transferencia en un canal del DMA.
void DMA_SetEnableRequest(DMAChannel_t chn, bool state)
{
	DMA0->ERQ = (DMA0->ERQ & ~(1 << chn)) + (state << chn);
}

// Devuelve el estado de la solicitud de transferencia de un canal.
bool DMA_GetEnableRequest(DMAChannel_t chn)
{
	return (DMA0->ERQ >> chn) & 1;
}

// Configura el valor del Source Modulo).
void DMA_SetSourceModulo(DMAChannel_t chn, uint16_t mod)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_SMOD_MASK) + DMA_ATTR_SMOD(mod);
}

// Obtiene el valor del módulo
uint16_t DMA_GetSourceModulo(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_SMOD_MASK) >> DMA_ATTR_SMOD_SHIFT;
}

// Configura la dirección de destino de la transferencia.
void DMA_SetDestModulo(DMAChannel_t chn, uint16_t mod)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_DMOD_MASK) + DMA_ATTR_DMOD(mod);
}

// Obtiene el valor del módulo de destino.
uint16_t DMA_GetDestModulo(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_DMOD_MASK) >> DMA_ATTR_DMOD_SHIFT;
}

// Configura la dirección de origen de la transferencia.
void DMA_SetSourceAddr(DMAChannel_t chn, uint32_t addr)
{
	DMA0->TCD[chn].SADDR = addr;
}

// Obtiene la dirección de origen de la transferencia.
uint32_t DMA_GetSourceAddr(DMAChannel_t chn)
{
	return DMA0->TCD[chn].SADDR;
}

// Configura el offset de la dirección de origen.
void DMA_SetSourceAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].SOFF = offset;
}

// Obtiene el offset de la dirección de origen.
int32_t DMA_GetSourceAddrOffset(DMAChannel_t chn)
{
	return DMA0->TCD[chn].SOFF;
}

// Configura el tamaño de transferencia para Source Transfer.
void DMA_SetSourceTransfSize(DMAChannel_t chn, DMATranfSize_t txsize)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_SSIZE_MASK) + DMA_ATTR_SSIZE(txsize);
}

// Obtiene el tamaño de transferencia del source.
DMATranfSize_t DMA_GetSourceTransfSize(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_SSIZE_MASK) >> DMA_ATTR_SSIZE_SHIFT;
}

void DMA_SetSourceLastAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].SLAST = offset;
}
int32_t DMA_GetSourceLastAddrOffset(DMAChannel_t chn)
{
	return DMA0->TCD[chn].SLAST;
}

void DMA_SetDestAddr(DMAChannel_t chn, uint32_t addr)
{
	DMA0->TCD[chn].DADDR = addr;
}
uint32_t DMA_GetDestAddr(DMAChannel_t chn)
{
	return DMA0->TCD[chn].DADDR;
}

void DMA_SetDestAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].DOFF = offset;
}
int32_t DMA_GetDestAddrOffset(DMAChannel_t chn)
{
	return DMA0->TCD[chn].DOFF;
}

void DMA_SetDestTransfSize(DMAChannel_t chn, DMATranfSize_t txsize)
{
	DMA0->TCD[chn].ATTR = (DMA0->TCD[chn].ATTR & ~DMA_ATTR_DSIZE_MASK) + DMA_ATTR_DSIZE(txsize);
}
DMATranfSize_t DMA_GetDestTransfSize(DMAChannel_t chn)
{
	return (DMA0->TCD[chn].ATTR & DMA_ATTR_DSIZE_MASK) >> DMA_ATTR_DSIZE_SHIFT;
}

void DMA_SetDestLastAddrOffset(DMAChannel_t chn, int32_t offset)
{
	DMA0->TCD[chn].DLAST_SGA = offset;
}
int32_t DMA_GetDetLastAddrOffset(DMAChannel_t chn)
{
	return DMA0->TCD[chn].DLAST_SGA;
}

void DMA_SetMinorLoopTransCount(DMAChannel_t chn, uint32_t MinorLoopSize)
{
	DMA0->TCD[chn].NBYTES_MLOFFNO = MinorLoopSize & DMA_NBYTES_MLOFFNO_NBYTES_MASK;
}
uint32_t DMA_GetMinorLoopTransCount(DMAChannel_t chn)
{
	return DMA0->TCD[chn].NBYTES_MLOFFNO;
}

void DMA_SetCurrMajorLoopCount(DMAChannel_t chn, uint16_t count)
{
	DMA0->TCD[chn].CITER_ELINKNO = count & DMA_CITER_ELINKNO_CITER_MASK;
}
uint16_t DMA_GetCurrMajorLoopCount(DMAChannel_t chn)
{
	return DMA0->TCD[chn].CITER_ELINKNO & DMA_CITER_ELINKNO_CITER_MASK;
}

void DMA_SetStartMajorLoopCount(DMAChannel_t chn, uint16_t count)
{
	DMA0->TCD[chn].BITER_ELINKNO = count & DMA_BITER_ELINKNO_BITER_MASK;
}
uint16_t DMA_GetStartMajorLoopCount(DMAChannel_t chn)
{
	return DMA0->TCD[chn].BITER_ELINKNO & DMA_BITER_ELINKNO_BITER_MASK;
}
/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// Manejador de la interrupción del DMA.
void DMA1_IRQHandler()
{
	/* Clear the interrupt flag. */
	uint16_t status = DMA0->INT;
	DMA0->INT = 0xFFFF;

	// Recorre todos los canales y ejecuta el callback correspondiente si está configurado.
	for (int i = 0; i < 16; i++)
	{
		if ((status >> i) & 1)	// Si está activo
		{
			if (callback[i] != 0)	// Si tiene un callback
			{
				callback[i](); // Ejecuta el callback()
			}
		}
	}
}



