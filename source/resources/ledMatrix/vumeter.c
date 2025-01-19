/***************************************************************************/ /**
   @file    vumeter.h
   @brief   vumeter driver
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include "vumeter.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ON (true)
#define OFF (false)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void blinkBar(bar barID, float ms);
static void stopBlinkBar(bar barID);
static void turnOnBar(bar barID);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const float blinkTime = 500000; // 0.5s

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool state = ON;
static bar bandSelected = 0;
static uint8_t bandLevel[MATRIX_COLS] = {0};

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initVumeter(){

	LED_Color_t aux;

	initializeLEDMatrix();

	for(int i = 0; i < MATRIX_COLS; i++){
		bandLevel[i] = MATRIX_ROWS/2;
		turnOnBar(i);
	}

	for(int i = 0; i < MATRIX_COLS; i++){
		for(int j = 0; j < MATRIX_ROWS; j++){
			switch(j){
				case 0:
				case 1:
				case 2:
					aux.hexColor = COLOR_YELLOW;
					setLEDColor(j, i, aux);
					break;

				case 3:
				case 4:
				case 5:
					aux.hexColor = COLOR_RED;
					setLEDColor(j, i, aux);
					break;

				case 6:
				case 7:
					aux.hexColor = COLOR_PURPLE;
					setLEDColor(j, i, aux);
					break;
			}
		}
	}

	adjustBrightness(2);
	vumeterOn();
}

void vumeterOn(){
	state = ON;
}

void vumeterOff(){
	state = OFF;
}

void adjustBrightness(uint8_t bright){
	setLEDBrightness(bright);
}

void setLevel(uint8_t level){

	if(state == ON){
		state = OFF;
	}

	if(level < 0){
		level = 0;
	}
	else if(level > MATRIX_ROWS){
		level = MATRIX_ROWS;
	}

	stopBlinkBar(bandSelected);
	bandLevel[bandSelected] = level;
	blinkBar(bandSelected, blinkTime);
}

void setLevelVumeter(bar barID, uint8_t level){

	if(state == ON){
		state = OFF;
	}

	if(level < 0){
		level = 0;
	}
	else if(level > MAXLEVEL){
		level = MAXLEVEL;
	}

	bandLevel[barID] = level;
	turnOnBar(barID);
}

void selectBar(bar barID){

	if(state == ON){
		state = OFF;
	}

	if(barID < b34Hz){
		barID = b34Hz;
	}
	else if(barID > b14K2Hz){
		barID = b14K2Hz;
	}

	stopBlinkBar(bandSelected);
	turnOnBar(bandSelected);
	bandSelected = barID;
	blinkBar(bandSelected, blinkTime);
}

/*******************************************************************************
 *******************************************************************************
						LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void turnOnBar(bar barID){
	for(int i = 0; i < bandLevel[barID]; i++){
		turnOnLED(i, barID);
	}
}

static void stopBlinkBar(bar barID){
	for(int i = 0; i < bandLevel[barID]; i++){
		disableLEDBlink(i, barID);
	}
}

static void blinkBar(bar barID, float ms){
	for(int i = 0; i < bandLevel[barID]; i++){
		enableLEDBlink(i, barID, ms);
	}
}
