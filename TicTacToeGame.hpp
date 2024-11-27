// has the protocols and msg structs used in the multiplayer game Tic Tac Toe
// this class is used to send the game data between players 

#ifndef TICTACTOE_GAME_HPP
#define TICTACTOE_GAME_HPP

// my files
#include "TicTacToeProtocols.hpp"

// -1 = cats game
// 0 = still playing
// 1 = lost
// 2 = win
bool win = 0;

// the grid of the game - 3x3
// _ = empty spot
// X = host placed
// O = client placed
vector<vector> grid;

// input move 
// checks if the move is from the host - host is always X, client is always O
void processMove();
// game win procedure 
void gameWin();
// print out the game grid
void printGrid();

#endif