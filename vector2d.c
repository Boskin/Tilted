/*
 * vector2d.c
 *
 *  Created on: Nov 25, 2016
 *      Author: boer8364
 */

#include "vector2d.h"

#include <math.h>

#ifndef CONSTANT_PI
#define CONSTANT_PI 3.1415926535
#endif

void Vector2d_setMagnitude(Vector2d_t* self, double magnitude) {
    // If either coordinate is 0, set the other to the magnitude
    if(self->y == 0.0) {
        self->x = magnitude;
    } else if(self->x == 0.0) {
        self->y = magnitude;
    } else {
        // Multiply the vector accordingly
        double currentMagnitude = Vector2d_getMagnitude(self);
        self->x = self->x * magnitude / currentMagnitude;
        self->y = self->y * magnitude / currentMagnitude;
    }
}

void Vector2d_setDirection(Vector2d_t* self, double direction) {
    // Get the current magnitude
    double currentMagnitude = Vector2d_getMagnitude(self);
    // Compute the components using the magnitude and new direction
    self->x = currentMagnitude * cos(direction);
    self->y = currentMagnitude * sin(direction);
}
