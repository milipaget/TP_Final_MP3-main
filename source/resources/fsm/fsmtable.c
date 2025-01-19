#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "fsm.h"
#include "fsmtable.h"
#include "../../mp3Config.h"
#include "../eventos/eventQueue.h"
#include "../../gpio.h"
#include "../../board.h"
#include "../ledMatrix/vumeter.h"
#include "../ledMatrix/matrixLED.h"
#include "../../musicHandler/mp3Decoder.h"
#include "../../musicHandler/dma_music.h"
#include "../SDHC/ff.h"
#include "../timers/timer.h"

#include "../../DSP/EQUALIZER/equalizer.h"
#include "../../DSP/FFT/fft.h"

/*declaración de estados*/

extern STATE idle[]; //El equipo está prendido y se hacen los updates de las variables
extern STATE off[]; //Está "apagado", bajo consumo (no hace nada básicamente)
extern STATE errorState[]; //noSD originalmente

// prototipos

static void do_nothing(void);
static void increaseVolumeCount(void);
static void decreaseVolumeCount(void);
static void changePlayingState(void);
static void decreaseSongPointer(void);
static void increaseSongPointer(void);
static void changeToOff(void);
static void changeToOn(void);
static void increaseFilterCount(void);
static void changeToSDError(void);
static void changeToSongError(void);
static void changeToNoError(void);
static void checkSD(void);

static void spectrumAnalysis(q15_t* input, q15_t* outputEqualizer, q15_t* buffer, uint16_t samples, uint8_t volume);
static void selectSong(void);

#define BUFFERSIZE 200

char arrArchives[20][31]={{' '}};
bool arrTypes[20]={0};
mp3_id3Metadata myMetadata;

int16_t buff1[2*OUTBUFF_SIZE];
int16_t buff2[2*OUTBUFF_SIZE];
int16_t* pTable = buff1;
int16_t* pPrevTable = buff2;
uint16_t br;

int16_t buffer[2*BUFFERSIZE];

//uint8_t updateMP3Timer;

int8_t coefConfigFilters[5]={0, 0, 0, 0, 0}; //Uno para cada banda

int8_t arrFiltersPredef[][5]={	{ 0,-3,-6,-3, 0}, 	// Rock
								{-3, 0,-6,-3, 0}, 	// Jazz
								{-3,-6,-3, 0,-3}, 	// Clásica
								{ 0, 0, 0, 0, 0}, 	// None
								{-6,-3, 0,-3,-6} 	// Pop
							};
char generes[5][16]={ 	{"     Rock      "},
						{"     Jazz      "},
						{"    Classic    "},
						{"     None      "},
						{"     Pop       "}};

char bandas[5][16]={ 	{"     Banda 1   "},
						{"     Banda 2   "},
						{"     Banda 3   "},
						{"     Banda 4   "},
						{"     Banda 5   "}};

char dbsMSG[7][16]= { 	{"     -9 dB     "},
						{"     -6 dB     "},
						{"     -3 dB     "},
						{"      0 dB     "},
						{"      3 dB     "},
						{"      6 dB     "},
						{"      9 dB     "}};
int8_t dbs[7]={-9,-6,-3,0,3,6,9};
int8_t indexBandas = 0;
int8_t indexdbs = 3;


int8_t indexGeneres = 0;

typedef enum
{
	PING,
	PONG
}pingpong;

pingpong bufferUsing;
bool ping_pong = false; //false --> guardar en buff2.

int16_t* dmaBuff1;
int16_t* dmaBuff2;

q15_t outputEqualizer[2*OUTBUFF_SIZE],outputEqualizer2[2*OUTBUFF_SIZE];

mp3config_variables_t* PTR2mp3ConfigVariables;

/*** tablas de estado ***/
//{EVENTO, PRÓXIMO ESTADO, ACCIÓN A REALIZAR}

//TENEMOS QUE AGREGAR UNA INTERRUPCIÓN QUE SEA UPDATEAR LAS COSAS!!

STATE idle[]=
{
  	{None, idle, do_nothing},
  	{volumeUP, idle, increaseVolumeCount}, //Incremento la variable que tenga el nivel del volumen
	{volumeDOWN, idle, decreaseVolumeCount}, //Decremento la variable que tenga el nivel del volumen
	{playPause, idle, changePlayingState}, //Niego el estado anterior de reproducción
	{prevSong, idle, decreaseSongPointer}, //Decremento el puntero a la canción anterior
	{nextSong, idle, increaseSongPointer}, //Incremento el puntero a la canción anterior
	{onOff, off, changeToOff}, //Pongo modo off
	{changeFilter, idle, increaseFilterCount}, //Paso al siguiente filtro
	{SDCardOut, errorState, changeToSDError}, //Pongo en modo error
  	{FIN_TABLA, idle, do_nothing}
};

 STATE off[]=
{
  	{onOff, idle, changeToOn}, //Pongo modo on
	//Capaz hay que agregar lo de la SD
  	{FIN_TABLA,idle,do_nothing}
};

STATE errorState[]=
{
	//Capaz hay que agregar lo de off
  	{SDCardIn, idle, checkSD}, //Pongo en modo error
  	{FIN_TABLA, idle ,do_nothing}
};

//========interfaz=================


STATE *FSM_GetInitState(void)
{
	PTR2mp3ConfigVariables = initMp3Config();
	chEqCoeff(coefConfigFilters);
	songNum = SDSearch(arrArchives,arrTypes,-1);
	if(songNum == 0){
		changeToSongError();
		//gpioToggle(PIN_LED_RED);
		return (errorState);
	}
	//gpioToggle(PIN_LED_GREEN);
	selectSong();
 	return (idle);
}

///=========Rutinas de accion===============

/*Dummy function*/
static void do_nothing(void){

}

static void increaseVolumeCount(void){
	if(PTR2mp3ConfigVariables->volume < MAX_VOLUME){
		PTR2mp3ConfigVariables->volume++;
	}
	//gpioToggle(PIN_LED_GREEN);
}

static void decreaseVolumeCount(void){
	if(PTR2mp3ConfigVariables->volume > 0){
		PTR2mp3ConfigVariables->volume--;
	}
	//gpioToggle(PIN_LED_RED);
}

static void changePlayingState(void){
	PTR2mp3ConfigVariables->isPlaying = !PTR2mp3ConfigVariables->isPlaying;
	if(PTR2mp3ConfigVariables->isPlaying){
		DMA_ResumeMusic();
	}

	if(!PTR2mp3ConfigVariables->isPlaying){
		DMA_PauseMusic();
	}
	//gpioToggle(PIN_LED_GREEN);
}

static void decreaseSongPointer(void){
	if(PTR2mp3ConfigVariables->songPointer > 0){
		PTR2mp3ConfigVariables->songPointer--;
	}
	else if(PTR2mp3ConfigVariables->songPointer == 0){
		PTR2mp3ConfigVariables->songPointer = songNum;
	}
	selectSong();
	//gpioToggle(PIN_LED_RED);
}

static void increaseSongPointer(void){
	if(PTR2mp3ConfigVariables->songPointer < songNum){
		PTR2mp3ConfigVariables->songPointer++;
	}
	else if(PTR2mp3ConfigVariables->songPointer == songNum){
		PTR2mp3ConfigVariables->songPointer = 0;
	}
	selectSong();
	//gpioToggle(PIN_LED_RED);
}

static void changeToOff(void){
	PTR2mp3ConfigVariables->isOn = false;
	gpioToggle(PIN_LED_BLUE);

}

static void changeToOn(void){
	PTR2mp3ConfigVariables->isOn = true;
	gpioToggle(PIN_LED_GREEN);
}

static void increaseFilterCount(void){
	if(PTR2mp3ConfigVariables->filterType < EQTYPE){
		PTR2mp3ConfigVariables->filterType++;
	}
	else if(PTR2mp3ConfigVariables->filterType == EQTYPE){
		PTR2mp3ConfigVariables->filterType = 0;
	}
	//gpioToggle(PIN_LED_RED);
}

static void changeToSDError(void){
	PTR2mp3ConfigVariables->errorType = NO_SD;
}

static void changeToSongError(void){
	PTR2mp3ConfigVariables->errorType = NO_SONGS;
}

static void changeToNoError(void){
	PTR2mp3ConfigVariables->errorType = NO_ERROR;
}

static void checkSD(void){
	//initSD();
	songNum = SDSearch(arrArchives,arrTypes,-1) - 1; //-1 para que apunte a la última canción
	if(songNum == 0){
		changeToSongError();
	}
	changeToNoError();
}

void updateMP3Callback(void){
	if(PTR2mp3ConfigVariables->isPlaying){
		if(DMA_StatusBuffers() != 2){
			if(ping_pong){
				ping_pong = !ping_pong;
				br = MP3DecNextFrame(pPrevTable);
				spectrumAnalysis(pPrevTable, outputEqualizer2, buffer, OUTBUFF_SIZE, PTR2mp3ConfigVariables->volume);
				dmaBuff2 = outputEqualizer2;
				//dmaBuff2 = pPrevTable;
				for (int i = 0; i < OUTBUFF_SIZE; i++) {
					dmaBuff2[i] = 0x8000U + dmaBuff2[i] * (float) PTR2mp3ConfigVariables->volume/15 ;
					dmaBuff2[i] *= ((float)0xFFFU / 0xFFFFU);
				}
				DMAmusic((uint16_t*)dmaBuff2, OUTBUFF_SIZE-1, 44100);
			}
			else{
				ping_pong = !ping_pong;
				br = MP3DecNextFrame(pTable);
				spectrumAnalysis(pTable, outputEqualizer, buffer, OUTBUFF_SIZE, PTR2mp3ConfigVariables->volume);
				dmaBuff1 = outputEqualizer;
				//dmaBuff1 = pTable;
				for (int i = 0; i < OUTBUFF_SIZE; i++) {
					dmaBuff1[i] = 0x8000U + dmaBuff1[i] * (float)PTR2mp3ConfigVariables->volume/15;
					dmaBuff1[i] *= ((float)0xFFFU / 0xFFFFU);
				}
				DMAmusic((uint16_t*)dmaBuff1, OUTBUFF_SIZE-1, 44100);
			}
		}
		if(br==0){
			DMA_PauseMusic();
		}
	}

	else if(!PTR2mp3ConfigVariables->isPlaying){
		DMA_PauseMusic();
	}
}

void selectSong(void){
	if(!arrTypes[PTR2mp3ConfigVariables->songPointer]){
		//PTR2mp3ConfigVariables->songSelected = true;
		if (MP3SelectSong(arrArchives[PTR2mp3ConfigVariables->songPointer],&myMetadata)) { //Aca poner el nombre de la cancion bien.
			return;
		}

		pTable = buff1;
		pPrevTable = buff2;

		if (!MP3DecNextFrame(pTable)) {
			//int o;
		}

		// 16 bit to 12 bit and shifting
		for (int i = 0; i < OUTBUFF_SIZE; i++) {
			pTable[i] = 0x8000U + pTable[i] * (float)PTR2mp3ConfigVariables->volume/15;
			pTable[i] *= ((float)0xFFFU / 0xFFFFU);
		}

		br = 1;
		DMAmusic((uint16_t*)buff1, OUTBUFF_SIZE-1, 44100);
	}
}

static void spectrumAnalysis(q15_t* input, q15_t* outputEqualizer, q15_t* buffer, uint16_t samples, uint8_t volumen)
{
    // Copio el input concatenado a las ultimas muestras anteriores
    q15_t inputEqualizer[FFTSIZE];
    //memcpy(inputEqualizer, buffer, BUFFERSIZE*sizeof(int16_t));
    //memcpy(&inputEqualizer[BUFFERSIZE], input, samples*sizeof(int16_t));
    memcpy(inputEqualizer, input, samples*sizeof(int16_t));

    // Actualizo el buffer
    //memcpy(buffer, &inputEqualizer[samples], BUFFERSIZE*sizeof(int16_t));

    equalizer(inputEqualizer, outputEqualizer, samples + BUFFERSIZE);
    q15_t inputFFT[FFTSIZE], outputFFT[SPECTRUM_GROUPS];
    bool ifftFlag = false;

    for (uint16_t i = 0; i < BUFFERSIZE; i++)
	{
		outputEqualizer[i] += buffer[i];		//overlap and add
	}

    // Actualizo el buffer
	memcpy(buffer, &outputEqualizer[samples], BUFFERSIZE*sizeof(int16_t));

    // Guardo las muestras correspondientes al audio recibido
    memcpy(inputFFT, outputEqualizer, samples*sizeof(int16_t));
    //fftGroups(inputFFT, outputFFT, samples, ifftFlag, volumen);

    uint8_t matrix[64][3];

    /*for (uint8_t column = 0; column < SPECTRUM_GROUPS; column++)
    {
        for (uint8_t row = 0; row < 8; row++)
        {
            if ((8 - row) <= outputFFT[column])
            {
                matrix[8*row + column][0] = 31 * row;
                matrix[8*row + column][1] = 255 - 31* row;
                matrix[8*row + column][2] = 0;
            }
            else
            {
                matrix[8*row + column][0] = 0;
                matrix[8*row + column][1] = 0;
                matrix[8*row + column][2] = 0;
            }
        }
    }*/

    //DMAChangeAllMatrix(matrix, 20);

}


