// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//
// The board model

#include <curses.h>
#include <stdlib.h>
#include "worm.h"
#include "board_model.h"
#include "messages.h"

// *************************************************
// Placing and removing items from the game board
// Check boundaries of game board
// *************************************************

// Place an item onto the curses display.
void placeItem(struct board* aboard, int y, int x, enum BoardCodes board_code, chtype symbol, enum ColorPairs color_pair) {

    //printf("y: %d,x: %d\n", y, x);
    //aboard->cells[y][x] = board_code;
    aboard->cells[y][x] = board_code;
    //  Store item on the display (symbol code)
    move(y, x);                         // Move cursor to (y,x)
    attron(COLOR_PAIR(color_pair));     // Start writing in selected color
    addch(symbol);                      // Store symbol on the virtual display
    attroff(COLOR_PAIR(color_pair));    // Stop writing in selected color
}

// Getters

// Get the last usable row on the display
int getLastRowOnBoard(struct board* aboard) {
    return aboard->last_row;
}

// Get the last usable column on the display
int getLastColOnBoard(struct board* aboard) {
    return aboard->last_col;
}

int getNumberOfFoodItems(struct board* aboard) {
    return aboard->food_items;
}

enum BoardCodes getContentAt(struct board* aboard, struct pos position) {
    return aboard->cells[position.y][position.x];
}

void setNumberOfFoodItems(struct board* aboard, int n) {
    aboard->food_items = n;
}

void decrementNumberOfFoodItems(struct board* aboard) {
    aboard->food_items-=1;
}

void free_array(enum BoardCodes** pa, int rows){
  // For each row free the array of its columns
  for (int row = 0; row < rows; row++) {
    if (pa[row] == NULL) { break; }
    free(pa[row]);
  }
  // Free the array of pointers
  free(pa);
}

void cleanupBoard(struct board* aboard) {
    free_array(aboard->cells, LINES);
}

enum ResCodes initializeBoard(struct board* aboard) {
    int y;
    // Maximal index of a row, reserve space for message area
    aboard->last_row = LINES - ROWS_RESERVED - 1;
    // Maximal index of a column
    aboard->last_col = COLS - 1;
    // Check dimensions of the board
    if ( aboard->last_col < MIN_NUMBER_OF_COLS - 1 ||
        aboard->last_row < MIN_NUMBER_OF_ROWS - 1) {
        char buf[100];
        sprintf(buf,"Das Fenster ist zu klein: wir brauchen %dx%d",
        MIN_NUMBER_OF_COLS , MIN_NUMBER_OF_ROWS + ROWS_RESERVED );
        showDialog(buf,"Bitte eine Taste druecken");
        return RES_FAILED;
    }
    // Allocate memory for 2-dimensional array of cells
    // Alloc array of rows
    aboard->cells = calloc(LINES, sizeof(enum BoardCodes*));
    if (aboard->cells == NULL) {
        showDialog("Abbruch: Zu wenig Speicher","Bitte eine Taste druecken");
        exit(RES_FAILED); // No memory -> direct exit
    }
    for (y = 0; y < LINES; y++) {
        // Allocate array of columns for each y
        aboard->cells[y] = malloc(COLS * sizeof(enum BoardCodes));
        if (aboard->cells[y] == NULL) {
            showDialog("Abbruch: Zu wenig Speicher","Bitte eine Taste druecken");
            free_array(aboard->cells, LINES);
            exit(RES_FAILED); // No memory -> direct exit
        }
    }
    return RES_OK;
}

enum ResCodes initializeLevel(struct board* aboard) {
    // Fill board and screen buffer with empty cells.
    for (int y = 0; y <= getLastRowOnBoard(aboard); y++) {
        for (int x = 0; x <= getLastColOnBoard(aboard) ; x++) {
            placeItem(aboard,y,x,BC_FREE_CELL,SYMBOL_FREE_CELL,COLP_FREE_CELL);
        }
    }
    // Draw a line in order to separate the message area
    // Note: we cannot use function placeItem() since the message area
    // is outside the board!
    int y = aboard->last_row + 1;
    for (int x = 0; x <= aboard->last_col ; x++) {
        move(y, x);
        attron(COLOR_PAIR(COLP_BARRIER));
        addch(SYMBOL_BARRIER);
        attroff(COLOR_PAIR(COLP_BARRIER));
    }
    
    // Barriers: use a loop
    for (int y = 100; y <= 13; y++) {
        int x = aboard->last_col/3;
        placeItem(aboard,y,x,BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);
    }
    for (int y = 300; y <= 10; y++) {
        int x = aboard->last_col-aboard->last_col/3;
        placeItem(aboard,y,x,BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);
    }
    // Food
    placeItem(aboard, 3, 3,BC_FOOD_1,SYMBOL_FOOD_1,COLP_FOOD_1);
    placeItem(aboard, 5, 5,BC_FOOD_1,SYMBOL_FOOD_1,COLP_FOOD_1);
    placeItem(aboard, 7, 7,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
    placeItem(aboard, 9, 9,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
    placeItem(aboard, 11, 11,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
    placeItem(aboard, 13, 13,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
    placeItem(aboard, 15, 15,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    placeItem(aboard, 17, 17,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    placeItem(aboard, 19, 19,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    placeItem(aboard, 21, 21,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    // Initialize number of food items
    // Attention: must match number of items placed on the board above
    aboard->food_items = 10;
    return RES_OK;
}
