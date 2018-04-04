/*
 * player.h
 *
 *  Created on: Nov 25, 2016
 *      Author: boer8364
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <inttypes.h>
#include "lcdDriver.h"
#include "vector2d.h"

// Standard player radius
#define PLAYER_RADIUS 4
// Maximum player speed
#define PLAYER_SPEED 5
// Player color
#define PLAYER_COLOR MAKE_COLOR16(0, 61, 0)

// Structure representing a player
typedef struct Player {
    // The current player position
    Vector2d_t position;
} Player_t;

// Initializes a player to the given position
void Player_init(Player_t* self, Vector2d_t* position);
// Moves a player by the given velocity vector
void Player_move(Player_t* self, Vector2d_t* movementVector);
// Draws the player
void Player_draw(Player_t* self);

#endif /* PLAYER_H_ */
