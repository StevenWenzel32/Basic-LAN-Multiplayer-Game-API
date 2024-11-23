// These are the basic networking and socket related functions and basic msg sends and recieves
#ifndef BASIC_NETWORK_HPP
#define BASIC_NETWORK_HPP

// unsure if being used
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>      // bzero
#include <cerrno>


// being used
#include <iostream>       // in and out to the screen
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netdb.h>        // gethostbyname
#include <fcntl.h>        // for fcntl -- to set non-blocking
#include <netinet/tcp.h>  // SO_REUSEADDR -- to set reuse
#include <unistd.h>       // read, write, close
#include <string.h>       // memset
#include <cstring>        // for strerror
#include <errno.h>        // errno
#include <arpa/inet.h>    // conversion from little endian and big endian

// hard coded values
// port to use/ listen on
#define PORT "2087"
// # of connection requests for the server to listen to at a time
// based losely off of the class size
#define BACKLOG 50
// end of the whole message -- not sure if it's needed
#define MSG_END "byebyeBABY"
// size to make the buffer for recieving the data in msgs
#define BUFFER_SIZE 1468

// name spaces
using namespace std;

// structs 
struct baseMsg {
    // the length of the msg being sent
    unsigned int length;
    // the type of msg being sent
    unsigned char type;
    

};

// making addrinfo
// make a addrinfo for listening using - server side
struct addrinfo* makeAddrinfo(string type, int port);
// make a addrinfo for socket structs - client side
struct addrinfo* makeAddrinfo(string type, const char* serverIp, int port)

// make a socket for any purpose as set in the makeAddrinfo funcs
int makeSocket(struct addrinfo* servinfo);

//socket settings
// make the sd non blocking -- curenntly only seems useful for listening sockets
void setNonblocking(int serverSd);
// make the socket reusable after discconnect - usually server side
void setSocketReuse(int serverSd);

// bind the socket to the port
void bindSocket(int serverSd, struct addrinfo* servinfo);

// listen on the socket for up to backlog # of connections
void listening(int serverSd, int backlog);

// Accept the connection as a new socket -- server side
// is non-blocking
int acceptConnection(int serverSd);

// connect the socket to the server, do error checks, frees addrinfo list - client side
void connectSocket(int clientSd, struct addrinfo* servinfo);

// ends the connection
void closeSocket(int sd);

// msg sending
// send a UDP msg as char[]
void sendUdpMsg(int sd, char message[], struct addrinfo *servinfo);
// send a TCP msg as char[] 
void sendTcpMsg(int sd, char message[]);

// msg receiving
// should also make a blocking version **
// recieve a UDP msg, reliable delivery, returns the msg as char[] -- does not wait for a response/non blocking
char* receiveReliableUdp(int clientSd, struct addrinfo* servinfo);
// should also make a blocking version later **
// receive a UDP msg, returns the msg as char[] -- does not wait for a response/non blocking
char* receiveUdp(int clientSd, struct addrinfo* servinfo);

// receive msg, blocking/stop and wait, returns msg as char* - tcp
char* receiveBlockingTcp(int sd);
// this is a great example of how the other receives should be made later on ***
// receive msg, non-blocking does not wait, returns msg as char* - tcp
char* receiveNonblockingTcp(int sd);

#endif