// This class is meant to be inherited by the knight/host and the slime/client
// holds the commone functions they both use to interact with the server 
// also holds the player data

// my files
#include "player.hpp"

// announces that you are avaliable to play, carries contact info
// maybe add a player username
void registerPlayerOut(){
    // set your ipAddr
    this.setIpAsLocal();
    // send your ip adder and port on the broadcast msg to the LAN - udp
    registerMsg(this.ipAdder, this.port);
}

// puts the player into the player list
void registerPlayerIn(string ip, unsigned int port){
    // up the player count
    playerCounter++;
    // create a new player object
    Player newPlayer;
    // put in the new id
    newPlayer.id = playerCounter;
    // get the players ip
    newPlayer.ipAdder = ip;
    // get the players port
    newPlayer.port = port;
    // put the player into the players list
    players.emplace(newPlayer.id, newPlayer);
}

// get a list of avaiable games to join -- instead of query server use clients own list
// can possibly show who is playing if using usernames
void listGames(){
    // check if the list is empty 
    if (avaliableGames.empty()){
        cout << "no avalible games, try again later" << endl;
        return;
    }

    // start the list
    cout << "Game List: " << endl;

    // print out the local list of games
    for (auto i = avaliableGames.begin(); i != avaliableGames.end(); ++i){
        cout << "Game ID: " << i->first << endl;
    }
    // end the list 
    cout << "----------END OF LIST----------" << endl;
}

// create a game and braodcast it on the LAN
void createGameOut(){
    // create a game object 
    struct game newGame;
    // up the game count 
    gameCounter++;
    // give id 
    newGame.id = gameCounter;
    // assign yourself as host
    newGame.host = this;
    // make sure the client player is empty 
    newGame.client = nullptr;
    // create a listening port for the host - just tcp for now will add UDP as well later
    // use the default port given for listening 
    makeAddrinfo("tcp", PORT);
    // update your current game
    this.currentGame = newGame;
    // put the game object into local list
    avaliableGames.emplace(newGame.id, newGame);
    // broadcast the creation of the game - udp
    createGameMsg(newGame);
}

// handling the recieving of a notification that a new game was created
void createGameIn(struct game newGame){
    // up your local gameCounter
    gameCounter++;
    // reset the game counter to fit your local map 
    newGame.id = gameCounter;
    // add the given game to your local game list
    avaliableGames.emplace(gameCounter, newGame);
}

// notify the host of the game that you are joining, broadcast the game is full
void joinGameOut(int gameId){
    // send join msg to host - tcp
    joinGameMsg(this.ipAdder, gameId);
    // connect to host
    connectToHost("tcp", avaliableGames.at(gameId).host);
    // update the currentGame
    this.currentGame = avaliableGames.at(gameId);
    // update your game list by removing the game you joined
    avaliableGames.erase(gameId);
    // broadcast that the game is full and should be removed from the list of games
    gameFullMsg(gameId);
}

// handling the recieving of a notification that a game is full
void joinGameIn(int gameId){
    // check if the gameId is the game you are in
    if (gameId == this.currentGameId){
        // send msg to the terminal
        cout << "A new player has joined your game" << endl;
    }
    // remove the game from the list
    avaliableGames.erase(gameId);
}

// leave the game - handles both cases of the calling player being host and client 
void exitGameOut(){
    // if not host
    if (this.currentGame.host != this){
        // send exit msg to the host - tcp
        exitGameMsg(this.currentGame.host.ipAdder, this.currentGame);
        // disconnect from host -- create a discconect method *************
        disconnectFromPlayer(this.currentGame.host.port);
        // broadcast out the game is back in list
        createGameMsg(this.currentGame);
    } 
    // if host
    else{
        // disconnect from client -- create a discconect method *************
        disconnectFromPlayer(this.currentGame.client.port);
        // end game session ************** -- not super sure if this is needed
        endGameSession();
    }
    
    // reset your game
    this.currentGame = nullptr;
}

// handles revceiving a exitGameMsg -- might not actually need
void exitGameIn(){
    // print to terminal that the other player has left
    cout << "Your friend the Slime has left the game" << endl;
}

// remove yourslef from your player list and broadcast to others to remove you
void unregisterOut(){
    // remove yourself from player list
    players.erase(this.id);
    // broadcast out that others should remove you from player list
    unregisterMsg(this.ipAdder);
    // reset playerId
    this.playerId = 0;
}

// handles reciving a broadcasted unregisterMsg -- might not actually need a list of players
void unregisterIn(string playerIp){
    // search the map for the player that matches
    for (auto i = players.begin(); i != players.end(); ++i){
        // if the same ip
        if (i->second.ipAdder == playerIp){
            // remove the player 
            players.erase(i->first);
        }
    }
}

// helper functions

// connect the client player to the host player - client side
// works with tcp and udp
void connectToHost(string type, Player host){
    // make the addrinfo
    struct addrinfo* servinfo = makeAddrinfo(type, host.ipAddr, PORT);

    // make the socket
    int clientSd = makeSocket(servinfo);
}

// accepts the connection to the client player - host side
void acceptClientPlayer(){

}

// disconnects the socket if TCP
// stops listening for msgs from the other player if UDP
void disconnectFromPlayer(unsigned int port, string playerIp){

}



// getters and setters
string getPlayerIp(){
    return this.ipAdder;
}

void setPlayerIp(string ip){
    this.ipAdder = ip;
}

// set the players ip to the local users ip 
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