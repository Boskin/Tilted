/*
 * uartLogger.c
 *
 *  Created on: Dec 7, 2016
 *      Author: boer8364
 */


#include "uartLogger.h"
#include <math.h>

#ifdef UART_DEBUG

static void numToString(int32_t num, char* str) {
    unsigned int i = 0;

    // Flag set if num is negative
    uint8_t neg = 0;
    // This code only works for positive numbers
    if(num < 0) {
        str[0] = '-';
        neg = 1;
        num = -num;
    } else if(num == 0) {
        // If the number is 0, set the string and exit immediately
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    // Find the number of digits in this number
    int digitCount = ((int)log10(num)) + 1;

    // Loop through to compute the digits
    while(num > 0) {
        str[digitCount - i - 1 + neg] = num % 10 + '0';
        num /= 10;
        ++i;
    }
    // Terminate the string
    str[i + neg] = '\0';
}

void UART_Logger_sendBytes(uint8_t* byteArray, unsigned int length) {
    // Iterate through the array and send all of the bytes
    unsigned int i;
    for(i = 0; i < length; ++i) {
        UART_Logger_sendByte(byteArray[i]);
    }
}

void UART_Logger_sendString(const char* str) {
    // Keep sending bytes until the NULL terminator is encountered
    unsigned int i;
    for(i = 0; str[i] != '\0'; ++i) {
        UART_Logger_sendByte((uint8_t)str[i]);
    }
}

void UART_Logger_sendNumSigned(int32_t num) {
    // Convert the number to a string and send it
    char numStr[6];
    numToString(num, numStr);
    UART_Logger_sendString(numStr);
}

#endif
