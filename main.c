//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include <inttypes.h>
#include <stdlib.h>
#include "bitmaps.h"
#include "msp.h"
#include "player.h"
#include "wall.h"
#include "collision.h"
#include "globalMacros.h"
#include "lcdDriver.h"
#include "hardwareConfig.h"
#include "myISR.h"
#include "wallBuffer.h"
#include "uartLogger.h"

// Buffer holding wall objects
volatile WallBuffer_t wallBuffer;

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    // Initialize the player
    Player_t player;
    // Start the player in the center of the screen
    Vector2d_t initialPlayerPosition = {
        ((double)LCD_SCREEN_WIDTH) / 2.0,
        ((double)LCD_SCREEN_HEIGHT) / 2.0
    };
    Player_init(&player, &initialPlayerPosition);

    // Initialize the wall buffer
    WallBuffer_init((WallBuffer_t*)&wallBuffer, WALL_BUFFER_SIZE);

    // Create the stationary boundary
    Wall_t gameBoundary;
    // Do not have a gap, this keeps the player from leaving the screen
    Wall_init(&gameBoundary, 0.0, 0.0);

    // Configure peripherals
    configureADC();
    configureTimer();
    configurePins();

#ifdef UART_DEBUG
    configureUART();
#endif

    enableButtonInterrupts();
    LCD_init();

    // Prepare the game background color
    LCD_setBackgroundColor(MAKE_COLOR16(0, 0, 31));

    // Globally enable interrupts
    __enable_interrupt();

    while(1) {
        // Draw the title screen
        LCD_sendCustomBuffer(START_SCREEN_BITMAP);
        // Block until the button has been pressed
        while(!buttonPressed);
        // Disable button interrupts
        disableButtonInterrupts();

        // Seed the random number generator
        srand((unsigned int)TIMER_A0->R);

        // Rest the timer count
        resetTimer();
        // Enable timer interrupts
        enableTimerInterrupts();

        while(1) {

#ifdef TIMING_INFO_DEBUG
#ifndef UART_DEBUG
            // Stop the timer
            TIMER_A1->CTL &= ~TIMER_A_CTL_MC_3;
#endif
            TIMER_A1->R = 0;
#ifndef UART_DEBUG
            // Start the timer
            TIMER_A1->CTL |= TIMER_A_CTL_MC_2;
#endif
#endif

            // Request an ADC sample at the start of the game loop
            ADC14->CTL0 |= ADC14_CTL0_SC;

            // Compute gravity vector using the latest ADC samples
            Vector2d_t movementVector = {
                (double)adc14Readings.lastXReading,
                (double)-adc14Readings.lastYReading
            };

            /* Set the magnitude of the vector so that it can be used as a
             * proper velocity vector directly */
            double accelerometerMagnitude = Vector2d_getMagnitude(&movementVector);
            double movementMagnitude = (PLAYER_SPEED * accelerometerMagnitude) / HARDWARE_CONFIG_ADC_MAX_MAGNITUDE;
            Vector2d_setMagnitude(&movementVector, movementMagnitude);

            /* ***** Collision checking section ***** */

            // Check if there is anything in the buffer
            if(wallBuffer.numItems > 0) {
                // The inner-most wall is at the tail, so just check the tail
                CollisionCode_t collisionResult = collision(&player,
                    (Wall_t*)wallBuffer.tail);

                // Handle the collision accordingly
                if(collisionResult == WALL_COLLISION) {
                    /* Calculate a normal vector to add to the movement vector
                     * so that the player does not bypass the wall */
                    Vector2d_t normalVector = calculateNormalVector(&player,
                        &movementVector, WALL_SPEED);
                    // Add the calculated normal vector
                    Vector2d_selfAdd(&movementVector, &normalVector);
                } else if(collisionResult == GAP_COLLISION) {
                    // If the player breached the wall, despawn it
                    WallBuffer_removeWall((WallBuffer_t*)&wallBuffer, NULL);

#ifdef UART_DEBUG
                    UART_Logger_sendString("Wall Despawned\r");
#endif

                    /* Increase the difficulty of the game, if it is not at
                     * maximum difficulty yet */
                    if(TIMER_A0->CCR[0] > WALL_SPAWN_PERIOD_MIN) {
                        // Disable timer interrupts when changing settings
                        disableTimerInterrupts();

                        // Drop the wall spawn period (speed up spawning)
                        TIMER_A0->CCR[0] -= WALL_SPAWN_PERIOD_DECREMENT;

                        /* Check if the current timer count is above the new
                         * threshold */
                        if(TIMER_A0->R > TIMER_A0->CCR[0]) {
                            /* This should guarantee the timer count will be
                             * below the threshold */
                            TIMER_A0->R -= WALL_SPAWN_PERIOD_DECREMENT;
                        }

                        // Re-enable timer interrupts
                        enableTimerInterrupts();
                    }
                }
            }

            // Check if the player collided with the outer, stationary wall
            CollisionCode_t boundaryCollision = collision(&player, &gameBoundary);
            if(boundaryCollision == WALL_COLLISION) {
                /* Calculate a normal vector for the stationary wall so that
                 * it does not go out of bounds */
                Vector2d_t normalVector = calculateNormalVector(&player,
                    &movementVector, 0);
                // Add the normal vector to the movement vector
                Vector2d_selfAdd(&movementVector, &normalVector);
            }

            // Move the player using the calculated movement vector
            Player_move(&player, &movementVector);

            // Draw the boundary wall
            Wall_draw(&gameBoundary);

            // Draw the walls in the wall buffer
            // Iterator pointer
            Wall_t* iter = (Wall_t*)wallBuffer.tail;
            unsigned int i = 0;
            for(; i < wallBuffer.numItems; ++i) {
                // Close the walls in at the according speed
                iter->radius -= WALL_SPEED;
                // Draw the wall
                Wall_draw(iter);
                // Advance the iterator pointer
                advanceBufferPointer((WallBuffer_t*)&wallBuffer, &iter);
            }

            // If the last wall (the tail) closes in, the player loses
            if(wallBuffer.numItems > 0 &&
               wallBuffer.tail->radius <= PLAYER_RADIUS + 1) {
                /* Disable timer interrupts: there is no need to spawn anymore
                 * walls */
                disableTimerInterrupts();
                // Clear the contents of the wall buffer
                WallBuffer_emptyBuffer((WallBuffer_t*)&wallBuffer);
                // Leave the game loop
                break;
            }

            // Draw the player
            Player_draw(&player);

            // Send the buffer contents to the LCD
            LCD_sendAndClearBuffer();

#ifdef UART_DEBUG
#ifdef TIMING_INFO_DEBUG
            UART_Logger_sendByte((uint8_t)'[');
            UART_Logger_sendNumSigned((int32_t)TIMER_A1->R);
            UART_Logger_sendString("] ");
#endif
            UART_Logger_sendString("End of frame\r");
#endif
        }
        // Re-enable button interrupts to get passed the game over screen
        enableButtonInterrupts();
        // Draw the game over screen
        LCD_sendCustomBuffer(END_SCREEN_BITMAP);

        // Block until the button is pushed
        while(!buttonPressed);
        // Reset the button interrupt
        buttonPressed = 0;

        // Reset the timer threshold to minimum difficulty
        TIMER_A0->CCR[0] = WALL_SPAWN_PERIOD_MAX;
    }
}
