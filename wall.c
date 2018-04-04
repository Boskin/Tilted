/*
 * wall.c
 *
 *  Created on: Nov 28, 2016
 *      Author: boer8364
 */

#include "wall.h"

void Wall_init(Wall_t* self, double gapStartAngle, double gapEndAngle) {
    // Initializes the wall object
    self->gapStartAngle = gapStartAngle;
    self->gapEndAngle = gapEndAngle;
    self->radius = WALL_INITIAL_RADIUS;
}

void Wall_draw(Wall_t* self) {
    // Set the foreground color
    LCD_setForegroundColor(WALL_WALL_COLOR);
    // Draw the wall as a full circle first
    LCD_drawCircle(WALL_CENTER_X, WALL_CENTER_Y, self->radius);
    // Set the background color
    LCD_setForegroundColor(WALL_GAP_COLOR);
    // Draw the gap by drawing over the wall
    LCD_drawArc(WALL_CENTER_X, WALL_CENTER_Y, self->radius,
        self->gapStartAngle, self->gapEndAngle);
}
