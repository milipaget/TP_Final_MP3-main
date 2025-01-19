/*******************************************************************************
   @file    eventQueue.c
   @brief   Event handler
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "eventQueue.h"
#include <stdlib.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define EVENT_BUFFER_SIZE 64

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static event_t buffer[EVENT_BUFFER_SIZE + 1] = {0};
uint32_t head = 0;
uint32_t tail = 0;

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
event_t getNextEvent(void){
    if (tail == head)
        return None;
    event_t aux_event = buffer[head];
    head = (head == EVENT_BUFFER_SIZE - 1) ? 0 : head + 1;
    return aux_event;
}

event_t popEvent(void){
    if (tail == head)
        return None;
    tail = (tail == 0) ? EVENT_BUFFER_SIZE - 1 : tail - 1;
    return buffer[tail];
}

void putEvent(event_t evnt){
    buffer[tail] = evnt;
    tail = (tail == EVENT_BUFFER_SIZE - 1) ? 0 : tail + 1;
    if (tail == head){
        head = (head == EVENT_BUFFER_SIZE - 1) ? 0 : head + 1;
    }
}

/*******************************************************************************
 ******************************************************************************/

