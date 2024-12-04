// This class is the makes the "player"
// it holds the player data that is unique to each client running the game
// This allows the player to be either a client or a host for a game
// this holds the protocols that lets the players connect with each other
// this holds the main: it listens for other players, sends msgs on broadcast
// and it starts the game

// my files
#include "player.hpp"

// port to use to listen and send on broadcast
#define PORT "2087"

// flag to exit loops when shutting down
volatile sig_atomic_t shutdown_flag = 0;

// handles the shut down of the listening loop
void signalHandler(int signum){
    cout << "Received signal: " << signum << ". Initiating shutdown..." << std::endl;
    shutdown_flag = 1;
}

// custom constructor - does nothing for now
Player::Player() {
    // Constructor definition
}

// custom destructor - does nothing for now
Player::~Player() {
    // Destructor definition
}

// announces that you are avaliable to play, carries contact info
// maybe add a player username
void Player::registerPlayerOut(){
    // set your ipAddr
    setIpAsLocal();
    // send your ip addr and port on the broadcast msg to the LAN - udp
    registerMsg(this->ip);
}

// puts the player into the player list -- need mutexs
void Player::registerPlayerIn(string ip){
    // up the player count
    playerCounter++;
    // create a new player object
    struct player newPlayer;
    // put in the new id
    newPlayer.id = playerCounter;
    // get the players ip
    newPlayer.ip = ip;
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(playersMutex);
    // put the player into the players list
    players.emplace(newPlayer.id, newPlayer);
}

// get a list of avaiable games to join -- instead of query server use clients own list
// can possibly show who is playing if using usernames
void Player::listGames(){
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(gamesMutex);
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
void Player::createGameOut(){
    // up the game count 
    gameCounter++;
    // give id 
    int gameId = gameCounter;
    // update your current game
    this->currentGame = gameId;
    // mark yourself as host 
    this->game.host = true;
    // broadcast the creation of the game - udp 
    createGameMsg(gameId, this->ip);
    // start up the game - this is in the tictactoe protocols file *************
    this->game.startGame();
}

// handling the recieving of a notification that a new game was created -- need mutexs
void Player::createGameIn(int gameId, string hostIp){
    // up your local gameCounter
    gameCounter++;
    // create a new game object 
    struct game newGame;
    // reset the game counter to fit your local map 
    newGame.id = gameCounter;
    // fill in host
    newGame.hostIp = hostIp;
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(gamesMutex);
    // add the given game to your local game list
    avaliableGames.emplace(gameCounter, newGame);
}

// notify the host of the game that you are joining, broadcast the game is full
void Player::joinGameOut(int gameId){
    // send join msg to host - tcp
    joinGameMsg(this->ip);
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(gamesMutex);
    // connect to host and update the playerSd
    this->playerSd = connectToHost("tcp", avaliableGames.at(gameId).hostIp);
    // update the game playerSd too 
    this->game.playerSd = this->playerSd;
    // update the currentGame
    this->currentGame = gameId;
    // update your game list by removing the game you joined
    avaliableGames.erase(gameId);
    // broadcast that the game is full and should be removed from the list of games
    gameFullMsg(gameId);
}

// handling the recieving of a notification that a game is full -- need mutexs
void Player::joinGameIn(int gameId){
    // check if the gameId is the game you are in
    if (gameId == this->currentGame){
        // send msg to the terminal
        cout << "A new player has joined your game" << endl;
    }
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(gamesMutex);
    // remove the game from the list
    avaliableGames.erase(gameId);
}

// leave the game - handles both cases of the calling player being host and client 
void Player::exitGameOut(){
    // if host
    if (this->game.host){
        // disconnect from client
        disconnectFromPlayer(this->playerSd);
        // end game session ************** -- not super sure if this is needed -- but if it is it will be in the tictactoe protocols
 //       endGame();
    } 
    // if not host
    else{
        // send exit msg to the host - tcp
        exitGameMsg();
        // disconnect from host
        disconnectFromPlayer(this->playerSd);
        // broadcast out the game is back in list
        createGameMsg(this->currentGame, this->ip);
    }
    
    // reset your game
    this->currentGame = 0;
}

// handles revceiving a exitGameMsg -- might not actually need
void Player::exitGameIn(){
    // print to terminal that the other player has left
    cout << "Your friend the Slime has left the game" << endl;
}

// remove yourself from your player list and broadcast to others to remove you
void Player::unregisterOut(){
    // broadcast out that others should remove you from player list
    unregisterMsg(this->ip);
    // reset id
    this->id = 0;
}

// handles reciving a broadcasted unregisterMsg -- might not actually need a list of players -- need mutexs
void Player::unregisterIn(string playerIp){
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(playersMutex);
    // search the map for the player that matches
    for (auto i = players.begin(); i != players.end(); ++i){
        // if the same ip
        if (i->second.ip == playerIp){
            // remove the player 
            players.erase(i->first);
        }
    }
}

// helper functions

// connect the client player to the host player - client side
// works with tcp and udp, returns the SD
int Player::connectToHost(string type, string hostIp){
    // make the addrinfo
    struct addrinfo* servinfo = makeAddrinfo(type, hostIp, PORT);
    // make the socket
    return makeSocket(servinfo);
}

// accepts the connection to the client player - host side
// returns the new SD
int Player::acceptClientPlayer(){
    return acceptConnection(this->playerSd);
}

// closes the connection socket or the listening socket
void Player::disconnectFromPlayer(int playerSd){
    closeSocket(playerSd);
}

// set the players ip to the local users ip 
void Player::setIpAsLocal(){
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
    hints.ai_family = AF_INET; 
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
    this->ip = ipStr;
}


// msg creation and sending functions

// the port might be unneeded
// send a broadcast msg to register the player into other players list
// sending your own ip
void Player::registerMsg(string ip){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip);
    // create the baseMsg
    // 1 = register msg
    struct baseMsg msg(1, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// send a broadcast msg to unregister the player from player lists
// sending your own ip
void Player::unregisterMsg(string ip){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip);
    // create the baseMsg
    // 2 = "unregister"
    struct baseMsg msg(2, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// send a msg to the other player that you are leaving the game
// might not be needed escentailly sending just a default msg ****
void Player::exitGameMsg(){
    // create the defualt msg to send
    string payload = "other player has left the game";
    // create the baseMsg
    // 3 = "exitGame"
    struct baseMsg msg(3, payload.c_str(), payload.size());
    // send the baseMsg
    sendUdpMsg(this->playerSd, msg, this->playerinfo);
}

// broadcast the creation of a game, sends the game info
void Player::createGameMsg(int gameId, string hostIp){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%d:%s", gameId, hostIp);
    // create the baseMsg
    // 4 = "createGame"
    struct baseMsg msg(4, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// broadcast that the game you just joined is full, send gameId
void Player::gameFullMsg(int gameId){
    // put the passed in values into an acceptable payload
    char payload[32];
    snprintf(payload, sizeof(payload), "%d", gameId);
    // create the baseMsg
    // 5 = "gameFull"
    struct baseMsg msg(5, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// tell the host you have joined their game, send your ip and the gameId
void Player::joinGameMsg(string ip){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip.c_str());
    // create the baseMsg
    // 6 = "joinGame"
    struct baseMsg msg(6, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->playerSd, msg, this->playerinfo);
}

// make a new thread, send the msg recieved
thread* Player::makeThread(Player* player, baseMsg* msg){
    // create the thread
    thread new_thread(&Player::processMsgs, player, msg);
    // create a pointer
    thread* threadPointer;
    return threadPointer;
}

// process the messages being sent over broadcast
void Player::processMsgs(baseMsg* msg){
    // check the type of the msg
    // register
    if (msg->type == 1){
        // create storage the length of the payload
        string ip(msg->payload.begin(), msg->payload.end());
        // send the payload
        registerPlayerIn(ip);
    } 
    // unregister
    else if (msg->type == 2){
        // create storage the length of the payload
        string ip(msg->payload.begin(), msg->payload.end());
        // send the payload
        unregisterIn(ip);
    } 
    // exit game
    else if (msg->type == 3){
        // send the default msg
        exitGameIn();
    } 
    // create game
    else if (msg->type == 4){
        // break up the payload by length
        int gameId;
        string hostIp(msg->payload.begin() + sizeof(int), msg->payload.end());
        // fill in the host ip
        memcpy(&gameId, msg->payload.data(), sizeof(int));
        // send the payload
        createGameIn(gameId, hostIp);
    } 
    // game full/ a game has been joined
    else if (msg->type == 5 || msg->type == 6){
        // create storage the length of the payload
        int gameId;
        memcpy(&gameId, msg->payload.data(), sizeof(int));
        // send the payload
        joinGameIn(gameId);
    } else {
        cerr << "ERROR: Unknown Msg type, unable to process msg" << endl;
    }
    delete msg;
}

// function with loop to run while in a game

// listen for msgs on the broadcast
void Player::listenForMsgs(){
    while (!shutdown_flag){
        // read in a msg here 
        baseMsg* msg = receiveNonblockingUdp(this->broadSd, this->broadcastAddr);
        // make sure there is a msg 
        if (msg != nullptr){
            // make thread and have them run processMsgs() and pass in the msg recieved
            thread* new_thread = makeThread(this, msg);
            // put thread in vector
            threads.push_back(new_thread);
        }
    }
}

// print out the help message to the user
void Player::printHelp(){
    cout << "These are the avaliable commands:" << endl;
    cout << "   help                - Display this message" << endl;
    cout << "   register            - Registers you into the player list" << endl;
    cout << "   listGames          - Lists the games avaliable to join" << endl;
    cout << "   createGame         - Host a game and wait for another player to join" << endl;
    cout << "   joinGame <gameId>  - Join the game assigned to the gameId" << endl;
    cout << "   exitGame           - Exit your current game" << endl;
    cout << "   unregister          - Unregister yourself from the player list" << endl;
}

// main thread sends msgs - handles the player executing/sending out their broadcast msgs and protocols
// when a game gets started through here the game will take over the thread that this has been called in - in this case main
void Player::sendMsgs(int broadSd, struct addrinfo* clientinfo){
    // print out a msg to the terminal prompting user to enter a command
    cout << "Welcome to the world of high stakes Tic Tac Toe (gambling will come in future update)" << endl;
    cout << "When you see this #: it is prompt to enter a command" << endl;
    printHelp();
    // store the input from the player
    string input;
    char buffer[1024];

    // accept input from the user
    while(!shutdown_flag || !gameEnded){
        // prompt user for input
        cout << "#: " << endl;
        // get their response
        getline(cin, input);

        // split up the input
        // put the string into the stream
        istringstream stream(input);
        // vector to store the tokens
        vector<string> tokens;
        // holds the current token
        string token;

        // break up the stream by spaces
        while (stream >> token){
            tokens.push_back(token);
        }

        // if nothing was given
        if (tokens.empty()){
            continue; 
        }
        // check for the basic commands
        else if (tokens[0] == "exitGame"){
            exitGameOut();
        } else if (tokens[0] == "help"){
            printHelp();
        } else if (tokens[0] == "register"){
            registerPlayerOut();
        } else if (tokens[0] == "listGames"){
            listGames();
        } else if (tokens[0] == "createGames"){
            createGameOut();
        } else if (tokens[0] == "joinGame"){
            // grab the game Id
            string gameId = tokens[1];
            // check that the id is a number
            if (all_of(gameId.begin(), gameId.end(), static_cast<int(*)(int)>(isdigit))){
                joinGameOut(stoi(gameId));
            }
        } else if (tokens[0] == "unregister"){
            unregisterOut();
        } else {
            cout << "ERROR: Unknown command" << endl;
        }
    }
}