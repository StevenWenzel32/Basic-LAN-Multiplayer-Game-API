// These are the basic networking and socket related functions and basic msg sends and recieves
#ifndef BASIC_NETWORK_HPP
#define BASIC_NETWORK_HPP

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

// port to use/ listen on
#define PORT "2087"
// # of connection requests for the server to listen to at a time, used in listen call
#define BACKLOG 50

using namespace std;

// make a addrinfo for listening using - server side
struct addrinfo* makeAddrinfo(string type, int port);
// make a addrinfo for socket structs - client side
struct addrinfo* makeAddrinfo(string type, const char* serverIp, int port)

// make a socket for any purpose as set in the makeAddrinfo funcs
int makeSocket(struct addrinfo* servinfo);

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

#endif