/***************************************************************************//**
  @file     dma.h
  @brief    DMA for music purposes
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/
#ifndef DMA_H_
#define DMA_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef void (*dma_callback_t)(void);
typedef struct DMA_config_t
{
	void * source;
	void * dest;
	uint8_t req_channel; // ftm0 a ftm7 del 21 al 27
	dma_callback_t dma_callback;
	uint16_t source_len; //cuantos elementos
	uint8_t source_data_len; //cuantos bytes la data
}DMA_Config_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void DMAmusicInit();
void DMA_ResumeMusic();
void DMA_PauseMusic();
bool DMAmusic(uint16_t* source,uint32_t cantRequest,uint32_t sampleRate);
uint8_t DMA_StatusBuffers(void);
#endif /* DMA_H_ */
