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
char grid[2][2];

// input move 
// checks if the move is from the host - host is always X, client is always O
void processMove();
// game win procedure 
void gameWin();
// print out the game grid
void printGrid();
// start a game and the make the player who started it the host -- the game logic "main"
void startGame();
// end the game - to be called in gameWin() also called when players
void endGame();

#endif