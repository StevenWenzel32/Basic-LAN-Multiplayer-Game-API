// These are the basic networking and socket related functions and basic msg sends and recieves

// my files
#include "basicNetworking.hpp"

// handle making the socket struct for listening -- can make both TCP and UDP, takes in port #
// can later add in params to change the family and optional flags
struct addrinfo* makeAddrinfo(string type, string port){
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
    if ((status = getaddrinfo(NULL, port.c_str(), &server_addr, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    return servinfo;
}

// handle making the socket structs -- can make both TCP and UDP sockets, takes in port #
struct addrinfo* makeAddrinfo(string type, string serverIp, string port){
    // for checking the return of getaddrinfo
    int status;
    // holds the info for the client address
    struct addrinfo client_addr;
    // points to the results that are in a linked list - is returned
    struct addrinfo *servinfo; 
    
    // create the struct and address info
    // make sure the struct is empty
    memset(&client_addr, 0, sizeof(client_addr));
    // doesn't matter if its ipv4 or ipv6
    client_addr.ai_family = AF_UNSPEC;
    // check what type of socket
    if (type == "udp"){
        // udp sockets
        client_addr.ai_socktype = SOCK_DGRAM;
    } else if (type == "tcp"){
        // tcp stream sockets
        client_addr.ai_socktype = SOCK_STREAM;
    }
    
    // getaddrinfo with error check
    if ((status = getaddrinfo(serverIp.c_str(), port.c_str(), &client_addr, &servinfo)) != 0 ) {
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
        cerr << "error making the socket: sd - " << sd << endl;
    }
    return sd;
}

// make the sd non-blocking -- only being used for listening sockets for now
void setNonblocking(int sd){
    // get the current flags
    int flags = fcntl(sd, F_GETFL, 0);
    // turn on the non-blocking flag
    fcntl(sd, F_SETFL, flags | O_NONBLOCK); 
}

// set the socket reuse function to help free up unused sockets and ports
void setSocketReuse(int sd){
    // Enable socket reuse without waiting for the OS to recycle it
    // set the so-reuseaddr option
    const int on = 1;
    int success = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));
    // check if the option call failed
    if (success == -1) {
        cerr << "Error setting the socket reuse option: sd = " << sd << endl;
    }
}

// set the socket reuse function to help free up unused sockets and ports
void setSocketBroadcast(int sd){
    const int on = 1;
    int success = setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(int));
    // check if the option call failed
    if (success == -1) {
        cerr << "Error setting the socket broadcast option: sd = " << sd << endl;
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

// types of sending and recieving below here -- alow with some helpers

vector<char> serializeBaseMsg(const baseMsg& msg){
    // make the vector to hold the msg
    vector<char> serializedMsg;

    // convert the byte order to network for long type
    unsigned int length = htonl(msg.length);
    // put the length of the msg into the vector
    serializedMsg.insert(serializedMsg.end(), reinterpret_cast<char*>(&length), reinterpret_cast<char*>(&length) + sizeof(length));

    // put in the msg type 
    serializedMsg.push_back(msg.type);
    // put in the payload
    serializedMsg.insert(serializedMsg.end(), msg.payload.begin(), msg.payload.end());

    return serializedMsg;
}

// send a UDP msg as baseMsg - takes in adrrinfo
void sendUdpMsg(int sd, const baseMsg& msg, struct addrinfo *servinfo){
    // convert the baseMsg into a vecctor<char>
    vector<char> converted = serializeBaseMsg(msg);

    int bytes_sent = sendto(sd, converted.data(), converted.size(), 0, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bytes_sent == -1){
        cerr << "Problem with UDP send" << endl;
    }
}

// send a UDP msg as baseMsg - takes in sturctaddr_in
void sendUdpMsg(int sd, const baseMsg& msg, struct sockaddr_in addrinfo){
    // convert the baseMsg into a vecctor<char>
    vector<char> converted = serializeBaseMsg(msg);

    int bytes_sent = sendto(sd, converted.data(), converted.size(), 0, reinterpret_cast<struct sockaddr*>(&addrinfo), sizeof(addrinfo));
    if (bytes_sent == -1){
        cerr << "Problem with UDP send" << endl;
    }
}

// send a TCP msg as baseMsg
void sendTcpMsg(int sd, const baseMsg& msg){
    // convert the baseMsg into a vecctor<char>
    vector<char> converted = serializeBaseMsg(msg);

    // the total bytes sent
    int total = 0;
    // total bytes to send
    int bytes = converted.size();

    // in case of partial sends, send until all bytes are sent -- only needed for tcp
    while (total < bytes){
        int bytes_sent = send(sd, converted.data() + total, bytes - total, 0);
        if (bytes_sent == -1){
            cerr << "Problem with TCP send" << endl;
            break;
        }
        total += bytes_sent;
    }
}

// // recieve a UDP msg, reliable delivery, returns the ackNum -- does not wait for a response/non blocking **********
// char* receiveReliableUDP(int clientSd, struct addrinfo* servinfo){
//     // "buffer" for reading in and returning the server ackNum
//     int ackNum = -1;
//     int nRead = 0;
//     nRead = recvfrom(clientSd, &ackNum, sizeof(int), MSG_DONTWAIT, servinfo->ai_addr, &(servinfo->ai_addrlen));
//         if (nRead == -1){
//             if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                 // do nothing simply nothing to read yet
//                 ackNum = nullptr;
//             } else {
//                 cerr << "Error reading from socket: clientSd = " << clientSd << endl;
//                 ackNum = nullptr;
//             }
//         } else if (nRead == 0) {
//             cerr << "Server closed the connection" << endl;
//             ackNum = nullptr;
//         } else {
// // cout << "CLIENT Recieved via non blocking read - AckNum = " << ackNum << endl;
//             return ackNum;
//         }
//     return nullptr;
// }

// // read the msg from the client and feed into the buffer ie: message -- for UDP
// char* receiveUdp(int sd, struct addrinfo* servinfo){
//     int messageBuf[BUFFER_SIZE] = {0};
//     // recieve the message into the msg[] array and make sure it was read correctly
//             int nRead = recvfrom(sd, &messageBuf, BUFFER_SIZE, 0, servinfo->ai_addr, &(servinfo->ai_addrlen));
//             if (nRead == -1){
//                 if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                     // do nothing simply nothing to read yet
//                     return nullptr;
//                 } else {
//                     cerr << "Error reading from socket: SD = " << sd << endl; 
//                     return nullptr;
//                 }
//             } else if (nRead == 0) {
//                 cerr << "Client closed the connection" << endl;
//                 return nullptr;
//             }
//             return messageBuf[0];
// }

// recieve a UDP msg, non blocking, returns msg as baseMsg*
baseMsg* receiveNonblockingUdp(int sd, struct addrinfo* servinfo){
    int messageBuf[BUFFER_SIZE] = {0};
    // recieve the message into the msg[] array and make sure it was read correctly
    int nRead = recvfrom(sd, &messageBuf, BUFFER_SIZE, 0, servinfo->ai_addr, &(servinfo->ai_addrlen));
    // error checks
    if (nRead == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // do nothing simply nothing to read yet
            return nullptr;
        } else {
            cerr << "Error reading from socket: SD = " << sd << endl; 
            return nullptr;
        }
    } else if (nRead == 0) {
        cerr << "Client closed the connection" << endl;
        return nullptr;
    }
    
    // check if the length of the packet has come through
    if (nRead < sizeof(unsigned int) + sizeof(unsigned char)){
        // not enough data for the packet header
        return nullptr;
    }

    // to hold the packet length
    unsigned int packetLength = 0;
    // copy the first 4 bytes into the packetLength
    memcpy(&packetLength, messageBuf, sizeof(packetLength));
    // convert the byte order from network into host for type long
    packetLength = ntohl(packetLength);

    // get the msg type
    unsigned char packetType = messageBuf[sizeof(unsigned int)];

    // check if you have the whole packet now
    if (nRead < sizeof(unsigned int) + sizeof(unsigned char) + packetLength){
        // don't have the whole packet yet
        return nullptr;
    }

    // grab the payload of the msg
    vector<char> packetPayload(messageBuf + sizeof(unsigned int) + sizeof(unsigned char), 
        messageBuf + sizeof(unsigned int) + sizeof(unsigned char) + packetLength);

    // create a baseMsg to return
    baseMsg* msg = new baseMsg(packetType, packetPayload.data(), packetPayload.size());

    return msg;
}

// recieve a UDP msg, non blocking, returns msg as baseMsg* - takes in a struct sockadrr_in
baseMsg* receiveNonblockingUdp(int sd, struct sockaddr_in addrinfo){
    int messageBuf[BUFFER_SIZE] = {0};
    // put the size of the addrinfo into right var
    socklen_t addrLen = sizeof(addrinfo);
    // recieve the message into the msg[] array and make sure it was read correctly
    int nRead = recvfrom(sd, &messageBuf, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&addrinfo), &addrLen);
    // error checks
    if (nRead == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // do nothing simply nothing to read yet
            return nullptr;
        } else {
            cerr << "Error reading from socket: SD = " << sd << endl; 
            return nullptr;
        }
    } else if (nRead == 0) {
        cerr << "Client closed the connection" << endl;
        return nullptr;
    }
    
    // check if the length of the packet has come through
    if (nRead < sizeof(unsigned int) + sizeof(unsigned char)){
        // not enough data for the packet header
        return nullptr;
    }

    // to hold the packet length
    unsigned int packetLength = 0;
    // copy the first 4 bytes into the packetLength
    memcpy(&packetLength, messageBuf, sizeof(packetLength));
    // convert the byte order from network into host for type long
    packetLength = ntohl(packetLength);

    // get the msg type
    unsigned char packetType = messageBuf[sizeof(unsigned int)];

    // check if you have the whole packet now
    if (nRead < sizeof(unsigned int) + sizeof(unsigned char) + packetLength){
        // don't have the whole packet yet
        return nullptr;
    }

    // grab the payload of the msg
    vector<char> packetPayload(messageBuf + sizeof(unsigned int) + sizeof(unsigned char), 
        messageBuf + sizeof(unsigned int) + sizeof(unsigned char) + packetLength);

    // create a baseMsg to return
    baseMsg* msg = new baseMsg(packetType, packetPayload.data(), packetPayload.size());

    return msg;
}

// receive msg, blocking/stop and wait - tcp
baseMsg receiveBlockingTcp(int sd){
    // define the header size
    const size_t HEADER_SIZE = sizeof(unsigned int) + sizeof(unsigned char);
    // to store the msg header
    char headerBuffer[HEADER_SIZE];

    // count the bytes read
    int totalRead = 0;
    // read in the header
    while (totalRead < HEADER_SIZE){
        // read the data from the socket
        int nRead = recv(sd, headerBuffer + totalRead, HEADER_SIZE - totalRead, 0);
        if (nRead == -1){
            cerr << "Error reading from socket: SD = " << sd << endl; 
            break; 
        } else if (nRead == 0) {
            cerr << "Client closed the connection" << endl;
            break;
        }
        // add the current bytes read to the total
        totalRead += nRead;
    }

    // get the header info
    unsigned int payloadLength = 0;
    unsigned char type = 0;
    
    // put the header info into their vars
    memcpy(&payloadLength, headerBuffer, sizeof(payloadLength));
    memcpy(&type, headerBuffer + sizeof(payloadLength), sizeof(type));
    
    // convert to host byte order
    payloadLength = ntohl(payloadLength);

    // read in the payload 
    vector<char> payload(payloadLength);
    // reset the totalRead - reuse it!
    totalRead = 0;
    // read in the whole payload
    while(totalRead < payloadLength){
        int nRead = recv(sd, payload.data() + totalRead, payloadLength - totalRead, 0);
        if (nRead == -1){
            cerr << "Error reading from socket: SD = " << sd << endl; 
            break; 
        } else if (nRead == 0) {
            cerr << "Client closed the connection" << endl;
            break;
        }
        // add the current bytes read to the total
        totalRead += nRead;
    }

    // create a new baseMsg
    struct baseMsg msg(type, payload.data(), payload.size());
    return msg;
}

// receive msg, non-blocking does not wait - tcp
baseMsg* receiveNonblockingTcp(int sd){
    // "buffer" for reading in the server response
    char readInBuffer[BUFFER_SIZE];
    // holds the incomplete data until whole packet comes through
    string incompletePacket;
    // count the # of bytes read
    int nRead;

    // while there is data to read in the socket
    while ((nRead = recv(sd, &readInBuffer, BUFFER_SIZE - 1, 0)) > 0){
        // null terminate th buffer to help other functions work right
        readInBuffer[nRead] = '\0';
        // add what is read to the request
        incompletePacket.append(readInBuffer, nRead);

        // grab the data sent and put into packets
        while (true){
            // check if the length of the packet has come through
            if (incompletePacket.size() < sizeof(unsigned int) + sizeof(unsigned char)){
                // not enough data for the packet header
                break;
            }

            // to hold the packet length
            unsigned int packetLength = 0;
            // copy the first 4 bytes into the packetLength
            memcpy(&packetLength, incompletePacket.data(), sizeof(packetLength));
            // convert the byte order from network into host for type long
            packetLength = ntohl(packetLength);

            // check if you have the whole packet now
            if (incompletePacket.size() < sizeof(unsigned int) + sizeof(unsigned char) + packetLength){
                // don't have the whole packet yet
                break;
            }

            // get the packet type -- the byte after the length
            unsigned char packetType = incompletePacket[sizeof(unsigned int)];
            // get the payload -- everything after the length + type - takes in (start, end)
            vector<char> packetPayload(incompletePacket.begin() + sizeof(unsigned int) + sizeof(unsigned char), incompletePacket.begin() + sizeof(unsigned int) + sizeof(unsigned char) + packetLength);
            // make the baseMsg to return
            baseMsg* msg = new baseMsg(packetType, packetPayload.data(), packetPayload.size());
            // remove the full packet from the buffer
            incompletePacket.erase(0, sizeof(unsigned int) + sizeof(unsigned char) + packetLength);

            // process the packet received
            return msg;
        }
    }

    // check for receive errors
    if (nRead == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // do nothing simply nothing to read yet
        } else {
            cerr << "Error reading from socket: SD = " << sd << endl; 
        }
    } else if (nRead == 0) {
        cerr << "Client closed the connection" << endl;
    }

    // if you get here there is nothing
    return nullptr;
}
