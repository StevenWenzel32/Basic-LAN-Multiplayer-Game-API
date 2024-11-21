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

// announces that you are avaliable to play, carries contact info and puts the player into the avalible player list
// maybe add a player username
void Register(){
    // get the player username
    // get the player ip 
    // get the player port for the game???
    // send msg to the server with the player info
    registerMsg();
}

// get a list of avaiable games to join
// query the server
// can possibly show who is playing if using usernames
void ListGames(){
    // send a request for the list of games to the server
    gameListMsg();
}

// send the server my ip and tell it to make a game with me as the host
void CreateGame(struct player host){
    // make the new game
    struct game newGame;
    // give id
    newGame.id = avaliableGames.size() + 1;
    // fill in host
    newGame.host = host;
    // update player count
    newGame.players = 1;
    // put into list of games
    avaliableGames.push_back(newGame);
}

// allows the user to join the game
// contact info, other needed info
void JoinGame(){
    // send the host connection details to the client
    // notify host of joining player
    // remove the player from the list of available players
    Unregister();
}

// exit the game gracefully
void ExitGame(){
    // remove the player from the game
    // put the player back into the available players list
    Register();
    // send the other player a message
}

// remove the player from the list of possible/available players
void Unregister(){

}