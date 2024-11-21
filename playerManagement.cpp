// This holds the generic game networking protocols -- at least for a two player game

// go through and get rid of the includes that aren't needed
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <string.h>       // memset
#include <errno.h>        // errno
#include <fstream>        // ofstream for file creation
#include <netdb.h>        // gethostbyname
#include <sstream>        // for stringstream stuff
#include <random>         // for random #
#include <chrono>         // for steady_clock and timer stuff
#include <queue>          // for queue, duh
#include <cerrno>
#include <cstring>        // for strerror
#include <vector>
#include <array>

using namespace std;

// need a list of games being played
// need a list of games that need another player

// need a list of players avalible/not in a game/in lobby
// need a list of players currently playing/unavailable

// announces that you are avaliable to play, carries contact info and puts the player into the avalible player list
// maybe add a player username
void Register(){

}

// list of avialbe games to join
// query the server or broadcast to LAN -- pick one
// can possibly show who is playing if using usernames
void ListGames(){

}

// user starts a game and annoucnes that it is free to join
// if using usernames could have the game show who is playing/hosting
// has the contact info
void CreateGame(){

}

// allows the user to join the game
// contact info, other needed info
void JoinGame(){
    // connect the joining/client player to the host player
    // remove the player from the list of available players
    Unregister();
}

// exit the game gracefully
void ExitGame(){
    // remove the player from the game
    // remove the player from the playing list
    // send the other player a message
}

// remove the player from the list of possible/available players
void Unregister(){

}

// manages the player: joining, hosting, and leaving
int main (int argc, char* argv[]) {
    // check that the command line has the right # of params
    if (argc < 3){
        cerr << "Error: Not enough parameters passed in. Usage: " << argv[0] << " <testNumber> <windowSize>\n";
        return 1;
    }

    // 

    return 0;
} 