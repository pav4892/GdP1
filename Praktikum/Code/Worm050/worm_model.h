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

#include <stdbool.h>
#include "worm.h"

#ifndef _WORM_MODEL_H
#define _WORM_MODEL_H

// Directions for the worm
typedef enum {
    WORM_UP,
    WORM_DOWN,
    WORM_LEFT,
    WORM_RIGHT,
} Worm_Heading;

extern int theworm_maxindex;
// Functions concerning the management of the worm data
extern Res_Codes initializeWorm(int len_max, int headpos_y, int headpos_x, Worm_Heading dir, Color_Pairs color);
extern void showWorm();
extern void moveWorm(Game_States* agame_state);
extern void setWormHeading(Worm_Heading dir);
extern void cleanWormTail();
extern bool isInUseByWorm(int new_headpos_y, int new_headpos_x);


#endif  // #define _WORM_MODEL_H
