/*
 * vector2d.h
 *
 *  Created on: Nov 25, 2016
 *      Author: boer8364
 */

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

#include <inttypes.h>
#include <math.h>

// Structure representing a 2-component vector
typedef struct Vector2d {
    double x;
    double y;
} Vector2d_t;

// Computes and returns the magnitude of the given vector
inline double Vector2d_getMagnitude(Vector2d_t* self) {
    return sqrt((double)(self->x) * self->x + (double)(self->y) * self->y);
}

// Sets the magnitude of the vector while retaining its direction
void Vector2d_setMagnitude(Vector2d_t* self, double magnitude);

// Computes and returns the direction of the vector
inline double Vector2d_getDirection(Vector2d_t* self) {
    return atan2(self->y, self->x);
}

// Set the direction of the vector while retaining its magnitude
void Vector2d_setDirection(Vector2d_t* self, double direction);

// Adds the given vector to self
inline void Vector2d_selfAdd(Vector2d_t* self, const Vector2d_t* val) {
    self->x += val->x;
    self->y += val->y;
}

// Subtracts the given vector from self
inline void Vector2d_selfSubtract(Vector2d_t* self, const Vector2d_t* val) {
    self->x -= val->x;
    self->y -= val->y;
}

// Adds 2 vectors and returns the result
inline Vector2d_t Vector2d_add(const Vector2d_t* a, const Vector2d_t* b) {
    Vector2d_t sum;
    sum.x = a->x + b->x;
    sum.y = a->y + b->y;
    return sum;
}

// Subtracts 2 vectors and returns the result
inline Vector2d_t Vector2d_subtract(const Vector2d_t* a, const Vector2d_t* b) {
    Vector2d_t difference;
    difference.x = a->x - b->x;
    difference.y = a->y - b->y;
    return difference;
}

#endif /* VECTOR2D_H_ */
