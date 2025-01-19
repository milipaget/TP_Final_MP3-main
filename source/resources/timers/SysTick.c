/***************************************************************************//**
  @file     SysTick.c
  @brief    Application functions
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#include "SysTick.h"
#include "hardware.h"

#define DEPELOPMENT_MODE 1
#define MAX_CALLBACKS 5
	//typedef void (*systick_callback)(void);

systick_callback sys_callback [5] = {NULL,NULL,NULL,NULL,NULL};
static uint8_t cantSystickCallbacks = 0;


#define SYSTICK_LOAD_INIT ((__CORE_CLOCK__/(SYSTICK_ISR_FREQUENCY_HZ)) - 1U)



bool SysTick_Init (void (*funcallback)(void))
{
	static bool yaInit=false;
#if DEVELOPMENT_MODE
	if(!yaInit && funcallback)
#endif
	{
		SysTick->CTRL= 0x00;
		SysTick->LOAD= SYSTICK_LOAD_INIT;
		SysTick->VAL= 0x00;
		SysTick->CTRL= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk ;
		sys_callback[cantSystickCallbacks] = funcallback;
		cantSystickCallbacks++;
		yaInit=true;
	}
	return yaInit;
}

// La funcion systick es llamada automaticamente

__ISR__ SysTick_Handler(void)
{
#if DEVELOPMENT_MODE
	if(systick_callback)
#endif	//Development mode
	{
		for(int i = 0; i<cantSystickCallbacks; i++)
		{
			sys_callback[i]();
		}
	}
}

bool __addSysticCallback__(systick_callback funcallback)
{
	if(cantSystickCallbacks < MAX_CALLBACKS)
	{
		if(funcallback != NULL)
		{
			sys_callback[cantSystickCallbacks] = funcallback;
			cantSystickCallbacks++;
			return true;
		}
		return false;
	}
	return false;
}
// PARA SYSTICK INIT NECESITO:
// AFECTAR CTRL, LOAD para reload value, VAL para limpiar el actual, CTRL para enable
// si esta devinido systick mode, gpioMode(systick_IRQ_TEST_PIN, output); devuelvo 0 si pude
