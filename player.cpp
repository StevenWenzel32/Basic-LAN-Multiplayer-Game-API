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
int playerId;
int currentGameId;

// announces that you are avaliable to play, carries contact info and puts the player into the avalible player list
// maybe add a player username
void Register(){
    // get the player username
    // get the player ip 
    // get the player port for the game???
    // send msg to the server with the player info
    registerMsg();
    // maybe grab the response and player id here
}

// get a list of avaiable games to join
// query the server
// can possibly show who is playing if using usernames
void ListGames(){
    // send a request for the list of games to the server
    gameListMsg(playerId);
}

// send the server my player id and tell it to make a game with me as the host
void CreateGame(){
    // send the server the playerId
    createGameMsg(playerId);
}

// tell the server to let me join this game
void JoinGame(int gameId){
    // send the playerId and gameId to the server
    joinGameMsg(playerId, gameId);
}

// send playerId and game Id to the server to have me removed from the game
void ExitGame(){
    // send msg
    exitGameMsg(playerId, currentGameId);
}

// tell server to remove me from list of players
void Unregister(){
    // send server my id
    unregisterMsg(playerId);
}