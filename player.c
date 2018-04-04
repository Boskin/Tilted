/*
 * player.c
 *
 *  Created on: Nov 25, 2016
 *      Author: boer8364
 */

#include "player.h"

void Player_init(Player_t* self, Vector2d_t* position) {
    self->position = *position;
}

void Player_move(Player_t* self, Vector2d_t* movementVector) {
    // Simply add the movement vector to the player position
    Vector2d_selfAdd(&self->position, movementVector);
}

void Player_draw(Player_t* self) {
    // Set the drawing color to the player drawing color
    LCD_setForegroundColor(PLAYER_COLOR);
    /* Fill a circle with the player radius subtracted by 1 so that collisions
     * make more visible sense */
    LCD_fillCircle((int16_t)self->position.x, (int16_t)self->position.y,
        PLAYER_RADIUS - 1);
}
