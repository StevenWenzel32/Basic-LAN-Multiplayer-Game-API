// this holds the protocols and code to run the Slime/Client side of the platformer game: Knight and Slime
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

using namespace std;

// handle making the socket structs
// can later add in params to change the family and scoktype
struct addrinfo* makeGetaddrinfo(const char* serverIp, const char* port){
    // for checking the return of getaddrinfo
    int status;
    // holds the info for the client address
    struct addrinfo client_addr;
    // points to the results that are in a linked list - is returned
    struct addrinfo *servinfo; 
    
    // create the struct and address info
    // make sure the struct is empty
    memset(&client_addr, 0, sizeof client_addr);
    // doesn't matter if its ipv4 or ipv6
    client_addr.ai_family = AF_UNSPEC;
    // tcp stream sockets
    client_addr.ai_socktype = SOCK_DGRAM;
    
    // getaddrinfo with error check
    if ((status = getaddrinfo(serverIp, port, &client_addr, &servinfo)) != 0 ) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    return servinfo;
}

// make the socket and do error checks, and return the Sd
int makeSocket(struct addrinfo* servinfo){
    // open a stream-oriented socket with the internet address family
    int clientSd = socket(servinfo->ai_family, servinfo->ai_socktype, 0);
    // check for error
    if(clientSd == -1){
        cerr << "ERROR: Failed to make socket" << endl;
        exit(1);
    }
    
    return clientSd;
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

// take the game world updates from the host and send client postion and actions to the host
int main (int argc, char* argv[]) {
    // join the level

    // while loop 
        // continually manage the clinet/slime players positions and actions
        // contnually check for updates from the host/knight player

    // while loop when they are outside a level - until the host player disconnects or the client disconnects

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