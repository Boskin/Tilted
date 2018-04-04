/*
 * hardwareConfig.h
 *
 *  Created on: Dec 4, 2016
 *      Author: boer8364
 */

#ifndef HARDWARECONFIG_H_
#define HARDWARECONFIG_H_

#include "msp.h"

// Maximum magnitude of x and y readings from ADC (found in datasheet)
#define HARDWARE_CONFIG_ADC_MAX_MAGNITUDE 3095.96

// Configures the ADC for the accelerometer
void configureADC();
// Configures the timers
void configureTimer();

#ifdef UART_DEBUG
// Configures a 9600 baud UART for communication through USB
void configureUART();
#endif

// Configures GPIO (for the button)
void configurePins();

// Enables timer interrupts, this is inline since it gets called frequently
inline void enableTimerInterrupts() {
    // Clear the timer flags just in case
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    // Set the interrupt flags
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;
    TIMER_A0->CTL |= TIMER_A_CTL_IE;
}

// Disables timer interrupts, this is inline since it gets called frequently
inline void disableTimerInterrupts() {
    // Clear the timer interrupt flags
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
    TIMER_A0->CTL &= ~TIMER_A_CTL_IE;
}

// Set the timer count to 0
inline void resetTimer() {
    // Set the timer count to 0
    TIMER_A0->R = 0;
}

// Extern declaration of the buttonPressed flag
extern volatile uint8_t buttonPressed;

// Enables button interrupts
inline void enableButtonInterrupts() {
    // Clear the interrupt flags
    P3IFG = 0x00;
    // Select the negative edge
    P3IES |= BIT5;
    // Enable the interrupt
    P3IE |= BIT5;
    // Reset the global button press flag
    buttonPressed = 0;
}

// Disables button interrupts
inline void disableButtonInterrupts() {
    // Clear the interrupt enable flag
    P3IE &= ~BIT5;
}

#endif /* HARDWARECONFIG_H_ */
