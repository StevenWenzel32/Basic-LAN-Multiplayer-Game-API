// has the protocols and msg structs used in the multiplayer game Tic Tac Toe
// this class is used to send the game data between players 

#ifndef TICTACTOE_PROTOCOLS_HPP
#define TICTACTOE_PROTOCOLS_HPP

// my files
#include "basicNetworking.hpp"

// send move - used by both client and host
void sendMove(int x, int y);
// send game state - used by host 
void sendState(char* grid, bool over);
// send game win msg - to be sent on the local
void sendWinMsg();
// send game lose msg - to be sent on the local
void sendLoseMsg();
// send cats game msg - to be sent on the local
void sendCatsMsg();


#endif