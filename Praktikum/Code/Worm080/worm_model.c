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
int sizeChanged = 0;
// The current heading of the worm
// These are offsets from the set {-1,0,+1}



// *****************************************************
// Functions concerning the management of the worm data
// *****************************************************

// Getters
struct pos getWormHeadPos(struct worm* aworm) {
    // Structures are passed by value!
    // -> we return a copy here
    return aworm->wormpos[aworm->headindex];
}

int getWormLength(struct worm* aworm) {
    return aworm->cur_lastindex;
}

// Initialize the worm
enum ResCodes initializeWorm(struct worm* aworm, int len_max, int len_cur, struct pos headpos, enum WormHeading dir, enum ColorPairs color) {
    int i;
    // Initialize last usable index to len_max -1
    aworm->maxindex = len_max - 1 ;
    // Current last usable index in array. May grow upto maxindex
    aworm->cur_lastindex = len_cur;
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

int globalGrowth;

void growWorm(struct worm* aworm, enum Boni growth) {
    globalGrowth = growth;
    aworm->cur_lastindex += growth;
    /* Play it safe and inhibit surpassing the bound
    if (aworm->cur_lastindex + growth <= aworm->maxindex) {
        aworm->cur_lastindex += growth;
    } else {
        aworm->cur_lastindex = aworm->maxindex;
    }
    sizeChanged = 1;
    */
}

// Show the worms's elements on the display
// Simple version
//
// Hier werden actually die 0-chars angezeigt an der passenden Stelle des Bildschirms via ncurses angezeigt 
//

void showWorm(struct board* aboard, struct worm* aworm) {
        initializeLevel(aboard);
        int thingy = 0; 

        if(COLS >= 80) {
            thingy = 15; 
        } else {
            thingy = 1;
        }
        
        for(int y = 0; y <= aboard->last_row+5; y++) {
            for(int i = 1; i <= thingy; i++) {
                move(y, aboard->last_col+i);
                attron(COLOR_PAIR(COLP_FREE_CELL));
                addch(SYMBOL_FREE_CELL);
                attroff(COLOR_PAIR(COLP_FREE_CELL));
            }
        }


    for(int i = 0; i <= getWormLength(aworm) - 1; i++) {
        if(aworm->wormpos[i].y != UNUSED_POS_ELEM) {
            placeItem(aboard, aworm->wormpos[i].y, aworm->wormpos[i].x, BC_USED_BY_WORM,SYMBOL_WORM_INNER_ELEMENT, aworm->wcolor);
        }
    }

    if(aworm->wormpos[aworm->headindex].y != UNUSED_POS_ELEM) {
        placeItem(aboard, aworm->wormpos[aworm->headindex].y, aworm->wormpos[aworm->headindex].x, BC_USED_BY_WORM,SYMBOL_WORM_HEAD_ELEMENT, aworm->wcolor);
    }

    if(!sizeChanged) {
        if(aworm->headindex + 1 == aworm->cur_lastindex) {
            placeItem(aboard, aworm->wormpos[0].y, aworm->wormpos[0].x, BC_USED_BY_WORM,SYMBOL_WORM_TAIL_ELEMENT,aworm->wcolor);
        } else {
            if(aworm->wormpos[aworm->headindex+1].y != UNUSED_POS_ELEM) {
                placeItem(aboard, aworm->wormpos[aworm->headindex+1].y, aworm->wormpos[aworm->headindex+1].x, BC_USED_BY_WORM,SYMBOL_WORM_TAIL_ELEMENT, aworm->wcolor);
            }
        }
    } 

    if(sizeChanged) {
        for (int y = 0; y <= aboard->last_row; y++) {
            for (int x = 0; x <= aboard->last_col ; x++) {
                if(aboard->cells[y][x] == 0) {
                    placeItem(aboard,y,x,BC_FREE_CELL,SYMBOL_FREE_CELL,COLP_FREE_CELL);
                }
            }
        }
    }
    
}

void moveWorm(struct board* aboard, struct worm* aworm,
        enum GameStates* agame_state) {
        struct pos headpos;

        // Get the current position of the worm's head element
        headpos = aworm->wormpos[aworm->headindex];
        // Compute new head position according to current heading.
        // Do not store the new head position in the array of positions, yet.
        headpos.x += aworm->dx;
        headpos.y += aworm->dy;

        // Check if we would hit something (for good or bad) or are going to leave
        // the display if we move the worm's head according to worm's
        // last direction.
        // We are not allowed to leave the display's window.
        if (headpos.x < 0) {
            *agame_state = WORM_OUT_OF_BOUNDS;
        } else if (headpos.x > getLastColOnBoard(aboard) ) {
            *agame_state = WORM_OUT_OF_BOUNDS;
        } else if (headpos.y < 0) {
            *agame_state = WORM_OUT_OF_BOUNDS;
        } else if (headpos.y > getLastRowOnBoard(aboard) ) {
            *agame_state = WORM_OUT_OF_BOUNDS;
        } else {
            // We will stay within bounds.
            // Check if the worm's head will collide with itself at the new position
            // Hitting food is good, hitting barriers or worm elements is bad.
            switch (getContentAt(aboard,headpos)) {
                case BC_FOOD_1:
                    *agame_state = WORM_GAME_ONGOING;
                    // Grow worm according to food item digested
                    growWorm(aworm, BONUS_1);
                    decrementNumberOfFoodItems(aboard);
                    break;
                case BC_FOOD_2:
                    *agame_state = WORM_GAME_ONGOING;
                    // Grow worm according to food item digested
                    growWorm(aworm, BONUS_2);
                    decrementNumberOfFoodItems(aboard);
                    break;
                case BC_FOOD_3:
                    *agame_state = WORM_GAME_ONGOING;
                    // Grow worm according to food item digested
                    growWorm(aworm, BONUS_3);
                    decrementNumberOfFoodItems(aboard);
                    break;
                case BC_BARRIER:
                    // That's bad: stop game
                    *agame_state = WORM_CRASH;
                    break;
                case BC_USED_BY_WORM:
                    // That's bad: stop game
                    *agame_state = WORM_CROSSING;
                    break;
                default:
                    // Without default case we get a warning message.
                    {;} // Do nothing. C syntax dictates some statement, here.
            }
        }
    
        // Check the status of *agame_state
        // Go on if nothing bad happened
        if ( *agame_state == WORM_GAME_ONGOING ) {
            // So all is well: we did not hit anything bad and did not leave the
            // window. --> Update the worm structure.
            // Increment headindex
            // Go round if end of worm is reached (ring buffer)
            aworm->headindex++;

            if (aworm->headindex >= aworm->cur_lastindex) {
                aworm->headindex = 0;
                sizeChanged = 0;
            }

            // Store new coordinates of head element in worm structure
            aworm->wormpos[aworm->headindex] = headpos;
            napms(NAP_TIME);
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

void cleanWormTail(struct board* aboard, struct worm* aworm) { 
    if(aworm->headindex == aworm->cur_lastindex-1) {
        if(aworm->wormpos[0].y != UNUSED_POS_ELEM) {
            placeItem(aboard, aworm->wormpos[0].y, aworm->wormpos[0].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
        }
    } else {
        if(aworm->wormpos[aworm->headindex+1].y != UNUSED_POS_ELEM) {
            placeItem(aboard, aworm->wormpos[aworm->headindex+1].y, aworm->wormpos[aworm->headindex+1].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
        }
    }

/*
    // Check the array of worm elements.
    // Is the array element at tailindex already in use?
    // Checking either array theworm_wormpos_y
    // or theworm_wormpos_x is enough.
    if ( aworm->wormpos[tailindex].x != UNUSED_POS_ELEM ) {
        // YES: place a SYMBOL_FREE_CELL at the tail's position
        placeItem(aboard, aworm->wormpos[1].y, aworm->wormpos[1].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
        placeItem(aboard, aworm->wormpos[2].y, aworm->wormpos[2].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
        placeItem(aboard, aworm->wormpos[3].y, aworm->wormpos[3].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
        placeItem(aboard, aworm->wormpos[4].y, aworm->wormpos[3].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
    }
    */
}



// Entscheiden in welche Richtung der Wurm als naechstes geht, indem die WORM_X(Durch Arrow-Key Input Capture entschieden) interpretiert wird und die Variablen aworm->dx/y anpasst, welche dann in der main loop die Print Richtung
// enscheiden
void setWormHeading(struct worm* aworm, enum WormHeading dir) {
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
