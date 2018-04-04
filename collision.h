/*
 * collision.h
 *
 *  Created on: Dec 2, 2016
 *      Author: boer8364
 */

#ifndef COLLISION_H_
#define COLLISION_H_

#include <inttypes.h>
#include "player.h"
#include "vector2d.h"
#include "wall.h"

// Constant vector that holds the coordinates of the center of the screen
extern const Vector2d_t SCREEN_CENTER;

// Enumeration representing different collision-related events
typedef enum CollisionCode {
    NO_COLLISION = 0,
    WALL_COLLISION,
    GAP_COLLISION
} CollisionCode_t;

/* Function that checks if an angle is within an interval between startAngle
 * and endAngle, it checks to see if the given angle is clockwise of starAngle
 * and counter-clockwise of endAngle, all angles should be within the bound
 * [-pi, pi] */
uint8_t angleWithinInterval(double angle, double startAngle, double endAngle);
/* Function that checks for a collision between a player and a wall and returns
 * the appropriate collision code */
CollisionCode_t collision(Player_t* player, Wall_t* wall);
/* Function that calculates a normal vector that points toward the center of
 * the screen */
Vector2d_t calculateNormalVector(Player_t* player, Vector2d_t* gravityVector, int16_t wallSpeed);

#endif /* COLLISION_H_ */
