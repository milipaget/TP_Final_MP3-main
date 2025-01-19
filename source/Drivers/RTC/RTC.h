/***************************************************************************//**
  @file     dma.h
  @brief    Driver for Dynamic Memory Access
  @author   TEAM OREO
 ******************************************************************************/
#ifndef RTC_H_
#define RTC_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void RTCInit(void);
void RTCSetTime(uint8_t hours,uint8_t mins,uint8_t seconds);
void RTCGetRealTime(uint8_t* hours,uint8_t* mins,uint8_t* seconds);
void RTCgetRealTimeSTR(char* arr);
#endif /* RTC_H_ */
