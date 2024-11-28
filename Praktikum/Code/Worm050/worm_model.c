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

// Array of y positions for worm elements
int theworm_wormpos_y[WORM_LENGTH];
// Array of x positions for worm elements
int theworm_wormpos_x[WORM_LENGTH];
// Last usable index into the arrays
// theworm_wormpos_y and theworm_wormpos_x
int theworm_maxindex = 0;

int theworm_headindex = 0;
int tailindex = 0;

// The current heading of the worm
// These are offsets from the set {-1,0,+1}
int theworm_dx;
int theworm_dy;

Color_Pairs theworm_wcolor; 


// *****************************************************
// Functions concerning the management of the worm data
// *****************************************************


// Initialize the worm
Res_Codes initializeWorm(int len_max, int headpos_y, int headpos_x, Worm_Heading dir, Color_Pairs color) {
    // Initialize last usable index to len_max -1
    theworm_maxindex = len_max - 1;
    // Mark all elements as unused in the arrays of positions
    // theworm_wormpos_y[] and theworm_wormpos_x[]
    // An unused position in the array is marked
    // with code UNUSED_POS_ELEM
    for(int i = 0; i <= theworm_maxindex; i++) {
        theworm_wormpos_x[i] = UNUSED_POS_ELEM;
    }
    for(int i = 0; i <= theworm_maxindex; i++) {
        theworm_wormpos_y[i] = UNUSED_POS_ELEM;
    }
    
    // Reset to 0 because I want to use it as a counter for the amount of worm "0"s are displayed rn
    theworm_maxindex = 0;

    // Initialize position of worms head
    theworm_wormpos_x[theworm_headindex] = 0;
    theworm_wormpos_y[theworm_headindex] = getLastRow()-2;
    
    // Initialize the heading of the worm
    setWormHeading(dir);
    // Initialize color of the worm
    theworm_wcolor = color;
    return RES_OK;
}

// Show the worms's elements on the display
// Simple version
//
// Hier werden actually die 0-chars angezeigt an der passenden Stelle des Bildschirms via ncurses angezeigt 
//
void showWorm() {
    // Due to our encoding we just need to show the head element
    // All other elements are already displayed
    placeItem(
            theworm_wormpos_y[theworm_maxindex],
            theworm_wormpos_x[theworm_maxindex],
            SYMBOL_WORM_INNER_ELEMENT,theworm_wcolor);
}

void moveWorm(Game_States* agame_state) {
    // Get the current position of the worm's head element and
    // compute the new head position according to current heading.
    // Do not store the new head position in the array of positions, yet.
    int headpos_x = theworm_wormpos_x[theworm_headindex] + theworm_dx;
    int headpos_y = theworm_wormpos_y[theworm_headindex] + theworm_dy;

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
        if (isInUseByWorm(headpos_y, headpos_x)) {
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
        theworm_headindex += 1;
       
        //Check if the worm is at max length and if so, rearrange the array by shifting it 1 to the left
        if(theworm_headindex == WORM_LENGTH) {
            for(int i = 0; i < WORM_LENGTH; i++) {
                theworm_wormpos_x[i] = theworm_wormpos_x[i+1];
            }
            for(int i = 0; i < WORM_LENGTH; i++) {
                theworm_wormpos_y[i] = theworm_wormpos_y[i+1];
            }
            theworm_headindex -= 1;
            theworm_maxindex -= 1;
        }

        // Store new coordinates of head element in worm structure
        theworm_wormpos_x[theworm_headindex] = headpos_x;
        theworm_wormpos_y[theworm_headindex] = headpos_y;
    }
}


// A simple collision detection
bool isInUseByWorm(int new_headpos_y, int new_headpos_x) {
    int i;
    bool collision = false;
    i = theworm_headindex;
    // Compare the position of the current worm element with the new_headpos
    do {        
        if(theworm_wormpos_x[i] == new_headpos_x && theworm_wormpos_y[i] == new_headpos_y) {
            collision = true;
        }
        i-=1;
    } while ( i != theworm_headindex && theworm_wormpos_x[i] != UNUSED_POS_ELEM);

    // Return what we found out.
    return collision;
}

void cleanWormTail() {
    // Compute tailindex
    if(theworm_headindex >= WORM_LENGTH) {
        tailindex = 1;
    }

    tailindex = tailindex + 0;
    // Check the array of worm elements.
    // Is the array element at tailindex already in use?
    // Checking either array theworm_wormpos_y
    // or theworm_wormpos_x is enough.
    if ( theworm_wormpos_x[tailindex] != UNUSED_POS_ELEM ) {
        // YES: place a SYMBOL_FREE_CELL at the tail's position
        placeItem(theworm_wormpos_y[tailindex], theworm_wormpos_x[tailindex], SYMBOL_FREE_CELL, COLP_FREE_CELL);
    }
}



// Entscheiden in welche Richtung der Wurm als naechstes geht, indem die WORM_X(Durch Arrow-Key Input Capture entschieden) interpretiert wird und die Variablen theworm_dx/y anpasst, welche dann in der main loop die Print Richtung
// enscheiden
void setWormHeading(Worm_Heading dir) {
    switch(dir) {
        case WORM_UP :// User wants up
            theworm_dx=0;
            theworm_dy=-1;
            break;
        case WORM_DOWN :// User wants down
            theworm_dx=0;
            theworm_dy=1;
            break;
        case WORM_LEFT      :// User wants left
            theworm_dx=-1;
            theworm_dy=0;           
            break;
        case WORM_RIGHT      :// User wants right
            theworm_dx=1;
            theworm_dy=0;
            break;
    }
} 
