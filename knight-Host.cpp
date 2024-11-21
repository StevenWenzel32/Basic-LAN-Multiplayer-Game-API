// this holds the protocols and code to run the knight/host side of the platformer game Knight and Slime
// these are the portocols to run once the 2 players are connected

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
#include <chrono>         // for steady_clock and timer stuff
#include <queue>          // for queue, duh
#include <cerrno>
#include <cstring>        // for strerror
#include <vector>
#include <array>

// my files
#include "player.hpp"

using namespace std;

class KnightHost : public Player{

};

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
    server_addr.ai_socktype = SOCK_DGRAM;
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

// set the socket resue function to help free up unused sockets and ports
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

// close the socket and check for errors
void closeSocket(int sd){
    int bye = close(sd);
    if (bye == -1){
        cerr << "Error closing socket" << endl;
    }
}

// send a simple UDP msg as int[]
void sendMsg(int sd, int message[], struct addrinfo *servinfo){
    int bytes_sent = sendto(sd, message, BUFFER_SIZE, 0, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bytes_sent == -1){
        cerr << "Problem with simple send" << endl;
    }
}

// recieve and return the ackNum from server without waiting for a response - used for sliding window
int readAckNoBlock(int clientSd, struct addrinfo* servinfo){
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

// manage the game world and the host player
int main (int argc, char* argv[]) {
    // create/launch the level
    
    // while loop 
        // continually manage the host/knight players positions and actions
        // contnually check for updates from the client/slime player

    // while loop when they are outside a level - until the host player disconnects

    return 0;
} 
// connect to the server, connect to another player, disconnect from the server
// check for messages from the server 
int main (int argc, char* argv[]) {
    // check that the command line the right # of params
    if (argc < 4){
        cerr << "Error: Not enough parameters passed in. Usage: " << argv[0] << " <PORT>, <Method>, <Host/URL>\n";
        return 1;
    }

    // params passed in through command line
    string port = argv[1];
    string method = argv[2];
    string url = argv[3];

    // parse the input
    pair<string, string> hostParse = parseHost(url);

    // build
    string msg = buildRequest(method, hostParse.first, hostParse.second);

    // make the socket structs and error check
    // pass in serverIP/domain -- getaddrinfo does the DNS lookup for us, how nice!
    struct addrinfo* servinfo = makeGetaddrinfo(hostParse.first, port);

    // make the socket
    int clientSd = makeSocket(servinfo);

    // connect the socket to the server
    connectSocket(clientSd, servinfo);

    // ensure all the data in the msg is sent
    sendAllData(clientSd, msg);

    // recieve the server response
    string reply = readResponse(clientSd);

    // process server response
    processResponse(reply);
    
    // call that handles error checks in other function
    closeSocket(clientSd);

    return 0;
}