/********************************************************************************
   @file     pit.h
   @brief    pit driver
   @author   Grupo 3
*********************************************************************************/

#ifndef _PIT_H_
#define _PIT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAXTIMERS (4)
#define MAXCOUNT ((float)(4294967296 / __CORE_CLOCK__ / 2))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum states
{
  FREE,
  IDLE,
  RUNNING
};

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize pit driver
 * @param void
 * @return void
 */
void initPIT();

/**
 * @brief Initialize a new timer
 * @param time in us when callback will be called
 * @param funcallback callback function when timer's count ends
 * @return id of timer where new timer was allocated. Return MAXTIMERS if allocation failed
 */
uint8_t createTimerPIT(float time, void (*funcallback)(void));

/**
 * @brief Config time of timer of ID id
 * @param id timer's id which will be configured
 * @param time to be modified
 * @return void
 */
void configTimerTimePIT(uint8_t id, float time);

/**
 * @brief Free timer id to allocate a new timer instead
 * @param id timer's id which will be detroyed
 * @return void
 */
void destroyTimerPIT(uint8_t id);

/**
 * @brief reset timer
 * @param id timer's id which will be reseted
 * @return void
 */
void resetTimerPIT(uint8_t id);

/**
 * @brief start timer
 * @param id timer's id which will be started
 * @return void
 */
void startTimerPIT (uint8_t id);

/**
 * @brief stop timer
 * @param id timer's id which will be stopped
 * @return void
 */
void stopTimerPIT(uint8_t id);

/**
 * @brief get current timer count
 * @param id timer's id which count will be returned
 * @return timer count in ms
 */
uint32_t getTimePIT(uint8_t id);

/**
 * @brief get current timer state
 * @param id timer's id which state will be returned
 * @return state of the timer
 */
uint8_t getTimerStatePIT(uint8_t id);

/*******************************************************************************
 ******************************************************************************/

#endif // _PIT_H_

