// This class is the makes the "player"
// it holds the player data that is unique to each client running the game
// This allows the player to be either a client or a host for a game
// this holds the protocols that lets the players connect with each other
// this holds the main: it listens for other players, sends msgs on broadcast
// and it starts the game

// my files
#include "player.hpp"

// port to use to listen and send on broadcast
#define UDP_PORT "2087"
#define TCP_PORT "2088"

// flag to exit loops when shutting down
volatile sig_atomic_t shutdown_flag = 0;

// handles the shut down of the listening loop
void signalHandler(int signum){
    cout << "Received signal: " << signum << ". Initiating shutdown..." << std::endl;
    shutdown_flag = 1;
}

// custom constructor - does nothing for now
Player::Player() {
    
}

// custom destructor - does nothing for now
Player::~Player() {
    for (auto& t : threads) {
        if (t->joinable()) {
            t->join();
        }
        delete t;
    }
}

// announces that you are avaliable to play, carries contact info
// maybe add a player username
void Player::registerPlayerOut(){
    try {
        // set your ipAddr
        setIpAsLocal();
        if (this->ip.empty()){
            cerr << "ERROR: IP address failed to be set correctly" << endl;
        }
        cout << "ip has been set" << endl;
        // send your ip addr and port on the broadcast msg to the LAN - udp
        registerMsg(this->ip);
        cout << "registering msg has been sent" << endl;
    } catch (const exception& e){
        cerr << "Exception in registerPlayerOut" << endl;
    }
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
    // check if your ip is empty
    if (this->ip == ""){
        cout << "You're unregistered and can't make a game yet" << endl;
        return;
    }
    // up the game count 
    gameCounter++;
    // give id 
    int gameId = gameCounter;
    // update your current game
    this->currentGame = gameId;
    // mark yourself as host 
    this->game.host = true;
cout << "ip being given for the game host = " << this->ip << endl;
    // broadcast the creation of the game - udp 
    createGameMsg(gameId, this->ip);
    // start up the game - this is in the tictactoe protocols file
    this->game.startGame();
}

// handling the recieving of a notification that a new game was created -- need mutexs
void Player::createGameIn(int gameId, string hostIp){
    // Validate the IP address
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, hostIp.c_str(), &(sa.sin_addr)) != 1) {
        cerr << "Error: Invalid IP address received: " << hostIp << endl;
        return; // Ignore the invalid game creation request
    }
    // up your local gameCounter
    gameCounter++;
    // create a new game object 
    struct game newGame;
    // reset the game counter to fit your local map 
    newGame.id = gameCounter;
cout << "creating a new game this is the HostIP = " << hostIp << endl;
    // fill in host
    newGame.hostIp = hostIp;
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(gamesMutex);
    // add the given game to your local game list
    avaliableGames.emplace(gameCounter, newGame);
}

// notify the host of the game that you are joining, broadcast the game is full
void Player::joinGameOut(int gameId){
    try {
        // lock the mutex before accessing the shared memory 
        lock_guard<mutex> lock(gamesMutex);
        // check if the gameId exsits
        if (avaliableGames.find(gameId) == avaliableGames.end()){
            // the game don't exists
            cout << "The game you want to join doesn't exist" << endl;
            return;
        }
cout << "about to call connectToHost" << endl;
cout << "hostIP = " << avaliableGames.at(gameId).hostIp << endl;
        // connect to host and update the playerSd
        this->playerSd = connectToHost("tcp", avaliableGames.at(gameId).hostIp);
cout << "connected to host" << endl;
        // update the game playerSd too 
        this->game.playerSd = this->playerSd;
        // update the currentGame
        this->currentGame = gameId;
cout << "about to send joingGameMsg" << endl;
        // send join msg to host - tcp
        joinGameMsg(this->ip);
cout << "past joingGameMsg" << endl;
        // update your game list by removing the game you joined
        avaliableGames.erase(gameId);
        // broadcast that the game is full and should be removed from the list of games
        gameFullMsg(gameId);
cout << "game full msg sent" << endl;
        // start the game that you joined - this is in the tictactoe protocols file
        this->game.startGame();
    } catch (const out_of_range& e){
        cerr << "ERROR: Game ID is out of range" << endl;
    } catch (const exception& e){
        cerr << "ERROR: an exception occured while trying to join the game" << endl;
    }
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
    // check if you are in a game
    if (this-> currentGame == 0){
        cout << "Your not in a game" << endl;
        return;
    }
    // if host
    if (this->game.host){
        // disconnect from client
        disconnectFromPlayer(this->playerSd);
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
    struct addrinfo* hostinfo = makeAddrinfo(type, hostIp, UDP_PORT);
    // update the playerInfo
    this->playerinfo = hostinfo;
    // make the socket
    return makeSocket(hostinfo);
}

// accepts the connection to the client player - host side
// returns the new SD
int Player::acceptClientPlayer(){
    return acceptConnection(this->tcpListenSd);
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

    // resolve the host name to an IP address
    if (getaddrinfo(hostname, nullptr, &hints, &res) != 0){
        cerr << "get addrinfo failed" << endl;
        return;
    }

    // convert the IP into a string
    char ipStr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, ipStr, sizeof(ipStr)) == nullptr){
        cerr << "inet_ntop failed" << endl;
        freeaddrinfo(res);
        return;
    }
    // set the ip
cout << "this is the players ip = " << string(ipStr) << endl;
    this->ip = string(ipStr);

    freeaddrinfo(res);
}


// msg creation and sending functions

// the port might be unneeded
// send a broadcast msg to register the player into other players list
// sending your own ip
void Player::registerMsg(string ip){
    // check if the ip is empty
    if (ip == ""){
        cout << "Your IP has not been set right and you can't register" << endl;
        return;
    }
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip);
    // create the baseMsg
    // 1 = register msg
    struct baseMsg msg('1', payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// send a broadcast msg to unregister the player from player lists
// sending your own ip
void Player::unregisterMsg(string ip){
    // check if the ip is empty
    if (ip == ""){
        cout << "You were never registered" << endl;
        return;
    }
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip);
    // create the baseMsg
    // 2 = "unregister"
    struct baseMsg msg('2', payload, strlen(payload));
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
    struct baseMsg msg('3', payload.c_str(), payload.size());
    // send the baseMsg
    sendUdpMsg(this->playerSd, msg, this->playerinfo);
}

// broadcast the creation of a game, sends the game info
void Player::createGameMsg(int gameId, string hostIp){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%d:%s", gameId, hostIp.c_str());
    // create the baseMsg
    // 4 = "createGame"
    struct baseMsg msg('4', payload, strlen(payload));
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
    struct baseMsg msg('5', payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// tell the host you have joined their game, send your ip and the gameId
void Player::joinGameMsg(string ip){
    // check if the ip is empty
    if (ip == ""){
        cout << "Your not registered yet and can't join a game" << endl;
        return;
    }
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", ip.c_str());
    // create the baseMsg
    // 6 = "joinGame"
    struct baseMsg msg('6', payload, strlen(payload));
cout << "about to send the upd msg" << endl;
    // send the baseMsg
    sendUdpMsg(this->playerSd, msg, this->playerinfo);
}

// make a new thread, send the msg recieved
thread* Player::makeThread(Player* player, baseMsg* msg){
cout << "inside make thread" << endl;
    // create the thread
    thread* new_thread = new thread(&Player::processMsgs, player, msg);
cout << "make thread about to return" << endl;
    return new_thread;
}

// process the messages being sent over broadcast
void Player::processMsgs(baseMsg* msg){
cout << "inside processMsgs" << endl;
    // check the type of the msg
    // register
    if (msg->type == '1'){
        // create storage the length of the payload
        string ip(msg->payload.begin(), msg->payload.end());
        // send the payload
        registerPlayerIn(ip);
    } 
    // unregister
    else if (msg->type == '2'){
        // create storage the length of the payload
        string ip(msg->payload.begin(), msg->payload.end());
        // send the payload
        unregisterIn(ip);
    } 
    // exit game
    else if (msg->type == '3'){
        // send the default msg
        exitGameIn();
    } 
    // create game
    else if (msg->type == '4'){
        // check the payload size

        // break up the payload by length
        int gameId;
        // fill in the gameId
        memcpy(&gameId, msg->payload.data(), sizeof(int));
        string payload(msg->payload.begin(), msg->payload.end());
cout << "whole payload = " << payload << endl;
        // find the delimiter :
        size_t delim = payload.find(':');
        // fill in the host ip
        string hostIp = payload.substr(delim + 1);
cout << "the hostIp that has been recovered = " << hostIp << endl;
        // send the payload
        createGameIn(gameId, hostIp);
    } 
    // game full/ a game has been joined
    else if (msg->type == '5' || msg->type == '6'){
        // create storage the length of the payload
        int gameId;
        memcpy(&gameId, msg->payload.data(), sizeof(int));
        // send the payload
        joinGameIn(gameId);
    } else {
        cerr << "ERROR: Unknown Msg type: " << msg->type <<  ", unable to process msg" << endl;
    }
    delete msg;
}

// function with loop to run while in a game

// listen for msgs on the broadcast
void Player::listenForMsgs(){
    while (!shutdown_flag){
cout << "calling recieve non block udp" << endl;
        // read in a UDP broadcast msg here 
        baseMsg* msg = receiveNonblockingUdp(this->broadSd, this->broadcastAddr);
        // make sure there is a msg 
        if (msg != nullptr){
cout << "made a new thread to process a msg that was recieved" << endl;
            // make thread and have them run processMsgs() and pass in the msg recieved
            thread* new_thread = makeThread(this, msg);
cout << "thread has been made and maybe finished running?" << endl;
            // put thread in vector
            threads.push_back(new_thread);
        }

        // check for connections
        int clientSd = acceptClientPlayer();
        // check if a connection was made
        if (clientSd != -1){
            // change the saved sd
            // the TCP msgs are read and handled in the startGame()
            this->playerSd = clientSd;
        }
    }
}

// print out the help message to the user
void Player::printHelp(){
    cout << "These are the avaliable commands:" << endl;
    cout << "   help               - Display this message" << endl;
    cout << "   register           - Registers you into the player list" << endl;
    cout << "   listGames          - Lists the games avaliable to join" << endl;
    cout << "   createGame         - Host a game and wait for another player to join" << endl;
    cout << "   joinGame <gameId>  - Join the game assigned to the gameId" << endl;
    cout << "   exitGame           - Exit your current game" << endl;
    cout << "   unregister         - Unregister yourself from the player list" << endl;
    cout << "   exit               - Exit the program" << endl;
    cout << "   rules              - Get the rules of the game" << endl;
}

// print out the rules of the game
void Player::printRules(){
    cout << "These are the rules of the game:" << endl;
    cout << "Moves are based on the 3x3 grid. 0 0 is the top left corner. Get 3 in a row to win. Host always goes first." << endl;
    cout << "Type in exit to leave the current game." << endl;
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
        cout << "#: ";
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
        } else if (tokens[0] == "exit") {
            exit(1);
        }
        // check for the basic commands
        else if (tokens[0] == "exitGame"){
            exitGameOut();
        } else if (tokens[0] == "help"){
            printHelp();
        } else if (tokens[0] == "register"){
            cerr << "calling the registerPlayerOut" << endl;
            registerPlayerOut();
            cout << "registerPlayerOut command was processed" << endl;
        } else if (tokens[0] == "listGames"){
            listGames();
        } else if (tokens[0] == "createGame"){
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
        } else if (tokens[0] == "rules"){
            printRules();
        } else {
            cerr << "ERROR: Unknown command" << endl;
        }
    }
}