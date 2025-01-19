/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TIMER_TICK_MS       1
#define TIMER_MS2TICKS(ms)  ((ms)/TIMER_TICK_MS)

#define TIMERS_MAX_CANT     16
#define TIMER_INVALID_ID    255

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Timer Modes
enum { TIM_MODE_SINGLESHOT, TIM_MODE_PERIODIC, CANT_TIM_MODES };

// Timer Alias
typedef uint32_t ttick_t;
typedef uint8_t tim_id_t;
typedef void (*tim_callback_t)(void);
void alltimersStop(bool active);

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Inicializa el timer y los perifericos correspondientes
 */
void timerInit(void);

/**************************** Non-blocking Services ****************************/

/**
 * @brief Pide la identificaci�n de un timer
 * @return la ID del timer a utilizar
 */
tim_id_t timerGetId(void);

/**
 * @brief Arranca un nuevo contador
 * @param id ID del temporizador a arrancar
 * @param ticks el equivalente del tiempo que dura el timer en ticks
 * @param mode SINGLESHOT or PERIODIC
 * @param callback funcion a llamar cuando se acabe el tiempo
 */
void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback);

/**
 * @brief Se encarga de pausar el timer correspondiente
 * @param id ID del timer a finalizar
 */
void timerStop(tim_id_t id);

/**
 * @brief Verifica si se acabo el tiempo de un timer
 * @param id ID del timer que quiero checkear
 * @return 1 = timer expired
 */
bool timerExpired(tim_id_t id);

/**
 * @brief Me informa si un timer esta activo
 * @param id ID del timer a checkear
 * @return 1 = timer running
 */
bool getTimerRunning(tim_id_t id);

/**************************** Blocking Services ****************************/

/**
 * @brief Espera el tiempo especificado, con un timer interno.
 * @param ticks el equivalente del tiempo en ticks
 */
void timerDelay(ttick_t ticks);

/**
 * @brief Se encarga de arrancar nuevamente un timer
 * @param id ID del timer que debo hacer un re-running
 */
void timerResume(tim_id_t id);

/**
 * @brief Se encarga de las ISR (?
 */
void timer_isr(void);

void setPeriodAndCNT(tim_id_t id, ttick_t ticks);
/*******************************************************************************
 ******************************************************************************/

#endif // _TIMER_H_
