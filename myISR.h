/*
 * myISR.h
 *
 *  Created on: Dec 4, 2016
 *      Author: boer8364
 */

#ifndef MYISR_H_
#define MYISR_H_

#include <inttypes.h>

// Structure containing accelerometer readings
typedef struct ADC14Readings {
    // X-axis accelerometer reading
    volatile int16_t lastXReading;
    // Y-axis accelerometer reading
    volatile int16_t lastYReading;
} ADC14Readings_t;

// Global instance of the structure
extern volatile ADC14Readings_t adc14Readings;

// Flag that determines if the start/stop button was pressed
extern volatile uint8_t buttonPressed;

#endif /* MYISR_H_ */
