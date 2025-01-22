/***************************************************************************//**
  @file     mp3Decoder.h
  @brief    Decoder for music files
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#ifndef _MP3READER_H_
#define _MP3READER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Cantidad de MUESTRAS en un frame
// Obs: Cada muestra ocupa 2 bytes
#define OUTBUFF_SIZE    1152U
#define ID3_SIZE 128 //128 bytes tiene id3 data de un frame
#define GENRES 148
#define MAX_PATH 50

typedef struct
{   //1 byte extra para agregar terminador '\0'
	bool isThereID3Data;
	unsigned char title[31];
	unsigned char artist[31];
	unsigned char album[31];
	unsigned char year[5];
	unsigned char comment[31];
	char genre[31];
}mp3_id3Metadata;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

bool MP3DecInit();

/**
 le pasas un path y te selecciona y abre dicha cancion
*/
bool MP3SelectSong(char* filePath, mp3_id3Metadata* outMetadata);


/*
 decodifica el siguiente frame de data y lo guarda en el outBuff
*/
uint16_t MP3DecNextFrame(int16_t* outBuff);

int SDSearch(char* bufferData, bool* typeData, int index);

//void MP3ReadID3Data(mp3_id3Metadata* outMetadata, char* filePath);
/*******************************************************************************
 ******************************************************************************/

#endif // _MP3DEC_H_
