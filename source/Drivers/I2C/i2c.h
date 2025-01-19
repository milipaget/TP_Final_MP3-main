/***************************************************************************//**
  @file     i2c.h
  @brief   	I2C Driver for K64F.
  @author   TEAM OREO
 ******************************************************************************/

#ifndef I2C_H_
#define I2C_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include "MK64F12.h"
#include "core_cm4.h"
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define I2C_CANT_IDS   3

#define __MODULE_CLOCK__ 1000000U

#define SLAVE 0
#define MASTER 1

#define READ 1
#define WRITE 0
#define MAX_CONCATENATE_MODES 10

#define I2C_OK 1
#define I2C_ERROR 0
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct {
    uint8_t role;
    uint16_t baudrate;
    //bool parity;  //false apagado
    //bool parityType; //0-> even; 1->odd.
    //uint8_t systickTicks;
    //uint8_t length_parity
} i2c_cfg_t;

typedef struct	// todos los punteros corresponden a arreglos de largo cantIterations
{
	uint8_t *mode;
	uint16_t *cantBytes;
	uint8_t data2BeSend[10][1000];
	uint8_t slaveAddress;
	uint8_t cantIterations;
}i2c_transfer_t;

enum{ IDLE, CONTACTING_SLAVE , WRITING_SLAVE , LISTENING_SLAVE };

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialize I2C driver
 * @param id
 * @param role
*/
void i2cInit(uint8_t id, i2c_cfg_t config);

void i2cStartTransaction(uint8_t id, i2c_transfer_t transfer);
//void i2cMasterSaidSomething(uint8_t id, bool mode, uint8_t slaveAddress, uint8_t cantBytes);

void i2cListeningSlave(); //F2

void i2cWritingSlave(uint8_t id, uint8_t* slaveData, uint8_t slaveAddress, uint8_t cantBytes); //F1

bool i2cGetFlagEndTransaction(void);

uint8_t i2cGetDataFromTransfer(uint8_t* data,uint8_t cant_Bytes);

#endif /* I2C_H_ */
