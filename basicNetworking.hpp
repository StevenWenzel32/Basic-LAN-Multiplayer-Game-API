// These are the basic networking and socket related functions and basic msg sends and recieves
#ifndef BASIC_NETWORK_HPP
#define BASIC_NETWORK_HPP

// unsure if being used
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>      // bzero
#include <errno.h>        // errno
#include <cerrno>
#include <cstring>        // for strerror

// being used
#include <iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netdb.h>        // gethostbyname
#include <fcntl.h>        // for fcntl -- to set non-blocking
#include <netinet/tcp.h>  // SO_REUSEADDR -- to set reuse
#include <unistd.h>       // read, write, close
#include <string.h>       // memset

// port to use/ listen on
#define PORT "2087"
// # of connection requests for the server to listen to at a time
// based losely off of the class size
#define BACKLOG 50

using namespace std;

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
// recieve a UDP msg, reliable delivery, returns the ackNum -- does not wait for a response
int receiveReliableUDP(int clientSd, struct addrinfo* servinfo);


#endif