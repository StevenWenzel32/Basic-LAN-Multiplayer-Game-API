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
#include <vector>         // vector bu dur

// hard coded values
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
    // data to send should be other data packet structs
    vector<char> payload;

    // constructor with passing in values
    baseMsg(unsigned char msgType, const char* payloadData, unsigned int payloadSize){
        // get the payload size and the size of the type var
        length = payloadSize;
        // pass in the type
        type = msgType;
        // pass in the payload data and its size
        payload.assign(payloadData, payloadData + payloadSize);
    }
};

// making addrinfo
// make a addrinfo for listening using - server side
struct addrinfo* makeAddrinfo(string type, string port);
// make a addrinfo for socket structs - client side
struct addrinfo* makeAddrinfo(string type, string serverIp, string port);

// make a socket for any purpose as set in the makeAddrinfo funcs
int makeSocket(struct addrinfo* servinfo);

//socket settings
// make the sd non blocking -- curenntly only seems useful for listening sockets
void setNonblocking(int serverSd);
// make the socket reusable after discconnect - usually server side
void setSocketReuse(int serverSd);
// turn on broadcasting for the socket
void setSocketBroadcast(int sd);

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
// helper function to searliaze the baseMsg into a vector<char> so the send will work right
vector<char> serializeBaseMsg(const baseMsg& msg);
// send a UDP msg as baseMsg - takes in an addrinfo
void sendUdpMsg(int sd, const baseMsg& message, struct addrinfo *destinfo);
// send a UDP msg as baseMsg - takes in the structaddr_in
void sendUdpMsg(int sd, const baseMsg& message, struct sockaddr_in addr);
// send a TCP msg as baseMsg
void sendTcpMsg(int sd, const baseMsg& message);

// msg receiving
// should also make a blocking version **
// recieve a UDP msg, reliable delivery, returns the msg as char[] -- does not wait for a response/non blocking
//char* receiveReliableUdp(int clientSd, struct addrinfo* servinfo);
// should also make a blocking version later **
// receive a UDP msg, returns the msg as char[] -- does not wait for a response/non blocking
//char* receiveUdp(int clientSd, struct addrinfo* servinfo);
// actually used ***
// recieve a UDP msg, non blocking, returns msg as baseMsg* - takes in adrrinfo
baseMsg* receiveNonblockingUdp(int clientSd, struct addrinfo* addrinfo);
// recieve a UDP msg, non blocking, returns msg as baseMsg* - takes in structaddr_in
baseMsg* receiveNonblockingUdp(int clientSd, struct sockaddr_in addrinfo);

// receive msg, blocking/stop and wait, returns msg as char* - tcp
baseMsg receiveBlockingTcp(int sd);

// this is being used
// this is a great example of how the other receives should be made later on ***
// receive a TCP msg, non-blocking does not wait, returns msg as baseMsg*
baseMsg* receiveNonblockingTcp(int sd);

#endif