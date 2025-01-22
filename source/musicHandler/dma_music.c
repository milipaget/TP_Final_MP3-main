/***************************************************************************//**
  @file     dma_music
  @brief    DMA for music purposes
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "hardware.h"
#include <stddef.h>
#include "dma_music.h"
#include "dac.h"
#include "../resources/timers/timerPIT.h"
 /******************************************************************************
  *
  *
  *
  *
  *
  * 	ACA HAY QUE PONER DOS DMA, UNO PARA LA MATRIZ Y OTRO PARA LA CANCION
  * 	LO QUE HAY QUE TENER EN CUENTA ES QUE HAY QUE HACER LAS IRQ ESPECIALIZADAS
  *
  *
  *
  *
  *
  *
  */
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define TIMER_PIT 1134 // 20ns* TIMER_PIT \approx 44.1kHz
#define FREQ_PIT 50e6
#define BUFFER_SIZE 1024//5*4033
#define SOURCE_OFFSET 2
#define DESTINATION_OFFSET 0
#define TRANSFER_ATTRIBUTES DMA_ATTR_SSIZE(0x1) | DMA_ATTR_DSIZE(0x1)
#define MINORLOOP 0x02
#define CITER BUFFER_SIZE
#define BITER BUFFER_SIZE
#define ENABLE_SCANDGA DMA_CSR_ESG_MASK | DMA_CSR_INTMAJOR_MASK
/* Structure with the TCD fields. */
typedef struct
{
	uint32_t SADDR;
	uint16_t SOFF;
	uint16_t ATTR;
	union
	{
		uint32_t NBYTES_MLNO;
		uint32_t NBYTES_MLOFFNO;
		uint32_t NBYTES_MLOFFYES;
	};
	uint32_t SLAST;
	uint32_t DADDR;
	uint16_t DOFF;
	union
	{
		uint16_t CITER_ELINKNO;
		uint16_t CITER_ELINKYES;
	};
	uint32_t DLASTSGA;
	uint16_t CSR;
	union
	{
		uint16_t BITER_ELINKNO;
		uint16_t BITER_ELINKYES;
	};
}TCD_t;

enum{
	FREE_BUFF = false,
	FULL_BUFF = true
}statusBuffer;
enum{
	bufferA,bufferB
};
/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
static void dma_irq_handler(uint8_t channel);
static TCD_t TCDA __attribute__ ((aligned (32))),TCDB __attribute__ ((aligned (32)));
static bool statusBufferA = FREE_BUFF, statusBufferB = FREE_BUFF;
static uint8_t actualBuffer = bufferA; // 0 bufferA, 1 bufferB
static pit_config pitConfig;
static uint8_t pitTimerID;
static uint32_t actualSampleRate;
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void DMAmusicInit()
{
		dacInit();
		pitConfig.id = PIT_ID_0;
		pitConfig.LDVAL_ = TIMER_PIT;
		actualSampleRate = FREQ_PIT/TIMER_PIT;
		timerPITinit(pitConfig, NULL);
		//Aca hay que configurar para que accione el request con el PIT

		SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
		SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

		/*****************************************/
		DAC0->DAT[0].DATH = 0x08;
		DAC0->DAT[0].DATL = 0x00;
		/*****************************************/
		DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK |DMAMUX_CHCFG_TRIG_MASK| DMAMUX_CHCFG_SOURCE(63); // El 63 viene de filmina de dani
		/*****************************************/
		NVIC_ClearPendingIRQ(DMA0_IRQn);
		NVIC_EnableIRQ(DMA0_IRQn);
}
bool DMAmusic(uint16_t* source,uint32_t cantRequest,uint32_t sampleRate)
{
	if(actualSampleRate != sampleRate)
	{
		setTimerPITLDVAL(PIT_ID_0,FREQ_PIT/sampleRate);
		actualSampleRate=sampleRate;
	}
	if(!statusBufferA)
	{
		/*************************************************/
		TCDA.SADDR = (uint32_t)(source);
		TCDA.DADDR = (uint32_t)&DAC0->DAT[0].DATL;
		TCDA.SOFF = SOURCE_OFFSET;
		TCDA.DOFF = DESTINATION_OFFSET;
		TCDA.ATTR = TRANSFER_ATTRIBUTES;
		TCDA.NBYTES_MLNO = MINORLOOP;
		TCDA.CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(cantRequest);
		TCDA.BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(cantRequest);
		TCDA.DLASTSGA = (uint32_t)&(TCDB);
		/*************************************************/
		if(!statusBufferB)
		{
			actualBuffer = bufferA;
			DMA0->TCD[0].SADDR = TCDA.SADDR;
			DMA0->TCD[0].DADDR = TCDA.DADDR;
			DMA0->TCD[0].SOFF = TCDA.SOFF;//0x02;
			DMA0->TCD[0].DOFF = TCDA.DOFF;
			DMA0->TCD[0].ATTR = TCDA.ATTR;
			DMA0->TCD[0].NBYTES_MLNO =TCDA.NBYTES_MLNO; //0x02;
			DMA0->TCD[0].CITER_ELINKNO = TCDA.CITER_ELINKNO;
			DMA0->TCD[0].BITER_ELINKNO = TCDA.BITER_ELINKNO;
			DMA0->TCD[0].SLAST = 0;//-config.source_len;
			DMA0->TCD[0].DLAST_SGA = 0;
			DMA0->TCD[0].CSR |=DMA_CSR_INTMAJOR_MASK;
			DMA0->ERQ |= DMA_ERQ_ERQ0_MASK;
		}
		statusBufferA = FULL_BUFF;

	}
	else if(!statusBufferB)
	{
		/*************************************************/
		TCDB.SADDR = (uint32_t)(source);
		TCDB.DADDR = (uint32_t)&DAC0->DAT[0].DATL;
		TCDB.SOFF = SOURCE_OFFSET;
		TCDB.DOFF = DESTINATION_OFFSET;
		TCDB.ATTR = TRANSFER_ATTRIBUTES;
		TCDB.NBYTES_MLNO = MINORLOOP;
		TCDB.CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(cantRequest);
		TCDB.BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(cantRequest);
		TCDB.DLASTSGA = (uint32_t)&(TCDA);
		/*************************************************/
		if(!statusBufferA)
		{
			actualBuffer = bufferB;
			DMA0->TCD[0].SADDR = TCDB.SADDR;
			DMA0->TCD[0].DADDR = TCDB.DADDR;
			DMA0->TCD[0].SOFF = TCDB.SOFF;//0x02;
			DMA0->TCD[0].DOFF = TCDB.DOFF;
			DMA0->TCD[0].ATTR = TCDB.ATTR;
			DMA0->TCD[0].NBYTES_MLNO =TCDB.NBYTES_MLNO; //0x02;
			DMA0->TCD[0].CITER_ELINKNO = TCDB.CITER_ELINKNO;
			DMA0->TCD[0].BITER_ELINKNO = TCDB.BITER_ELINKNO;
			DMA0->TCD[0].SLAST = 0;//-config.source_len;
			DMA0->TCD[0].DLAST_SGA = 0;
			DMA0->TCD[0].CSR |= DMA_CSR_INTMAJOR_MASK;
			DMA0->ERQ |= DMA_ERQ_ERQ0_MASK;
		}
		statusBufferB = FULL_BUFF;
	}
	else
	{
		return false;
	}

	timerPITStart(pitTimerID);
	return true;
}

void DMA_ResumeMusic()
{
	timerPITStart(pitTimerID);
}
void DMA_PauseMusic()
{
	timerPITStart(pitTimerID);
	DAC0->DAT[0].DATH = 0x08;
	DAC0->DAT[0].DATL = 0x00;
}
uint8_t DMA_StatusBuffers(void)
{
	return statusBufferA + statusBufferB;
}
void DMA0_IRQHandler(){
	DMA0->INT = 0x01;

	if(actualBuffer == bufferA)
	{
		statusBufferA = FREE_BUFF;
		if(statusBufferB)
		{
			actualBuffer = bufferB;
			DMA0->TCD[0].SADDR = TCDB.SADDR;
			DMA0->TCD[0].DADDR = TCDB.DADDR;
			DMA0->TCD[0].SOFF = TCDB.SOFF;//0x02;
			DMA0->TCD[0].DOFF = TCDB.DOFF;
			DMA0->TCD[0].ATTR = TCDB.ATTR;
			DMA0->TCD[0].NBYTES_MLNO =TCDB.NBYTES_MLNO; //0x02;
			DMA0->TCD[0].CITER_ELINKNO = TCDB.CITER_ELINKNO;
			DMA0->TCD[0].BITER_ELINKNO = TCDB.BITER_ELINKNO;
			DMA0->TCD[0].SLAST = 0;//-config.source_len;
			DMA0->TCD[0].DLAST_SGA = 0;
			DMA0->TCD[0].CSR |= DMA_CSR_INTMAJOR_MASK;
			DMA0->ERQ |= DMA_ERQ_ERQ0_MASK;
		}
		else
		{
			DMA_PauseMusic();
		}
	}
	else if(actualBuffer == bufferB)
	{
		statusBufferB = FREE_BUFF;
		if(statusBufferA)
		{
			actualBuffer = bufferA;
			DMA0->TCD[0].SADDR = TCDA.SADDR;
			DMA0->TCD[0].DADDR = TCDA.DADDR;
			DMA0->TCD[0].SOFF = TCDA.SOFF;//0x02;
			DMA0->TCD[0].DOFF = TCDA.DOFF;
			DMA0->TCD[0].ATTR = TCDA.ATTR;
			DMA0->TCD[0].NBYTES_MLNO =TCDA.NBYTES_MLNO; //0x02;
			DMA0->TCD[0].CITER_ELINKNO = TCDA.CITER_ELINKNO;
			DMA0->TCD[0].BITER_ELINKNO = TCDA.BITER_ELINKNO;
			DMA0->TCD[0].SLAST = 0;//-config.source_len;
			DMA0->TCD[0].DLAST_SGA = 0;
			DMA0->TCD[0].CSR |=DMA_CSR_INTMAJOR_MASK;
			DMA0->ERQ |= DMA_ERQ_ERQ0_MASK;
		}
		else
		{
			DMA_PauseMusic();
		}
	}

	//	if(dma_callback[channel] != NULL){
//		dma_callback[channel]();
//	}
	//FTM2->SC &= ~FTM_SC_TOF_MASK;

}

void DMA_Error_IRQHandler(void){
	DMA0->CERR = DMA_CERR_CAEI(1);
}

