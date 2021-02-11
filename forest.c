#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "neillsimplescreen.h"

#define COLUMNS 80
#define ROWS 30
#define G 250
#define L (10*G)
#define GENERATIONS 1000
#define TREE '@'
#define FIRE '*'
#define EMPTY ' '

enum bool {false, true};
typedef enum bool bool;

/*Do testing of the functions*/
void test(void);
int testRandom(int prob);
/*Initialises the forest array with a defined character*/
void populateForest(char forest[ROWS][COLUMNS], char state);
/*Prints forest array*/
void printForest (char forest[ROWS][COLUMNS]);
/*Changes tiles from oldState to newState*/
void changeState (char forest[ROWS][COLUMNS],\
     char oldState, char newState, double odds);
/*check the forest for trees and convert new fires to fires*/
int forestFire(char forest[ROWS][COLUMNS]);
/*When a tree has adjacent fire, it will become a new fire*/
bool isFireAdjacent(char forest[ROWS][COLUMNS], int i, int j);
/*returns true/false to an inputed probability*/
bool chance(int denominator);

int main(void)
{
   char forest[ROWS][COLUMNS];
   int i;

   srand(time(NULL));
   test();
   populateForest(forest, EMPTY);
   neillclrscrn();

   for(i = 0; i < GENERATIONS; i++){
      changeState(forest, EMPTY, TREE, G);
      changeState(forest, TREE, FIRE, L);
      if(forestFire(forest) >= 0){
         neillcursorhome();
         printForest(forest);
         neillbusywait(0.1);
      }
   }
   return 0;
}

void test(void)
{
   int i, j, expected, mostFreq, count = 0;
   char grid[ROWS][COLUMNS];

   /*ensure populate function populates all cells*/
   populateForest(grid, EMPTY);
   for(i = 0; i < ROWS; i++){
      for(j = 0; j < COLUMNS; j++){
         assert((grid[i][j]) == EMPTY);
      }
   }

   /*testing random function*/
   assert(chance(true) == true);
   assert(chance(false) == false);

   /*checking distribution during standard cases*/
   /*testing for TREES*/
   mostFreq = testRandom(G);
   expected = (int)((double)ROWS*COLUMNS) *\
                   ((double)1.0 / G);
   assert((mostFreq >= expected - 1) &&
          (mostFreq <= expected + 1));
   /*testing for FIRE/lightning strikes*/
   mostFreq = testRandom(L);
   expected = (int)(((double)(ROWS * COLUMNS) *\
                    ((double)1.0 / L)));
   assert((mostFreq <= expected + 1));

   /*testing changeState*/
   count = 0;
   populateForest(grid, TREE);
   changeState(grid, TREE, FIRE, L);
   for(i = 0; i < ROWS; i++){
      for(j = 0; j < COLUMNS; j++){
         if(grid[i][j] == FIRE){
            count++;
         }
      }
   }
   assert(count < 8);

   /*Testing forestFire*/
   /*Checking a middle tile*/
   populateForest(grid, TREE);
   grid[20][20] = FIRE;
   assert(forestFire(grid) == 8);

   /*checking corner of board*/
   populateForest(grid, TREE);
   grid[0][0] = FIRE;
   assert(forestFire(grid) == 3);

   /*checking side of board*/
   populateForest(grid, TREE);
   grid[10][0] = FIRE;
   assert(forestFire(grid) == 5);

   /*checking FIRE doesn't spread from empty*/
   populateForest(grid, EMPTY);
   grid[20][20] = FIRE;
   assert(forestFire(grid) == 0);

   /*Testing checkAdjacent*/
   /*Fire in neighbouring tile*/
   populateForest(grid, TREE);
   grid[20][19] = FIRE;
   assert(isFireAdjacent(grid, 20, 20) == true);

   /*Fire not in neighbouring tile*/
   populateForest(grid, TREE);
   grid[20][19] = FIRE;
   assert(isFireAdjacent(grid, 20, 24) == false);

}

int testRandom(int prob)
{
   int i, j, count = 0, mostFreq = 0, largestNum = 0;
   int freqTrees[40] = {0};

   for(i = 0; i < 10000; i++){
      for(j = 0; j < ROWS*COLUMNS; j++){
         if (chance(prob)){
            count++;
         }
      }
      ++freqTrees[count];
      count = 0;
   }
   for(i = 0; i < 40; i++){
      if (freqTrees[i] > largestNum){
         mostFreq = i;
         largestNum = freqTrees[i];
      }
   }
   return mostFreq;
}



void populateForest(char forest[ROWS][COLUMNS], char state)
{
   int i, j;
   for(i = 0; i < ROWS; i++){
      for(j = 0; j < COLUMNS; j++){
         forest[i][j] = state;
      }
   }
}

void printForest(char forest[ROWS][COLUMNS])
{
   int i, j;
   for(i = 0; i < ROWS; i++){
      for(j = 0; j < COLUMNS; j++){
         if(forest[i][j] == FIRE){
            neillfgcol(red);
         }
         else if(forest[i][j] == TREE){
            neillfgcol(green);
         }
         printf("%c", forest[i][j]);
      }
      printf("\n");
   }
}

int forestFire(char forest[ROWS][COLUMNS])
{
   int i, j, count = 0;
   char newFire = '!';

   for(i = 0; i < ROWS; i++){
      for(j = 0; j < COLUMNS; j++){
         if(forest[i][j] == TREE){
            count += isFireAdjacent(forest, i, j);
         }
      }
   }
   /*remove old fires*/
   changeState(forest, FIRE, EMPTY, true);
   /*initalise new fires from placeholder character*/
   changeState(forest, newFire, FIRE, true);
   return count;
}

bool isFireAdjacent(char forest[ROWS][COLUMNS], int i, int j)
{
   int iArea, jArea;
   /*placeholder character to differentiate from old fires*/
   char newFire = '!';

   for(iArea = (i - 1); iArea <= i + 1; iArea++){
      if(iArea >= 0 && iArea < ROWS){
         for(jArea = (j - 1);jArea <= j + 1; jArea++){
            if(jArea >= 0 && jArea < COLUMNS){
               if(forest[iArea][jArea] == FIRE){
                  forest[i][j] = newFire;
                  return true;
               }
            }
         }
      }
   }
   return false;
}

void changeState(char forest[ROWS][COLUMNS],\
     char oldState, char newState, double odds)
{
   int i, j;
   for(i = 0; i < ROWS; i++){
      for(j = 0; j < COLUMNS; j++){
         if((forest[i][j] == oldState) && (chance(odds))){
            forest[i][j] = newState;
         }
      }
   }
}

/*idea for random number generator derived from Q&A*/
bool chance(int denominator)
{
   int numerator = 1;
   double odds = (double)numerator/(double)denominator;

   if(denominator == 0){
      return false;
   }
   else if(((double)rand() / (double)RAND_MAX) < odds){
      return true;
   }
   else{
      return false;
   }
}
