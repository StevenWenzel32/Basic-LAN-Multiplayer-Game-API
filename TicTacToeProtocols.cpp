// has the protocols and msg structs used in the multiplayer game Tic Tac Toe
// this class is used to send the game data between players 

// my files
#include "TicTacToeProtocols.hpp"

// send move - used by the client
void sendMove(int x, int y){
    // put the data into a char*
    char payload[128];
    snprintf(payload, sizeof(payload), "%d:%d", x, y);
    // make a new base msg
    // 1 = move
    struct baseMsg msg(1, payload, strlen(payload));
    basicNetworking::sendTcpMsg(this.playerSd, msg);
}

// send game state - used by host 
void sendState(char* grid, bool over){
    // put the data into a char*
    char payload[256];
    snprintf(payload, sizeof(payload), "%d:%d", grid, over);
    // make a new base msg
    // 2 = state
    struct baseMsg msg(2, payload, strlen(payload));
    basicNetworking::sendTcpMsg(this.playerSd, msg);
}

// send game win msg - to be sent on the local
void sendWinMsg(){
    cout << "Congrats you won the match!" << endl;
}

// send game lose msg - to be sent on the local
void sendLoseMsg(){
    cout << "Whelp you lost" << endl;
}

// send game lose msg - to be sent on the local
void sendCatsMsg(){
    cout << "No one wins" << endl;
}

