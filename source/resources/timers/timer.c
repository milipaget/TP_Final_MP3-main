/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "SysTick.h"
#include "timer.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TIMER_DEVELOPMENT_MODE    1
#define TIMER_ID_INTERNAL   0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	ttick_t             period;
	ttick_t             cnt;
	tim_callback_t      callback;
	uint8_t             mode : 1;
	uint8_t             running : 1;
	uint8_t             expired : 1;
	uint8_t             unused : 5;
} timer_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

bool allTimersStop = false;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static timer_t timers[TIMERS_MAX_CANT];
static tim_id_t timers_cant = TIMER_ID_INTERNAL + 1;

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void timerInit(void)
{
	// TODO: descomentar (?
	static bool yaInit = false;
	if (yaInit)
	    return;
	
	yaInit = SysTick_Init(&timer_isr); // init peripheral*/
}

tim_id_t timerGetId(void)
{
#ifdef TIMER_DEVELOPMENT_MODE
	if (timers_cant >= TIMERS_MAX_CANT)
	{
		return TIMER_INVALID_ID;
	}
	else
#endif // TIMER_DEVELOPMENT_MODE
	{
		return timers_cant++;
	}
}

void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback)
{
#ifdef TIMER_DEVELOPMENT_MODE
	if ((id < timers_cant) && (mode < CANT_TIM_MODES))
#endif // TIMER_DEVELOPMENT_MODE
	{
		// disable timer
		timers[id].running = 0;
		timers[id].expired = 0;

		// configure timer
		timers[id].mode = (mode != TIM_MODE_SINGLESHOT);
		timers[id].period = ticks;
		timers[id].cnt = ticks;
		timers[id].callback = callback;

		// enable timer
		timers[id].running = 1;
	}
}
void setPeriodAndCNT(tim_id_t id, ttick_t ticks)
{
	timers[id].period = ticks;
	timers[id].cnt = ticks;
}
void timerStop(tim_id_t id)
{
	timers[id].running = 0;
	//timers[id].expired = 0;
}

void timerResume(tim_id_t id)
{
	timers[id].running = 1;
}

bool timerExpired(tim_id_t id)
{
//	bool aux = timers[id].expired;
//	if (aux)
//		timers[id].expired = false;
	return timers[id].expired;
}

bool getTimerRunning(tim_id_t id)
{
	return timers[id].running;
}

void timerDelay(ttick_t ticks)
{
	timerStart(TIMER_ID_INTERNAL, ticks, TIM_MODE_SINGLESHOT, NULL);
	while (!timerExpired(TIMER_ID_INTERNAL))
	{

	}
}

void alltimersStop(bool active)
{
	for (tim_id_t id = 0; id < timers_cant; id++)
	{
		timers[id].running = active;
	}
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void timer_isr(void)
{
	for (tim_id_t id = 0; id < TIMERS_MAX_CANT; id++)	//Reviso los timers
	{
		if (timers[id].running) 		// Solo los timers activos
		{
			if (--timers[id].cnt == 0)	//si termina el contador
			{
				// set flag
				timers[id].expired = true;

				// Si tengo un callback asignado, lo ejecuto
				if (timers[id].callback != NULL)
				{
					timers[id].callback();
				}

				// si es periodico reseteo
				if (timers[id].mode == TIM_MODE_PERIODIC)
				{
					timers[id].cnt = timers[id].period;
				}
				else
				{
					timerStop(id);
				}
			}
		}
	}
}

/*******************************************************************************
 ******************************************************************************/
