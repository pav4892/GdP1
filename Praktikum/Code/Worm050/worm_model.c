// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//
// The worm model

#include <curses.h>
#include "worm.h"
#include "board_model.h"
#include "worm_model.h"

// ********************************************************************************************
// Global variables
// ********************************************************************************************

int tailindex = 0;

// The current heading of the worm
// These are offsets from the set {-1,0,+1}



// *****************************************************
// Functions concerning the management of the worm data
// *****************************************************


// Initialize the worm
Res_Codes initializeWorm(struct worm* aworm, int len_max, struct pos headpos, Worm_Heading dir, Color_Pairs color) {
    int i;
    // Initialize last usable index to len_max -1
    aworm->maxindex = len_max - 1 ;
    // Initialize headindex
    aworm->headindex = 0; // Index pointing to head position is set to 0
    // Mark all elements as unused in the array of positions.
    // This allows for the effect that the worm appears element by element at
    // the start of each level
    for (i = 0; i <= aworm->maxindex; i++) {
    aworm->wormpos[i].y = UNUSED_POS_ELEM;
    aworm->wormpos[i].x = UNUSED_POS_ELEM;
    }
    // Initialize position of worms head
    aworm->wormpos[aworm->headindex] = headpos;
    // Initialize the heading of the worm
    setWormHeading(aworm, dir);
    // Initialze color of the worm
    aworm->wcolor = color;
    return RES_OK;
}
// Show the worms's elements on the display
// Simple version
//
// Hier werden actually die 0-chars angezeigt an der passenden Stelle des Bildschirms via ncurses angezeigt 
//
void showWorm(struct worm* aworm) {
    // Due to our encoding we just need to show the head element
    // All other elements are already displayed
    placeItem(
            aworm->wormpos[aworm->headindex].y,
            aworm->wormpos[aworm->headindex].x,
            SYMBOL_WORM_INNER_ELEMENT,aworm->wcolor);
}

void moveWorm(struct worm* aworm, Game_States* agame_state) {
    // Get the current position of the worm's head element and
    // compute the new head position according to current heading.
    // Do not store the new head position in the array of positions, yet.
    int headpos_x = aworm->wormpos[aworm->headindex].x + aworm->dx;
    int headpos_y = aworm->wormpos[aworm->headindex].y + aworm->dy;

    struct pos newHeadposStruct = {
        .x = headpos_x,
        .y = headpos_y
    };

    // Check if we would hit something (for good or bad) or are going to leave
    // the display if we move the worm's head according to worm's last
    // direction. We are not allowed to leave the display's window.
    if (headpos_x < 0) {
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else if (headpos_x > getLastCol() ) {
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else if (headpos_y < 0) {
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else if (headpos_y > getLastRow() ) {
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else {
        // We will stay within bounds.
        // Check if the worm's head will collide with itself at the new position
        if (isInUseByWorm(aworm, newHeadposStruct)) {
            // That's bad: stop game
            *agame_state = WORM_CROSSING;
        }
    }
    // Check the status of *agame_state
    // Go on if nothing bad happened
    if ( *agame_state == WORM_GAME_ONGOING ) {
        // So all is well: we did not hit anything bad and did not leave the
        // window. --> Update the worm structure.
        // Increment theworm_headindex
        // Go round if end of worm is reached (ring buffer)
        aworm->headindex += 1;
       
        //Check if the worm is at max length and if so, rearrange the array by shifting it 1 to the left
        if(aworm->headindex == WORM_LENGTH) {
            for(int i = 0; i < WORM_LENGTH; i++) {
                aworm->wormpos[i].x = aworm->wormpos[i+1].x;
            }
            for(int i = 0; i < WORM_LENGTH; i++) {
                aworm->wormpos[i].y = aworm->wormpos[i+1].y;
            }
            aworm->headindex -= 1;
            aworm->maxindex -= 1;
        }

        // Store new coordinates of head element in worm structure
        aworm->wormpos[aworm->headindex].x = headpos_x;
        aworm->wormpos[aworm->headindex].y = headpos_y;
    }
}


// A simple collision detection
bool isInUseByWorm(struct worm* aworm, struct pos new_headpos) {
    bool collision = false;

    ///Using my own simpler and easier to understand collision detection system

        for(int i = 0; i <= aworm->headindex; i++) {
        if(new_headpos.x == aworm->wormpos[i].x && new_headpos.y == aworm->wormpos[i].y) {
            collision = true;
        }

    }

    // Return what we found out.
    return collision;
}

void cleanWormTail(struct worm* aworm) {
    // Compute tailindex
    if(aworm->headindex >= WORM_LENGTH) {
        tailindex = 1;
    }

    tailindex = tailindex + 0;
    // Check the array of worm elements.
    // Is the array element at tailindex already in use?
    // Checking either array theworm_wormpos_y
    // or theworm_wormpos_x is enough.
    if ( aworm->wormpos[tailindex].x != UNUSED_POS_ELEM ) {
        // YES: place a SYMBOL_FREE_CELL at the tail's position
        placeItem(aworm->wormpos[tailindex].y, aworm->wormpos[tailindex].x, SYMBOL_FREE_CELL, COLP_FREE_CELL);
    }
}



// Entscheiden in welche Richtung der Wurm als naechstes geht, indem die WORM_X(Durch Arrow-Key Input Capture entschieden) interpretiert wird und die Variablen aworm->dx/y anpasst, welche dann in der main loop die Print Richtung
// enscheiden
void setWormHeading(struct worm* aworm, Worm_Heading dir) {
    switch(dir) {
        case WORM_UP :// User wants up
            aworm->dx=0;
            aworm->dy=-1;
            break;
        case WORM_DOWN :// User wants down
            aworm->dx=0;
            aworm->dy=1;
            break;
        case WORM_LEFT      :// User wants left
            aworm->dx=-1;
            aworm->dy=0;           
            break;
        case WORM_RIGHT      :// User wants right
            aworm->dx=1;
            aworm->dy=0;
            break;
    }
} 
