#include "FTM.h"        // Algo de frecuencia
#include "matrixLED.h"   // :)
#include "../timers/timerPIT.h"        // Algo de los timers
#include "PWM.h"        // Para mandar a la matriz
#include "MK64F12.h"    // De la plaquitax
#include "../../pinout.h"     // Configura qué pines se usan para x cosa

// En config.h  #define DIN_PIN PORTNUM2PIN(PB, 9) // GPIO to transfer leds data


// Constantes internas para temporización y control del PWM
#define TICKS_PER_PERIOD (63)       // 50MHz * 1.25us Es del PWM
#define TIME_HIGH_BIT_0 (20)        // PWM
#define TIME_HIGH_BIT_1 (40)        // PWM
#define REFRESH_PERIOD_TICKS (2772) // 44 ticks * 1.25us = 55us

// Rojo (R), Verde (G) y Azul (B), cada uno con 8 bits de datos (28 bits por led en total, 256 niveles de brillo).
#define LED_BITS_PER_PIXEL (24)
#define TOTAL_LEDS (MATRIX_ROWS * MATRIX_COLS)      // Total de leds en la matriz

#define REFRESH_RATE_HZ (41666.0f) // 24Hz
#define DEFAULT_BLINK_INTERVAL_MS (1000000.0f) // 1s

#define SCALE_COLOR(color, brightness) ((int)((color) * (brightness) / 100))
#define LED_ON (1)
#define LED_OFF (0)

typedef enum {GREEN_STATE, RED_STATE, BLUE_STATE} color_states_t;

// Estructura para representar el estado de un LED
typedef struct {
    LED_Color_t color;
    uint8_t isOn : 1;
    uint8_t isBlinking : 1;
} LED_State_t;

// Matriz que tiene la información de los leds
static LED_State_t ledMatrix[TOTAL_LEDS] = {0};
// Matriz que tiene todos los bits que voy a tener que mandar para representar la matriz
static uint16_t pwmBuffer[TOTAL_LEDS * LED_BITS_PER_PIXEL + 2] = {0};

// Color default de los LEDs
static LED_Color_t defaultColorLED;


// Variables de funcionamiento
static uint8_t ledBrightness = MAX_BRIGHTNESS / 6;
static uint8_t refreshTimerId = 0;
static uint8_t blinkTimerId = 0;

// Declaraciones de funciones internas
static void updateLEDMatrix(void);
static void refreshMatrix(void);
static void handleRefreshEnd(void);
static void toggleBlinkingLEDs(void);

// Inicializa la matriz con todos los leds en defaultColorLED
void initializeLEDMatrix(void) {

    defaultColorLED.hexColor = COLOR_ORANGE;
    defaultColorLED.brightness = ledBrightness;

    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            setLEDColor(row, col, defaultColorLED);
        }
    }

    // Inicializa matriz
    updateLEDMatrix();

    // Inicializa lo relacionado a PWM
    //initPIT();
    PWM_Init();
    PWM_SetTickPerPeriod(TICKS_PER_PERIOD);
    PWM_GenWaveform(pwmBuffer, TOTAL_LEDS * LED_BITS_PER_PIXEL + 2, 1, handleRefreshEnd);

    /*HAY QUE CAMBIAR LA FORMA EN LA QUE SE INCIALIZAN LOS TIMERS!!!! Fijense en timerPIT :D*/
    // Timer para actualizar la matriz
    refreshTimerId = createTimerPIT(REFRESH_RATE_HZ, refreshMatrix);
    // Timer para el blink -> CAPAZ NO LO NECESITAMOS
    blinkTimerId = createTimerPIT(DEFAULT_BLINK_INTERVAL_MS, toggleBlinkingLEDs);

    startTimerPIT(refreshTimerId);
    startTimerPIT(blinkTimerId);
}
// Prende el LED indicado
void turnOnLED(uint8_t row, uint8_t col) {
    ledMatrix[(row * MATRIX_COLS) + col].isOn = LED_ON;
    updateLEDMatrix();
}
// Apaga el LED indicado
void turnOffLED(uint8_t row, uint8_t col) {
    ledMatrix[(row * MATRIX_COLS) + col].isOn = LED_OFF;
    updateLEDMatrix();
}
// Prende todos los leds
void turnOnAllLEDs(void) {
    for (int i = 0; i < TOTAL_LEDS; i++) {
        ledMatrix[i].isOn = LED_ON;
    }
    updateLEDMatrix();
}
// Cambia el brillo de la matriz de LED enera
void setLEDBrightness(uint8_t brightnessPercentage) {
    if (brightnessPercentage > MAX_BRIGHTNESS) {
        brightnessPercentage = MAX_BRIGHTNESS;
    } else if (brightnessPercentage <= MIN_BRIGHTNESS) {
        brightnessPercentage = MIN_BRIGHTNESS;
    }
    ledBrightness = brightnessPercentage;
}
// Asigna un color a un LED 
void setLEDColor(uint8_t row, uint8_t col, LED_Color_t color) {
    ledMatrix[row * MATRIX_COLS + col].color = color;
    updateLEDMatrix();
}
// Hace que el LED titile
void enableLEDBlink(uint8_t row, uint8_t col, float intervalMs) {
    ledMatrix[row * MATRIX_COLS + col].isBlinking = LED_ON;
    ledMatrix[row * MATRIX_COLS + col].isOn = LED_ON;
    configTimerTimePIT(blinkTimerId, intervalMs); // Esto no sé si está bien
}
// Hace que el LED no titile
void disableLEDBlink(uint8_t row, uint8_t col) {
    ledMatrix[row * MATRIX_COLS + col].isBlinking = LED_OFF;
    ledMatrix[row * MATRIX_COLS + col].isOn = LED_OFF;
}

static void toggleBlinkingLEDs(void) {
    for (int i = 0; i < TOTAL_LEDS; i++) {
        if (ledMatrix[i].isBlinking == LED_ON) {
            ledMatrix[i].isOn = !ledMatrix[i].isOn;
        }
    }
    updateLEDMatrix();
}

static void refreshMatrix(void) {
    FTM_StartClock(FTM0); // Esto ver lo de FTM
}

static void handleRefreshEnd(void) {
    FTM_StopClock(FTM0); // Ver lo de FTM
}

static void updateLEDMatrix(void) {
    int currentBit = 7;  // Posición del bit actual que se está procesando.
    color_states_t currentColor = GREEN_STATE;  // Comienza con el componente de color verde.
    uint8_t bitValue = 0;  // Valor del bit a escribir en el buffer PWM.
    uint8_t tempColor;  // Color temporal escalado según el brillo.

    // Recorre cada LED de la matriz.
    for (int i = 0; i < TOTAL_LEDS; i++) {
        for (int j = 0; j < LED_BITS_PER_PIXEL; j++) {
            // Si el LED está encendido:
            if (ledMatrix[i].isOn == LED_ON) {
                switch (currentColor) {
                case GREEN_STATE:     // Procesar el componente verde del color.
                    tempColor = SCALE_COLOR(ledMatrix[i].color.green, ledBrightness);   // Ajusta el brillo del componente verde.
                    bitValue = (tempColor >> currentBit) & 0x1;                         // Extrae el bit actual del componente verde.
                    currentColor = currentBit-- == 0 ? RED_STATE : currentColor;                // Cambia al componente rojo si se han procesado todos los bits del verde.
                    currentBit = currentBit < 0 ? 7 : currentBit;                       // Reinicia el bit actual a 7 si llega a -1.
                    break;

                case RED_STATE:     // Procesar el componente rojo del color.
                    tempColor = SCALE_COLOR(ledMatrix[i].color.red, ledBrightness);
                    bitValue = (tempColor >> currentBit) & 0x1;
                    currentColor = currentBit-- == 0 ? BLUE_STATE : currentColor;
                    currentBit = currentBit < 0 ? 7 : currentBit;
                    break;

                case BLUE_STATE:     // Procesar el componente azul del color.
                    tempColor = SCALE_COLOR(ledMatrix[i].color.blue, ledBrightness);
                    bitValue = (tempColor >> currentBit) & 0x1;
                    currentColor = currentBit-- == 0 ? GREEN_STATE : currentColor;
                    currentBit = currentBit < 0 ? 7 : currentBit;
                    break;
                }
            } else {    // Si el LED está apagado, el bitValue siempre es 0.
                bitValue = 0;
            }
            // Asigna el valor del bit a `pwmBuffer`.
            pwmBuffer[i * LED_BITS_PER_PIXEL + j] = bitValue == 0 ? TIME_HIGH_BIT_0 : TIME_HIGH_BIT_1;
        }
    }

    // Añade dos pulsos de tiempo bajo al final del buffer PWM.
    pwmBuffer[TOTAL_LEDS * LED_BITS_PER_PIXEL] = TIME_HIGH_BIT_0;
    pwmBuffer[TOTAL_LEDS * LED_BITS_PER_PIXEL + 1] = TIME_HIGH_BIT_0;
}
