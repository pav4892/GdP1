#include <curses.h>
#include <string.h>

#define RES_OK 0
#define RES_FAILED 1

// *********************************************
// Standard curses initialization and cleanup
// *********************************************

// Initialize application with respect to curses settings
void initializeCursesApplication() {
    initscr(); // Initialize the curses screen

    // Note:
    // The call to initscr() defines various global variables of the curses framework.
    // stdscr, LINES, COLS, TRUE, FALSE

    noecho();  // Characters typed are not echoed
    cbreak();  // No buffering of stdin
    nonl();    // Do not translate 'return key' on keyboard to newline character
    keypad(stdscr, TRUE); // Enable the keypad
    curs_set(0);          // Make cursor invisible
    // Begin in non-single-step mode (getch will not block)
    nodelay(stdscr, TRUE);  // make getch to be a non-blocking call
}

// Reset display to normale state and terminate curses application
void cleanupCursesApp(void)
{
    standend();   // Turn off all attributes
    refresh();    // Write changes to terminal
    curs_set(1);  // Set cursor state to normal visibility
    endwin();     // Terminate curses application
}

int roundIntDivisibleByTwo(int inputInt) {
    if(inputInt % 1 == 0.5) {
        return inputInt + 0.5;
    }
    return inputInt;
}

// ********************************************************************************************
// MAIN
// ********************************************************************************************

int main(void) {
    int res_code;         // Result code from functions
    char message[50];
    int msg_len;
    int min_rows = 3;

    // Here we start

    initializeCursesApplication();

    //Create the message that will be displayed in the center of the screen
    snprintf(message, sizeof(message), "Das Fenster hat %d Zeilen und %d Spalten", LINES, COLS);
    msg_len = strlen(message);                   // Compute length of our message

    // Maximal LINES and COLS are set by curses for the current window size. LINES AND COLS are vars given by curses and just work.
    // LINES is equal to height of terminal, COLS is equal to width of terminal
    // Check if the window is large enough to display our message
    if (COLS < msg_len || LINES < min_rows) {
        
        // Restore Normal terminal functionality
     
        cleanupCursesApp();
        
        // Print a conventional error message via printf.
        // Note: this only works after the call to  cleanupCursesApp();
        // COLS < 10 ? msg_len+1 : msg_len   ist noetig, da, wenn COLS einstellig ist, die Fehlermeldung nicht mehr akkurat ist. In JEDEM Fall werden mit dieser "message" 40 (Also "message" ist immer 39) COLS benoetigt
        printf("Das Fenster ist zu klein: wir brauchen mindestens %dx%d", COLS < 10 ? msg_len+1 : msg_len, min_rows);
        // Set the result code to report the error
        res_code = RES_FAILED;
    } else {
        // Center output calculations

        // Verticially(This will be the Y-parameter offset starting from the Top. Plugging mid_row into snprintf below will push the message e.g. 5 down from the top)
        // Additionally a padding of \n has to be added at the end because we want it to be centered and rn we only pushing it 5 down and it is still stuck at the bottom(on top of the command line)
        int mid_row = LINES/2;     

        // Horizontally
        int start_col = COLS/2-msg_len/2;

        //Calculate Padding that will be added after message in order to have the messaged centered verticially

        int mid_row_rounded = roundIntDivisibleByTwo(mid_row);
        char padding[mid_row_rounded + 1]; // +1 for the null terminator
        memset(padding, '\0', sizeof(padding)); // Initialize with null

        for(int i = 0; i <= mid_row_rounded - 1; i++) {
            strcat(padding, "\n");  
        }


        // Write our message centered onto the display (mvprintw(y,x,string))
        mvprintw(mid_row, start_col, message);
        printw(padding);

        //Write the Letters into each corner of the screen

        mvaddch(0,0,'A');
        mvaddch(0,COLS-1,'B');
        mvaddch(LINES-1,COLS-1,'C');
        mvaddch(LINES-1,0,'D');


        // Refresh the screen in order to show all changes on the screen
        refresh();

        nodelay(stdscr, FALSE);  // make getch to be a blocking call until the next time the keyboard is pressed
        // Wait for keyboard input(next keypress)
        getch();
        nodelay(stdscr, TRUE);  // make getch to be a non-blocking call
       
        // Restore Normal terminal functionality
        cleanupCursesApp();

        // Set the result code to report success
        res_code = RES_OK;


    }

    return res_code;
}
