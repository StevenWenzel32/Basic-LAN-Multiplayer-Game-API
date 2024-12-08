// This class is the makes the "player"
// it holds the player data that is unique to each client running the game
// This allows the player to be either a client or a host for a game
// this holds the protocols that lets the players connect with each other
// this holds the main: it listens for other players, sends msgs on broadcast
// and it starts the game
// All players are listening and sending on the same port (2087)
#ifndef PLAYER_HPP
#define PLAYER_HPP

// go through and get rid of the includes that aren't needed
// default librairies 
#include <stdlib.h>
#include <iostream>
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <errno.h>        // errno
#include <fstream>        // ofstream for file creation
#include <netdb.h>        // gethostbyname
#include <random>         // for random #
#include <chrono>         // for steady_clock and timer stuff
#include <queue>          // for queue, duh
#include <cerrno>

// for sure being used
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cstring>       // For memset
#include <unistd.h>      // For gethostname
#include <netdb.h>       // For getaddrinfo
#include <arpa/inet.h>   // For inet_ntop
#include <mutex>         // for mutexes
#include <sstream>       // for stringstream stuff
#include <cctype>        // For std::isdigit
#include <thread>        // For std::thread
#include <algorithm>     // For std::all_of
#include <signal.h>      // for the shutdown signal
#include <csignal>
#include <stdio.h>
#include <string>

// my files
#include "TicTacToeGame.hpp"
#include "globalFlags.hpp"

// port to use to listen and send on broadcast
#define UDP_PORT "2087"
#define TCP_PORT "2088"

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
};

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
    // remove yourslef from your player list and broadcast to others to remove you
    void unregisterOut();

    // in functions / msg processing
    // puts the player into the player list
    void registerPlayerIn(string ip);
    // handling the recieving of a notification that a new game was created
    void createGameIn(int gameId, string hostIp);
    // handling the recieving of a notification that a game is full
    void joinGameIn(int gameId);
    // handles reciving a broadcasted unregisterMsg -- might not actually need a list of players
    void unregisterIn(string playerIp);

    // other functions
    // get a list of avaiable games to join -- instead of query server use clients own list
    // can possibly show who is playing if using usernames
    void listGames();

    // helper functions for the above core functions
    // connect the client player to the host player - client side
    void connectToHost(string type, string hostIp, string port);
    // accepts the connection to the client player - host side
    void acceptClientPlayer();
    // set the players ip to the local users ip 
    void setIpAsLocal();
    // print out the commands for this program - not the game!
    void printHelp();

    // process the messages being sent over broadcast
    void processUdpMsgs(baseMsg* msg);
    // processes the cmds given by the user
    void inputPrompt();
    // prompts user for input
    void processProgramCmds(string input);
    // listen for msgs on the broadcast - manages the match making
    void listenForUdpMsgs();
    // listen for TCP connection from other players
    void listenForTcpConnect();

    // msg creation and sending funcs
    // the port might be unneeded
    // send a broadcast msg to register the player into other players list
    // sending your own ip
    void registerMsg(string ip);
    // send a broadcast msg to unregister the player from player lists
    // sending your own ip
    void unregisterMsg(string ip);
    // broadcast the creation of a game, sends the game info
    void createGameMsg(int gameId, string hostIp);
    // broadcast that the game you just joined is full, send gameId
    void gameFullMsg(int gameId);
    // send a TCP msg to the client saying that you're connected to them now
    void clientJoinMsg();

//    protected:
    // vars related to the players device
    // the current ip address of the player
    string ip = "";
    // used to place player in the local unordered map
    int id = 0;
    // if usernames are being used the player can change it at any time
//    string username;

    // sd to use to listen for TCP connections
    int tcpListenSd;
    // the socket for sending broadcast msgs and for listening for broadcast msgs
    int broadSd;
    // the addrinfo for sending on broadcast
    struct sockaddr_in broadcastAddr{};

    // vars related to the current session - include host info, and the other players info
    // game changes when they leave or join a game - add in game deletes and fill in the destructor
    TicTacToe game;

    // list of players - expecting around 50 players - class size is less than 45 - does not contain yourself
    unordered_map<int, player> players;
    // used to create player ids
    int playerCounter = 1;

    // list of games that can be joined
    // maps the gameId to the game
    unordered_map<int, struct game> avaliableGames;
    // counter to create game ids
    int gameCounter = 1;

    // holds threads to make sure they are cleaned up nice later
    vector<thread*> threads; 
    // mutex to protect the players map
    mutex playersMutex;
    // mutex to protect the game map
    mutex gamesMutex;
};

#endif