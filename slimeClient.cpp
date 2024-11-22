// this holds the protocols and code to run the Slime/Client side of the platformer game: Knight and Slime
// these are the portocols to run once the 2 players are connected

// my files
#include "slimeClient.hpp"

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