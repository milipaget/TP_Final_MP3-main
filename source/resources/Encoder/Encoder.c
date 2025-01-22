#include "Encoder.h"        // :)
#include  "hardware.h"
#include    "../timers/timer.h"       // Para poder controlar los timers: crearlos y configurarlos...
#include    "../../gpio.h"        // Configuración de los gpio: qué pin y en qué modo...
#include    "../../pinout.h"      // Configuración relqacionada a la placa -> qué pines de qué puerto usa
#include    <stdlib.h>         // :)
#include "..\eventos\eventQueue.h"     // Para poner el evento en la cola


// Estados activos para las señales del encoder
#define ROTARY_ACTIVE LOW  // Estado activo para las señales del encoder

// Constantes de tiempo para el manejo del encoder
#define DEBOUNCE_DELAY 10  // Tiempo para evitar rebotes (en ms... se multiplica en función de time)
#define SWITCH_SAMPLING_TIME 10
#define DOUBLE_CLICK_TIME 350
#define ENCODER_EVENT_DELAY 100  // Tiempo para procesar eventos del encoder (en ms)
#define MAX_LONG_CLICK_TIME 10000
#define LONG_CLICK_TIME 2000

// Variable para ignorar entradas durante el tiempo de 
static bool ignoreRotaryInput = false;

// ID del temporizador asociado al encoder
static uint8_t rotaryTimerID;

// Declaración de funciones
static void encoderACallback(void);  // Callback para el canal A
static void rebootEncoderDetection(void);  // Rehabilita detección tras temporizador

// Inicializa el encoder rotativo
bool initEncoder(void) {

    // Configurar pines del encoder como entradas
    gpioMode(PORT_ENCODER_RCHA, INPUT);
    gpioMode(PORT_ENCODER_RCHB, INPUT);

    // Asignar callbacks a las interrupciones de los pines
    if (gpioIRQ(PORT_ENCODER_RCHA, GPIO_IRQ_MODE_FALLING_EDGE, &encoderACallback)) {
        return true; // Error al configurar interrupción
    }
    /*if (gpioIRQ(PORT_ENCODER_RCHB, GPIO_IRQ_MODE_FALLING_EDGE, &encoderBCallback)) {
        return true; // Error al configurar interrupción
    }*/

    // Crear temporizador para manejar eventos del encoder: ONESHOT -> Al terminar este tiempo activa el encoder de nuevo
    rotaryTimerID = timerGetId();//createTimer(ENCODER_EVENT_DELAY, &rebootEncoderDetection, TIM_MODE_SINGLESHOT);
    timerStart(rotaryTimerID, TIMER_MS2TICKS(100), TIM_MODE_SINGLESHOT, rebootEncoderDetection);//

     return false; // Inicialización exitosaTIM_MODE_SINGLESHOT
}

// Callback para el canal A del encoder
static void encoderACallback(void) {
	bool statusB = gpioRead(PORT_ENCODER_RCHB);
    // Ignorar entrada si está bloqueada o el pin A no está activo
    if (ignoreRotaryInput || gpioRead(PORT_ENCODER_RCHA) != ROTARY_ACTIVE) {
        return;
    }

    // Verificar estado del canal B para determinar el sentido -> Si gira en sentido antihorario B se prende antes que A
    if (statusB != ROTARY_ACTIVE) {
        // Movimiento antihorario detectado
        putEvent(volumeDOWN);  // Registrar evento de giro antihorario
    }
    else if(statusB == ROTARY_ACTIVE) {
        // Movimiento antihorario detectado
        putEvent(volumeUP);  // Registrar evento de giro antihorario
    }

    // Bloquear nuevas entradas y activar temporizador
    ignoreRotaryInput = true;
    timerStart(rotaryTimerID, TIMER_MS2TICKS(100), TIM_MODE_SINGLESHOT, rebootEncoderDetection);  // Se detectó recién que se prendió el A, no me fijo por un ratito
}

// Rehabilita la detección de entradas del encoder
static void rebootEncoderDetection(void) {
    ignoreRotaryInput = false; // Permitir nuevas interrupciones :) 
}

