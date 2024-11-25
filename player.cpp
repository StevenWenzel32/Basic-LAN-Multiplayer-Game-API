// This class is meant to be inherited by the knight/host and the slime/client
// holds the commone functions they both use to interact with the server 
// also holds the player data

// my files
#include "player.hpp"

// announces that you are avaliable to play, carries contact info
// maybe add a player username
void registerPlayerOut(){
    // get the player username
//    string username =;
    // get the player ip 
    int ip = ;
    // get the player port for the game???
    unsigned int port = ;
    // broadcast msg to the LAN - udp
    registerMsg(ip, port);
    // put yourself into your local player list
}

// puts the player into the player list
void registerPlayerIn(int ip, unsigned port){
    // put the player into the local player list

}

// get a list of avaiable games to join -- instead of query server use clients own list
// can possibly show who is playing if using usernames
void listGames(){
    // print out the local list of games

}

// create a game and braodcast it on the LAN
void createGameOut(){
    // create a game object 
    // create a listening port for the host
    // update your player object
    // update your current game
    // put the game object into local list
    // broadcast the creation of the game - udp
    createGameMsg(playerIp);
}

void createGameIn(){

}

// notify the host of the game that you are joining, broadcast the game is full// tell the server to let me join this game
void joinGameOut(int game){
    // send join msg to host - tcp
    joinGameMsg(playerIp, game);
    // wait for response from host
    // update your game list by removing the game you joined
    // broadcast that the game is full and should be removed from the list of games
    // update the currentGame
    currentGame = game;
}

void joinGameIn(int game){

}

// leave the game - handles both cases of the calling player being host and client 
void exitGameOut(){
    // if not host
        // send exit msg to the host - tcp
        exitGameMsg(playerIp, currentGame);
        // disconnect from host
        // broadcast out the game is back in list
    // if host
        // disconnect from client
        // broadcast out the game is gone from list
        // end game session
    // reset your game
    currentGame = 0;
    // update local game list
}

void exitGameIn(){

}

// remove yourslef from your player list and broadcast to others to remove you
void unregisterOut(){
    // remove yourself from player list
    // broadcast out that others should remove you from player list
    unregisterMsg(playerIp);
    // reset playerIp
    playerIp = 0;
}

void unregisterIn(){

}