/*******************************************************************************
   @file    mp3Config.h
   @brief   Contiene las variables y constantes necesarias para el funcionamiento
            del mp3
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#ifndef _MP3CONFIG_H_
#define _MP3CONFIG_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define EQTYPE 4
#define MAX_VOLUME 30

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
//const char *eqTypes[EQTYPE] = {"None", "Rock", "Pop", "Bass"};
enum filterTypes{NO_FILTER, ROCK, POP, BASS};
enum errorTypes{NO_ERROR, NO_SD, NO_SONGS};

typedef struct 
{
	uint8_t volume;
    uint8_t songPointer;
    bool songSelected;
    bool isPlaying;
    bool isOn;
    uint8_t errorType;
    uint8_t filterType;
}mp3config_variables_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
static int songNum; //Esta variable se actualiza cuando se inserta la SD card (uint8_t???)
static mp3config_variables_t mp3ConfigVariables;
//mp3config_variables* pointer2config = mp3ConfigVariables

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initial configuration 
 */
mp3config_variables_t* initMp3Config(void);
/*******************************************************************************
 ******************************************************************************/

#endif // _MP3CONFIG_H_
