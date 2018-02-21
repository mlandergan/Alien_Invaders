/************** ECE2049 Lab 1 ******************/
/**************  9/6/2017   ******************/
/***************************************************/
#include <stdlib.h>
#include <msp430.h>
#include "peripherals.h"

#define max 13// used for array max element

// create a data type GAME_STATE for the switch case
enum GAME_STATE {WAITING = 0, BEGIN_GAME = 1, DRAW_NEW_ALIENS = 2, CHECK_KEYPAD =3, ALIENS_DESCEND = 4, GAME_OVER = 5, NEW_ROUND = 6};

// Function Prototypes
void spaces_to_fill (int LEVEL);
void swDelay(char numLoops);
void polling (char numLoops, int LEVEL);
void descend();
int level4aliens();
int noAliens();
void lose(char numloops);
void clean();

// Declare globals variables
unsigned char R_1[] = "             ", R_2[] = "             ", R_3[] = "             ", R_4[] = "             ";
unsigned char currKey=0;

// Main
void main(void)

{
    WDTCTL = WDTPW | WDTHOLD;      // Stop watchdog timer

    initLeds(); // configure LEDS
    configDisplay(); // configure display
    configKeypad(); // configure keypad

    //Code setup
    int LEVEL = 1; // initalize level to start at 1
    enum GAME_STATE state = WAITING; // create an instance of GAME_STATE and intiliaze it to WAITING case
    Graphics_clearDisplay(&g_sContext); // Clear the display



    while (1)    // Forever loop
    {
        switch (state){
            case WAITING: // display at the begining of the game
                Graphics_drawStringCentered(&g_sContext, "SPACE INVADERS", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Press * to begin", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);

               currKey = getKey(); // get the current key being pressed

               if (currKey == '*'){  	// if the current key is a * 
                   state = BEGIN_GAME;  // set state to BEGIN_GAME
                  }else {
                   state = WAITING; // if not remain in WAITING state
                  }

                break;
            case BEGIN_GAME:
                clean(); // clear all aliens
                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "3...", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(1);

                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "2...", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(1);

                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "1...", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(1);

                Graphics_clearDisplay(&g_sContext); // Clear the display
                state = DRAW_NEW_ALIENS;
                break;

            case DRAW_NEW_ALIENS:
                spaces_to_fill(LEVEL); // call the spaces_to_fill function with LEVEL as the input
                state = CHECK_KEYPAD;
                break;

            case CHECK_KEYPAD:
                polling(1, LEVEL);

                if(noAliens() == 0){ // check to see if all the aliens are dead
                    state = ALIENS_DESCEND;     // if not, advance the rows
                }
                else{
                    state = NEW_ROUND; // all of the aliens are dead, advance the level
                }
                break;

            case ALIENS_DESCEND:
                descend();
                configDisplay();
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, R_1, AUTO_STRING_LENGTH, 38, 15, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, R_2, AUTO_STRING_LENGTH, 38, 35, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, R_3, AUTO_STRING_LENGTH, 38, 55, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, R_4, AUTO_STRING_LENGTH, 38, 75, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                // check if aliens reached the fourth row
                if(level4aliens() == 1){
                    swDelay(1);  // theres aliens in the fourth row
                    state = GAME_OVER; // set the state to GAME_OVER
                }
                else{
                    state = DRAW_NEW_ALIENS; // there are still aliens on the screen, set state to DRAW_NEW_ALIENS
                }
                break;

            case GAME_OVER:
                //print message, LED flash, buzzer sounds
                lose(4); // call lose function, loop 2 times
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "You lose!!!", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(8);
                Graphics_clearDisplay(&g_sContext);
                BuzzerOff(); // turn off buzzer
                setLeds(0x0); // turn off LEDs
                state = WAITING; // restart game, set state to WAITING
                break;

            case NEW_ROUND:
                LEVEL++; // add 1 to LEVEL
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "Next level!", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(8);
                state = BEGIN_GAME; // set state to BEGIN_GAME
                break;
        }

    }  // end while (1)
}

// check to see if there are any aliens in the arrays
int noAliens(){
    volatile unsigned int k = 0, counter = 0;
    for(k = 0; k < max; k++){
        if((R_3[k] == ' ') && (R_2[k] == ' ') && (R_1[k] == ' ')){ // check if index in array is empty
            counter++;
        }
    }
  if(counter == max){ // all of the rows are empty
      return 1; 
  }
 return 0; // there are still aliens

}

// function lose takes in numLoops and returns nothing
// activates buzzer and flashes LED
void lose(char numLoops){
    volatile unsigned int j;
    for (j=0; j<numLoops; j++) {
        setLeds(0xF); // turn on all the LEDS
        BuzzerOn(); // turn on the buzzer
        swDelay(2); 
        BuzzerOff(); // turn off buzzer
        setLeds(0x0); // turn off LEDs
        swDelay(2);
    }
}

// check if there are any aliens in the fourth row 
int level4aliens(){
    volatile unsigned int k = 0;
    for(k = 0; k < max; k++){ // iterate through the array
        if(R_4[k] != ' '){ // if there are any aliens 
            return 1; // game over aliens in 4th row
        }
    }
    return 0; // there aren't any aliens in the 4th row
}

// function clean takes in nothing and returns nothing
// clears all aliens in the rows
void clean(){
   volatile unsigned int k = 0;
   for(k = 0; k < max; k++){ // iterate through the size of the array
           R_1[k] = ' '; // set all of the indexes to empty
           R_2[k] = ' ';
           R_3[k] = ' ';
           R_4[k] = ' ';
       }
}

// function descend takes in nothing and returns nothing
// copies elements from one row to the next row
void descend(){
    volatile unsigned int l = 0;
    for(l = 0; l < max; l++){
        R_4[l] = R_3[l];
    }
    for(l = 0; l < max; l++){
        R_3[l] = R_2[l];
    }
    for(l = 0; l < max; l++){
        R_2[l] = R_1[l];
    }
    for(l = 0; l < max; l++){
            R_1[l] = ' ';
        }
}

// function spaces_to_fill 
// takes in an integer and returns nothing
// randomly fills in aliens in the first row
void spaces_to_fill (int LEVEL){
   volatile unsigned int spaces=0, j, k, max2 = 6 , min = LEVEL;
    spaces = rand() % (max2 + 1 - min) + min; // generate a number between current LEVEL and 6
    for (j = 0; j < spaces;  ){
        k = rand() % max2; // between 0 and 5
        if(R_1[2*(k+1)] == ' '){ // if the space isn't filled
            R_1[2*(k+1)] = ((k+1) + 0x30); // fill it
            j++;
        }
      }

    Graphics_drawStringCentered(&g_sContext, R_1, AUTO_STRING_LENGTH, 38, 15, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
   }

// function swDelay takes in a char, numLoops and returns nothing
// delays for a set peroid of time
void swDelay(char numLoops)
{

    volatile unsigned int i,j;  // volatile to prevent optimization
                                        // by compiler

    for (j=0; j<numLoops; j++)
    {
        i = 50000 ;                 // SW Delay
        while (i > 0)               // could also have used while (i)
           i--;
    }
}

// function polling takes in a numLoops and levels. Returns nothing
// listens to keypresses and deletes the number in the lowest row
void polling (char numLoops, int LEVEL)
{
    volatile unsigned int i,j, index;  // volatile to prevent optimization by compiler
    for (j=0; j<numLoops; j++)
    {
        i = 9/LEVEL ;                 // SW Delay
        while (i > 0) {              // could also have used while (i)
            currKey = getKey();      // retrieve the latest key
                if ((currKey >= '1') && (currKey <= '6')) {  // check if its a number between 1 and 6
                   index = (2*(currKey - 0x30)); // calculate the index 
                   if(R_3[index] == currKey){ // check if the number is in the third row
                         R_3[index] = ' '; // if it is clear it
                         Graphics_clearDisplay(&g_sContext); 
                       }
                       else if(R_2[index] == currKey){ // check if the number is in the second row
                           R_2[index] = ' '; // if it is clear it
                           Graphics_clearDisplay(&g_sContext); 
                       }
                       else if(R_1[index] == currKey){ // check if the number is in the first row
                           R_1[index] = ' '; // if it is clear it 
                           Graphics_clearDisplay(&g_sContext);
                       }
                   } // display the aliens on the screen
                Graphics_drawStringCentered(&g_sContext, R_1, AUTO_STRING_LENGTH, 38, 15, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, R_2, AUTO_STRING_LENGTH, 38, 35, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, R_3, AUTO_STRING_LENGTH, 38, 55, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
           i--;
      }



    }
  }

