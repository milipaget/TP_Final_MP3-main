/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "hardware.h"
#include "resources/fsm/fsm.h"
#include "resources/fsm/fsmtable.h" /*FSM Table*/
#include "resources/eventos/eventQueue.h"
#include "resources/timers/timer.h"
#include "resources/Buttons/drv_BOTONES.h"
#include "resources/Encoder/Encoder.h"
#include "resources/timers/timerPIT.h"
#include "resources/SDHC/ff.h"
#include "musicHandler/mp3Decoder.h"
#include "musicHandler/dma_music.h"
#include "Drivers/RTC/RTC.h"
#include "gpio.h"
#include "mp3Config.h"
#include "pinout.h"
#include "Matrix/dma_matrix.h"
#include "DSP/FFT/fft.h"
#include "resources/PowerManagement/sleep.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
STATE *p2state = NULL; // puntero al estado
FATFS FatFs;

uint8_t updateMP3Timer;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init(void)
{
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_GREEN, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);
	gpioWrite(PIN_LED_RED, !LED_ACTIVE);
	gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
	gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
	timerInit();
	initButtons();
	initEncoder();
	fft_Init();
	if (f_mount(&FatFs, "1:/", 0) == FR_OK) {
		int a;
	}
	MP3DecInit();
	DMAmusicInit();
	p2state = FSM_GetInitState(); // Inicializo la FSM con el estado inicial
	DMAmatrixInit();
	updateMP3Timer = timerGetId();
	setUpSleep();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void)
{
	static event_t nextEvent;

	nextEvent = getNextEvent();
	if (nextEvent != None)
	{
		p2state = fsm(p2state, nextEvent);
	}
	updateMP3();
}


