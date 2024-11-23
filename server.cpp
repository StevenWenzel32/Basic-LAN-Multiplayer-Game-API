// This is basically the lobby -- handles the server side of making the player connections

// my files
#include "server.hpp"

// player struct to hold player info
struct player{
    // name to identify the player
    string username = "";
    // id for the player
    int id = 0;
    // port to use for connection to the player's game
    unsigned short port;
    // players ip address
    array<unsigned short, 8> ip;
}Player;

// game struct to hold game info
struct game{
    int id = 0;
    // the knight/host/player1
    struct player host;
    // the slime/client/player2
    struct player client;
    unsigned char players = 0;
}Game;

// need a list of players
// expecting around 50 players - class size is less than 45
unordered_map<int, Player> players;
// used to create player ids
int playerCounter = 1;

// need a list of games being played -- expecting roughly 25 games to be played at once
// maps the gameId to the game
unordered_map<int, Game> fullGames;
// need a list of games that need another player
// maps the gameId to the game
unordered_map<int, Game> avaliableGames;
// counter to create game ids
int gameCounter = 1;



// holds the threads to ensure proper shutdown later on 
vector<pthread_t> threads; 

// struct to pass data to threads
struct threadData {
    int sd;
    int repetition;
};

// flag for shutting the server down -- ends all while loops - would probably be best to find a way to end for loops too
volatile sig_atomic_t shutdown_flag = 0;

// handles the shut down of the server
void signalHandler(int signum) {
    // cerr << "Server received shutdown signal: " << signum << ". Initiating clean shutdown..." << endl;
    shutdown_flag = 1;
}

// parse the GET request: first = method, second = file
pair<string, string> parseRequest(string& msg){
    // make a stream for the msg
    stringstream ss(msg);
    // make the strings to be stored
    string method;
    string file;
    // needed to put the http string into, but is not used elsewhere
    string http;
    
    // break the status line into seperate vars and check if it worked -- can switch to using the (fail(), etc.) for more helpful error checks for the user 
    if (!(ss >> method >> file >> http)){
        // return the error code so it can be put in the response
        return {"400", "Bad Request"};
    }

    // check if the file has no . ending -- ie no .html -- checks for 400 error
    if(file.find_last_of('.') == string::npos){
        return {"400", "No file extension"};
    }

    return {method, file};
}

// function that the threads call. reads the request, processes it and returns a response
// might have to change the format to use if checks after each check* method to return the error quicker and to avoid possible errors
void* processRequest(void *ptr) {
    // cast back into data struct
    struct thread_data *data = (thread_data*) ptr;

    string request = readRequest(data->sd);

    // get the method and file from the request -- checks for 400
    pair<string, string> requestParse = parseRequest(request);

    int code = 200;
    // if the parsing fails return 400
    if (requestParse.first == "400"){
        code = 400;
    }

    // check if the server supports the method -- check for 405 (and 418 if I have time)
    int methodCode = checkMethod(requestParse.first);
    // check if the code is 200
    if (code == 200){
        code = methodCode;
    }

    // build arbitrary forbidden and unauthed lists to check: first = forbidden, second = unauthed
    pair<vector<string>, vector<string>> fileLists = buildFileLists();
    // check if the file is in a forbidden or unauthed list -- checks for 401 and 403
    int forbiddenCode = checkFileLists(requestParse.second, fileLists.first, fileLists.second);
    // check if the code is 200
    if (code == 200){
        code = forbiddenCode;
    }

    // check if the server has the file -- checks for 404
    pair<int, string> fileStuff = checkForFile(requestParse.second);
    // check if the code is 200
    if (code == 200){
        code = fileStuff.first;
    }

    // get the codeName and body -- if all is well the code should still be 200
    pair<string, string> codeDetails = checkCode(code);

    // build the response
    string response = buildResponse(HTTP, to_string(code), codeDetails.first, codeDetails.second, fileStuff.second);

    // send response
    sendAllData(data->sd, response);

    // close after sending the response
    closeSocket(data->sd);

    // free thread data
    delete data;
    
    return nullptr;
}

// make a new thread and fill it's data struct
pthread_t makeThread(int sd, int reps){
    // create a new thread
    pthread_t new_thread;
    // create thread data
    struct thread_data *data = new thread_data;
    data->repetition = reps;
    data->sd = sd;

    // start the thread
    int status = pthread_create(&new_thread, NULL, processRequest, (void*) data);
    // check for thread creation error
    if (status != 0) {
        cerr << "Error making thread" << endl; 
        delete data;
    }

    return new_thread;
}

// announces that you are avaliable to play, carries contact info and puts the player into the avalible player list
// maybe add a player username
void Register(string username, int ip, unsigned int port){
    // create a new player object
    struct player newPlayer;
    // up the player counter
    playerCounter++;
    newPlayer.id = playerCounter;
    // get the player username - if using them
    newPlayer.username = username;
    // get the players ip
    newPlayer.ip = ip;
    // get the players port
    newPlayer.port = port;
    // put the player into the players list
    players.emplace(newPlayer.id, newPlayer);

    // send a success message to the player with their player ID
    registerSuccessMsg(newPlayer.id);
}

// list of avialbe games to join
// query the server or broadcast to LAN -- pick one
// can possibly show who is playing if using usernames
void ListGames(int playerId){
    // start of message
    string start = "Game List: \n";
    string list = "" + start;
    // go through all the games in the avaliable games list and display them
    for(int i = 0; i < avaliableGames.size(); i++){
        // print the game stats -- later store in var to try and integrate with UI
        list += "Game ID: " + avaliableGames.at(i).id + ", Host: " avaliableGames.at(i).host + ", Players: " << avaliableGames.at(i).players + "/2\n";
    }
    // send the list to the player all at once -- or perhaps in pieces in the loop
    sendGameListMsg(list, playerId);
}

// user starts a game and annoucnes that it is free to join
// if using usernames could have the game show who is playing/hosting
// has the contact info
void CreateGame(int playerId){
    // make the new game
    struct game newGame;
    // up the game count 
    gameCounter++;
    // give the game an id
    newGame.id = gameCounter;
    // fill in host
    newGame.host = players.at(playerId);
    // update player count
    newGame.players = 1;
    // put into list of games
    avaliableGames.emplace(newGame.id, newGame);

    // create the announcement
    string announce = "Player: " + playerId + ", has started a game! Check use ListGames() to take a look\n";
    // send anouncement to all connected players
    sendAnouncement(announce);
}

// puts the player into the game
void JoinGame(int playerId, int gameId){
    // find the game and its host
    struct player host = avaliableGames.at(gameId).host;
    // update the player count in the game
    avaliableGames.at(gameId).players++;
    // send the host connection details to the client
    sendHostInfo(host, playerId);
    // grab the game 
    struct game game = avaliableGames.at(gameId);
    // take game out of avaliable list
    avaliableGames.erase(gameId);
    // put game into the full list
    fullGames.emplace(gameId, game);
    // notify host of joining player
    playerJoinMsg(playerId, host);
}

// exit the game gracefully
void ExitGame(int playerId, int gameId){
    // grab the game
    struct game current = avaliableGames.at(gameId);
    // grab the host
    struct player host = current.host;
    // grab the client 
    struct player client = current.client;
    // check if the player is the host
    if (playerId == host.id){
        // send msg that the host has left to the client
        playerLeftMsg(playerId, client);
        // disconnect the host from the client
        // *****************
        // delete the game since host left
        fullGames.erase(gameId);
    } else {
        // send msg to the host that the client has left
        playerLeftMsg(playerId, host);
        // disconnect the client from the host
        // ************
        // remove the game from the full list
        fullGames.erase(gameId);
        // mark the client as empty
        current.client = nullptr;
        // put the game back into available games
        avaliableGames.emplace(gameId, current);
    }
}

// remove the player from the list of possible/available players
void Unregister(int playerId){
    // notify the player they are being unregistered
    unregisteredMsg();
    // remove the player from the list 
    players.erase(playerId);
}

// manages the player: joining, hosting, and leaving
int main (int argc, char* argv[]) {
    // Set up signal handling for SIGINT and SIGTERM so that the server can shut down properly
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // make the structs needed for the sockets
    struct addrinfo* servinfo = makeGetaddrinfo();

    // Create the listening socket on serverSd
    int serverSd = makeListeningSocket(servinfo);
    
    // set resue of socket
    setSocketReuse(serverSd);

    // bind socket
    bindSocket(serverSd, servinfo);

    // listen on socket for up to BACKLOG connections
    listening(serverSd, BACKLOG);
    // free the linked list of addrinfos - done with it after the listening call
    freeaddrinfo(servinfo);

    // never leave unless told to shutdown
    while (!shutdown_flag) {
        // accept new client connection
        int newSd = acceptConnection(serverSd);
        // check if there is a new valid connection
        if (newSd != -1){
            // make thread and pass the process request func
            pthread_t new_thread = makeThread(newSd, REPETITION);
            // put thread in vector
            threads.push_back(new_thread); 
        }
        // use threads to handle user msgs and requests make sure there is no issues with memory access
        // recieve msg
        // parse msg into request type
        // respond to msg
    }
    
    // once shutting down join all the threads
    for (pthread_t thread : threads) {
        pthread_join(thread, NULL);
    }

    // can close once the shutdown signal is recieved
    closeSocket(serverSd);

    return 0;
} 




struct gameListMsg {

};

struct joinGameMsg {

};

struct exitGameMsg {

};