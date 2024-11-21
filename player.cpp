// This class is meant to be inherited by the knight/host and the slime/client
// holds the commone functions they both use to interact with the server 
// also holds the player data

// my files
#include "player.hpp"

// announces that you are avaliable to play, carries contact info and puts the player into the avalible player list
// maybe add a player username
void Register(){
    // get the player username
    string username =;
    // get the player ip 
    int ip = ;
    // get the player port for the game???
    unsigned int port = ;
    // send msg to the server with the player info
    registerMsg(username, ip, port);
    // maybe grab the response and player id here ***********
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
    // maybe grab the response and game id here ********
}

// tell the server to let me join this game
void JoinGame(int gameId){
    // send the playerId and gameId to the server
    joinGameMsg(playerId, gameId);
    // update the currentGameId
    currentGameId = gameId;
}

// send playerId and game Id to the server to have me removed from the game
void ExitGame(){
    // send msg
    exitGameMsg(playerId, currentGameId);
    // reset game id
    currentGameId = 0;
}

// tell server to remove me from list of players
void Unregister(){
    // send server my id
    unregisterMsg(playerId);
    // reset playerId
    playerId = 0;
}