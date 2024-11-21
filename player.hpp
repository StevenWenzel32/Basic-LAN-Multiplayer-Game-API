// This class is meant to be inherited by the knight/host and the slime/client
// holds the commone functions they both use to interact with the server 
// also holds the player data
#ifndef PLAYER_HPP
#define PLAYER_HPP

// default librairies 
#include <string>

// my files
// for the socket related functions and basic msg sends and recieves
#include "basicNetworking.hpp"

class Player {
    public: 
    Player();
    ~Player();

    // connection related functions
    // connect to the game server -- calls functions to make the sockets
    void connectToServer();
    // kill the connection to the server
    void disconnectFromServer();

    // game related functions
    // announces that you are avaliable to play, carries contact info and puts the player into the avalible player list
    // maybe add a player username
    void registerPlayer();
    // get a list of avaiable games to join
    // query the server
    // can possibly show who is playing if using usernames
    void listGames();
    // send the server my player id and tell it to make a game with me as the host
    void createGame();
    // tell the server to let me join this game
    void joinGame(int gameId);
    // send playerId and game Id to the server to have me removed from the game
    void exitGame();
    // tell server to remove me from list of players
    void unregister();

    protected:
    // vars related to the players device
    // the port the game is running on
    unsigned int port;
    // the current ip address of the player
    string ipAddr;
    // if usernames are being used the player can change it at any time
    string username;

    // the sd to use to connect to the server
    int serverSocket;

    // vars related to the current session
    // playerId is the same until they disconnect from the server/unregsiter
    int playerId;
    // game id changes when they leave or join a game
    int currentGameId;
};

#endif