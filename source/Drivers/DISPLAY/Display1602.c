
/*****************************************************************************
  @file     Display1602.c
  @brief    Display1602
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <resources/timers/timer.h>
#include "../I2C/i2c.h"
#include "Display1602.h"
#include "../../resources/ringbuffer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define RBSIZE 512
#define REFRESH_MS 60
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void pulseEnable(uint8_t msg);
static void splitData(uint8_t aux,uint8_t config);
static void refreshScreen();
static bool pushTransaction(uint8_t data);
static void sendText(uint8_t text);
static void sendCommand(uint8_t command);
static void homeDisp();
static void clearRow(uint8_t row);
static void delay();
static void setCursor(uint8_t a, uint8_t b);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static tim_id_t timerId;
static i2c_transfer_t i2cTransfer;
static ring_buffer_t ring_buffer;
static char buf_arr[RBSIZE];
static uint8_t modes_a[1] = {WRITE};
static uint16_t numBytes[1] = {1};
static uint16_t cantTransactions = 0;
static uint8_t text[2][16];
static bool changeText = false;

static uint8_t __backlightval, __displayfunction,__displaycontrol,__displaymode ;
/*******************************************************************************
 *******************************************************************************
 *                      GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void Disp1602Init(void)
{

	i2c_cfg_t configuration = {MASTER, 0x1D};
	/**********Initial config************/
	 /* Create and initialize ring buffer */
	ring_buffer_init(&ring_buffer, buf_arr, sizeof(buf_arr));
	timerId=timerGetId();
	i2cInit(0, configuration);

	__displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x10DOTS;
	__backlightval = LCD_BACKLIGHT;

	i2cTransfer.mode = modes_a;
	i2cTransfer.cantBytes =numBytes;
	i2cTransfer.cantIterations = 1;
	i2cTransfer.slaveAddress = 0x27;
	/************************************/
	//4-bits mode.
	clearRow(0);
	clearRow(1);
	pushTransaction(0x30);

	timerDelay(TIMER_MS2TICKS(25));

	pushTransaction(0x30);

	timerDelay(TIMER_MS2TICKS(20));

	pushTransaction(0x30);
	timerDelay(TIMER_MS2TICKS(15));

	pushTransaction(0x20);
	sendCommand(LCD_FUNCTIONSET | __displayfunction);

	__displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	sendCommand(LCD_DISPLAYCONTROL|__displaycontrol);


	// Initialize to default text direction (for roman languages)
	__displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	sendCommand(LCD_ENTRYMODESET | __displaymode);


	clearDisp();

	homeDisp();

	timerDelay(TIMER_MS2TICKS(50));
	timerStart(timerId, TIMER_MS2TICKS(REFRESH_MS), TIM_MODE_PERIODIC, refreshScreen);
	timerDelay(TIMER_MS2TICKS(100));

}

void writeLine(char* in, uint8_t line){
	timerStop(timerId);
	int i;
	for(i=0; i<16 && in[i]!='\0'; i++)
	{
		text[line][i] = in[i];
	}
	for(int j=i; j<16; j++)
	{
		text[line][j] = ' ';
	}
	changeText = true;
	timerResume(timerId);
}
void writeChar(char a, uint8_t row, uint8_t col)
{
	text[row][col] = a;
	changeText = true;
}
void displayOff(void)
{
	__displaycontrol = LCD_DISPLAYOFF;
	__backlightval = LCD_NOBACKLIGHT;
	sendCommand(LCD_DISPLAYCONTROL | __displaycontrol);
	refreshScreen();
	timerStop(timerId);
}
void displayOn(void)
{
	__displaycontrol = LCD_DISPLAYON;
	__backlightval = LCD_BACKLIGHT;
	timerResume(timerId);
	sendCommand(LCD_DISPLAYCONTROL | __displaycontrol);
	sendCommand(LCD_ENTRYMODESET | __displaymode);
}
/*******************************************************************************
 *******************************************************************************
 *                      LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static bool pushTransaction(uint8_t data)
{
	ring_buffer_queue(&ring_buffer,data);
	cantTransactions++;
	pulseEnable(data);
	return true;

}
void clearDisp()
{
	clearRow(0);
	clearRow(1);
	sendCommand(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delay();
	delay();
	delay();
}
static void homeDisp()
{
	sendCommand(LCD_RETURNHOME);// clear display, set cursor position to zero
	delay();
	delay();
	delay();
}
void cursorOn()
{
	__displaycontrol |= LCD_CURSORON;
	sendCommand(LCD_DISPLAYCONTROL | __displaycontrol);
}
void blinkOff() {
	__displaycontrol &= ~LCD_BLINKON;
	sendCommand(LCD_DISPLAYCONTROL | __displaycontrol);
}
void blinkOn() {
	__displaycontrol |= LCD_BLINKON;
	sendCommand(LCD_DISPLAYCONTROL | __displaycontrol);
}
static void delay()
{
	ring_buffer_queue(&ring_buffer,LCD_BACKLIGHT);
	cantTransactions++;
	pulseEnable(LCD_BACKLIGHT);
}
static void clearRow(uint8_t row)
{
	for(int i =0 ; i< 16; i++)
	{
		text[row][i] = ' ';
	}
}
static void setCursor(uint8_t a, uint8_t b){
    uint8_t rowOffset = a? 0x40: 0x00;
    splitData(LCD_SETDDRAMADDR | (b + rowOffset), LCD_BACKLIGHT);
}
/****************************************************/
/************************ Back **********************/
/****************************************************/
static void refreshScreen(){
	if(changeText)
	{
		changeText =false;
		setCursor(0,0);
		for(int i = 0; i<16; i++)
		{
			sendText(text[0][i]);
		}
		setCursor(1,0);
		for(int i = 0; i<16; i++)
		{
			sendText(text[1][i]);
		}
	}
	if(i2cGetFlagEndTransaction() && cantTransactions)
	{
		ring_buffer_dequeue_arr(&ring_buffer,(char *)i2cTransfer.data2BeSend[0],cantTransactions);
		i2cTransfer.cantBytes[0] = cantTransactions;
		cantTransactions = 0;
		i2cStartTransaction(0,i2cTransfer);
	}
}
static void splitData(uint8_t aux,uint8_t config)
{
	uint8_t tmp = (aux & 0xF0)|config | __backlightval;
	pushTransaction(tmp);
	tmp = ((aux<<4) & 0xF0) |config | __backlightval;
	pushTransaction(tmp);
}
static void pulseEnable(uint8_t msg){
	uint8_t tmp = msg | En;
	ring_buffer_queue(&ring_buffer,tmp);
	cantTransactions++;
	tmp = msg & (~En);
	ring_buffer_queue(&ring_buffer,tmp);
	cantTransactions++;
}
static void sendText(uint8_t text)
{
	splitData(text, Rs);
}
static void sendCommand(uint8_t command){
	splitData(command, 0);
}
/*******************************************************************************
 ******************************************************************************/
