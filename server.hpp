// This is basically the lobby -- helps the players connect to each other
// the players register, create games, join games, exit games, and unregister here
// the server has direct TCP connections with the players to send them updates - cause using broadcasts is confusing
#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <queue>          // for queue, duh
#include <cerrno>
#include <cstring>        // for strerror
#include <vector>
#include <array>
#include <unordered_map>
#include <signal.h>       // for the shutdown signal
#include <fcntl.h>        // for fcntl -- to set non-blocking

// my files
// for the socket related functions and basic msg sends and recieves
#include "basicNetworking.hpp"

// structs 
// game struct to be put into local list of games to join
struct game{
    // for quick id
    int id = 0;
    // the knight/host/player1
    string hostIp;
};

// player struct to be put into local list of registered players 
struct player{
    // for quick id
    int id = 0;
    // how to contact them
    string ip;
    // the socket used to send msgs to them
    int sd;
};

// holds threads to make sure they are cleaned up nice later
vector<thread*> threads; 
// mutex to protect the players map
mutex playersMutex;
// mutex to protect the game map
mutex gamesMutex;
// list of players - expecting around 50 players - class size is less than 45 - does not contain yourself
unordered_map<int, player> players;
// used to create player ids
int playerCounter = 1;

// list of games that can be joined
// maps the gameId to the game
unordered_map<int, struct game> avaliableGames;
// counter to create game ids
int gameCounter = 1;


// update msgs
// send out the new game list
// send out the new player list - don't think this is needed yet


// functions to process the msgs sent from players


#endif