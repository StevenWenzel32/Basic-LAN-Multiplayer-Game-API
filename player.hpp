// This class is meant to be inherited by the knight/host and the slime/client
// holds the commone functions they both use to interact with the server 
// also holds the player data
#ifndef PLAYER_HPP
#define PLAYER_HPP

// go through and get rid of the includes that aren't needed
// default librairies 
#include <string>
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
#include <array>

// for sure being used
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cstring>       // For memset
#include <unistd.h>      // For gethostname
#include <netdb.h>       // For getaddrinfo
#include <arpa/inet.h>   // For inet_ntop

// my files
// for the socket related functions and basic msg sends and recieves
#include "basicNetworking.hpp"

// port to use/ listen on
#define PORT "2087"

// structs 
// send a msg to the broadcast
struct announcement {
    // the length of the msg being sent
    unsigned int length;
    // the type of msg being sent
    unsigned char type;
    // data to send should be other data packet structs
    vector<char> payload;

    // constructor with passing in values
    baseMsg(unsigned char msgType, const char* payloadData, unsigned int payloadSize){
        // get the payload size and the size of the type var
        length = payloadSize + sizeof(type);
        // pass in the type
        type = msgType;
        // pass in the payload data and its size
        payload.assign(payloadData, payloadData + payloadSize);
    }
};

// game struct to hold game info
struct game{
    // for quick id
    int id = 0;
    // the knight/host/player1
    Player host;
    // the slime/client/player2
    Player client;
    // # of players in the game
//    unsigned char players = 0;
}Game;

// list of players - expecting around 50 players - class size is less than 45 - does not contain yourself
vector<int, Player> players;
// used to create player ids
int playerCounter = 1;

// list of games that can be joined
// maps the gameId to the game
unordered_map<int, struct game> avaliableGames;
// counter to create game ids
int gameCounter = 1;

class Player {
    public: 
    Player();
    ~Player();

    // no longer using a server for mathcmaking or for scoreboards
    // connection related functions 
    // connect to the game server -- calls functions to make the sockets
//    void connectToServer();
    // kill the connection to the server
//    void disconnectFromServer();

    // out functions
    // broadcast on LAN avaliable to play, carries contact info
    // maybe add a player username
    void registerPlayerOut();
    // create a game and braodcast it on the LAN
    void createGameOut();
    // notify the host of the game that you are joining, broadcast the game is full
    void joinGameOut(int gameId);
    // leave the game - handles both cases of the calling player being host and client 
    void exitGameOut();
    // remove yourslef from your player list and broadcast to others to remove you
    void unregisterOut();

    // in functions / msg processing
    // puts the player into the player list
    void registerPlayerIn();
    void createGameIn();
    void joinGameIn(int gameId);
    void exitGameIn();
    void unregisterIn();

    // other functions
    // get a list of avaiable games to join -- instead of query server use clients own list
    // can possibly show who is playing if using usernames
    void listGames();

    // getters and setters
    int getPlayerIp();
    void setPlayerIp();


    protected:
    // vars related to the players device
    // the port the game is running on
    unsigned int port;
    // the current ip address of the player
    string ipAddr = "";
    // used to place player in the local unordered map
    int id = 0;
    // if usernames are being used the player can change it at any time
//    string username;

    // the sd to use to connect to the server
//    int serverSocket;

    // vars related to the current session
    // game changes when they leave or join a game
    struct game currentGame = nullptr;
};

#endif