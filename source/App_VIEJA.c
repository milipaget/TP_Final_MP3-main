/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
/*
#include <resources/timers/timer.h>
#include <stdio.h>
#include "board.h"		//Includes gpio.h
#include "Drivers/MP3DEC/mp3Reader.h"
#include "Drivers/DMA/dma_music.h"
#include "Drivers/DISPLAY/Display1602.h"
#include "Drivers/RTC/RTC.h"
#include "Drivers/DMA/dma_matrix.h"
#include "DSP/FFT/fft.h"
#include "resources/PoweManagement/sleep.h"
//#include "resources/Buttons/buttonAlt.h"
//#include "resources/encoder.h"
#include "resources/SDHC/ff.h"
//MP3_READER

#include "DSP/FFT/FFT.h"
#include "DSP/EQUALIZER/equalizer.h"

#include "Drivers/MP3DEC/mp3Reader.h"
#include "Drivers/DMA/dma_music.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/*
enum states{
	IDLE = 0,
	INIT,
	MENU,
	PLAY,
	MENU_EQ,
	MENU_SS,
	MENU_EQ_PREDEF,
	MENU_EQ_CUSTOM,
	GO_TO_SLEEP,
	MENU_SET_TIMER,
	MENU_EQ_CUSTOM_CH

};

static uint8_t currentState = IDLE;
static bool songPlaying = false; // Mostrar o no el ID3.

/****************Buttons********************/
/*
#define BUTTON_MEQ 1
#define BUTTON_MSS 2
#define BUTTON_ENCODER 4
#define BUTTON_PREDEF 1
#define BUTTON_CUSTOM 2
#define BUTTON_SET_TIMER 3
#define BUTTON_HMS 1
#define BUTTON_SMH 2
/*******************************************/
/*
#define BUFFERSIZE 200
/*****************Colors********************/
/*
#define RED  {0,255,0}
#define BLUE {0,0,255}
#define GREEN {255,0,0}
#define CYAN {255,0,255}
#define ORANGE {165,255,0}
#define YELLOW {255,255,0}
#define WHITE {255,255,255}
#define NO_COLOR {0,0,0}
/*******************************************/
/*
typedef enum
{
	PING,
	PONG
}pingpong;
/*******************************************************************************/
/*******************************************************************************/
/*
#define MAX_VOLUMEN 31
#define MIN_VOLUMEN 1
/*******************************************************************************/
/*******************************************************************************/
/*
#define MSG_MP3 			"      MP3      "
#define MSG_OREO_TEAM       "   Team Oreo   "
#define MSG_MAX_VOLUMEN		"  Max Volumen  "
#define MSG_MIN_VOLUMEN		"  Min Volumen  "
#define MSG_GOOD_BYE		"    Good Bye   "
#define MSG_MENU 			"     Menu      "
#define MSG_CHANGE_TIME 	"  Change Time  "
#define MSG_EQ_BA			"  Ecu. Bandas  "
#define MSG_EQ_PREF 		"  Ecu. Predef  "
#define MSG_MOD_PL 			"  Change Song  "
#define MSG_PREDEF_JAZZ 	"     Jazz      "
#define MSG_PREDEF_BLUES 	"     Blues	    "
#define MSG_PREDEF_ROCK    	"     Rock      "
#define MSG_PREDEF_POP      "      Pop      "

/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
/*
pingpong bufferUsing;

mp3_id3Metadata myMetadata;

//mp3_errors error;
//mp3_decoState decodingState;
//mp3_SamplesInfo samplesInfo;

bool isPausa = false;

bool resultDMA = false;

char arraySongs[1000][31];

tim_id_t timeOut,sampleEncoderAndButtons,matrixAnimationTimer;
uint8_t volumen =( MAX_VOLUMEN - MIN_VOLUMEN )/ 2;
uint8_t matrix[64][3];
char display16x2[2][16];
uint8_t actualizarMSGDISPLAY = false;
bool songSelected;
uint8_t buttons;
/********************************/
/*
int8_t hours, mins, seconds;
char msg[10];
/*********Show Files*************/
/*
#define MAXFILES 20
#define MINFILE 1
char arrArchives[20][31]={{' '}};
bool arrTypes[20]={0};
uint8_t cantFiles2Show;
uint8_t indexSS = 1;
/*********************************/
/*
q15_t outputEqualizer[2*OUTBUFF_SIZE],outputEqualizer2[2*OUTBUFF_SIZE];
int16_t buff1[2*OUTBUFF_SIZE];
int16_t buff2[2*OUTBUFF_SIZE];
bool ping_pong = false; //false --> guardar en buff2.
int16_t* pTable = buff1;
int16_t* pPrevTable = buff2;
int16_t* dmaBuff1;
int16_t* dmaBuff2;
int16_t buffer[2*BUFFERSIZE];
uint16_t br;
FATFS FatFs;
uint8_t encodermsg = 0;
/**********************************/
/*
int8_t coefConfigFilters[5]={0, 0, 0, 0, 0}; //Uno para cada banda
uint8_t config=0;
int8_t indexhms = 0; // 0 ->hour, 1->mins,2-> seconds
/*********************************/
/*
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
enum PREDEF
{
	ROCK,
	JAZZ,
	CLASIC,
	REGG,
	POP
};
int8_t indexGeneres = 0;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
/*
static void showVolumen(void);
static void goToSleep(void);
static void showMSG(char* Line1,char* Line2,bool doItNow);
static void sampleAll(void);
static void animationMatrix(void);
static void spectrumAnalysis(q15_t* input, q15_t* outputEqualizer, q15_t* buffer, uint16_t samples, uint8_t volume);
void convertSample(short* samples,uint16_t* convertedSamples, uint32_t len);
static void hms2str(char* arr);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa *//*
void App_Init_VIEJA (void)
{

	/*******Debug Variables******//*
	gpioMode(PIN_LED_RED, OUTPUT); //For debugging purpose
	gpioMode(PIN_LED_GREEN, OUTPUT); //For debugging purpose
	gpioMode(PIN_LED_BLUE, OUTPUT); //For debugging purpose
	gpioMode(PIN_SW3,INPUT); //For debugging purpose
	gpioWrite(PIN_LED_RED, !LED_ACTIVE);
	gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
	gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);

	/*****************************************/
	/************Init Decode MP3**************/
	/*****************************************//*
	timerInit();
	timeOut = timerGetId();
	sampleEncoderAndButtons = timerGetId();
	matrixAnimationTimer = timerGetId();
	fft_Init();	// solo acepta valores múltiplos de 3 entre -9dB y 9dB
	chEqCoeff(coefConfigFilters);
	if (f_mount(&FatFs, "1:/", 0) == FR_OK) {
			int a;
	}
	if (MP3DecInit()) {
		int a;
	}
	DMAmusicInit();
	/*****************************************/
	/*****************************************/
	/************Init Display 16x02***********/
	/*****************************************/
	//Disp1602Init(); Esto necesita I2C entonces no se pued ehacer aca :c
	/*****************************************/
	
	/*****************************************/
	/************Init Matrix 8x8**************/
	/*****************************************//*
	DMAmatrixInit();
	/*****************************************/

	/*****************************************/
	/************Init RTC*********************/
	/*****************************************//*
	RTCInit();
	/*****************************************/

	/*****************************************/
	/************Init Sleep*******************/
	/*****************************************//*
	setUpSleep();
	/*****************************************/
	
	/*****************************************/
	/************Init Buttons*****************/
	/*****************************************//*
	buttons_Init();
	/*****************************************/

	/*****************************************/
	/************* TimeOut *******************/
	/*****************************************//*
	timerStart(timeOut, TIMER_MS2TICKS(10000), TIM_MODE_PERIODIC, &goToSleep);
	/*****************************************/
/*
	encoder_Init();

	timerStart(sampleEncoderAndButtons, TIMER_MS2TICKS(200), TIM_MODE_PERIODIC, &sampleAll);
}

/* Función que se llama constantemente en un ciclo infinito *//*
void App_Run_VIEJA (void)
{
	switch(currentState)
	{
		case IDLE:
		{
			currentState = INIT;
			Disp1602Init();
			showMSG("    Team Oreo","     El mejor",true);
			timerStart(matrixAnimationTimer,TIMER_MS2TICKS(100),TIM_MODE_PERIODIC,&animationMatrix);

		}
		break;
		case INIT:
		{
			songSelected = false;

			bufferUsing = PING;

			//Hacer animaciones con la matriz de 8x8.
			if(buttons) // Falta a;adir esta parte -->|| ver si roto el encoder.
			{
				currentState = MENU;
				buttons=0;
				encodermsg=0;
				clearRotationFlag();
			}
		}
		break;
		case MENU:
		{
			timerStop(matrixAnimationTimer);
			//Ahora necesitamos actualizar el valor del volumen si es que me encuentro en el menu
			 //Aca recibo el numero del boton que se toco, si es cero es porque no se toco ningun boton.
			if(buttons == BUTTON_MEQ) //De ultima cambiar el define del valor.
			{
				buttons =0;
				currentState = MENU_EQ;
				showMSG("      Menu","    Equalizer",true);
			}
			else if(buttons == BUTTON_MSS)
			{
				buttons =0;
				currentState = MENU_SS;
				showMSG("      Menu","   Select Song",true);
				cantFiles2Show = SDSearch(arrArchives,arrTypes,-1);
			}
			else if(buttons == BUTTON_SET_TIMER)
			{
				buttons =0;
				currentState = MENU_SET_TIMER;
				showMSG("      Menu","   Set Timer",true);
				RTCGetRealTime(&hours,&mins,&seconds);
			}
			// Queremos que si hay cancion, muestr ID3.
			if(songSelected)
			{
				showMSG((char*)myMetadata.artist,(char*)myMetadata.title,false);
			}
			else
			{

				RTCgetRealTimeSTR(msg);
				if(isPausa)
				{
					showMSG("     PAUSE    ",msg,false);
				}
				else
					showMSG("    No song    ",msg,false);
			}
			if(encodermsg == ROTATE_RIGHT) //Ahora es asi, luego hay que poner giro derecha encoder
			{
				volumen++;
				showVolumen();
				clearRotationFlag();
				encodermsg=0;

			}
			else if(encodermsg == ROTATE_LEFT) //Ahora es asi, luego hay que poner giro izquierda encoder
			{
				volumen--;
				showVolumen();
				clearRotationFlag();
				encodermsg=0;
			}

		}
		break;
		case MENU_EQ:
		{

			// Aca hago las cosas trambolicas del ecualizador
			// Hay que tener en cuenta poner predefinido o no.
			//Tambien hay que mostrar en el display de 8x8 la banda en cuestion, hacer la animacion.
			if(buttons == BUTTON_PREDEF) //De ultima cambiar el define del valor.
			{
				buttons =0;
				currentState = MENU_EQ_PREDEF;
				showMSG("      Filters","    Predef",true);
			}
			else if(buttons == BUTTON_CUSTOM)
			{
				buttons =0;
				currentState = MENU_EQ_CUSTOM;
			}

		}
		break;
		case MENU_EQ_PREDEF:
		{
			showMSG("    Genre",generes[indexGeneres],false);
			if(encodermsg == ROTATE_RIGHT) //Ahora es asi, luego hay que poner giro derecha encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(++indexGeneres > 4)
				{
					indexGeneres = 0;
				}

			}
			else if(encodermsg == ROTATE_LEFT) //Ahora es asi, luego hay que poner giro izquierda encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(--indexGeneres < 0) // underflow
				{
					indexGeneres = 4;
				}
			}
			else if(buttons == BUTTON_ENCODER)
			{
				buttons = 0;
				chEqCoeff(arrFiltersPredef[indexGeneres]);
				currentState = MENU;
			}
		}
		break;
		case MENU_EQ_CUSTOM:
		{
			showMSG("    Genre",bandas[indexBandas],false);
			if(encodermsg == ROTATE_RIGHT) //Ahora es asi, luego hay que poner giro derecha encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(++indexBandas > 4)
				{
					indexBandas = 0;
				}

			}
			else if(encodermsg == ROTATE_LEFT) //Ahora es asi, luego hay que poner giro izquierda encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(--indexBandas < 0) // underflow
				{
					indexBandas = 4;
				}
			}
			else if(buttons == BUTTON_ENCODER)
			{
				buttons = 0;
				currentState = MENU_EQ_CUSTOM_CH;
			}
		}
		break;
		case MENU_EQ_CUSTOM_CH:
		{
			showMSG(bandas[indexBandas],dbsMSG[indexdbs],false);
			if(encodermsg == ROTATE_RIGHT) //Ahora es asi, luego hay que poner giro derecha encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(++indexdbs > 7)
				{
					indexdbs = 0;
				}

			}
			else if(encodermsg == ROTATE_LEFT) //Ahora es asi, luego hay que poner giro izquierda encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(--indexdbs < 0) // underflow
				{
					indexdbs = 6;
				}
			}
			else if(buttons == BUTTON_ENCODER)
			{
				buttons = 0;
				coefConfigFilters[indexBandas] = dbs[indexdbs];
				chEqCoeff(coefConfigFilters);
				currentState = MENU;
			}
		}
		break;
		case MENU_SS:
		{

			showMSG("    Name Song:",arrArchives[indexSS],false);
			if(encodermsg == ROTATE_RIGHT) //Ahora es asi, luego hay que poner giro derecha encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(++indexSS > cantFiles2Show-1)
				{
					indexSS = MINFILE;
				}

			}
			else if(encodermsg == ROTATE_LEFT) //Ahora es asi, luego hay que poner giro izquierda encoder
			{
				encodermsg=0;
				clearRotationFlag();
				if(--indexSS < MINFILE)
				{
					indexSS = cantFiles2Show-1;
				}
			}
			//Si no llega a funcionar, borrar el comentario que esta aca abajo.
			//songSelected = false;
			if(buttons == BUTTON_ENCODER)
			{
				buttons = 0;
				if(!arrTypes[indexSS])
				{
					songSelected = true;
					if (MP3SelectSong(arrArchives[indexSS],&myMetadata)) { //Aca poner el nombre de la cancion bien.
					    	return;
						}

						pTable = buff1;
						pPrevTable = buff2;

						if (!MP3DecNextFrame(pTable)) {
							int o;
						}

						// 16 bit to 12 bit and shifting
						for (int i = 0; i < OUTBUFF_SIZE; i++) {
						//	float auxF = pTable[i] * ((float));
							pTable[i] = 0x8000U + pTable[i] * (float)volumen/15;
							pTable[i] *= ((float)0xFFFU / 0xFFFFU);
							//int32_t auxF = ((int32_t)pTable[i] * ((int32_t)(volumen)))/ ( INT32_C(1)<<15);
						}


						br = 1;
						DMAmusic((uint16_t*)buff1, OUTBUFF_SIZE-1, 44100);

					currentState = MENU;
				}
				else
				{

					cantFiles2Show = SDSearch(arrArchives,arrTypes,indexSS);

					currentState = MENU_SS;
				}
			}
		}
		break;
		case GO_TO_SLEEP:
		{
			//Chequear si al salir de aca es necesario reestablecer los timers (Lo dudo)

			showMSG("      Sleeping","   Don't bother",true);
			enteringLLS(); //PC3, luego hay que cambiar o que todos los botones tengan acceso a ese pin.
			currentState = IDLE;
		}
		break;
		case MENU_SET_TIMER:
		{
			hms2str(msg);
			showMSG("   Set Timer",msg,true);
			if(buttons == BUTTON_ENCODER)
			{
				buttons = 0;
				currentState = MENU;
				RTCSetTime(hours,mins,seconds);
			}

			else if(buttons == BUTTON_HMS)
			{
				buttons = 0;
				if(++indexhms>2)
				{
					indexhms = 0;
				}
			}
			else if(buttons == BUTTON_HMS)
			{
				buttons = 0;
				if(--indexhms<0)
				{
					indexhms = 2;
				}
			}
			if(encodermsg == ROTATE_RIGHT) //Ahora es asi, luego hay que poner giro derecha encoder
			{
				encodermsg=0;
				clearRotationFlag();
				switch(indexhms)
				{
				case 0:
				{
					if(++hours>23)
					{
						hours = 0;
					}
				}break;
				case 1:
				{
					if(++mins>59)
					{
						mins = 0;
					}
				}break;
				case 2:
				{
					if(++seconds>59)
					{
						seconds = 0;
					}
				}break;
				}

			}
			else if(encodermsg == ROTATE_LEFT) //Ahora es asi, luego hay que poner giro izquierda encoder
			{
				encodermsg=0;
				clearRotationFlag();
				switch(indexhms)
				{
				case 0:
				{
					if(--hours<0)
					{
						hours = 23;
					}
				}break;
				case 1:
				{
					if(--mins<0)
					{
						mins = 59;
					}
				}break;
				case 2:
				{
					if(--seconds<0)
					{
						seconds = 59;
					}
				}break;
				}
			}
		}
		break;
	}

	if(songSelected)
	{
		if(DMA_StatusBuffers() != 2)
		{
			if(ping_pong)
			{
				ping_pong = !ping_pong;

				br = MP3DecNextFrame(pPrevTable);
				spectrumAnalysis(pPrevTable, outputEqualizer2, buffer, OUTBUFF_SIZE, volumen);
				dmaBuff2 = outputEqualizer2;
				//dmaBuff2 = pPrevTable;
				for (int i = 0; i < OUTBUFF_SIZE; i++) {
					dmaBuff2[i] = 0x8000U + dmaBuff2[i] * (float) volumen/15 ;
					dmaBuff2[i] *= ((float)0xFFFU / 0xFFFFU);

				}


				DMAmusic((uint16_t*)dmaBuff2, OUTBUFF_SIZE-1, 44100);
			}
			else
			{
				ping_pong = !ping_pong;

				br = MP3DecNextFrame(pTable);
				spectrumAnalysis(pTable, outputEqualizer, buffer, OUTBUFF_SIZE, volumen);
				dmaBuff1 = outputEqualizer;
				//dmaBuff1 = pTable;
				for (int i = 0; i < OUTBUFF_SIZE; i++) {

					dmaBuff1[i] = 0x8000U + dmaBuff1[i] * (float)volumen/15;
					dmaBuff1[i] *= ((float)0xFFFU / 0xFFFFU);

				}

				DMAmusic((uint16_t*)dmaBuff1, OUTBUFF_SIZE-1, 44100);
			}
		}
		if(br==0)
		{
			songSelected = 0;
			DMA_PauseMusic();
			isPausa = false;
		}
		if((buttons == BUTTON_ENCODER) && (currentState == MENU)) // aca poner button stop
		{
			buttons = 0;
			songSelected = false;
			isPausa = true;
			DMA_PauseMusic();
		}
	}
	if((buttons == BUTTON_ENCODER) && (isPausa == true) && (currentState == MENU)) //aca poner button stop
	{
		buttons = 0;
		songSelected = true;
		isPausa = false;
		DMA_ResumeMusic();
	}
}

/*******************************************************************************
 * 					Private functions
 ******************************************************************************//*
static void showMSG(char* Line1,char* Line2,bool doItNow) //Necesito Delay si no se rompe el display I2C
{
	static uint32_t count =0;

	if(count == 0 || doItNow)
	{
		count = 20000;
		writeLine(Line1,0);
		writeLine(Line2,1);
	}
	count--;
}
static void sampleAll(void) //Como dice la funcion, tambien hay que samplear el encoder, luego hay tengo que ver leer el estado del enconder para ver si fue giro derecha o giro izquierda.
{
	buttons = sampleButtonsAlt();
	encodermsg = checkRotationFlag();
	if(buttons!= 0 || encodermsg != 0 || songSelected)
	{
		setPeriodAndCNT(timeOut, TIMER_MS2TICKS(10000));
	}
}
static void showVolumen()
{
	char msg[16] = {' ',' ','V','o','l','u','m','e','n',':',' ',' ',' ',' ',' ',' '}; 
	msg[11] = (char)volumen/10 + '0';
	msg[12] = (char)volumen%10 + '0';
	showMSG(msg,' ',true);
}

static void goToSleep()
{
	currentState = GO_TO_SLEEP;
}
static void animationMatrix(void)
{
	static uint8_t auxCount = 0;
	static uint8_t redRow[8][3] = {RED,RED,RED,RED,RED,RED,RED,RED};
	static uint8_t blueRow[8][3] ={BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE};
	static uint8_t greenRow[8][3] ={GREEN,GREEN,GREEN,GREEN,GREEN,GREEN,GREEN,GREEN};
	static uint8_t no_colorRow[8][3] ={NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR};
	static uint8_t yellowRow[8][3] ={YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW};
	static uint8_t whiteRow[8][3] ={WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE};
	switch (auxCount)
	{
		case 0:
		{
			DMAChangeRow(whiteRow,0,15,false);
			DMAChangeRow(no_colorRow,1,15,false);
			DMAChangeRow(no_colorRow,2,15,false);
			DMAChangeRow(no_colorRow,3,15,false);
			DMAChangeRow(no_colorRow,4,15,false);
			DMAChangeRow(no_colorRow,5,15,false);
			DMAChangeRow(no_colorRow,6,15,false);
			DMAChangeRow(no_colorRow,7,15,true);
		}
		break;
		case 1:
		{
			DMAChangeRow(redRow,0,15,false);
			DMAChangeRow(whiteRow,1,15,true);
		}
		break;
		case 2:
		{
			DMAChangeRow(greenRow,0,15,false);
			DMAChangeRow(redRow,1,15,false);
			DMAChangeRow(whiteRow,2,15,true);
		}
		break;
		case 3:
		{
			DMAChangeRow(blueRow,0,15,false);
			DMAChangeRow(greenRow,1,15,false);
			DMAChangeRow(redRow,2,15,false);
			DMAChangeRow(whiteRow,3,15,true);
		}
		break;
		case 4:
		{
			DMAChangeRow(blueRow,0,15,false);
			DMAChangeRow(greenRow,1,15,false);
			DMAChangeRow(redRow,2,15,false);
			DMAChangeRow(whiteRow,3,15,true);
		}
		break;
		case 5:
		{
			DMAChangeRow(whiteRow,0,15,false);
			DMAChangeRow(blueRow,1,15,false);
			DMAChangeRow(greenRow,2,15,false);
			DMAChangeRow(redRow,3,15,false);
			DMAChangeRow(whiteRow,4,15,true);
		}
		break;
		case 6:
		{
			DMAChangeRow(redRow,0,15,false);
			DMAChangeRow(whiteRow,1,15,false);
			DMAChangeRow(blueRow,2,15,false);
			DMAChangeRow(greenRow,3,15,false);
			DMAChangeRow(redRow,4,15,false);
			DMAChangeRow(whiteRow,5,15,true);
		}
		break;
		case 7:
		{
			DMAChangeRow(greenRow,0,15,false);
			DMAChangeRow(redRow,1,15,false);
			DMAChangeRow(whiteRow,2,15,false);
			DMAChangeRow(blueRow,3,15,false);
			DMAChangeRow(greenRow,4,15,false);
			DMAChangeRow(redRow,5,15,false);
			DMAChangeRow(whiteRow,6,15,true);
		}
		break;
		case 8:
		{
			DMAChangeRow(blueRow,0,15,false);
			DMAChangeRow(greenRow,1,15,false);
			DMAChangeRow(redRow,2,15,false);
			DMAChangeRow(whiteRow,3,15,false);
			DMAChangeRow(blueRow,4,15,false);
			DMAChangeRow(greenRow,5,15,false);
			DMAChangeRow(redRow,6,15,false);
			DMAChangeRow(whiteRow,7,15,true);
		}
		break;
		case 9:
		{
			DMAChangeRow(blueRow,1,15,false);
			DMAChangeRow(greenRow,2,15,false);
			DMAChangeRow(redRow,3,15,false);
			DMAChangeRow(whiteRow,4,15,false);
			DMAChangeRow(blueRow,5,15,false);
			DMAChangeRow(greenRow,6,15,false);
			DMAChangeRow(redRow,7,15,false);
			DMAChangeRow(whiteRow,0,15,true);
		}
		break;
		case 10:
		{
			DMAChangeRow(blueRow,2,15,false);
			DMAChangeRow(greenRow,3,15,false);
			DMAChangeRow(redRow,4,15,false);
			DMAChangeRow(whiteRow,5,15,false);
			DMAChangeRow(blueRow,6,15,false);
			DMAChangeRow(greenRow,7,15,false);
			DMAChangeRow(redRow,0,15,false);
			DMAChangeRow(whiteRow,1,15,true);
		}
		break;
		case 11:
		{
			DMAChangeRow(blueRow,3,15,false);
			DMAChangeRow(greenRow,4,15,false);
			DMAChangeRow(redRow,5,15,false);
			DMAChangeRow(whiteRow,6,15,false);
			DMAChangeRow(blueRow,7,15,false);
			DMAChangeRow(greenRow,0,15,false);
			DMAChangeRow(redRow,1,15,false);
			DMAChangeRow(whiteRow,2,15,true);
			auxCount = 7;
		}
		break;	
	}
	auxCount++;
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
    fftGroups(inputFFT, outputFFT, samples, ifftFlag, volumen);

    uint8_t matrix[64][3];

    for (uint8_t column = 0; column < SPECTRUM_GROUPS; column++)
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
    }

    DMAChangeAllMatrix(matrix, 20);

}

static void hms2str(char* arr)
{
	arr[0] = ' ';
	arr[1] = ' ';
	arr[2] = ' ';
	arr[3] = ' ';
	arr[4] = (hours/10)%10 + '0';
	arr[5] = hours%10 + '0';
	arr[6] = ':';
	arr[7] = (mins/10)%10 + '0';
	arr[8] = mins%10 + '0';
	arr[9] = ':';
	arr[10] = (seconds/10)%10 + '0';
	arr[11] = seconds%10 + '0';
	arr[12] = ' ';
	arr[13] = ' ';
	arr[14] = ' ';
	arr[15] = ' ';
}
*/
