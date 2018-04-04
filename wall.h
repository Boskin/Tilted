/*
 * wall.h
 *
 *  Created on: Nov 28, 2016
 *      Author: boer8364
 */

#ifndef WALL_H_
#define WALL_H_

#include "lcdDriver.h"
#include "globalMacros.h"

// All walls will be centered at these coordinates
#define WALL_CENTER_X 63
#define WALL_CENTER_Y 63

// All walls' radii will close in at this rate (per frame)
#define WALL_SPEED 1

// All walls will start with this radius
#define WALL_INITIAL_RADIUS 63

/* All walls will be colored this way, the gap color will be the background
 * color */
#define WALL_WALL_COLOR MAKE_COLOR16(31, 63, 12)
#define WALL_GAP_COLOR MAKE_COLOR16(0, 0, 31)

// This is used in the generation of random wall angles
#define WALL_ANGLE_GENERATION_RESOLUTION 32

// All walls' gaps will have this angular arc length
#define WALL_GAP_ANGULAR_LENGTH CONSTANT_PI / 5

// The rate at which walls spawn will be bounded by these values
#define WALL_SPAWN_PERIOD_MAX 49152
#define WALL_SPAWN_PERIOD_MIN 15152
// Walls will spawn this many timer clock cycles faster after each one despawns
#define WALL_SPAWN_PERIOD_DECREMENT 2000

#include <inttypes.h>

typedef struct Wall {
    // Gap bounds, gapStartAngle is always counter-clockwise of gapEndAngle
    double gapStartAngle;
    double gapEndAngle;
    // Current radius of the wall
    int16_t radius;
} Wall_t;

/* Initialize the wall with the given parameters, gapStartAngle must be
 * counter-clockwise of gapEndAngle, the angles are bounded in the interval
 * [-pi, pi] */
void Wall_init(Wall_t* self, double gapStartAngle, double gapEndAngle);
// Draw the wall onto the LCD screen
void Wall_draw(Wall_t* self);

#endif
