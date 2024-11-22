// These are the basic networking and socket related functions and basic msg sends and recieves

// my files
#include "basicNetworking.hpp"

// handle making the socket struct for listening -- can make both TCP and UDP, takes in port #
// can later add in params to change the family and optional flags
struct addrinfo* makeAddrinfo(string type, int port){
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
    // check what type of socket
    if (type == "udp"){
        // udp sockets
        server_addr.ai_socktype = SOCK_DGRAM;
    } else if (type == "tcp"){
        // tcp stream sockets
        server_addr.ai_socktype = SOCK_STREAM;
    }
    // fill in my IP for me 
    server_addr.ai_flags = AI_PASSIVE;

    // getaddrinfo and error check in one -- doesn't need an IP/host because this is for listening
    if ((status = getaddrinfo(NULL, port, &server_addr, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    return servinfo;
}

// handle making the socket structs -- can make both TCP and UDP sockets, takes in port #
struct addrinfo* makeAddrinfo(string type, const char* serverIp, int port){
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
    // check what type of socket
    if (type == "udp"){
        // udp sockets
        server_addr.ai_socktype = SOCK_DGRAM;
    } else if (type == "tcp"){
        // tcp stream sockets
        server_addr.ai_socktype = SOCK_STREAM;
    }
    
    // getaddrinfo with error check
    if ((status = getaddrinfo(serverIp, port, &client_addr, &servinfo)) != 0 ) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    return servinfo;
}

// make the socket, do error checks, and return the Sd
int makeSocket(struct addrinfo* servinfo){
    // open a stream-oriented socket with the internet address family
    int sd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    // check if the socket call had an error
    if (sd == -1) {
        cerr << "error making the socket: serverSd - " << serverSd << endl;
    }
    return sd;
}

// make the sd non-blocking -- only being used for listening sockets for now
void setNonblocking(int serverSd){
    // get the current flags
    int flags = fcntl(serverSd, F_GETFL, 0);
    // turn on the non-blocking flag
    fcntl(serverSd, F_SETFL, flags | O_NONBLOCK); 
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
        cerr << "Error binding socket: serverSd - " << serverSd << " to port: ?port recognition not supported?" << endl;
    }
}

// close the socket and check for errors
void closeSocket(int sd){
    int bye = close(sd);
    if (bye == -1){
        cerr << "Error closing socket" << endl;
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

// connect the socket to the server, do error checks, frees addrinfo list - client side
void connectSocket(int clientSd, struct addrinfo* servinfo){
    int connectStatus = connect(clientSd, servinfo->ai_addr, servinfo->ai_addrlen);
    // check for error
    if(connectStatus == -1){
        cerr << "failed to connect to the server" << endl;
    }
    
    // free the linked list -- all done with our connections 
    freeaddrinfo(servinfo);
}





// types of sending and recieving below here

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

// send a simple UDP msg as int[]
void sendMsg(int sd, int message[], struct addrinfo *servinfo){
    int bytes_sent = sendto(sd, message, BUFFER_SIZE, 0, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bytes_sent == -1){
        cerr << "Problem with simple send" << endl;
    }
}