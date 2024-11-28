// has the game logic fopr two players to play a remote tic tac toe game

#ifndef TICTACTOE_GAME_HPP
#define TICTACTOE_GAME_HPP

// my files
#include "TicTacToeProtocols.hpp"

// if the game is over or not
bool over = 0;

// the grid of the game - 3x3
// _ = empty spot
// X = host placed
// O = client placed
char grid[3][3];

// input move 
// checks if the move is from the host - host is always X, client is always O
void processMove(int x, int y, bool host);
// go through and check for 3 in a row in the grid for the passed in mark
void checkForWin(char mark);
// checks if the passed in mark won -- also checks for a cats game
void checkRows(char mark);
// checks if the passed in mark won -- also checks for a cats game
void checkColumns(char mark);
// checks if the passed in mark won -- also checks for a cats game
void checkDiagonalLeftRight(char mark);
// checks if the passed in mark won -- also checks for a cats game
void checkDiagonalRightLeft(char mark);
// game win procedure 
void gameWin(char mark);
// print out the game grid
void printGrid();
// read in the msgs and pass them off for processing
void readMsg();
// start a game and the make the player who started it the host -- the game logic "main"
void startGame();

#endif