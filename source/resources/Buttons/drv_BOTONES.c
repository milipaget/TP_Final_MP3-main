/*******************************************************************************
   @file    drv_BOTONES.c
   @brief   Driver que inicializa y maneja el uso de los botones
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
********************************************************************************/

/********************************************************************************
*                           INCLUDE HEADER FILES                                *
*********************************************************************************/
#include "drv_BOTONES.h"

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH LOCAL SCOPE
 ******************************************************************************/
/**
 * @brief Esta función se encarga de inicializar el botón de cambiar el filtro.
 * @return void
 */
void initChangeFilterButton (void);

/**
 * @brief Esta función se encarga de inicializar el botón de pasar a la canción anterior.
 * @return void
 */
void initPrevSongButton (void);

/**
 * @brief Esta función se encarga de inicializar el botón de pasar a la siguiente canción.
 * @return void
 */
void initNextSongButton (void);

/**
 * @brief Esta función se encarga de inicializar el botón de encendido o apagado.
 * @return void
 */
void initOnOffButton (void);

/**
 * @brief Esta función se encarga de inicializar el botón de encendido o apagado.
 * @return void
 */
void initPlayPauseButton (void);

/**
 * @brief Esta función se encarga de inicializar el botón de encendido o apagado.
 * @return void
 */
void changeFilterButtonISR(void);

/**
 * @brief Esta función se encarga de inicializar el botón de encendido o apagado.
 * @return void
 */
void prevSongButtonISR(void);

/**
 * @brief Esta función se encarga de inicializar el botón de encendido o apagado.
 * @return void
 */
void nextSongButtonISR(void);

/**
 * @brief Esta función se encarga de inicializar el botón de encendido o apagado.
 * @return void
 */
void onOffButtonISR(void);

/**
 * @brief Esta función se encarga de inicializar el botón de play o pausa.
 * @return void
 */
void playPauseButtonISR(void);

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH LOCAL SCOPE
 ******************************************************************************/
uint8_t buttonTimer = 0;

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void initButtons(void){
    initChangeFilterButton();
    initNextSongButton();
    initOnOffButton();
    initPrevSongButton();
    initPlayPauseButton();

    //Se agrega un timer periódico para tener una verificación del estado de los botones
    buttonTimer = timerGetId();
    timerStart(buttonTimer, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, buttonMgmtISR); //(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback)

}

void buttonMgmtISR(void){
	changeFilterButtonISR();
	prevSongButtonISR();
	nextSongButtonISR();
	onOffButtonISR();
	playPauseButtonISR();
}

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH LOCAL SCOPE
 ******************************************************************************/
void initChangeFilterButton (void){
    gpioMode(CHANGE_FILTER_BUTTON_PIN, INPUT_PULLUP);
}

void initPrevSongButton (void){
    gpioMode(PREV_BUTTON_PIN, INPUT_PULLUP);
}

void initNextSongButton (void){
    gpioMode(NEXT_BUTTON_PIN, INPUT_PULLUP);
}

void initOnOffButton (void){
    gpioMode(ON_OFF_BUTTON_PIN, INPUT_PULLUP);
}

void initPlayPauseButton (void){
    gpioMode(PORT_ENCODER_BUTTON, INPUT_PULLUP);
}



void changeFilterButtonISR(void){
    static bool lastState = HIGH;
	bool state = gpioRead(CHANGE_FILTER_BUTTON_PIN);

	if(lastState == HIGH && state == LOW){

		lastState = state;

		putEvent(changeFilter);
	}
	else if (lastState == LOW && state == HIGH){

		lastState = state;
	}
}

void prevSongButtonISR(void){
    static bool lastState = HIGH;
	bool state = gpioRead(PREV_BUTTON_PIN);

	if(lastState == HIGH && state == LOW){

		lastState = state;

		putEvent(prevSong);
	}
	else if (lastState == LOW && state == HIGH){

		lastState = state;
	}
}

void nextSongButtonISR(void){
    static bool lastState = HIGH;
	bool state = gpioRead(NEXT_BUTTON_PIN);

	if(lastState == HIGH && state == LOW){

		lastState = state;

		putEvent(nextSong);
	}
	else if (lastState == LOW && state == HIGH){

		lastState = state;
	}
}

void onOffButtonISR(void){
    static bool lastState = HIGH;
	bool state = gpioRead(ON_OFF_BUTTON_PIN);

	if(lastState == HIGH && state == LOW){

		lastState = state;

		putEvent(onOff);
	}
	else if (lastState == LOW && state == HIGH){

		lastState = state;
	}
}

void playPauseButtonISR(void){
    static bool lastState = HIGH;
	bool state = gpioRead(PORT_ENCODER_BUTTON);

	if(lastState == HIGH && state == LOW){

		lastState = state;

		putEvent(playPause);
	}
	else if (lastState == LOW && state == HIGH){

		lastState = state;
	}
}
/*******************************************************************************
 ******************************************************************************/
