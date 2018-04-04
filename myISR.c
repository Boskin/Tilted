/*
 * myIsr.c
 *
 *  Created on: Dec 4, 2016
 *      Author: boer8364
 */
#include "myISR.h"

#include <stdlib.h>
#include "msp.h"
#include "globalMacros.h"
#include "uartLogger.h"
#include "wallBuffer.h"

volatile ADC14Readings_t adc14Readings;

// ADC ISR
void adc14ISR() {
    // X reading
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG0) {
        /* Normalize the x reading, anything smaller than 8192 is a negative
         * reading, while anything greater than that is a positive reading */
        adc14Readings.lastXReading = (ADC14->MEM[0] & 0x3fff) - 8192;
#ifdef UART_DEBUG
#ifdef TIMING_INFO_DEBUG
        // Send timing information in brackets
        UART_Logger_sendByte('[');
        UART_Logger_sendNumSigned(TIMER_A1->R);
        UART_Logger_sendString("] ");
#endif
        // Send and label the x reading
        UART_Logger_sendString("X: ");
        UART_Logger_sendNumSigned((int32_t)adc14Readings.lastXReading);
        UART_Logger_sendByte((uint8_t)'\r');
#endif
    }

    // Y reading
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG1) {
        /* Normalize the y reading, anything smaller than 8192 is a negative
         * reading, while anything greater than that is a positive reading */
        adc14Readings.lastYReading = (ADC14->MEM[1] & 0x3fff) - 8192;
#ifdef UART_DEBUG
#ifdef TIMING_INFO_DEBUG
        // Send timing information in brackets
        UART_Logger_sendByte('[');
        UART_Logger_sendNumSigned(TIMER_A1->R);
        UART_Logger_sendString("] ");
#endif
        // Send and label the y reading
        UART_Logger_sendString("Y: ");
        UART_Logger_sendNumSigned((int32_t)adc14Readings.lastYReading);
        UART_Logger_sendByte((uint8_t)'\r');
#endif

    }
}

// We require an external declaration of the game's wallBuffer
extern volatile WallBuffer_t wallBuffer;

// Timer A0 ISR
void timerAISR() {
    // Clear interrupt flags
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    /* Generate a random integer in the interval
     * [0, WALL_ANGLE_GENERATION_RESOLUTION) */
    int randomNumber = rand() % (WALL_ANGLE_GENERATION_RESOLUTION);
    /* Use the ratio between the angle generation resolution and the randomly
     * generated number to create an angle in the interval [0, 2*pi], then
     * normalize it so that the angle is in the interval [-pi, pi]  */
    double startAngle = 2 * CONSTANT_PI * ((double)randomNumber) /
        WALL_ANGLE_GENERATION_RESOLUTION - CONSTANT_PI;
    // The endAngle is simply the standard gap length clockwise of the start
    double endAngle = startAngle + WALL_GAP_ANGULAR_LENGTH;
    // If the start is out of the interval [-pi, pi], normalize it
    if(endAngle > CONSTANT_PI) {
        endAngle -= 2 * CONSTANT_PI;
    }

#ifdef UART_DEBUG
#ifdef TIMING_INFO_DEBUG
    // Log when the wall spawned
    UART_Logger_sendByte('[');
    UART_Logger_sendNumSigned(TIMER_A1->R);
    UART_Logger_sendString("] ");
#endif
    // Indicate that a wall spawned
    UART_Logger_sendString("Wall spawned\r");
#endif

    // Add the new wall to the buffer
    WallBuffer_addWall((WallBuffer_t*)&wallBuffer, startAngle, endAngle);
}

// Initialize the buttonPressed flag to 0 (not pressed)
volatile uint8_t buttonPressed = 0;

// Port 3 ISR
void port3ISR() {
    // Clear the interrupt source
    P3IFG = 0x00;

    // Set the button pressed flag
    buttonPressed = 1;
}
