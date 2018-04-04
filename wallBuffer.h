/*
 * wallBuffer.h
 *
 *  Created on: Dec 5, 2016
 *      Author: boer8364
 */

#ifndef WALLBUFFER_H_
#define WALLBUFFER_H_

#include "wall.h"

// Standard buffer size for the game
#define WALL_BUFFER_SIZE 10

// WallBuffer error codes
typedef enum WallBufferError {
    WALL_BUFFER_NO_ERROR = 0,
    WALL_BUFFER_OVERFLOW = 1,
    WALL_BUFFER_UNDERFLOW = 2,
    WALL_BUFFER_OVER_STEP = 3
} WallBufferError_t;

// Wall buffer structure, simply a circular queue that holds wall objects
typedef struct WallBuffer {
    // Array where data will be held
    volatile Wall_t* buffer;

    // Size of the arrray
    volatile uint32_t bufferSize;
    // Number of meaningful data items in the buffer
    volatile uint32_t numItems;

    /* Pointer to where items will be added, an item is added, then the
     * pointer is advanced */
    volatile Wall_t* head;
    // Pointer to the oldest item
    volatile Wall_t* tail;
} WallBuffer_t;

// Initialize a buffer with the given buffer size
void WallBuffer_init(WallBuffer_t* self, uint32_t bufferSize);
// Cleanup the array on the heap
void WallBuffer_destroy(WallBuffer_t* self);

// Adds a wall at the head given the start and end angles
WallBufferError_t WallBuffer_addWall(WallBuffer_t* self, double startAngle,
    double endAngle);
/* Removes a wall from the tail and returns its value in the pointer given, a
 * NULL pointer can be passed in for removedItem */
WallBufferError_t WallBuffer_removeWall(WallBuffer_t* self, Wall_t* removedItem);

// Empties the contents of the buffer (does NOT free the array from the heap)
void WallBuffer_emptyBuffer(WallBuffer_t* self);

/* Function that advances a pointer pointing in the buffer by 1 array slot and
 * deals with wrap-around correctly */
inline void advanceBufferPointer(WallBuffer_t* buffer, Wall_t** pointer) {
    // Advance the pointer by 1 array slot
    ++(*pointer);
    /* Check if the pointer has surpassed the address of the last buffer array
     * element */
    if(*pointer > &buffer->buffer[buffer->bufferSize - 1]) {
        // If it has, wrap it around to the start of the array
        *pointer = (Wall_t*)buffer->buffer;
    }
}

#endif /* WALLBUFFER_H_ */
