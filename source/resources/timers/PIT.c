/*
	El archivo que implementa un controlador para los temporizadores del periférico PIT (Periodic Interrupt Timer) 
	del microcontrolador MK64F12, utilizado para ejecutar tareas periódicas en sistemas embebidos.
*/

#include "PIT.h"
#include "MK64F12.h" 		// Biblioteca del microcontrolador MK64F12
#include "hardware.h"		// Definiciones de hardware
#include "../../gpio.h"
#include "string.h"

#define REFERENCE ((float)(0.02))	// Referencia de tiempo en microsegundos, usada para el cálculo del valor de cuenta

// Estructura de timer pit
typedef struct
{
	uint8_t state;			// Estado del temporizador (FREE, IDLE, RUNNING)
	void (*callback)(void);	// Función que será llamada cuando el temporizador expire
} TIMER_t;

static TIMER_t timer[MAXTIMERS];	// Array de temporizadores (hasta 4 timers)

// Habilita los times del PIT
void initPIT()
{
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;	// Habilita el reloj para el PIT
	PIT->MCR = 0;						// Deshabilita el PIT en modo de parada (MCR = 0)
	// Habilita las interrupciones para los 4 canales del PIT (de 0 a 3)
	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT1_IRQn);
	NVIC_EnableIRQ(PIT2_IRQn);
	NVIC_EnableIRQ(PIT3_IRQn);
}

// Crea un nuevo temporizador con un tiempo específico en microsegundos y asigna una callback
uint8_t createTimerPIT(float time, void (*funcallback)(void))
{

	uint8_t i = 0;
	bool found = false;

	// Busca un temporizador libre (en estado FREE)
	while (i < MAXTIMERS && !found)
	{
		if (timer[i].state == FREE)
		{
			// Configura el estado y asigna un tiempo y una función
			timer[i].state = IDLE;
			timer[i].callback = funcallback;
			PIT->CHANNEL[i].LDVAL = (int)(time / REFERENCE);
			PIT->CHANNEL[i].TCTRL |= PIT_TCTRL_TIE_MASK;
			found = true;
		}
		else
		{
			// Si el temporizador actual está ocupado, prueba con el siguiente
			i++;
		}
	}

	return i; // Devuelve el índice del temporizador asignado o MAXTIMERS si no se encontró un espacio libre
}

// Configura un temporizador existente con un nuevo valor de tiempo
void configTimerTimePIT(uint8_t id, float time)
{
	PIT->CHANNEL[id].TCTRL &= ~PIT_TCTRL_TEN_MASK; 	// Detiene el timer
	PIT->CHANNEL[id].LDVAL = (int)(time / REFERENCE);	// Modifica el valor con un nuevo tiempo
	PIT->CHANNEL[id].TCTRL |= PIT_TCTRL_TEN_MASK;	// Vuelve a habilitar el timer
}

// Destruye un temporizador, liberando el canal para ser reutilizado
void destroyTimerPIT(uint8_t id)
{
	PIT->CHANNEL[id].TCTRL &= ~PIT_TCTRL_TEN_MASK; // Detiene el temporizador
	timer[id].state = FREE;	// Lo marca como libre
}

// Inicia un temporizador (empieza a contar)
void startTimerPIT(uint8_t id)
{
	PIT->CHANNEL[id].TCTRL |= PIT_TCTRL_TEN_MASK;
	if (timer[id].state != FREE)
	{
		timer[id].state = RUNNING;	// Cambia el estado del temporizador a running
	}
}

// Detiene un temporizador (suspende el conteo)
void stopTimerPIT(uint8_t id)
{
	PIT->CHANNEL[id].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	if (timer[id].state != FREE)
	{
		timer[id].state = IDLE;	// Cambia el estado del temporizador a IDLER
	}
}

// Obtiene el valor actual del contador de un temporizador
uint32_t getTimePIT(uint8_t id)
{
	return PIT->CHANNEL[id].CVAL;
}

// Obtiene el estado actual de un temporizador (FREE, IDLE, RUNNING)
uint8_t getTimerStatePIT(uint8_t id)
{
	return timer[id].state;
}

// Rutina de interrupción para el canal PIT 0
void PIT0_IRQHandler(void)
{
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	if (timer[0].callback != NULL)
	{
		timer[0].callback();
	}
	NVIC_ClearPendingIRQ(PIT0_IRQn);
}
// Rutina de interrupción para el canal PIT 1
void PIT1_IRQHandler(void)
{
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1);
	if (timer[1].callback != NULL)
	{
		timer[1].callback();
	}
	NVIC_ClearPendingIRQ(PIT1_IRQn);
}
// Rutina de interrupción para el canal PIT 2
void PIT2_IRQHandler(void)
{

	PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF(1);
	if (timer[2].callback != NULL)
	{
		timer[2].callback();
	}
	NVIC_ClearPendingIRQ(PIT2_IRQn);
}
// Rutina de interrupción para el canal PIT 3
void PIT3_IRQHandler(void)
{

	PIT->CHANNEL[3].TFLG = PIT_TFLG_TIF(1);
	if (timer[3].callback != NULL)
	{
		timer[3].callback();
	}
	NVIC_ClearPendingIRQ(PIT3_IRQn);
}

