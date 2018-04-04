/*
 * hardwareConfig.c
 *
 *  Created on: Dec 4, 2016
 *      Author: boer8364
 */

#include "globalMacros.h"
#include "hardwareConfig.h"
#include "wall.h"

void configureADC() {
    //Configure pins to tertiary mode
    //X-axis pins
    P6SEL0 |= BIT1;
    P6SEL1 |= BIT1;

    //Y-axis pins
    P4SEL0 |= BIT0;
    P4SEL1 |= BIT0;

    // Disable the ADC module so that it can be configured
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;

    /* Configure the ADC:
     * ADC14_CTL0_SSE__SMCLK: use SMCLK as the clock source
     * ADC14_CTL0_SHT0_2: set the sample-and-hold time to 16 clock cycles
     * ADC14_CTL0_SHP: set to pulse sampling mode
     * ADC14_CTL0_CONSEQ_1: load a sequence of channels with samples
     * ADC14_CTL0_MSC: take multiple samples */
    ADC14->CTL0 |= ADC14_CTL0_SSEL__SMCLK | ADC14_CTL0_SHT0_2 |
        ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1 | ADC14_CTL0_MSC;
    // Set the ADC resolution to 14 bits
    ADC14->CTL1 |= ADC14_CTL1_RES__14BIT;

    // Enable interupts on channels 0 and 1
    ADC14->IER0 |= ADC14_IER0_IE0 | ADC14_IER0_IE1;

    // Hook channel 0 up to the x channel of the accelerometer
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_14;
    /* Hook channel 1 up to the y channel of the accelerometer, also make it
     * the end of the sequence of the samples being taken */
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_13 | ADC14_MCTLN_EOS;

    // Turn on the ADC and enable conversion
    ADC14->CTL0 |= ADC14_CTL0_ON | ADC14_CTL0_ENC;

    // Enable ADC interrupts in the NVIC
    NVIC_EnableIRQ(ADC14_IRQn);
}

void configureTimer() {
    // Configure the timer
    TIMER_A0->CTL = TIMER_A_CTL_MC_1 | TIMER_A_CTL_ID__2 | TIMER_A_CTL_SSEL__ACLK;
    // Put in the threshold value
    TIMER_A0->CCR[0] = WALL_SPAWN_PERIOD_MAX;

#ifdef TIMING_INFO_DEBUG
    TIMER_A1->CTL = TIMER_A_CTL_MC_2 | TIMER_A_CTL_SSEL__ACLK;
#endif

    NVIC_EnableIRQ(TA0_0_IRQn);
}

#ifdef UART_DEBUG
void configureUART() {
    // Change the TX and RX pins to their primary mode
    P1SEL0 |= BIT2 | BIT3;
    P1SEL1 &= ~(BIT2 | BIT3);

    // Reset the eUSCI module
    EUSCI_A0->CTLW0 |= UCSWRST;

    // Use ACLK as the clock source
    EUSCI_A0->CTLW0 |= UCSSEL__ACLK;
    // Divide it by 3 to get a 9600 baud rate
    EUSCI_A0->BRW = 3;
    // Set the modulation so that we get a 9600 baud rate
    EUSCI_A0->MCTLW = 0x92 << 8;

    // Enable the eUSCI module
    EUSCI_A0->CTLW0 &= ~UCSWRST;
}
#endif

void configurePins() {
    // Set the pin as an input
    P3DIR &= ~BIT5;
    // Enable a resistor
    P3REN |= BIT5;
    // Set the resistor to be pull-up
    P3OUT |= BIT5;

    // Enable interrupts from port 3
    NVIC_EnableIRQ(PORT3_IRQn);
}
