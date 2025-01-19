#ifndef _LEDMATRIXCONTROL_H_
#define _LEDMATRIXCONTROL_H_

#include <stdint.h>

// Constantes para configuración de la matriz de LEDs
#define MATRIX_ROWS (8)
#define MATRIX_COLS (8)
#define MIN_BRIGHTNESS (1)
#define MAX_BRIGHTNESS (100)

// Definiciones de colores
#define COLOR_RED (0xFF0000)
#define COLOR_BLUE (0x0000FF)
#define COLOR_GREEN (0x00FF00)
#define COLOR_WHITE (0xFFFFFF)
#define COLOR_ORANGE (0xFF6E00)
#define COLOR_PINK (0xE60066)
#define COLOR_PURPLE (0xFF99FF)
#define COLOR_CYAN (0x00FFFF)
#define COLOR_YELLOW (0xFFFF00)
#define COLOR_OFF (0x000000)

// Definiciones de colores en degradé de amarillo a rojo
#define COLOR_YELLOW (0xFFFF00)
#define COLOR_LIGHT_ORANGE (0xFFCC00)
#define COLOR_ORANGE_2 (0xFF9900)
#define COLOR_DARK_ORANGE (0xFF6600)
#define COLOR_RED_ORANGE (0xFF3300)
#define COLOR_LIGHT_RED (0xFF0033)
#define COLOR_RED (0xFF0000)
#define COLOR_DARK_RED (0x990000)
#define COLOR_OFF (0x000000)

// Estructura para representar un color y su brillo
typedef union {
    uint32_t hexColor;
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t brightness;
    };
} LED_Color_t;

// Declaración de funciones para controlar la matriz de LEDs
void initializeLEDMatrix(void);
void turnOnLED(uint8_t row, uint8_t col);
void turnOffLED(uint8_t row, uint8_t col);
void turnOnAllLEDs(void);
void setLEDBrightness(uint8_t brightnessPercentage);
void setLEDColor(uint8_t row, uint8_t col, LED_Color_t color);
void enableLEDBlink(uint8_t row, uint8_t col, float intervalMs);
void disableLEDBlink(uint8_t row, uint8_t col);

#endif // _LEDMATRIXCONTROL_H_

