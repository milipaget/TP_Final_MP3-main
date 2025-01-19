/*
 * gpio.c
 *
 *  Created on: Aug 12, 2022
 *      Author: Grupo 1
 */
#include "stdio.h"
#include "gpio.h"



static void setDigitalFilterState(pin_t pin, bool value);
static void configureDigitalFilterClock (pin_t pin, bool clockInput);
static void setDigitalFilterLength(pin_t pin, uint8_t ticks);


GPIO_Type*  baseGPIOPtrs[] = {	GPIOA,
								GPIOB,
								GPIOC,
								GPIOD,
								GPIOE
							};
PORT_Type* basePORTPtrs[] = {	PORTA,
								PORTB,
								PORTC,
								PORTD,
								PORTE
							};

uint8_t PORTIRQEnb[] =PORT_IRQS;

pinIrqFun_t callbacks[5][32] ={{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
							{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
							{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
							{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
							{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
								NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
							} ;


//Arreglo con mascaras para clock gating de puerto

uint32_t SIM_SCGC5_MASKS[5] = {SIM_SCGC5_PORTA_MASK ,
							   SIM_SCGC5_PORTB_MASK ,
							   SIM_SCGC5_PORTC_MASK ,
							   SIM_SCGC5_PORTD_MASK ,
							   SIM_SCGC5_PORTE_MASK};


void gpioMode(pin_t pin, uint8_t mode)
{
	// Basicamente vamos a seguir el esquema PORT | GPIO y de afuera a adentro vamos a ir seteando las cosas
	//Para referencia ver ppt de dani GPIO diapo 2

	/************************************
	 *     CLOCK GATING ACTIVATION		*/
	/************************************/
	//Vamos a habilitar solo puertos A, B, C, D o E. Por lo que solo nos interesa el registro 5 SCGC5
	SIM->SCGC5 |= SIM_SCGC5_MASKS[PIN2PORT(pin)];


	basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] &= ~((uint32_t)0b111 << PORT_PCR_MUX_SHIFT); //MUX CLEAR
	basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] |= 1 << PORT_PCR_MUX_SHIFT; //MUX SET TO GPIO PIN

	switch (mode)
	{
		case INPUT:
			{
				basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] &= ~((uint32_t)0b10); //Disabling pull enable.
				baseGPIOPtrs[PIN2PORT(pin)]->PDDR &= ~(1 << PIN2NUM(pin));
			}
			break;
		case OUTPUT:
			{
				baseGPIOPtrs[PIN2PORT(pin)]->PDDR |= 1 << PIN2NUM(pin);
			}
			break;
		case INPUT_PULLUP:
			{
				basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] |= ((uint32_t)0b11);
				baseGPIOPtrs[PIN2PORT(pin)]->PDDR &= ~((uint32_t)1 << PIN2NUM(pin));

			}
		break;
		case INPUT_PULLDOWN:
			{
				basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] |= ((uint32_t)0b10);
				basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] &= ~((uint32_t)0b1);
				baseGPIOPtrs[PIN2PORT(pin)]->PDDR &= ~((uint32_t)1 << PIN2NUM(pin));
			}
			break;
	}


}

void gpioWrite (pin_t pin, bool value)
{
    //Para escribir tengo que primero validar si el pin es efectivamente un output
    //Para eso es simplemente leer el bit de output (PDDR) del PCR --> PDDR = 1 --> OUTPUT
    if(baseGPIOPtrs[PIN2PORT(pin)]->PDDR & (1<<PIN2NUM(pin)))  // si el bit de output esta en 1
    {
        //Port Data Output Register --> ?? No lo toco al parecer consultar

        //Port Set Output Register PSOR
            // 0 no importa
            // 1 pone 1 a la salida
        //Port Clear Output Register PCOR
            // 0 no importa
            // 1 pone 0 a la salida (anti intuitivo xd)

        switch(value)
        {
        case HIGH:
        	baseGPIOPtrs[PIN2PORT(pin)]->PSOR |= (1 << PIN2NUM(pin));        //Pongo un 1 en el bit de PSOR
            break;

        case LOW:
        	baseGPIOPtrs[PIN2PORT(pin)]->PCOR |= (1 << PIN2NUM(pin));    //Pongo un 1 en el bit de PCOR
            break;
        }
    }
}

void gpioToggle (pin_t pin)
{
	baseGPIOPtrs[PIN2PORT(pin)]->PTOR |= (1 << PIN2NUM(pin));
}

bool gpioRead (pin_t pin)
{
	return baseGPIOPtrs[PIN2PORT(pin)]->PDIR & (1 << PIN2NUM(pin));
}

bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun)
{
	uint32_t interruptMode = 0;
	switch(PIN2PORT(pin))
	{
		case PA:NVIC_EnableIRQ(PORTA_IRQn);break;
		case PB:NVIC_EnableIRQ(PORTB_IRQn);break;
		case PC:NVIC_EnableIRQ(PORTC_IRQn);break;
		case PD:NVIC_EnableIRQ(PORTD_IRQn);break;
		case PE:NVIC_EnableIRQ(PORTE_IRQn);break;
		default: return false;
	}

	if(baseGPIOPtrs[PIN2PORT(pin)]->PDDR & (1 << PIN2NUM(pin))) // validacion.
	{
		return false;
	}
	if(!irqFun)
	{
		return false;
	}

	//Setting up
	basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] &= ~((uint32_t)0b1111<<PORT_PCR_IRQC_SHIFT); //Clear IRQC.


	switch(irqMode)
	{
		case GPIO_IRQ_MODE_DISABLE:
			// 0000
			interruptMode = (uint32_t)0b0000<<PORT_PCR_IRQC_SHIFT;   // [0000 1111 1111 1111] (es exrtra pero lo pongo por prolijidad)
			break;

		case GPIO_IRQ_MODE_RISING_EDGE:
			{
				interruptMode = (uint32_t)0b1001<<PORT_PCR_IRQC_SHIFT;
			}
			break;
		case GPIO_IRQ_MODE_FALLING_EDGE:
			{
				interruptMode = (uint32_t)0b1010<<PORT_PCR_IRQC_SHIFT;
			}
			break;
		case GPIO_IRQ_MODE_BOTH_EDGES:
			{
				interruptMode = (uint32_t)0b1011<<PORT_PCR_IRQC_SHIFT;
			}
			break;
		default:
		{
			return false;
		}
	}

	basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] |= interruptMode;
	callbacks[PIN2PORT(pin)][PIN2NUM(pin)] = irqFun;
	return true;
}

void clearFlag(pin_t pin)
{
	basePORTPtrs[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] |= 1<<PORT_PCR_ISF_SHIFT; // w1c
}
void InterruptHandler(int port)
{
	for (int i = 0; i < 32; i++ )
	{
		if (basePORTPtrs[port]->PCR[i] & (1<<PORT_PCR_ISF_SHIFT))
		{
			basePORTPtrs[port]->PCR[i] |= 1<<PORT_PCR_ISF_SHIFT;
			callbacks[port][i]();
			break;
		}
	}

}
void PORTA_IRQHandler(){InterruptHandler(PA);}
void PORTB_IRQHandler(){InterruptHandler(PB);}
void PORTC_IRQHandler(){InterruptHandler(PC);}
void PORTD_IRQHandler(){InterruptHandler(PD);}
void PORTE_IRQHandler(){InterruptHandler(PE);}


// *************************** digital filters *********************************

void setPinDigitalFilter(pin_t pin, bool value, bool clockInput, uint8_t ticks)
{
	setDigitalFilterState(pin, value);
	configureDigitalFilterClock(pin, clockInput);
	setDigitalFilterLength(pin, ticks);
}

static void setDigitalFilterState(pin_t pin, bool value)
{
	if (value)
		basePORTPtrs[PIN2PORT(pin)]->DFER |= (1 << PIN2NUM(pin));
	else
		basePORTPtrs[PIN2PORT(pin)]->DFER &= ~(1 << PIN2NUM(pin));

	basePORTPtrs[PIN2PORT(pin)]->DFCR |= 1;
}

static void configureDigitalFilterClock (pin_t pin, bool clockInput)
{
	uint32_t mask = basePORTPtrs[PIN2PORT(pin)]->DFER;

	//disable digital filters
	basePORTPtrs[PIN2PORT(pin)]->DFER = 0;

	if (clockInput)
		basePORTPtrs[PIN2PORT(pin)]->DFCR |= 1;
	else
		basePORTPtrs[PIN2PORT(pin)]->DFCR &= ~1;

	basePORTPtrs[PIN2PORT(pin)]->DFER = mask;
}

static void setDigitalFilterLength(pin_t pin, uint8_t ticks)
{
    uint32_t mask = basePORTPtrs[PIN2PORT(pin)]->DFER;

    //disable digital filters
    basePORTPtrs[PIN2PORT(pin)]->DFER = 0;

    //reset ticks
    basePORTPtrs[PIN2PORT(pin)]->DFWR &= ~(0b11111);

    if (ticks <= 0b11111)
        basePORTPtrs[PIN2PORT(pin)]->DFWR |= ticks;
    else
        basePORTPtrs[PIN2PORT(pin)]->DFWR |= 1;

    basePORTPtrs[PIN2PORT(pin)]->DFER = mask;
}
