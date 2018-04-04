/*
 * collision.c
 *
 *  Created on: Dec 2, 2016
 *      Author: boer8364
 */

#include "collision.h"
#include "lcdDriver.h"
#include "msp.h"
#include <math.h>
#include <inttypes.h>

#ifndef CONSTANT_PI
#define CONSTANT_PI 3.1415926535
#endif

const Vector2d_t SCREEN_CENTER = {LCD_SCREEN_WIDTH / 2 - 1, LCD_SCREEN_HEIGHT / 2 - 1};

uint8_t angleWithinInterval(double angle, double startAngle, double endAngle) {
    if(endAngle > startAngle) {
        return angle > startAngle && angle < endAngle;
    } else if(endAngle < startAngle) {
        return angle > startAngle || angle < endAngle;
    } else {
        return angle == startAngle;
    }
}

CollisionCode_t collision(Player_t* player, Wall_t* wall) {
    // Position of the player with respect to the center of the screen
    Vector2d_t playerPositionCenter = Vector2d_subtract(&player->position,
        &SCREEN_CENTER);

    // Distance of the player from the center of the screen
    double distance = Vector2d_getMagnitude(&playerPositionCenter);

    // Check if the player's distance is at or past the wall's radius
    if(distance >= wall->radius - PLAYER_RADIUS - 1) {
        // Check the angle of the player with respect to the center of the screen
        double playerAngle = Vector2d_getDirection(&playerPositionCenter);
        // Check if it is within the wall's gap angle interval
        if(angleWithinInterval(playerAngle, wall->gapStartAngle, wall->gapEndAngle)) {
            // These are the coordinates of the endpoints of the wall's gap
            int16_t bX1 = (int16_t)(wall->radius * cos(wall->gapStartAngle));
            int16_t bY1 = (int16_t)(wall->radius * sin(wall->gapStartAngle));
            int16_t bX2 = (int16_t)(wall->radius * cos(wall->gapEndAngle));
            int16_t bY2 = (int16_t)(wall->radius * sin(wall->gapEndAngle));

            // Put them into vector structs
            Vector2d_t gapStart = {bX1, bY1};
            Vector2d_t gapEnd = {bX2, bY2};

            /* Find the distances between the gap endpoints and the player's
             * position by subtracting their position vectors and taking their
             * magnitudes */
            Vector2d_t gapStartDistanceV =
                Vector2d_subtract(&playerPositionCenter, &gapStart);
            Vector2d_t gapEndDistanceV =
                Vector2d_subtract(&playerPositionCenter, &gapEnd);

            double gapStartDistance = Vector2d_getMagnitude(&gapStartDistanceV);
            double gapEndDistance = Vector2d_getMagnitude(&gapEndDistanceV);

            /* If either distance is greater than the player radius, the
             * player must be well within the wall gap, so the player breached
             * the wall */
            if(gapStartDistance > PLAYER_RADIUS &&
                gapEndDistance > PLAYER_RADIUS) {
                return GAP_COLLISION;
            }
        }
        /* If the player's angle was not within the wall's gap, it simply hit
         * the surface of the wall */
        return WALL_COLLISION;
    }
    /* If the player distance has not surpassed the wall radius, no collision
     * occurred */
    return NO_COLLISION;
}

Vector2d_t calculateNormalVector(Player_t* player, Vector2d_t* gravity, int16_t wallMovementSpeed) {
    // Get the position of the player with respect to the center of the screen
    Vector2d_t playerPositionCenter = Vector2d_subtract(&player->position,
        &SCREEN_CENTER);
    // Get the player's angle with respect to the screen
    double playerAngle = Vector2d_getDirection(&playerPositionCenter);
    /* Calculate the magnitude of the normal vector as the magnitude of the
     * gravity vector plus the wall close-in speed */
    double normalMagnitude = Vector2d_getMagnitude(gravity) + wallMovementSpeed;

    /* Form the normal vector with the magnitude and the player angle rotated
     * pi radians (because the normal vector points in the direction of the
     * center of the screen */
    Vector2d_t normalVector = {0, 0};
    Vector2d_setMagnitude(&normalVector, normalMagnitude);
    Vector2d_setDirection(&normalVector, playerAngle + CONSTANT_PI);

    return normalVector;
}
