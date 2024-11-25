// This class is meant to be inherited by the knight/host and the slime/client
// holds the commone functions they both use to interact with the server 
// also holds the player data

// my files
#include "player.hpp"

// announces that you are avaliable to play, carries contact info
// maybe add a player username
void registerPlayerOut(){
    // send your ip adder and port on the broadcast msg to the LAN - udp
    registerMsg(this.ipAdder, this.port);
}

// puts the player into the player list
void registerPlayerIn(int ip, unsigned port){
    // up the player count
    playerCounter++;
    // create a new player object
    struct player newPlayer;
    // put in the new id
    newPlayer.id = playerCounter;
    // get the players ip
    newPlayer.ip = ip;
    // get the players port
    newPlayer.port = port;
    // put the player into the players list
    players.emplace(newPlayer.id, newPlayer);
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

// getters and setters
string getPlayerIp(){
    return this.ipAdder;
}

void setPlayerIp(string ip){
    this.ipAdder = ip;
}

void setIpAsLocal(){
    char hostname[1024];
    memset (hostname, 0, sizeof(hostname));

    // get hostname of the system
    if (gethostname(hostname, sizeof(hostname)) == -1){
        cerr << "get hostname failed" << endl;
        return;
    }

    // to store the addrinfo we use
    struct addrinfo hints;
    // pointer to the list
    struct addrinfo *res;
    // fill in the addrinfo
    memset(&hints, 0, sizeof(hints));
    // ipv4 for simplicity 
    hints.ai.family = AF_INET; 
    // tcp because ???
    hints.ai_socktype = SOCK_STREAM; 

    // resolve the host name to an IP address
    if (getaddrinfo(hostname, nullptr, &hints, &res) != 0){
        cerr << "get addrinfo failed" << endl;
    }

    // convert the IP into a string
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, ipStr, sizeof(ipStr));

    freeaddrinfo(res);
    this.ipAdder = ipStr;
}