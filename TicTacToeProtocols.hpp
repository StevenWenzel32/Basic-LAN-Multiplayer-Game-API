// has the protocols and msg structs used in the multiplayer game Tic Tac Toe
// this class is used to send the game data between players 

#ifndef TICTACTOE_PROTOCOLS_HPP
#define TICTACTOE_PROTOCOLS_HPP

// my files
#include "player.hpp"

// send move - used by both client and host
void sendMove(int x, int y, bool host);
// read in a move sent by the other player
void readMove();
// send game state - used by host 
void sendState(vector<int> rows, bool win);
// read in the state sent by the host
void readState();
// send game win msg - to be sent on the local
void sendWinMsg();
// send game lose msg - to be sent on the local
void sendLoseMsg();


#endif