/*
 * wallBuffer.c
 *
 *  Created on: Dec 5, 2016
 *      Author: boer8364
 */

#include "wallBuffer.h"

#include <stdlib.h>

void WallBuffer_init(WallBuffer_t* self, uint32_t bufferSize) {
    // Allocate the buffer array
    self->buffer = malloc(sizeof(*self->buffer) * bufferSize);
    // Set the buffer size
    self->bufferSize = bufferSize;
    // Set the buffer to its initial state
    self->numItems = 0;
    self->head = self->buffer;
    self->tail = self->buffer;
}

void WallBuffer_destroy(WallBuffer_t* self) {
    // Free memory if not NULL
    if(self->buffer) {
        free((void*)self->buffer);
        // Set the pointers to NULL
        self->buffer = NULL;
        self->head = NULL;
        self->tail = NULL;

        // Reset the size and numItems fields
        self->bufferSize = 0;
        self->numItems = 0;
    }
}

WallBufferError_t WallBuffer_addWall(WallBuffer_t* self, double startAngle,
    double endAngle) {
    // Check if the buffer is not at a full capacity
    if(self->numItems < self->bufferSize) {
        // Increment the number of items
        ++self->numItems;
        // Initialize the wall at the head
        Wall_init((Wall_t*)self->head, startAngle, endAngle);
        // Advance the head pointer by 1
        advanceBufferPointer(self, (Wall_t**)&self->head);
        return WALL_BUFFER_NO_ERROR;
    }

    return WALL_BUFFER_OVERFLOW;
}

WallBufferError_t WallBuffer_removeWall(WallBuffer_t* self, Wall_t* removedItem) {
    // Check if the buffer is not empty
    if(self->numItems > 0) {
        // Decrement the number of items
        --self->numItems;
        // Return the removed item, if necessary
        if(removedItem) {
            *removedItem = *self->tail;
        }
        // Advance the tail pointer by 1 array slot
        advanceBufferPointer(self, (Wall_t**)&self->tail);
        return WALL_BUFFER_NO_ERROR;
    }

    return WALL_BUFFER_UNDERFLOW;
}

void WallBuffer_emptyBuffer(WallBuffer_t* self) {
    self->numItems = 0;
    self->head = self->buffer;
    self->tail = self->buffer;
}
