// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//

#ifndef _WORM_H
#define _WORM_H

// ********************************************************************************************
// Constants, data structures
// ********************************************************************************************

// Result codes of functions

typedef enum {
    RES_OK,
    RES_FAILED,
} Res_Codes;

// Dimensions and bounds
#define NAP_TIME    100   // Time in milliseconds to sleep between updates of display
#define MIN_NUMBER_OF_ROWS  3   // The guaranteed number of rows available for the board
#define MIN_NUMBER_OF_COLS 10   // The guaranteed number of columns available for the board
#define WORM_LENGTH 20+1 // Maximal length of the worm. Length here is 20: the + 1 is needed because I'm lazy

// ### Codes for the array of positions ###
// Unused element in the worm arrays of positions
#define UNUSED_POS_ELEM -1

// Numbers for color pairs used by curses macro COLOR_PAIR
typedef enum {
    COLP_USER_WORM = 1,
    COLP_FREE_CELL,
} Color_Pairs;

// Symbols to display
#define SYMBOL_FREE_CELL ' '
#define SYMBOL_WORM_INNER_ELEMENT '0'


// Game state codes
typedef enum {
    WORM_GAME_ONGOING,
    WORM_OUT_OF_BOUNDS,    // Left screen
    WORM_GAME_QUIT,        // User likes to quit
    WORM_CROSSING, // Worm head crossed another worm element
} Game_States;

#endif  // #define _WORM_H
