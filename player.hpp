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
    void registerPlayerIn(string ip, unsigned int port);
    // handling the recieving of a notification that a new game was created
    void createGameIn(struct game newGame);
    // handling the recieving of a notification that a game is full
    void joinGameIn(int gameId);
    // handles revceiving a exitGameMsg -- might not actually need
    void exitGameIn();
    // handles reciving a broadcasted unregisterMsg -- might not actually need a list of players
    void unregisterIn(string playerIp);

    // other functions
    // get a list of avaiable games to join -- instead of query server use clients own list
    // can possibly show who is playing if using usernames
    void listGames();

    // helper functions for the above core functions
    // connect the client player to the host player - client side
    void connectToHost(string type, Player host);
    // accepts the connection to the client player - host side
    void acceptClientPlayer();
    // disconnects the socket if TCP
    // stops listening for msgs from the other player if UDP
    void disconnectFromPlayer(int playerSd);
    

    // getters and setters
    string getPlayerIp();
    void setPlayerIp(string ip);
    // set the players ip to the local users ip 
    void setIpAsLocal();

    // msg creation and sending funcs
    // the port might be unneeded
    // send a broadcast msg to register the player into other players list
    // sending your own ip
    void registerMsg(string ip, string port);
    // send a broadcast msg to unregister the player from player lists
    // sending your own ip
    void unregisterMsg(string ip);
    // send a msg to the other player that you are leaving the game
    // might not be needed escentailly sending just a default msg ****
    void exitGameMsg();
    // broadcast the creation of a game, sends the game info
    void createGameMsg(struct game game);
    // broadcast that the game you just joined is full, send gameId
    void gameFullMsg(int gameId);
    // tell the host you have joined their game, send your ip and the gameId
    void joinGameMsg(string ip);
    // listen for msgs on the broadcast
    void listenForMsgs();

//    protected:
    // vars related to the players device
    // the port the game is running on
    unsigned int port;
    // the current ip address of the player
    string ipAddr = "";
    // used to place player in the local unordered map
    int id = 0;
    // if usernames are being used the player can change it at any time
//    string username;

    // the sd to use to connect to the other player
    // can be either tcp or udp - can later use both to connect to the player
    int playerSd;
    // if the above if a udp - this is the other players info
    struct addrinfo* playerinfo;
    // the socket for sending broadcast msgs and for listening for broadcast msgs
    int broadSd;
    // the addrinfo for sending on broadcast
    sockaddr_in broadcastAddr{};


    // vars related to the current session
    // game changes when they leave or join a game
    struct game currentGame = nullptr;
};

#endif

// finish the msg sending functions 
// add in the proccessing of the msgs recieved 
// find a way to start the game
// function for listening multitrheaded and protected
// function for sending msgs
    // if joining a game this will start a new thread that uses tcp and stop and wait for sending game data 