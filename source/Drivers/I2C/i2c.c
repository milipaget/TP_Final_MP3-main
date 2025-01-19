/***************************************************************************//**
  @file     i2c.c
  @brief    driver i2c
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "hardware.h"
#include "i2c.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//#define MEASURE_IRQ_TIME 1

#define I2C0_SCL PORTNUM2PIN(PE,24)
#define I2C0_SDA PORTNUM2PIN(PE,25)

#define I2C_INTERRUPT PORTNUM2PIN(PD,1)

#define I2C1_SCL PORTNUM2PIN(PC,10)
#define I2C1_SDA PORTNUM2PIN(PC,11)

//Hacemos mascaras para PCR --> MUX = (5  o 2) + ODE = 1  + PE = 1 + PS = 1

#define PCR_CONFIG_I2C0 0b10100100011
#define PCR_CONFIG_I2C1 0b01000100011

#define MULT_F 			0b00

//hablar con address que no este --> no akw

//hablar con acelerometro --> akw --> 0x1D

void I2CX_IRQHandler(uint8_t id);
static void i2cStart(uint8_t id);

/*********MAX defines**********/

//Module clock --> 50MHz --> Ref: Pagina 3 user manual (y medido)
typedef enum{
    I2C_SPEED_909091HZ = 0x10,	//max


//    I2C_SPEED_97656HZ = 0x17,
//    I2C_SPEED_78125HZ = 0x1D,
//    I2C_SPEED_65104HZ = 0x1E,
//    I2C_SPEED_48828HZ = 0x23,
//    I2C_SPEED_32522HZ = 0x26,
//    I2C_SPEED_24414HZ = 0x2B,
//    I2C_SPEED_16276HZ = 0x2E,
//    I2C_SPEED_12207HZ = 0x33,
//    I2C_SPEED_8138HZ = 0x39,
//    I2C_SPEED_4069HZ = 0x3E,


    I2C_SPEED_13115HZ = 0x3F	//min
}i2c_speed_t;

#define TIME_MASK 0x1D// por ahora, despues va a ser variable
/*******************************/

/*******General purpose defines*****/

/*******************************/

/**************Typedef and enum***************/

/****************************************/


/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
I2C_Type* i2cs[I2C_CANT_IDS] = {
								I2C0,
								I2C1,
								I2C2
								};


//static uint8_t cantBytes2BeSend_total = {};
static uint8_t cantBytes[10];
static uint8_t data2BeSend[10][254];	// se puede hacer mas prolijo
static uint8_t numIteration = 0;
static uint8_t cantIterations = 0;
static uint8_t data2BeRead_Buffer[254];
static uint8_t bytes2BeRead = 0;
static uint16_t numBytesRead = 0;
static uint16_t numBytesWrite = 0;
static uint8_t slaveAddress = 0;
static bool slaveAddressEnd = true;
static uint8_t modes[10];  //
static bool endTransaction = true;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void i2cInit(uint8_t id, i2c_cfg_t config)
{
	switch(id)
	{
		case 0:
		{
			SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK; //enable the clock gating for I2C0
			SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //enable the clock gating for PORTE

			NVIC_EnableIRQ(I2C0_IRQn);

			PORTE->PCR[PIN2NUM(I2C0_SDA)] = PCR_CONFIG_I2C0;
			PORTE->PCR[PIN2NUM(I2C0_SCL)] = PCR_CONFIG_I2C0;
		}break;
		case 1:
		{
			SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK; //enable the clock gating for I2C1
			NVIC_EnableIRQ(I2C1_IRQn);

			PORTE->PCR[PIN2NUM(I2C1_SDA)] = PCR_CONFIG_I2C1;
			PORTE->PCR[PIN2NUM(I2C1_SCL)] = PCR_CONFIG_I2C1;
		}break;
		case 2:
		{
			SIM->SCGC1 |= SIM_SCGC1_I2C2_MASK; //enable the clock gating for I2C2
			NVIC_EnableIRQ(I2C2_IRQn);
		}break;
	}
	//Habilitar pin para medir interrupcion
	gpioMode(I2C_INTERRUPT,OUTPUT);

	// Configuracion
	switch(config.role)
	{
		case SLAVE:
		{
			return;
		}break;

		case MASTER:
		{
		//Configure clock
		i2cs[id]->F = ((MULT_F << I2C_F_MULT_SHIFT) | TIME_MASK);

		//Enable I2C
		i2cs[id]->C1 = (I2C_C1_IICEN_MASK);

		//Clear STARTF and STOPF flags
		i2cs[id]->FLT |= (I2C_FLT_STARTF_MASK | I2C_FLT_STOPF_MASK);

		//Clear CTF complete transfer flag
		uint8_t dummydata = i2cs[id]->D;

		}break;
	}
}

void i2cStartTransaction(uint8_t id, i2c_transfer_t transfer)
{
	if(endTransaction)
	{
		numIteration = 0;
		i2cs[id]->C1 |= I2C_C1_IICIE_MASK; // interruptions
		i2cs[id]->FLT |= (I2C_FLT_STARTF_MASK | I2C_FLT_STOPF_MASK);
		i2cs[id]->C1 &= ~(I2C_C1_RSTA_MASK);		//Limpio flag de repeated start

		//Clear CTF complete transfer flag
		uint8_t dummydata = i2cs[id]->D;

		slaveAddress = transfer.slaveAddress;
		cantIterations = transfer.cantIterations;
		//modes = transfer.mode;
		endTransaction = false;
		for (int i = 0; i < cantIterations; i++)
		{
			modes[i] = transfer.mode[i];
			cantBytes[i] = transfer.cantBytes[i];
			for (int j = 0; j < cantBytes[i]; j++)
			{
				data2BeSend[i][j] = transfer.data2BeSend[i][j];
			}
		}
		i2cStart(id);
	}
}

static void i2cStart(uint8_t id)
{
	slaveAddressEnd = false;
	numBytesRead = 0;
	numBytesWrite = 0;
	i2cs[id]->C1 |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK);
	i2cs[id]->D = (slaveAddress << 1) | modes[numIteration]; // Luego hay que checkear lo del mode para concatenacion.
}

bool i2cGetFlagEndTransaction(void)
{
	return endTransaction;
}
uint8_t i2cGetDataFromTransfer(uint8_t* data,uint8_t cant_Bytes)
{
	if(cant_Bytes <= bytes2BeRead)
	{
		uint8_t output = bytes2BeRead - cant_Bytes;
		memcpy(data,data2BeRead_Buffer,cant_Bytes);
		bytes2BeRead = 0;
		return output;
	}
	else
	{
		return 0xFF;
	}

}
/*******************************************************************************
 ******************************************************************************/

void I2CX_IRQHandler(uint8_t id)
{
#ifdef MEASURE_IRQ_TIME
	gpioWrite(I2C_INTERRUPT,HIGH);
#endif
	//if(rs)
	i2cs[id]->S |= I2C_S_IICIF_MASK; // importante, porque te dice explicitamente que hay que limpiar por software.

	if(i2cs[id]->C1 & I2C_C1_MST_MASK) //master mode.
	{
		if(i2cs[id]->C1 & I2C_C1_TX_MASK)	// write
		{
			if(cantBytes[numIteration] && slaveAddressEnd)
			{
				cantBytes[numIteration]--;
				i2cs[id]->D = data2BeSend[numIteration][numBytesWrite++];
#ifdef MEASURE_IRQ_TIME
				gpioWrite(I2C_INTERRUPT,LOW);
#endif
				return;
			}
			else if(!slaveAddressEnd)
			{
				slaveAddressEnd = true;
				if (!(i2cs[id]->S & I2C_S_RXAK_MASK))
				{
					if(modes[numIteration] == 1) //Luego ampliar aqui
					{
						i2cs[id]->C1 &= ~(I2C_C1_TX_MASK); // Rx mode. No estoy seguro, porque hay que tener en cuenta el mode me parece.When in master receive mode and there is only 1 byte to be received, TXACK should be set before dummy read.
						if(cantBytes[numIteration] == 1)
						{
							i2cs[id]->C1 |= (I2C_C1_TXAK_MASK); // Note 3.
						}
						else
						{
							i2cs[id]->C1 &= ~(I2C_C1_TXAK_MASK);
						}
						uint8_t dummyRead = i2cs[id]->D;
#ifdef MEASURE_IRQ_TIME
						gpioWrite(I2C_INTERRUPT,LOW);
#endif
						return;
					}
					else
					{
						cantBytes[numIteration]--;
						i2cs[id]->D = data2BeSend[numIteration][numBytesWrite++];
#ifdef MEASURE_IRQ_TIME
						gpioWrite(I2C_INTERRUPT,LOW);
#endif
						return;
					}
				}
				else
				{
					if (numIteration == (cantIterations - 1))
					{
						i2cs[id]->C1 &= ~I2C_C1_MST_MASK; // Para el stop.
						endTransaction = true;
						//i2cs[id]->C1 &= ~(I2C_C1_IICIE_MASK);
					}
					else
					{
						i2cs[id]->C1 |= I2C_C1_RSTA_MASK; // Para el repeated start.
						numIteration++;
						i2cStart(id);
					}
#ifdef MEASURE_IRQ_TIME
					gpioWrite(I2C_INTERRUPT,LOW);
#endif
					return;
				}
			}
			else
			{
				if (numIteration == (cantIterations - 1))
				{
					i2cs[id]->C1 &= ~I2C_C1_MST_MASK; // Para el stop.
					endTransaction = true;
					//i2cs[id]->C1 &= ~(I2C_C1_IICIE_MASK);
				}
				else
				{
					i2cs[id]->C1 |= I2C_C1_RSTA_MASK; // Para el repeated start.
					numIteration++;
					i2cStart(id);
				}
#ifdef MEASURE_IRQ_TIME
				gpioWrite(I2C_INTERRUPT,LOW);
#endif
				return;
			}
		}
		else  //Rx Mode
		{
			cantBytes[numIteration]--;
			if(!cantBytes[numIteration]) // si es 0
			{
				if (numIteration == (cantIterations - 1))
				{
					i2cs[id]->C1 &= ~I2C_C1_MST_MASK; // Para el stop.
					endTransaction = true;
					//i2cs[id]->C1 &= ~(I2C_C1_IICIE_MASK);
				}
				else
				{
					i2cs[id]->C1 |= I2C_C1_RSTA_MASK; // Para el repeated start.
					numIteration++;
					i2cStart(id);
				}
			}
			else if (cantBytes[numIteration] == 1) //Ante ultimo.
			{
				i2cs[id]->C1 |= I2C_C1_TXAK_MASK;
			}
			bytes2BeRead++;
			data2BeRead_Buffer[numBytesRead++] = i2cs[id]->D;
#ifdef MEASURE_IRQ_TIME
			gpioWrite(I2C_INTERRUPT,LOW);
#endif
			return; //Por el momento
		}
	}
	else //slaveMode
	{
		i2cs[id]->C1 &= ~(I2C_C1_MST_MASK);
#ifdef MEASURE_IRQ_TIME
		gpioWrite(I2C_INTERRUPT,LOW);
#endif
		return;
	}
#ifdef MEASURE_IRQ_TIME
	gpioWrite(I2C_INTERRUPT,LOW);
#endif
}

__ISR__ I2C0_IRQHandler (void){I2CX_IRQHandler(0);}
__ISR__ I2C1_IRQHandler (void){I2CX_IRQHandler(1);}
__ISR__ I2C2_IRQHandler (void){I2CX_IRQHandler(2);}
