/*******************************************************************************
   @file    eventQueue.h
   @brief   Event handler
   @author  Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

#ifndef _EVENTQUEUE_H_
#define _EVENTQUEUE_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

typedef enum event_type{
  None,
  volumeUP,
  volumeDOWN,
  playPause,
  prevSong,
  nextSong,
  onOff,
  changeFilter, 
  //AutoNextSong,
  SDCardOut,
  SDCardIn
} event_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Establece el siguiente elemento en la cola de eventos
 * @param void
 */
event_t getNextEvent(void);

/**
 * @brief Agrega un elemento a la cola de eventos
 * @param event_t evento a agregar
 */
void putEvent(event_t event);

/**
 * @brief Elimina un elemento de la cola de eventos
 * @param void
 */
event_t popEvent(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _EVENTQUEUE_H_
