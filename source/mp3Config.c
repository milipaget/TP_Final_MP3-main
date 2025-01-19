/***************************************************************************//**
  @file     mp3Config.c
  @brief    Timer driver. Advance implementation
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "mp3Config.h"

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
mp3config_variables_t* initMp3Config(void){
    mp3ConfigVariables.volume = MAX_VOLUME/2; //uint8_t
    mp3ConfigVariables.songPointer = 1; //uint8_t Arranca en 1 porque tenemos el archivo ese de android maldito
    mp3ConfigVariables.songSelected = false; //Esto capaz es al pedo
    mp3ConfigVariables.isPlaying = true;
    mp3ConfigVariables.isOn = true;
    mp3ConfigVariables.errorType = NO_ERROR; //uint8_t
    mp3ConfigVariables.filterType = NO_FILTER; //uint8_t

    return &mp3ConfigVariables;
}

/******************************************************************************/
