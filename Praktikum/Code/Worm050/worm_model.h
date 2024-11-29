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
#include "board_model.h"

#ifndef _WORM_MODEL_H
#define _WORM_MODEL_H

// A worm structure
struct worm {
    int maxindex; // Last usable index into the array pointed to by wormpos
    int headindex; // An index into the array for the worm's head position
    // 0 <= headindex <= maxindex
    struct pos wormpos[WORM_LENGTH]; // Array of x,y positions of all elements
    // of the worm
    // The current heading of the worm
    // These are offsets from the set {-1,0,+1}
    int dx;
    int dy;
    // Color of the worm
    Color_Pairs wcolor;
};

// Directions for the worm
typedef enum {
    WORM_UP,
    WORM_DOWN,
    WORM_LEFT,
    WORM_RIGHT,
} Worm_Heading;

extern int theworm_maxindex;
// Functions concerning the management of the worm data
extern Res_Codes initializeWorm(struct worm* aworm, int len_max, struct pos headpos, Worm_Heading dir, Color_Pairs color);
extern void showWorm(struct worm* aworm);
extern void cleanWormTail(struct worm* aworm);
extern void moveWorm(struct worm* aworm, Game_States* agame_state);
extern bool isInUseByWorm(struct worm* aworm, struct pos new_headpos);
extern void setWormHeading(struct worm* aworm, Worm_Heading dir);


#endif  // #define _WORM_MODEL_H
