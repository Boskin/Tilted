/*
 * lcdDriver.h
 *
 *  Created on: Nov 23, 2016
 *      Author: Boskin
 */

#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_

#include <inttypes.h>

// Bounds checking flag
#define LCD_PIXEL_DRAW_BOUNDS_CHECK

// Dimensions of the LCD screen
#define LCD_SCREEN_WIDTH 128
#define LCD_SCREEN_HEIGHT 128

// LCD delay utility function/macro used in initialization
#define LCD_DELAY(delay) __delay_cycles((delay) * 48)

// LCD command macros
#define LCD_CMD_SLEEP_OUT 0x11
#define LCD_CMD_NORMAL_DISPLAY_MODE 0x13
#define LCD_CMD_SET_GAMMA 0x26
#define LCD_CMD_DISPLAY_ON 0x29
#define LCD_CMD_COLUMN_ADDRESS_SELECT 0x2a
#define LCD_CMD_ROW_ADDRESS_SELECT 0x2b
#define LCD_CMD_RAM_WRITE 0x2c
#define LCD_CMD_DATA_ACCESS_CONTROL 0x36
#define LCD_CMD_PIXEL_FORMAT_CONTROL 0x3a
#define LCD_CMD_FRAME_RATE_CONTROL 0xb1
#define LCD_CMD_POWER_CONTROL 0xc0

// Error enumeration
typedef enum LCD_Error {
    // Indicates no error occurred
    LCD_NO_ERROR = 0,
    /* Indicates that a pixel was attempted to be drawn out of bounds of the
     * screen */
    LCD_OUT_OF_BOUNDS = 1
} LCD_Error_t;

void LCD_init();

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
// Places a single pixel at (x, y)
LCD_drawPixel(int16_t x, int16_t y);


#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
/* Fills a rectangle bounded by the opposite diagonal corners (x0, y0) and
 * (x1, y1) */
LCD_fillRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
/* Draws a line from (x0, y0) to (x1, y1), this function can draw vertical
 * and horizontal lines, but there are dedicated functions which do that, which
 * are recommended for use rather than this one in that case */
LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
/* Draws a vertical line at x bounded vertically by y0 and y1, it does not
 * matter whether y0 or y1 is the lower or upper bound and viceversa */
LCD_drawLineVertical(int16_t x, int16_t y0, int16_t y1);

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
/* Draws a horizontal line at y bounded horizontally by x0 and x1, it does
 * not matter whether x0 or x1 is the lower or upper bound and viceversa */
LCD_drawLineHorizontal(int16_t x0, int16_t x1, int16_t y);

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
/* Uses the Breshman circle drawing algorithm to draw an unfilled circle
 * centered at (centerX, centerY) with a radius of r */
LCD_drawCircle(int16_t centerX, int16_t centerY, int16_t r);

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
/* Uses the Breshman circle drawing algorithm and the line drawing functions
 * to fill a circle centered at (centerX, centerY) with a radius of r */
LCD_fillCircle(int16_t centerX, int16_t centerY, int16_t r);

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
/* Uses a slightly modified version of the Breshman circle drawing algorithm to
 * draw the arc of the specified angle interval along the circle centered at
 * (centerX, centerY) with a radius of r, startAngle must always be
 * counter-clockwise of endAngle */
LCD_drawArc(int16_t centerX, int16_t centerY, int16_t r, double startAngle, double endAngle);

// Clear the contents of the buffer to the background color
void LCD_clearBuffer();
// Send the contents of the buffer to the LCD controller
void LCD_sendBuffer();
// Send, then clear, the contents of the buffer
void LCD_sendAndClearBuffer();

/* Send a buffer of data to the LCD controller, the buffer size must be 32768
 * elements long, the order in which contents are drawn starts from the top
 * left corner and moves horizontally, then downward */
void LCD_sendCustomBuffer(const uint8_t* buffer);

// Macro that creates a 16-bit color given the intensities
#define MAKE_COLOR16(r, g, b) ((uint16_t)(((r) << 11) | ((g) << 5) | (b)))

// Foreground and background colors
extern uint16_t foregroundColor;
extern uint16_t backgroundColor;

// Function to set the foreground color, it is black by default
inline void LCD_setForegroundColor(uint16_t color) {
    foregroundColor = color;
}

// Function to set the background color, it is white by default
inline void LCD_setBackgroundColor(uint16_t color) {
    backgroundColor = color;
}

#endif /* LCDDRIVER_H_ */
