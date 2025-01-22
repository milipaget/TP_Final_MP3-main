/***************************************************************************//**
  @file     timerPIT.h
  @brief    Driver for Periodic Interrupt Timer module
  @author   TEAM OREO
 ******************************************************************************/
#ifndef TIMERPIT_H_
#define TIMERPIT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "hardware.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
//typedef uint8_t pit_id;
typedef enum
{
	PIT_ID_0 = 0x00,
	PIT_ID_1 = 0x01,
	PIT_ID_2 = 0x02,
	PIT_ID_3 = 0x03
}pit_id;

typedef struct {
	pit_id id;
	uint32_t LDVAL_;
	uint32_t CVAL_;                              /**< Current Timer Value Register, array offset: 0x104, array step: 0x10 */
	uint32_t TCTRL_;                             /**< Timer Control Register, array offset: 0x108, array step: 0x10 */
	uint32_t TFLG_;
}pit_config;


typedef void (*pittimer_callback)(void);

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void timerPITinit(pit_config config, pittimer_callback callback );
void setTimerPITLDVAL(uint8_t id,uint32_t LDVAL);
void clearInterruptFlag(void);
void timerPITStart(pit_id id);
void timerPITStop(pit_id id);

#endif /* TIMERPIT_H_ */
