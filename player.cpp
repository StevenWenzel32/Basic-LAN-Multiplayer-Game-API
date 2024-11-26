// This class is the makes the "player"
// it holds the player data that is unique to each client running the game
// This allows the player to be either a client or a host for a game
// this holds the protocols that lets the players connect with each other
// this holds the main: it listens for other players, sends msgs on broadcast
// and it starts the game

// my files
#include "player.hpp"

// announces that you are avaliable to play, carries contact info
// maybe add a player username
void registerPlayerOut(){
    // set your ipAddr
    this.setIpAsLocal();
    // send your ip addr and port on the broadcast msg to the LAN - udp
    registerMsg(this.ipAddr, this.port);
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
    newPlayer.ipAddr = ip;
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
    // start up the game *************
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
    joinGameMsg(this.ipAddr, gameId);
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
        exitGameMsg(this.currentGame.host.ipAddr);
        // disconnect from host
        disconnectFromPlayer(this.hostSocket);
        // broadcast out the game is back in list
        createGameMsg(this.currentGame);
    } 
    // if host
    else{
        // disconnect from client
        disconnectFromPlayer(this.hostSocket);
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
    unregisterMsg(this.ipAddr);
    // reset playerId
    this.playerId = 0;
}

// handles reciving a broadcasted unregisterMsg -- might not actually need a list of players
void unregisterIn(string playerIp){
    // search the map for the player that matches
    for (auto i = players.begin(); i != players.end(); ++i){
        // if the same ip
        if (i->second.ipAddr == playerIp){
            // remove the player 
            players.erase(i->first);
        }
    }
}

// helper functions

// connect the client player to the host player - client side
// works with tcp and udp, returns the SD
int connectToHost(string type, Player host){
    // make the addrinfo
    struct addrinfo* servinfo = makeAddrinfo(type, host.ipAddr, PORT);
    // make the socket
    return makeSocket(servinfo);
}

// accepts the connection to the client player - host side
// returns the new SD
void acceptClientPlayer(){
    return acceptConnection(this.playerSd);
}

// closes the connection socket or the listening socket
void disconnectFromPlayer(int playerSd){
    closeSocket(playerSd);
}

// getters and setters
string getPlayerIp(){
    return this.ipAddr;
}

void setPlayerIp(string ip){
    this.ipAddr = ip;
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
    this.ipAddr = ipStr;
}


// msg creation and sending functions

// the port might be unneeded
// send a broadcast msg to register the player into other players list
// sending your own ip
void registerMsg(string ip, string port){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip);
    // create the baseMsg
    // 1 = register msg
    struct baseMsg msg(1, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this.broadSendSd, msg, this.broadinfo);
}

// send a broadcast msg to unregister the player from player lists
// sending your own ip
void unregisterMsg(string ip){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip);
    // create the baseMsg
    // 2 = "unregister"
    struct baseMsg msg(2, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this.broadSendSd, msg, this.broadinfo);
}

// send a msg to the other player that you are leaving the game
// might not be needed escentailly sending just a default msg ****
void exitGameMsg(){
    // create the defualt msg to send
    string payload = "other player has left the game";
    // create the baseMsg
    // 3 = "exitGame"
    struct baseMsg msg(3, payload.c_str, payload.size());
    // send the baseMsg
    sendUdpMsg(this.playerSocket, msg, this.playerinfo);
}

// broadcast the creation of a game, sends the game info
void createGameMsg(struct game game){
    // put the passed in values into an acceptable payload
    // put in a function to serialize the game info **************
    const char* payload = game.id;
    payload += game.host
    // create the baseMsg
    // 4 = "createGame"
    struct baseMsg msg(4, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this.broadSendSd, msg, this.broadinfo);
}

// broadcast that the game you just joined is full, send gameId
void gameFullMsg(int gameId){
    // put the passed in values into an acceptable payload
    char payload[32];
    snprintf(payload, sizeof(payload), "%d", gameId);
    // create the baseMsg
    // 5 = "gameFull"
    struct baseMsg msg(5, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this.broadSendSd, msg, this.broadinfo);
}

// tell the host you have joined their game, send your ip and the gameId
void joinGameMsg(string ip){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip.c_str());
    // create the baseMsg
    // 6 = "joinGame"
    struct baseMsg msg(6, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this.playerSd, msg, this.playerinfo);
}


// function with loop to run while in a game

// listen for msgs on the broadcast
void listenForMsgs(){
    while (true){
        // process msgs in new threads
        processMsgs();
            
        // send out any msgs the user wants to send 
    }
}

int main (int argc, char* argv[]) {
    // setup the listening socket for broadcast msgs
    struct addrinfo* clientinfo = makeAddrinfo("udp", PORT);
    // make the socket
    this.broadSd = makeSocket(clientinfo);
    // set the option to resuse 
    setSocketReuse(this.broadSd);
    // turn on the broadcast - for sending 
    setSocketBroadcast(this.broadSd);
    // bind the socket - for listening 
    bindSocket(this.broadSd, clientinfo);
    // free after being used -- will probably be moved again 
    freeaddrinfo(clientinfo);

    // setup the socket for sending broadcast msgs
    // use ipv4
    this.broadcastAddr.sin_family = AF_INET;
    // convert the port to network byte order
    this.broadcastAddr.sin_port = htons(atoi(PORT));
    // set the boradcast addr
    this.broadcast.sin_addr.s_addr = inet_addr("255.255.255.255");

    // Thread for listening for broadcast msgs -- always running until shutdown
    // must make a listenForMsgs() *************
    thread listenerThread(listenForMsgs, this.broadSd);

    // main thread sends msgs
    sendMessages(this.broadSd, clientinfo);
    
    // make sure the listeing thread has ended before closing
    listenerThread.join();
    
    // close the broadcast socket used for listening and sending
    closeSocket(this.broadSd);
    return 0;
} 