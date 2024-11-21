// This is basically the lobby -- handles the server side of making the player connections

// go through and get rid of the includes that aren't needed
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <string.h>       // memset
#include <errno.h>        // errno
#include <fstream>        // ofstream for file creation
#include <netdb.h>        // gethostbyname
#include <sstream>        // for stringstream stuff
#include <random>         // for random #
#include <queue>          // for queue, duh
#include <cerrno>
#include <cstring>        // for strerror
#include <vector>
#include <array>
#include <unordered_map>
#include <signal.h>       // for the shutdown signal
#include <fcntl.h>        // for fcntl -- to set non-blocking

using namespace std;

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

// port to use/ listen on
#define PORT "2087"
// # of connection requests for the server to listen to at a time, used in listen call
#define BACKLOG 50

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

// handle making the socket struct for listening -- makes a UDP socket
// can later add in params to change the family and socktype and optional flags and port #
struct addrinfo* makeGetaddrinfo(){
    // for checking the return of getaddrinfo
    int status;
    // holds the info for the server address
    struct addrinfo server_addr;
    // points to the results that are in a linked list - is returned
    struct addrinfo *servinfo; 
    
    // create the struct and address info
    // make sure the struct is empty
    memset(&server_addr, 0, sizeof(server_addr));
    // doesn't matter if its ipv4 or ipv6
    server_addr.ai_family = AF_UNSPEC;
    // tcp stream sockets
    server_addr.ai_socktype = SOCK_STREAM;
    // fill in my IP for me 
    server_addr.ai_flags = AI_PASSIVE;

    // getaddrinfo and error check in one -- doesn't need an IP/host because this is for listening
    if ((status = getaddrinfo(NULL, PORT, &server_addr, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    return servinfo;
}

// make the listening socket, make it non-blocking, and do error checks, and return the Sd
int makeListeningSocket(struct addrinfo* servinfo){
    // open a stream-oriented socket with the internet address family
    int serverSd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    // check if the socket call had an error
    if (serverSd == -1) {
        cerr << "error making the socket: serverSd - " << serverSd << endl;
    }

    // get the current flags
    int flags = fcntl(serverSd, F_GETFL, 0);
    // turn on the non-blocking flag
    fcntl(serverSd, F_SETFL, flags | O_NONBLOCK); 

    return serverSd;
}

// set the socket reuse function to help free up unused sockets and ports
void setSocketReuse(int serverSd){
    // Enable socket reuse without waiting for the OS to recycle it
    // set the so-reuseaddr option
    const int on = 1;
    int success = setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));
    // check if the option call failed
    if (success == -1) {
        cerr << "Error setting the socket reuse option: serverSd - " << serverSd << endl;
    }
}

// bind the socket
void bindSocket(int serverSd, struct addrinfo* servinfo){
    // Bind the socket to the port we passed into getaddrinfo
    int binding = bind(serverSd, servinfo->ai_addr, servinfo->ai_addrlen);
    // check if the bind had an error
    if (binding == -1) {
        cerr << "Error binding socket: serverSd - " << serverSd << " to port: " << PORT << endl;
    }
}

// listen on the socket
void listening(int serverSd, int backlog){
    // instruct the OS to Listen to up to N connection requests on the socket
    int listening = listen(serverSd, backlog);
    // check if listen has an error
    if (listening == -1) {
        cerr << "Error listening on socket: serverSd - " << serverSd << endl;
    } else {
        cout << "Server: Waiting for connections..." << endl;
    }
}

// close the socket and check for errors
void closeSocket(int sd){
    int bye = close(sd);
    if (bye == -1){
        cerr << "Error closing socket" << endl;
    }
}

// send a simple UDP msg as int[] -- how to send msgs????
// ****************
void sendMsg(int sd, int message[], struct addrinfo *servinfo){
    int bytes_sent = sendto(sd, message, BUFFER_SIZE, 0, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bytes_sent == -1){
        cerr << "Problem with simple send" << endl;
    }
}

// recieve msg, non blocking
// ***************
int recieveMsg(int clientSd, struct addrinfo* servinfo){
    // "buffer" for reading in and returning the server ackNum
    int ackNum = -1;
    int nRead = 0;
    nRead = recvfrom(clientSd, &ackNum, sizeof(int), MSG_DONTWAIT, servinfo->ai_addr, &(servinfo->ai_addrlen));
        if (nRead == -1){
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // do nothing simply nothing to read yet
                ackNum = -1;
            } else {
                cerr << "Error reading from socket: clientSd = " << clientSd << endl;
                ackNum = -2;
            }
        } else if (nRead == 0) {
            cerr << "Server closed the connection" << endl;
            ackNum = -3;
        } else {
// cout << "CLIENT Recieved via non blocking read - AckNum = " << ackNum << endl;
            return ackNum;
        }
    return -4;
}
// combine with above
// read the request from the client
string readRequest(int sd){
    // string to hold and return the request
    string request;
    // "buffer" for reading in the server response
    char buffer[BUFFER_SIZE];
    while (true){
        int nRead = recv(sd, &buffer, BUFFER_SIZE - 1, 0);
        if (nRead == -1){
            cerr << "Error reading from socket: SD = " << sd << endl; 
            return ""; 
        } else if (nRead == 0) {
            cerr << "Client closed the connection" << endl;
            break;
        } 
        // null terminate th buffer to help other functions work right
        buffer[nRead] = '\0';
        // add what is read to the request
        request.append(buffer);

        // check for the end of the request and exit if found -- means we got the whole message
        if (request.find("\r\n\r\n") != string::npos){
            break;
        }
    }
    return request;
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

int acceptConnection(int serverSd){
    // connector's address information can be either IPv4 or IPv6
    struct sockaddr_storage their_addr;
    // size of clients address
    socklen_t their_AddrSize = sizeof(their_addr);
    // Accept the connection as a new socket
    int newSd = accept(serverSd, (struct sockaddr *)&their_addr, &their_AddrSize);
    // check if the connection was made properly
    if (newSd == -1) {
        // check if there are no pending connections -- not a real error 
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
        } else {
            // connect fails
            cerr << "Error accepting connection on socket: serverSd - " << serverSd << endl;
        }
    } else {
      //  cerr << "Connection made on socket: newSd - " << newSd << endl;
    }
    return newSd;
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


struct baseMsg {
    // the type of msg being sent
    unsigned char type;
    // the length of the msg being sent
    unsigned int length;

};

struct gameListMsg {

};

struct joinGameMsg {

};

struct exitGameMsg {

};