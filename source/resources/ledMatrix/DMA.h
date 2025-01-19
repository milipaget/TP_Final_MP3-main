/***************************************************************************/ /**
   @file     FTM.h
   @brief    Simple FTM services
   @author   Grupo 3
  ******************************************************************************/

#ifndef _DMA_H_
#define _DMA_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"
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

typedef enum
{
  DMA_TransSize_8Bit = 0x0,
  DMA_TransSize_16Bit = 0x01,
  DMA_TransSize_32Bit = 0x02,
  DMA_TransSize_16BitBurst = 0x04,
  DMA_TransSize_32BitBurst = 0x05
} DMATranfSize_t;

typedef enum
{
  DMA_FixedPriority = 0x0,
  DMA_RoundRobin = 0x4,
} DMAArbitr_t;

typedef enum
{
  DMA_CH0,
  DMA_CH1,
  DMA_CH2,
  DMA_CH3,
  DMA_CH4,
  DMA_CH5,
  DMA_CH6,
  DMA_CH7,
  DMA_CH8,
  DMA_CH9,
  DMA_CH10,
  DMA_CH11,
  DMA_CH12,
  DMA_CH13,
  DMA_CH14,
  DMA_CH15,

} DMAChannel_t;

typedef void (*callback_t)(void);

void DMA_Init(void);

void DMA_StartTransfer(DMAChannel_t chn);

void DMAMUX_ConfigChannel(DMAChannel_t chn, bool enable, bool trigger, dma_request_source_t source);

void DMA_SetChannelInterrupt(DMAChannel_t chn, bool mode, callback_t cb);

void DMA_SetEnableRequest(DMAChannel_t, bool);
bool DMA_GetEnableRequest(DMAChannel_t);

void DMA_SetSourceModulo(DMAChannel_t chn, uint16_t mod);
uint16_t DMA_GetSourceModulo(DMAChannel_t chn);

void DMA_SetSourceAddr(DMAChannel_t, uint32_t addr);
uint32_t DMA_GetSourceAddr(DMAChannel_t);

void DMA_SetSourceAddrOffset(DMAChannel_t, int32_t offset);
int32_t DMA_GetSourceAddrOffset(DMAChannel_t);

void DMA_SetSourceTransfSize(DMAChannel_t, DMATranfSize_t txsize);
DMATranfSize_t DMA_GetSourceTransfSize(DMAChannel_t);

void DMA_SetSourceLastAddrOffset(DMAChannel_t, int32_t offset);
int32_t DMA_GetSourceLastAddrOffset(DMAChannel_t);

void DMA_SetDestModulo(DMAChannel_t chn, uint16_t mod);
uint16_t DMA_GetDestModulo(DMAChannel_t chn);

void DMA_SetDestAddr(DMAChannel_t, uint32_t addr);
uint32_t DMA_GetDestAddr(DMAChannel_t);

void DMA_SetDestAddrOffset(DMAChannel_t, int32_t offset);
int32_t DMA_GetDestAddrOffset(DMAChannel_t);

void DMA_SetDestTransfSize(DMAChannel_t, DMATranfSize_t txsize);
DMATranfSize_t DMA_GetDestTransfSize(DMAChannel_t);

void DMA_SetDestLastAddrOffset(DMAChannel_t, int32_t offset);
int32_t DMA_GetDetLastAddrOffset(DMAChannel_t);

void DMA_SetMinorLoopTransCount(DMAChannel_t, uint32_t MinorLoopSize);
uint32_t DMA_GetMinorLoopTransCount(DMAChannel_t);

void DMA_SetCurrMajorLoopCount(DMAChannel_t, uint16_t count);
uint16_t DMA_GetCurrMajorLoopCount(DMAChannel_t);

void DMA_SetStartMajorLoopCount(DMAChannel_t, uint16_t count);
uint16_t DMA_GetStartMajorLoopCount(DMAChannel_t);

/*******************************************************************************
******************************************************************************/

#endif // _DMA_H_
