// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "prep.h"
#include "worm.h"
#include "worm_model.h"
#include "board_model.h"
#include "messages.h"

// Management of the game
void initializeColors();
void readUserInput(struct worm* aworm, Game_States* agame_state );
Res_Codes doLevel();

// ************************************
// Management of the game
// ************************************


// Initialize colors of the game
void initializeColors() {
    // Define colors of the game
    start_color();
    init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLP_FREE_CELL, COLOR_BLACK, COLOR_BLACK);
    init_pair(COLP_BARRIER, COLOR_RED, COLOR_BLACK);
}

void readUserInput(struct worm* aworm, Game_States* agame_state ) {

    //Capture the Arrow Keys and act accordingly

    int ch; // For storing the key codes

    if ((ch = getch()) > 0) {
        // Is there some user input?
        // Blocking or non-blocking depends of config of getch
        switch(ch) {
            case 'q' :    // User wants to end the show
                *agame_state = WORM_GAME_QUIT;
                break;
            case KEY_UP :// User wants up
                setWormHeading(aworm, WORM_UP);
                break;
            case KEY_DOWN :// User wants down
                setWormHeading(aworm, WORM_DOWN); 
                break;
            case KEY_LEFT :// User wants down
                setWormHeading(aworm, WORM_LEFT); 
                break;
            case KEY_RIGHT :// User wants right
                setWormHeading(aworm, WORM_RIGHT);
                break;
            case 's' : // User wants single step 
                nodelay(stdscr, FALSE);  // make getch to be a blocking call. We do that so I the single step is done by 1 keypress and then the execution order is blocked
                break;
            case ' ' : // Terminate single step; make getch non-blocking again
                nodelay(stdscr, TRUE);  // make getch to be a non-blocking call
                break;
        }
    }
    return;
}

Res_Codes doLevel() {

    // Das ist die Hauptfunktion und hier wird auch die main-loop gehandelt und viele andere Funktionen aufgerufen
    struct worm userworm; // Local variable for storing the user’s worm
    struct pos bottomLeft;
    Game_States game_state; // The current game_state

    Res_Codes res_code; // Result code from functions
    bool end_level_loop;    // Indicates whether we should leave the main loop

    // At the beginnung of the level, we still have a chance to win
    game_state = WORM_GAME_ONGOING;

    // There is always an initialized user worm.
    // Initialize the userworm with its size, position, heading.
    bottomLeft.y =  getLastRow();
    bottomLeft.x =  0;

    res_code = initializeWorm(&userworm, WORM_LENGTH, bottomLeft, WORM_RIGHT, COLP_USER_WORM);
    if ( res_code != RES_OK) {
    //    return res_code;
    }
    // Show border line in order to separate the message area
    showBorderLine();
    // Show worm at its initial position
    showWorm(&userworm);

    // Display all what we have set up until now
    refresh();
    // Start the loop for this level
    end_level_loop = false; // Flag for controlling the main loop
    while(!end_level_loop) {
        // Process optional user input
        readUserInput(&userworm, &game_state); 
        if ( game_state == WORM_GAME_QUIT ) {
            end_level_loop = 1;
            continue; // Go to beginning of the loop's block and check loop condition
        }
        // Process userworm
        if(userworm.headindex >= WORM_LENGTH-1) {
            cleanWormTail(&userworm);
        }
        // Now move the worm for one step
        moveWorm(&userworm, &game_state);
        // Bail out of the loop if something bad happened
        if ( game_state != WORM_GAME_ONGOING ) {
            end_level_loop = 1;
            continue; // Go to beginning of the loop's block and check loop condition
        }
        // Show the worm at its new position
        showWorm(&userworm);

        // Inform user about position and length of userworm in status window
        showStatus(&userworm);

        userworm.maxindex += 1;
        // END process userworm

        // Sleep a bit before we show the updated window
        napms(NAP_TIME);

        // Display all the updates
        refresh();

    }
    // Preset res_code for rest of the function
    res_code = RES_OK;

    // For some reason we left the control loop of the current level
    // Check why according to game_state
    switch (game_state) {
        case WORM_GAME_QUIT:
            // User must have typed 'q' for quit
            showDialog("Sie haben die aktuelle Runde abgebrochen!",
            "Bitte Taste druecken");
            break;
        case WORM_OUT_OF_BOUNDS:
            showDialog("Sie haben das Spiel verloren,"
            " weil Sie das Spielfeld verlassen haben",
            "Bitte Taste druecken");
            break;
        case WORM_CROSSING:
            showDialog("Sie haben das Spiel verloren,"
            " weil Sie einen Wurm gekreuzt haben",
            "Bitte Taste druecken");
            break;
        default:
        showDialog("Interner Fehler!","Bitte Taste druecken");
        // Set error result code. This should never happen.
        res_code = RES_INTERNAL_ERROR;
    }
    // For some reason we left the control loop of the current level.
    // However, in this version we do not yet check for the reason.
    // There is no user feedback at the moment!

    // Normal exit point
    return res_code;
}


// ********************************************************************************************
// MAIN
// ********************************************************************************************

int main(void) {
    Res_Codes res_code;         // Result code from functions

    // Here we start
    initializeCursesApplication();  // Init various settings of our application
    initializeColors();             // Init colors used in the game

    // Maximal LINES and COLS are set by curses for the current window size.
    // Note: we do not cope with resizing in this simple examples!

    // Check if the window is large enough to display messages in the message area
    // a has space for at least one line for the worm
    if ( LINES < MIN_NUMBER_OF_ROWS + ROWS_RESERVED || COLS < MIN_NUMBER_OF_COLS ) {
        // Since we not even have the space for displaying messages
        // we print a conventional error message via printf after
        // the call of cleanupCursesApp()
        cleanupCursesApp();
        printf("Das Fenster ist zu klein: wir brauchen mindestens %dx%d\n",
                MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS + ROWS_RESERVED);
        res_code = RES_FAILED;
    } else {
        res_code = doLevel();
        cleanupCursesApp();
    }
    return res_code;
}
