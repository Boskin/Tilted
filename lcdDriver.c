/*
 * lcdDriver.c
 *
 *  Created on: Nov 19, 2016
 *      Author: Boskin
 */
#include "lcdDriver.h"

#include <math.h>
#include "msp.h"

uint16_t pixelBuffer[LCD_SCREEN_HEIGHT * LCD_SCREEN_WIDTH];
/* Array of pointers that will be used to point to the starts of rows in the
 * pixelBuffer, this will allow us to address pixel data in the same way one
 * would address elements of a matrix */
uint16_t* pixelBufferOverlay[LCD_SCREEN_HEIGHT];

// Foreground and background colors
uint16_t foregroundColor = 0x0000;
uint16_t backgroundColor = 0xffff;

inline void LCD_sendByte(uint8_t bytePacket) {
    // Block until the SPI transmitter is ready
    while(UCB0STATW & UCBUSY);
    // Send the packet
    UCB0TXBUF = bytePacket;
}

void LCD_writeCommand(uint8_t cmd) {
    // Turn on command mode for the LCD
    P3OUT &= ~BIT7;
    // Send the desired command
    LCD_sendByte(cmd);
    // Turn off command mode for the LCD
    P3OUT |= BIT7;
}

void LCD_writeData(uint8_t data) {
    // Simply send the data
    LCD_sendByte(data);
}

/* Initialize the hardware for the LCD:
 * SPI clock: P1.5
 * SPI MOSI: P1.6
 * Chip select: P5.0
 * LCD reset pin: P5.7
 * LCD DC pin (command/data select): P3.7 */
void LCD_init() {
    // Configure primary configuration for P1.5 (SPI clock) and P1.6 (SPI MOSI)
    P1SEL0 |= BIT5 | BIT6;
    P1SEL1 &= ~(BIT5 | BIT6);

    /* Configure directions for the reset, command/data select, and the chip
     * select pin */
    P3DIR |= BIT7;
    P5DIR |= BIT0 | BIT7;

    // Clock configuration
    // Unlock the clock module
    CS->KEY = 0x695a;
    // Reset the clock configuration
    CS->CTL0 = 0;
    // Set DC0 to run at 24MHz
    CS->CTL0 = 0x04 << 16;
    /* Configure clock sources:
     * ACLK hooks up to REFOCLK (32.678kHz)
     * SMCLK hooks up to DC0CLK
     * HMCLKC hooks up to DC0CLKC */
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    // Lock the clock module
    CS->KEY = 0;

    // SPI configuration
    // Reset the SPI module
    EUSCI_B0_SPI->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    // The SPI needs a 16MHz clock (experiment with this later)
    /* Clock source will be SMCLK (running at 48MHz and divided down to 16Mhz),
     * MSB will be sent first,
     * 3 pin mode will be used,
     * 8 bit data will be used,
     * Inactive state on the clock will be low (clock polarity) */
    EUSCI_B0_SPI->CTLW0 |= EUSCI_B_CTLW0_CKPH | EUSCI_B_CTLW0_SSEL__SMCLK | EUSCI_B_CTLW0_MSB | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_MST;

    // 48MHz / 16Mhz = 3
    EUSCI_B0_SPI->BRW = 1;

    EUSCI_B0_SPI->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

    // Reset the LCD
    P5OUT &= ~BIT7;
    LCD_DELAY(50);
    P5OUT |= BIT7;
    LCD_DELAY(120);

    P5OUT &= ~BIT0;

    // Run some commands
    // Turn off sleep mode
    LCD_writeCommand(LCD_CMD_SLEEP_OUT);
    LCD_DELAY(200);

    // Set the gamma value
    LCD_writeCommand(LCD_CMD_SET_GAMMA);
    LCD_writeData(0x04);

    // Set the frame rate (needs more investigation)
    LCD_writeCommand(LCD_CMD_FRAME_RATE_CONTROL);
    LCD_writeData(0x0a);
    LCD_writeData(0x14);

    // Set the power mode
    LCD_writeCommand(LCD_CMD_POWER_CONTROL);
    LCD_writeData(0x0a);
    LCD_writeData(0x00);

    // Set the pixel format to 16 bit mode
    LCD_writeCommand(LCD_CMD_PIXEL_FORMAT_CONTROL);
    LCD_writeData(0x05);
    LCD_DELAY(10);

    // Set the pixel data write order
    LCD_writeCommand(LCD_CMD_DATA_ACCESS_CONTROL);
    LCD_writeData(0xc8);

    // Set the display mode to normal
    LCD_writeCommand(LCD_CMD_NORMAL_DISPLAY_MODE);

    // Set the column (x) window
    LCD_writeCommand(LCD_CMD_COLUMN_ADDRESS_SELECT);
    // Lower x bound
    LCD_writeData(0x02 >> 8);
    LCD_writeData(0x02);
    // Upper x bound
    LCD_writeData(0x81 >> 8);
    LCD_writeData(0x81);

    // Set the row (y) window
    LCD_writeCommand(LCD_CMD_ROW_ADDRESS_SELECT);
    // Lower y bound
    LCD_writeData(0x03 >> 8);
    LCD_writeData(0x03);
    // Upper y bound
    LCD_writeData(0x82 >> 8);
    LCD_writeData(0x82);

    // Start with a white screen
    // Initiate a display write
    LCD_writeCommand(LCD_CMD_RAM_WRITE);
    // Write every pixel to be white
    unsigned int i = 0;
    for(; i < LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT; ++i) {
        LCD_writeData(0xff);
        LCD_writeData(0xff);

        pixelBuffer[i] = 0xffff;
    }

    /* Setup the pixelBufferOverlay so that its pointers point to the start of
     * each row on the screen */
    for(i = 0; i < LCD_SCREEN_HEIGHT; ++i) {
        pixelBufferOverlay[i] = &pixelBuffer[i * LCD_SCREEN_WIDTH];
    }

    LCD_DELAY(10);
    // Turn the display on
    LCD_writeCommand(LCD_CMD_DISPLAY_ON);
}

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_drawPixel(int16_t x, int16_t y) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    if(x >= 0 && x < LCD_SCREEN_WIDTH && y >= 0 && y < LCD_SCREEN_HEIGHT) {
        pixelBufferOverlay[y][x] = foregroundColor;
        return LCD_NO_ERROR;
    }
    return LCD_OUT_OF_BOUNDS;
#else
    pixelBufferOverlay[y][x] = foregroundColor;
#endif
}

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_fillRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    if(x0 >= 0 && x0 < LCD_SCREEN_WIDTH && y0 >= 0 && y0 < LCD_SCREEN_HEIGHT &&
       x1 >= 0 && x1 < LCD_SCREEN_WIDTH && y1 >= 0 && y1 < LCD_SCREEN_HEIGHT) {
#endif
        // Find the bounds based on the given coordinates
        unsigned int xMin;
        unsigned int yMin;
        unsigned int xMax;
        unsigned int yMax;

        if(x0 < x1) {
            xMin = x0;
            xMax = x1;
        } else {
            xMin = x1;
            xMax = x0;
        }

        if(y0 < y1) {
            yMin = y0;
            yMax = y1;
        } else {
            yMin = y1;
            yMax = y0;
        }

        // Iterate through and fill everything within the rectangle
        unsigned int x;
        unsigned int y;
        for(y = yMin; y <= yMax; ++y) {
            for(x = xMin; x <= xMax; ++x) {
                pixelBufferOverlay[y][x] = foregroundColor;
            }
        }

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
        return LCD_NO_ERROR;
    }
    return LCD_OUT_OF_BOUNDS;
#endif
}

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    if(x0 < 0 || x0 >= LCD_SCREEN_WIDTH || y0 < 0 && y0 >= LCD_SCREEN_HEIGHT ||
       x1 < 0 || x1 >= LCD_SCREEN_WIDTH || y1 < 0 && y1 >= LCD_SCREEN_HEIGHT) {
        return LCD_OUT_OF_BOUNDS;
    }
#endif
    // Check the change in x and y
    int deltaX = x1 - x0;
    int deltaY = y1 - y0;
    // If either is 0, use the corresponding simple line drawing function
    if(deltaX == 0) {
        LCD_drawLineVertical(x0, y0, y1);
    }
    if(deltaY == 0) {
        LCD_drawLineHorizontal(x0, x1, y0);
    }

    // Check if the line is more horizontal
    if(abs(deltaX) > abs(deltaY)) {
        // Check if x is increasing or decreasing from the start point
        int xAdv = deltaX > 0 ? 1 : -1;
        int x = x0;
        int y = y0;
        float error = 0.f;
        float slope = (float)deltaY / deltaX;
        while(x != x1) {
            pixelBufferOverlay[y][x] = foregroundColor;
            x += xAdv;
            error += slope;
            if(error >= 1.f) {
                error = 0.f;
                ++y;
            }
        }
    } else {
        int yAdv = deltaY > 0 ? 1 : -1;
        int x = x0;
        int y = y0;
        float error = 0.f;
        float slope = (float)deltaX / deltaY;
        while(y != y1) {
            pixelBufferOverlay[y][x] = foregroundColor;
            y += yAdv;
            error += slope;
            if(error >= 1.f) {
                error = 0.f;
                ++x;
            }
        }
    }

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    return LCD_NO_ERROR;
#endif
}

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_drawLineVertical(int16_t x, int16_t y0, int16_t y1) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    if(x < 0 || x >= LCD_SCREEN_WIDTH ||
       y0 < 0 || y0 >= LCD_SCREEN_HEIGHT || y1 < 0 || y1 >= LCD_SCREEN_HEIGHT) {
        return LCD_OUT_OF_BOUNDS;
    }
#endif

    int16_t yMin;
    int16_t yMax;

    if(y0 < y1) {
        yMin = y0;
        yMax = y1;
    } else {
        yMin = y1;
        yMax = y0;
    }

    for(; yMin <= yMax; ++yMin) {
        pixelBufferOverlay[yMin][x] = foregroundColor;
    }

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    return LCD_NO_ERROR;
#endif
}

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_drawLineHorizontal(int16_t x0, int16_t x1, int16_t y) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    if(y < 0 || y >= LCD_SCREEN_HEIGHT ||
       x0 < 0 || x0 >= LCD_SCREEN_WIDTH || x1 < 0 || x1 >= LCD_SCREEN_WIDTH) {
        return LCD_OUT_OF_BOUNDS;
    }
#endif

    int16_t xMin;
    int16_t xMax;

    if(x0 < x1) {
        xMin = x0;
        xMax = x1;
    } else {
        xMin = x1;
        xMax = x0;
    }

    for(; xMin <= xMax; ++xMin) {
        pixelBufferOverlay[y][xMin] = foregroundColor;
    }

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    return LCD_NO_ERROR;
#endif
}

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_drawCircle(int16_t centerX, int16_t centerY, int16_t r) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    if(centerX - r < 0 || centerX + r >= LCD_SCREEN_WIDTH ||
       centerY - r < 0 || centerY + r >= LCD_SCREEN_HEIGHT) {
        return LCD_OUT_OF_BOUNDS;
    }
#endif

    int x = r;
    int y = 0;
    int error = 0;

    while(x >= y) {
        // First quadrant
        pixelBufferOverlay[centerY + y][centerX + x] = foregroundColor;
        pixelBufferOverlay[centerY + x][centerX + y] = foregroundColor;

        // Second quadrant
        pixelBufferOverlay[centerY + y][centerX - x] = foregroundColor;
        pixelBufferOverlay[centerY + x][centerX - y] = foregroundColor;

        // Third quadrant
        pixelBufferOverlay[centerY - y][centerX - x] = foregroundColor;
        pixelBufferOverlay[centerY - x][centerX - y] = foregroundColor;

        // Fourth quadrant
        pixelBufferOverlay[centerY - y][centerX + x] = foregroundColor;
        pixelBufferOverlay[centerY - x][centerX + y] = foregroundColor;

        ++y;
        error += 1 + 2 * y;
        if(2 * (error - x) + 1 > 0) {
            --x;
            error += 1 - 2 * x;
        }
    }

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    return LCD_NO_ERROR;
#endif
}

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_fillCircle(int16_t centerX, int16_t centerY, int16_t r) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    if(centerX - r < 0 || centerX + r >= LCD_SCREEN_WIDTH ||
       centerY - r < 0 || centerY + r >= LCD_SCREEN_HEIGHT) {
        return LCD_OUT_OF_BOUNDS;
    }
#endif

    int x = r;
    int y = 0;
    int error = 0;

    while(x >= y) {
        // Lines from 1st quadrant to 4th quadrant
        LCD_drawLineVertical(centerX + x, centerY - y, centerY + y);
        LCD_drawLineVertical(centerX + y, centerY - x, centerY + x);

        // Lines from 2nd quadrant to 3rd quadrant
        LCD_drawLineVertical(centerX - x, centerY - y, centerY + y);
        LCD_drawLineVertical(centerX - y, centerY - x, centerY + x);

        ++y;
        error += 1 + 2 * y;
        if(2 * (error - x) + 1 > 0) {
            --x;
            error += 1 - 2 * x;
        }
    }

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    return LCD_NO_ERROR;
#endif
}


// Macros to check if values are within x/y bounds to save some typing
#define BOUND_X(val) (val >= xLowerBound && val <= xUpperBound)
#define BOUND_Y(val) (val >= yLowerBound && val <= yUpperBound)

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
LCD_Error_t
#else
void
#endif
LCD_drawArc(int16_t centerX, int16_t centerY, int16_t r, double startAngle, double endAngle) {
    if(startAngle == endAngle) {
#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
        return LCD_OUT_OF_BOUNDS;
#else
        return;
#endif
    }

    int16_t x = r;
    int16_t y = 0;
    int16_t error = 0;

    int16_t x0 = (int16_t)(r * cos(startAngle));
    int16_t x1 = (int16_t)(r * cos(endAngle));
    int16_t y0 = (int16_t)(r * sin(startAngle));
    int16_t y1 = (int16_t)(r * sin(endAngle));

    int16_t xLowerBound;
    int16_t yLowerBound;
    int16_t xUpperBound;
    int16_t yUpperBound;

    // Use the coordinates of the endpoints to create a rectangular window
    if(x0 < x1) {
        xLowerBound = x0;
        xUpperBound = x1;
    } else {
        xLowerBound = x1;
        xUpperBound = x0;
    }
    if(y0 < y1) {
        yLowerBound = y0;
        yUpperBound = y1;
    } else {
        yLowerBound = y1;
        yUpperBound = y0;
    }

    /* If the x coordinates have different signs, the arc goes across two
     * quadrants horizontally, so the y bounds must be pulled to either
     * the negative or positive limits of the circle the arc lives on */
    if((x0 >= 0) != (x1 >= 0)) {
        if(y0 > 0) {
            yUpperBound = r;
        } else {
            yLowerBound = -r;
        }
    }

    /* If the y coordinates have different signs, the arc goes across two
     * quadrants vertically, so the x bounds must be pulled to either the
     * negative or positive limits of the circle the arc lives on */
    if((y0 >= 0) != (y1 >= 0)) {
        if(x0 > 0) {
            xUpperBound = r;
        } else {
            xLowerBound = -r;
        }
    }

    /* Duplicate the Breshman circle drawing algorithm, except make sure the
     * coordinates that are about to be drawn are within the x and y bounds
     * calculated above */
    while(x >= y) {
        // First quadrant
        if(BOUND_X(x) && BOUND_Y(y)) {
            pixelBufferOverlay[centerY + y][centerX + x] = foregroundColor;
        }
        if(BOUND_X(y) && BOUND_Y(x)) {
            pixelBufferOverlay[centerY + x][centerX + y] = foregroundColor;
        }

        // Second quadrant
        if(BOUND_X(-x) && BOUND_Y(y)) {
            pixelBufferOverlay[centerY + y][centerX - x] = foregroundColor;
        }
        if(BOUND_X(-y) && BOUND_Y(x)) {
            pixelBufferOverlay[centerY + x][centerX - y] = foregroundColor;
        }

        // Third quadrant
        if(BOUND_X(-x) && BOUND_Y(-y)) {
            pixelBufferOverlay[centerY - y][centerX - x] = foregroundColor;
        }
        if(BOUND_X(-y) && BOUND_Y(-x)) {
            pixelBufferOverlay[centerY - x][centerX - y] = foregroundColor;
        }

        // Fourth quadrant
        if(BOUND_X(x) && BOUND_Y(-y)) {
            pixelBufferOverlay[centerY - y][centerX + x] = foregroundColor;
        }
        if(BOUND_X(y) && BOUND_Y(-x)) {
            pixelBufferOverlay[centerY - x][centerX + y] = foregroundColor;
        }

        ++y;
        error += 1 + (y << 1);
        if(((error - x) << 1) + 1 > 0) {
            --x;
            error += 1 - (x << 1);
        }
    }

#ifdef LCD_PIXEL_DRAW_BOUNDS_CHECK
    return LCD_NO_ERROR;
#endif
}

// The bound checking macros are no longer needed
#undef BOUND_X
#undef BOUND_Y

void LCD_clearBuffer() {
    /* Iterate through the elements of the buffer and set them to the
     * background color */
    unsigned int i;
    for(i = 0; i < LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT; ++i) {
        pixelBuffer[i] = backgroundColor;
    }
}

void LCD_sendBuffer() {
    // Iterate through the elements of the buffer and send them over SPI
    LCD_writeCommand(LCD_CMD_RAM_WRITE);
    unsigned int i;
    for(i = 0; i < LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT; ++i) {
        // Write the MSB first
        LCD_writeData(pixelBuffer[i] >> 8);
        LCD_writeData(pixelBuffer[i]);
    }
}

void LCD_sendAndClearBuffer() {
    // Send, then clear, the contents
    LCD_writeCommand(LCD_CMD_RAM_WRITE);
    unsigned int i;
    for(i = 0; i < LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT; ++i) {
        LCD_writeData(pixelBuffer[i] >> 8);
        LCD_writeData(pixelBuffer[i]);

        pixelBuffer[i] = backgroundColor;
    }
}

void LCD_sendCustomBuffer(const uint8_t* buffer) {
    // Iterate through the custom array and send the contents
    LCD_writeCommand(LCD_CMD_RAM_WRITE);
    unsigned int i;
    for(i = 0; i < LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT * 2; ++i) {
        LCD_writeData(buffer[i]);
    }
}
