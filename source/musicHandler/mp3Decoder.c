/***************************************************************************//**
  @file     mp3Decoder.c
  @brief    Decoder for music files
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <musicHandler/mp3Decoder.h>
#include "resources/SDHC/ff.h"
#include "resources/helix/pub/mp3dec.h"

#include <stddef.h>
#include <string.h>

#ifdef MP3_DEBUG
#include <stdio.h>
#endif


#include "gpio.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define readFileBuffer_SIZE		MAINBUF_SIZE

// Accepted sample rate
#define MP3_SAMPRATE	44100

#define TESTPIN		PORTNUM2PIN(PB, 3)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
#define TAG_SIZE 128

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static HMP3Decoder mp3Dec;

static FIL mp3File;

static MP3FrameInfo mp3Info;

static bool fileOpen = false;
static int pepe=0;
static char path[MAX_PATH];
static int pathOffset = 0;

bool errorfat = 0;
UINT a = 0;
FATFS fs;
FRESULT result;
UINT *dummy;
DIR dir;
FILINFO fno;

void MP3ReadID3Data(mp3_id3Metadata* outMetadata);
void clearPath(void);
void initPath(void);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool MP3DecInit() {
	mp3Dec = MP3InitDecoder();
	return mp3Dec == NULL;
}


bool MP3SelectSong(char* songName, mp3_id3Metadata* outMetadata) {

	if (fileOpen) {
		f_close(&mp3File);
		fileOpen = false;
		initPath();
	}

	strcat(path,songName);

	if (f_open(&mp3File, path, FA_READ) == FR_OK) {

		MP3ReadID3Data(outMetadata);
		fileOpen = true;
		UINT bytesRead;
		BYTE readFileBuffer[readFileBuffer_SIZE];	// Buffer to store file content
		int err = 1, offset;

		while (!f_eof(&mp3File) && err) {		// voy a leer hasta encontra un frame para decodear, o hasta que no haya mas
			if (f_read(&mp3File, readFileBuffer, readFileBuffer_SIZE, &bytesRead) == FR_OK) {	// readFileBuffer va a ser el lugar donde guarde la info

				offset = MP3FindSyncWord(readFileBuffer, bytesRead);
				if (offset < 0) {
					continue;	//no pudo sincronizar asique vuelvo al f_read
				}

				err = MP3GetNextFrameInfo(mp3Dec, &mp3Info, readFileBuffer+offset);
				if (err) {
					f_lseek(&mp3File, f_tell(&mp3File) - bytesRead + offset + 1);	//actualizo el puntero al frame que sincronizamos.
				}
			}
			else {		// tiro error sad
				break;
			}
    	}

		if (!err) {
			
			if (mp3Info.samprate == MP3_SAMPRATE) { // Si no esta a los 44100 no lo tomo
				if (f_lseek(&mp3File, f_tell(&mp3File) - bytesRead + offset) == FR_OK) {	// actualizo el puntero al principio del frame ya que califica
					return 0;
				}
			}
		}

		clearPath();
    	f_close(&mp3File);
		fileOpen = false;
    }
    else {

    }

	return 1;	// Error
}

uint16_t MP3DecNextFrame(int16_t* outBuff) {

	if (!fileOpen) {
		return 0;	// No hice select song entonces no hago nada
	}

	BYTE readFileBuffer[readFileBuffer_SIZE];
	int16_t tempOutBuff[2*OUTBUFF_SIZE];	// por si tengo que convertir un tema a mono
	UINT bytesRead;
	int err, offset, bytesLeft;
	uint8_t* pDecoderBuff;

	bool SyncAgain = false;	// Find frame sync

	while (!f_eof(&mp3File)) {
		if (f_read(&mp3File, readFileBuffer, readFileBuffer_SIZE, &bytesRead) == FR_OK) {	// trabajo sobre readFileBuffer

			pepe++;
			bytesLeft = bytesRead;
			pDecoderBuff = readFileBuffer;

			if (SyncAgain) {
				offset = MP3FindSyncWord(pDecoderBuff, bytesLeft);
				if (offset < 0) {
					continue;
				}
				SyncAgain = false;
				bytesLeft -= offset;
				pDecoderBuff += offset;	// lo dejo apuntando al data frame
			}

			int progress = bytesLeft;
			// si es estereo uso el buffer temporal para luego convertir
			err = MP3Decode(mp3Dec, &pDecoderBuff, &bytesLeft, mp3Info.nChans == 1 ? outBuff : tempOutBuff, 0U);
			switch (err) {
				case ERR_MP3_NONE:
					MP3GetLastFrameInfo(mp3Dec, &mp3Info);
					//ya obtuve la info asique apunto al final del frame
					f_lseek(&mp3File, f_tell(&mp3File) - bytesLeft);

					// Si la cancion era stereo tengo que convertirla entonces a mono
					if (mp3Info.nChans == 2) {
						for (int i = 0; i < OUTBUFF_SIZE; i++) {
							outBuff[i] = (int16_t)(((int32_t)tempOutBuff[2*i] + (int32_t)tempOutBuff[2*i+1]) / 2);
						}
					}
					return mp3Info.outputSamps / mp3Info.nChans;
					break;
				case ERR_MP3_INDATA_UNDERFLOW:
				case ERR_MP3_MAINDATA_UNDERFLOW:

					if (!f_eof(&mp3File)) {		// Move file pointer to start of last frame
						f_lseek(&mp3File, f_tell(&mp3File) - progress);
					}
					break;
				default:
					// Si llegue aca, es que tuve un error que no programe
					//Entonces vuelvo el puntero al inicio del ultimo frame que decodie y busco hacer una sync de nuevo
					SyncAgain = true;
					f_lseek(&mp3File, f_tell(&mp3File) - progress + 1);
					break;
			}

		}
		else {

			break;
		}
	}

	return 0;

}

//
int SDSearch(char* bufferData, bool* typeData, int index)
{
	int countnames = 0;
	//char* ptr;

	//songNum = SDSearch(arrArchives,arrTypes,-1) - 1;

	if(index<0)
	{
		 result = f_opendir(&dir,"1:/");
		 pathOffset +=1;
		 path[0] = '1';
		 path[1] = ':';
		 path[2] = '/';
		 path[3] = '\0';
	}
	else
	{
		strcat(path,&(bufferData[index*31]));
		uint8_t len = strlen(path);
		path[len++] = '/';
		path[len] = '\0';
		countnames=1;
		result = f_opendir(&dir,path);
	}
	if(result==FR_OK)
	{
		int ff ;
		for(ff=0 ; ff<(20*31) ; ff++ )
			bufferData[ff] = '\0';

		while(1)
		{
			result = f_readdir(&dir, &fno);
			if (result != FR_OK || fno.fname[0] == 0) break;
			char* filename = fno.fname;
			int i;
			for (i = 0; i < 30 && filename[i]; i++);
			memcpy(&bufferData[countnames * 31 ], filename, i);
			typeData[countnames] = fno.fattrib & AM_DIR;
			countnames++;
		 }
	 }
	 f_closedir(&dir);

	 return countnames;
}


void MP3ReadID3Data(mp3_id3Metadata* outMetadata)
{
	//La ID3 Metadata se encuentra en los ultimos 128 bytes del archivo .mp3

	//Version ID3.V1 & ID3.V1.1 --> Vamos a leer los ultimos 128 bytes:


	UINT bytesRead;

	char mp3id3Data[300];
	FSIZE_t temp_ptr = mp3File.fptr;

	// Move the file pointer to the end of the file minus 128 bytes
	f_lseek(&mp3File, f_size(&mp3File) - 128);

	if(f_read(&mp3File,mp3id3Data,ID3_SIZE,&bytesRead) == FR_OK)
	{

		if((mp3id3Data[0] == 'T') &&
		   (mp3id3Data[1] == 'A') &&
		   (mp3id3Data[2] == 'G'))
		{
			outMetadata->isThereID3Data = true;

			/* Id3 tag elements don't necessarily end in a '\0' if they
			   occupy the full 30 character space. Thus, we'll have to
			   nul-terminate all strings just to be sure. */
			const unsigned char *s = mp3id3Data + 3;

			memcpy(outMetadata->title, s, 30);
			outMetadata->title[30] = '\0';
			s += 30;

			memcpy(outMetadata->artist, s, 30);
			outMetadata->artist[30] = '\0';
			s += 30;

			memcpy(outMetadata->album, s, 30);
			outMetadata->album[30] = '\0';
			s += 30;

			memcpy(outMetadata->year, s, 4);
			outMetadata->year[4] = '\0';
			s += 4;

			memcpy(outMetadata->comment, s, 30);
			outMetadata->comment[30] = '\0';
			s += 30;
		}
		else
		{
			//Version ID3.V2 --> Tag esta al comienzo
			//O simplemente no tiene ID3 data
			outMetadata->isThereID3Data = false;
		}
	}
	mp3File.fptr = temp_ptr;

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void clearPath()
{
	int p;
	for(p=0 ; p<MAX_PATH ; p++)
	{
		path[p] = '\0';
	}
}

void initPath(void){
	int p;
	path[0] = '1';
	path[1] = ':';
	path[2] = '/';
	path[3] = '\0';
	for(p=4 ; p<MAX_PATH ; p++)
	{
		path[p] = '\0';
	}
}

