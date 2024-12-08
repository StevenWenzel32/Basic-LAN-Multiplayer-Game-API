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

// custom constructor - does nothing for now
Player::Player() {
    
}

// custom destructor - does nothing for now
Player::~Player() {

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
//        cout << "ip has been set" << endl;
        // send your ip addr on the broadcast msg to the LAN - udp
        registerMsg(this->ip);
        cout << "You've been registered" << endl;
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
//cout << "player registered" << endl;
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
    // give id 
    int gameId = gameCounter;
    // up the game count 
    gameCounter++;
    // update your current game
    this->game.id = gameId;
    // mark yourself as host 
    this->game.host = true;
//cout << "ip being given for the game host = " << this->ip << endl;
cout << "You started the game with the ID = " << this->game.id << endl;
    // broadcast the creation of the game - udp 
    createGameMsg(gameId, this->ip);
    // start a game in the main thread - prevents the player from using program cmds while in game
    this->game.startGame();
}

// handling the recieving of a notification that a new game was created
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
    // give the new game its id 
    newGame.id = gameId;
    // fill in host
    newGame.hostIp = hostIp;
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(gamesMutex);
    // add the given game to your local game list
    avaliableGames.emplace(gameId, newGame);
}

// connect to the host of a game
void Player::joinGameOut(int gameId){
//cout << "Attempting to join game" << endl;
    try {
        // lock the mutex before accessing the shared memory 
        lock_guard<mutex> lock(gamesMutex);
        // check if the gameId exsits
        if (avaliableGames.find(gameId) == avaliableGames.end()){
            // the game don't exists
            cout << "The game you want to join doesn't exist" << endl;
            return;
        }
//cout << "about to call connectToHost" << endl;
//cout << "hostIP = " << avaliableGames.at(gameId).hostIp << endl;
        // connect to host and update the playerSd
        connectToHost("tcp", avaliableGames.at(gameId).hostIp, TCP_PORT);
        // update your current game.id
        this->game.id = gameId;
        // update your game list by removing the game you joined
        avaliableGames.erase(gameId);
        // broadcast that the game is full and should be removed from the list of games
        gameFullMsg(gameId);
//cout << "game full msg sent" << endl;
        // start a game in the main thread - prevents the player from using program cmds while in game
        this->game.startGame();
    } catch (const out_of_range& e){
        cerr << "ERROR: Game ID is out of range" << endl;
    } catch (const exception& e){
        cerr << "ERROR: an exception occured while trying to join the game" << endl;
    }
}

// handling the recieving of a notification that a game is full -- need mutexs
void Player::joinGameIn(int gameId){
    // lock the mutex before accessing the shared memory 
    lock_guard<mutex> lock(gamesMutex);
    // remove the game from the list
    avaliableGames.erase(gameId);
}

// remove yourself from your player list and broadcast to others to remove you
void Player::unregisterOut(){
    // broadcast out that others should remove you from player list
    unregisterMsg(this->ip);
    // reset id
    this->id = 0;
    cout << "You've been unregistered" << endl;
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
void Player::connectToHost(string type, string hostIp, string port){
    // make the addrinfo
    struct addrinfo* hostinfo = makeAddrinfo(type, hostIp, port);
    // update the playerInfo - needed in case of UDP
    this->game.playerinfo = hostinfo;
    // make the socket
    int hostSd = makeSocket(hostinfo);
    // connect to the socket
    connectSocket(hostSd, hostinfo);
    // set the socket to be non blocking
    setNonblocking(hostSd);
    // update the playerSd
    this->game.playerSd = hostSd;
}

// accepts the connection to the client player - host side
// returns the new SD
void Player::acceptClientPlayer(){
// cout << "trying to accept TCP connection +++++++" << endl;
    // check if a connection was made
    int clientSd = acceptConnection(this->tcpListenSd);
    if (clientSd != -1){
        cout << "A player has joined your game" << endl;
        // set the socket to be non blocking
        setNonblocking(clientSd);
        // change the saved playerSd
        this->game.playerSd = clientSd;
        // send a confirmantion msg to the client player
        clientJoinMsg();
    }
// cout << "end of accpetClientPlayer " << endl;
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
//cout << "this is the players ip = " << string(ipStr) << endl;
    this->ip = string(ipStr);

    freeaddrinfo(res);
}

// msg creation and sending functions

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
    struct baseMsg msg(1, payload, strlen(payload));
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
    struct baseMsg msg(2, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// broadcast the creation of a game, sends the game info
void Player::createGameMsg(int gameId, string hostIp){
    // put the passed in values into an acceptable payload
    char payload[256];
    snprintf(payload, sizeof(payload), "%d:%s", gameId, hostIp.c_str());
    // create the baseMsg
    // 3 = "createGame"
    struct baseMsg msg(3, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// broadcast that the game you just joined is full, send gameId
void Player::gameFullMsg(int gameId){
    // put the passed in values into an acceptable payload
    char payload[32];
    snprintf(payload, sizeof(payload), "%d", gameId);
    // create the baseMsg
    // 4 = "gameFull"
    struct baseMsg msg(4, payload, strlen(payload));
    // send the baseMsg
    sendUdpMsg(this->broadSd, msg, this->broadcastAddr);
}

// broadcast that the game you just joined is full, send gameId
void Player::clientJoinMsg(){
    // put the passed in values into an acceptable payload
    char payload[32];
    // the msg to send 
    string notice = "Connected to Host!";
    snprintf(payload, sizeof(payload), "%s", notice.c_str());
    // create the baseMsg
    // 3 = msg/notice to print
    struct baseMsg msg(3, payload, strlen(payload));
    // send the baseMsg
    sendTcpMsg(this->game.playerSd, msg);
// cout << "sent host connection confirmation" << endl;
}

// process the messages being sent over broadcast
void Player::processUdpMsgs(baseMsg* msg){
//cout << "inside process UDP Msgs" << endl;
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
    // create game
    else if (msg->type == 3){
        // put the whole payload into a string
        string payload(msg->payload.begin(), msg->payload.end());
//cout << "whole payload = " << payload << endl;
        // find the delimiter :
        size_t delim = payload.find(':');
        // fill in the gameId
        string gameId = payload.substr(0, delim);
//cout << "gameId after fill" << endl;
        // fill in the host ip
        string hostIp = payload.substr(delim + 1);
//cout << "the hostIp that has been recovered = " << hostIp << endl;
        // send the payload
        createGameIn(stoi(gameId), hostIp);
    } 
    // game full
    else if (msg->type == 4){
        int gameId;
        memcpy(&gameId, msg->payload.data(), sizeof(int));
        // send the payload
        joinGameIn(gameId);
    } else {
        cerr << "ERROR: Unknown UDP Msg type: " << msg->type <<  ", unable to process msg" << endl;
    }
    delete msg;
}

// function with loop to run while in a game

// listen for msgs on the broadcast - manages the match making
void Player::listenForUdpMsgs(){
    while (!GlobalFlags::shutdown_flag){
//cout << "in listen for broadcast" << endl;
        // read in a UDP broadcast msg here 
        baseMsg* msg = receiveNonblockingUdp(this->broadSd, this->broadcastAddr);
        // make sure there is a msg 
        if (msg != nullptr){
//cout << "made a new thread to process a UDP msg that was recieved" << endl;
            // make thread and have them run processMsgs() and pass in the msg recieved
            thread* new_thread = new thread(&Player::processUdpMsgs, this, msg);
//cout << "UDP msg thread has been made" << endl;
            // put thread in vector
            threads.push_back(new_thread);
        }
    }
}

// listen for msgs on the broadcast - manages the match making
void Player::listenForTcpConnect(){
    while (!GlobalFlags::shutdown_flag){
//cout << "in listen for TCP connect" << endl;
        // if no player is already connected 
        if (this->game.playerSd == 0){
            acceptClientPlayer();
        }
    }
}

// print out the help message to the user
void Player::printHelp(){
    cout << "These are the avaliable commands (This list does not include commands unique to the game):" << endl;
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

// process the cmds and input given from the user
void Player::processProgramCmds(string input){
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

    // error checks
    if (tokens.empty()){
        cerr << "no input given" << endl;
    } else if (tokens.size() >= 3){
        cerr << "too many arguments given" << endl;
    }
    // narrow the possiblity by size
    else if (tokens.size() == 1){
        // check for the basic commands
        if (tokens[0] == "exit") {
            cout << "exiting the program" << endl;
            // set the shutdown flag
            GlobalFlags::shutdown_flag = 1;
        } else if (tokens[0] == "help"){
            printHelp();
        } else if (tokens[0] == "register"){
            registerPlayerOut();
        } else if (tokens[0] == "listGames"){
            listGames();
        } else if (tokens[0] == "createGame"){
            createGameOut();
        } else if (tokens[0] == "unregister"){
            unregisterOut();
        } else if (tokens[0] == "rules"){
            this->game.printRules();
        } else {
            cerr << "ERROR: Unknown command" << endl;
        }
    } 
    // once there are more cmds with two args can break this up
    else if (tokens[0] == "joinGame" && tokens.size() == 2){
        // grab the game Id
        string gameId = tokens[1];
        // check that the id is a number
        if (all_of(gameId.begin(), gameId.end(), static_cast<int(*)(int)>(isdigit))){
            joinGameOut(stoi(gameId));
        } else {
            cerr << "ERROR: Unknown command" << endl;
        }
    }    
}

// main thread sends msgs - handles the player executing/sending out their broadcast msgs and protocols
// when a game gets started through here the game will take over the thread that this has been called in - in this case main
void Player::inputPrompt(){
    // print out a msg to the terminal prompting user to enter a command
    cout << "Welcome to the match making lobby" << endl;
    cout << "When you see this #: it is prompt to enter a command" << endl;
    printHelp();
    // store the input from the player
    string input;

    // accept input from the user
    while(!GlobalFlags::shutdown_flag){
        // set stdin to blocking mode -- it might have been inadvertantly changed to non-blocking due to the sockets being non-blocking
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);

        // prompt user for input
        cout << "#: " << flush;
//cout << "about to read input" << endl;
        // get their response
        if (getline(cin, input)){
//cout << "input read succesfully" << endl;
            if (!input.empty()){
                processProgramCmds(input);
            } 
        } else if (cin.eof()){
            cout << "getline failed. cin state: " << cin.rdstate() << endl;
            cerr << "End of input detected. Exiting program..." << endl;
            GlobalFlags::shutdown_flag = 1;
        } else if (cin.fail()){
            cout << "getline failed. cin state: " << cin.rdstate() << endl;
            cerr << "ERROR: reading in players input. clearing the input stream" << endl;
            // clear error flags
            cin.clear();
            // get rid of the invalid input in the stream
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cerr << "ERROR: Unidentified error while reading input stream" << endl;
        }
    }
}