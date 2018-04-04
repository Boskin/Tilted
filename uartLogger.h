/*
 * uartLogger.h
 *
 *  Created on: Dec 7, 2016
 *      Author: boer8364
 */

#include "globalMacros.h"

#ifdef UART_DEBUG
#ifndef UARTLOGGER_H_
#define UARTLOGGER_H_

#include <inttypes.h>
#include "msp.h"

// Sends the NULL-terminated string str
void UART_Logger_sendString(const char* str);
// Sends the specified array of bytes
void UART_Logger_sendBytes(uint8_t* byteArray, unsigned int length);
// Sends the signed number in ASCII
void UART_Logger_sendNumSigned(int32_t num);

// Sends a single byte, blocks until transmitter is ready
inline void UART_Logger_sendByte(uint8_t byte) {
    while(EUSCI_A0->STATW & UCBUSY);
    EUSCI_A0->TXBUF = byte;
}

#endif /* UARTLOGGER_H_ */
#endif
